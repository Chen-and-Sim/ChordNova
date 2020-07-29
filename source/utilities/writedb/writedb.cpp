// SmartChordGen-utility-WriteDatabase v1.5 [Build: 2020.7.2]
// (c) 2020 Wenge Chen, Ji-woon Sim.

#include <fstream>
#include <iostream>
#include <vector>
#include "..\..\main\source\functions.h"
#include "..\..\main\source\functions.cpp"

using namespace std;
vector<vector<int>> rec;
vector<vector<int>> organized;

vector<int> normal_form(vector<int>& set)
{
	int len = set.size();
	int i_rec = 0, interval;
	vector<int> intervals, best;
	for(int i = 0; i < len; ++i)
	{
		intervals.clear();
		for(int j = len - 1; j > 0; --j)
		{
			interval = set[(i + j) % len] - set[i];
			if(interval < 0)  interval += 12;
			intervals.push_back(interval);
		}
		if(i == 0 || intervals < best)
		{
			best = intervals;
			i_rec = i;
		}
	}
	vector<int> result;
	int copy = set[i_rec];
	for(int j = 0; j < len; ++j)
	{
		int val = set[(i_rec + j) % len] - copy;
		if(val < 0)  val += 12;
		result.push_back(val);
	}
	return result;
}

vector<int> prime_form(vector<int> set)
// We assume that "set" begins with 0.
{
	vector<int> normal1 = normal_form(set);
	vector<int> normal2, inversion;
	int len = set.size();
	for(int i = len - 1; i >= 0; --i)
	{
		int val = (-1) * set[i];
		if(val < 0)  val += 12;
		inversion.push_back(val);
	}
	normal2 = normal_form(inversion);
	return (normal1 < normal2) ? normal1 : normal2;
}

void organize(bool prime)
{
	vector<int> values;
	int value, pos;
	for(int i = 0; i < rec.size(); ++i)
	{
		if(prime)
			rec[i] = prime_form(rec[i]);
		else  rec[i] = normal_form(rec[i]); 
		value = 0;
		for(int j = 0; j < rec[i].size(); ++j)
			value += (1 << rec[i][j]);
		pos = find(values, value);
		if(pos != -1)
		{
			values.insert(values.begin() + pos, value);
			organized.push_back(rec[i]);
		}
	}
}

void to_text()
{
	for(int i = 0; i < organized.size(); ++i)
	{
		for(int j = 0; j < organized[i].size() - 1; ++j)
			fout << organized[i][j] << " ";
		fout << *organized[i].rbegin() << "\n";
	}
	fout.close();
}

void to_midi()
{
	int chord_count = organized.size(), note_count = 0;
	for(int i = 0; i < chord_count; ++i)
		note_count += organized[i].size();
	midi_head(chord_count, note_count);
	
	for(int i = 0; i < chord_count; ++i)
	{
		for(int j = 0; j < organized[i].size(); ++j)
			organized[i][j] += 60;
		chord_to_midi(organized[i]);
	}
	
	m_fout.write("\x00\xFF\x2F\x00", 4);
	m_fout.close();
}

vector<int> get_vec(vector<int>& note_set)
{
	int s_size = note_set.size();
	vector<int> d_all, count_vec;
	for(int i = 0; i < s_size; ++i)
		for(int j = i + 1; j < s_size; ++j)
		{
			int temp = min(note_set[j] - note_set[i], 12 + note_set[i] - note_set[j]);
			d_all.push_back(temp);
		}

	count_vec.assign(6, 0);
	for(int i = 0; i < d_all.size(); ++i)
		++count_vec[d_all[i] - 1];
	return count_vec;
}

void by_vec()
{
	int mins[6], maxs[6];
	int min, max;
	cout << '\n';
	for(int i = 1; i <= 6; ++i)
	{
		cout << " > Please input the desired range for interval \"" << i << "\": ";
		inputNums(min, max, 0, (const int)INF);
		mins[i - 1] = min;
		maxs[i - 1] = max;
	}
	
	vector<int> note_set, count_vec;
	int id = -1;
	while(id < 2047) // 2 ^ 11 - 1
	{
		next(note_set, id, true);
		count_vec = get_vec(note_set);
		bool b = true;
		for(int i = 0; i < 6; ++i)
			if(count_vec[i] < mins[i] || count_vec[i] > maxs[i])
				b = false;
		if(b)	 rec.push_back(note_set);
	}
}

int get_tension(vector<int>& note_set)
{
	vector<int> d_all;
	int s_size = note_set.size();
	for(int i = 0; i < s_size; ++i)
		for(int j = i + 1; j < s_size; ++j)
		{
			int temp = min(note_set[j] - note_set[i], 12 + note_set[i] - note_set[j]);
			d_all.push_back(temp);
		}

	vector<int> count_vec;
	count_vec.assign(6, 0);
	for(int i = 0; i < d_all.size(); ++i)
		++count_vec[d_all[i] - 1];
		
	int tension = 0;
	for(int i = 0; i < 6; ++i)
		tension = tension + count_vec[i] * (1 << ( (9 - i) % 5 ));
	return tension;
}

void by_subset()
{
	vector<int> notes, note_set;
	cout << "\n > Please input desired scale or chord as the original set: ";
	inputVec(notes, 0, 127);

	for(int i = 0; i < notes.size(); ++i)
		note_set.push_back((notes[i] - notes[0] + 12) % 12);
	bubble_sort(note_set);
	remove_duplicate(note_set);
	cout << " > Please note that the input is normalized into a compact set: ";
	cprint("\0", note_set, " ", ".\n");
	
	int n_min, n_max;
	cout << "\n > Please input desired range of numbers of note (N) for all subsets: ";
	inputNums(n_min, n_max, 0, (const int)INF);
	
	int t_min = 0, t_max = INF;
	cout << "\n > Would you like to filter by tension value (T) (Y / N)? ";
	char ch;  inputY_N(ch);
	if(ch == 'Y' || ch == 'y')
	{
		cout << "\n > Please input desired range for tension value (T) in integer: ";
		inputNums(t_min, t_max, 0, (const int)INF);
	}

	vector<int> indexes, subset;
	int s_size = note_set.size();
	int id = -1, max_id = (1 << s_size) - 1;
	while(id < max_id)
	{
		next(indexes, id, false);
		if(indexes.size() >= n_min && indexes.size() <= n_max)
		{
			subset.clear();
			for(int i = 0; i < indexes.size(); ++i)
				subset.push_back( note_set[indexes[i]] );
			int tension = get_tension(subset);
			if(tension >= t_min && tension <= t_max)
			{
				int copy = subset[0];
				for(int i = 0; i < subset.size(); ++i)
					subset[i] -= copy;
				rec.push_back(subset);
			}
		}
	}
}

int main()
{
	cout << "[[  SmartChordGen v1.5 [Build: 2020.7.2]  ]]\n"
		  << "[[  (c) 2020 Wenge Chen, Ji-woon Sim.     ]]\n\n"
		  << " > Utility - Write Database:\n";
	
	cout << " > Please assign a name for the database (.db): ";
	char _namedb[100] = "\0", namedb[100];
	inputFilename(_namedb, ".db", false);
	ignore_path_ext(namedb, _namedb);
	
	cout << "\n > Please input the name of the author of the database (.db): ";
	char nameauthor[100];
	cin.getline(nameauthor, 100, '\n');
	cin.clear();
	
	cout << "\n > Would you like to sort by vec or by subsets (Y - by vec / N - by subsets)? ";
	char ch;  inputY_N(ch);
	bool _by_vec = true;
	if(ch == 'N' || ch == 'n')  _by_vec = false;
	
	cout << "\n > Would you like to output the results as a MIDI (.mid) file (Y / N)? ";
	inputY_N(ch);
	bool midi_output = false;
	char namemidi[100];
	if(ch == 'Y' || ch == 'y')
	{
		midi_output = true;
		cout << "\n > Please assign a name for the MIDI (.mid) file: ";
		inputFilename(namemidi, ".mid", false);
		m_fout.open(namemidi, ios::binary | ios::trunc);
	}
	
	cout << "\n > Would you like to output the results in prime forms (Y / N)? ";
	inputY_N(ch);
	bool prime = false;
	if(ch == 'Y' || ch == 'y')
		prime = true;

	fout.open(_namedb, ios::trunc);
	fout << "// SmartChordGen Chord Database\n"
		  << "// (c) 2020 " << nameauthor << ".\n"
		  << "// Database begins below.\n"
		  << "title_en = " << namedb << endl
		  << "title_ch = " << namedb << endl;
		  
	if(_by_vec)  by_vec();
	else    by_subset();
	organize(prime);
	merge_sort(organized.begin(), organized.end(), smallerVec);
	to_text();
	if(midi_output)  to_midi();
	
	cout << "\n > Output finished. Now you can close the program.\n\n";
	system("pause"); 
	return 0;
}


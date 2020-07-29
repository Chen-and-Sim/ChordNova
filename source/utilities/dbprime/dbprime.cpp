// SmartChordGen-utility-ConvertDatabaseToPrimeForm v1.5 [Build: 2020.7.2]
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

void organize()
{
	vector<int> values;
	int value, pos;
	for(int i = 0; i < rec.size(); ++i)
	{
		rec[i] = prime_form(rec[i]);
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

	char str3[5] = "\x00\xFF\x2F\x00";
	m_fout.write(str3, 4);
	m_fout.close();
}

int main()
{
	cout << "[[  SmartChordGen v1.5 [Build: 2020.7.2]  ]]\n"
		  << "[[  (c) 2020 Wenge Chen, Ji-woon Sim.     ]]\n\n"
		  << " > Utility - Convert Database to Prime Form:\n";
	
	cout << " > Please input the name of the database (.db) file: ";
	char str1[100] = "\0", str2[100], path[100];
	inputFilename(str1, ".db", true);
	ignore_path_ext(str2, str1);
	strncpy(path, str1, strlen(str1) - strlen(str2) - 3);
	path[strlen(str1) - strlen(str2) - 3] = '\0';

	ifstream fin(str1, ios::in);
	char begin[10][100], ch;
	int count = 0;
	while(true)
	{
		ch = fin.peek();
		if(ch == '/' || ch == 't')
		{
			fin.getline(begin[count], 100, '\n');
			++count;
		}
		else  break;
	}

	int note;
	vector<int> set;
	do{
		set.clear();
		do{
			fin >> note;
			set.push_back(note);
		}  while(!fin.eof() && fin.get() != '\n');
		int size = set.size();
		if(size == 0)  break;
		rec.push_back(set);
	}  while(!fin.eof());
	fin.close();
	
	cout << "\n > Overwrite " << str2 << ".db? (Y / N) ";
	inputY_N(ch);
	if(ch == 'Y' || ch == 'y')
		fout.open(str1, ios::trunc);
	else
	{
		char str3[100];
		strncpy(str3, str1, strlen(str1) - 3);
		str3[strlen(str1) - 3] = '\0';
		strcat(str3, "-prime.db");
		fout.open(str3, ios::trunc);
		strcat(str2, "-prime.db");
		cout << "\n > The output will be saved as " << str2 << " in the same folder.\n";
	}
	
	char namemidi[100];
	cout << "\n > Please assign a name for the MIDI (.mid) file: ";
	inputFilename(namemidi, ".mid", false);
	strcat(path, namemidi);
	m_fout.open(path, ios::binary | ios::trunc);

	for(int i = 0; i < count; ++i)
		fout << begin[i] << endl;
	organize();
	merge_sort(organized.begin(), organized.end(), smallerVec);
	to_text();
	to_midi();

	cout << "\n > Output finished. Now you can close the program.\n\n";
	fout.close();
	m_fout.close();
	system("pause"); 
	return 0;
}


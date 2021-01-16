// ChordNova-utility-CharmingChords v3.0 [Build: 2020.11.27]
// Find all chord pairs (A, B) that satisfies the following conditions:
// a) the vector 'd' ('self_diff' in chorddata.h) of chord A is included in the set 'P';
// b) the movement vector from A to B ('vec' in chorddata.h) is included in the set 'Q';
// c) the vector 'd' of chord B is included in the set 'R'.
// (c) 2021 Wenge Chen, Ji-woon Sim.

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include "../../main/functions.h"
#include "../../main/functions.cpp"

using namespace std;
class Vector;
class Chord;
vector<int> P, Q, R;
bool sort_by_tension;

bool include	  (vector<int>&, vector<int>&);
void input_n	  (int&, int&);
void input_set	  (vector<int>&, char);
void reduce		  (int&);
double get_chroma(vector<int>&, vector<int>&);
bool _smaller    (const Chord&, const Chord&);

class Vector // Please distinguish it from the 'vector' in STL.
{
private:
	vector<int> components;
	int id;
	int max_id;
	int size;
	int choice;

public:
	Vector() {}
	Vector(int n)
	{
		size = n;
		choice = Q.size();
		id = -1;
		max_id = 1;
		for(int i = 0; i < size; ++i)
			max_id *= choice;
		--max_id;
	}

	int& operator[] (int index)
	{
		if(index >= 0 && index < size)
			return components[index];
	}
	
	int& get_id()      { return id; }
	int& get_max_id()  { return max_id; }
	int& get_size()    { return size; }
	vector<int>& get_components()  { return components; }

	void next()
	{
		components.clear();
		int copy = (++id), index = 0;
		while(index < size)
		{
			components.push_back( Q[copy % choice] );
			copy /= choice;
			++index;
		}
	}
	// e.g. Q = {-1, 0, 1}, choice = 3,
	// id = 5 = 2 * 3^0 + 1 * 3^1 + 0 * 3^2 + ... + 0 * 3^(size - 1)
	//  =>  elements = { Q[2], Q[1], Q[0], ... , Q[0] }
	
	bool valid()
	{
		int sum = 0;
		for(int i = 0; i < size - 1; ++i)
			sum = sum + (components[i] != components[i+1]);
		return sum;
	}
};

class Chord
{
private:
	vector<int> notes;
	vector<int> diff;
	vector<Vector> vecs;
	vector<int> rec_id;
	vector<int> count_vec;
	int id;
	int t_size;
	int group_count;
	int progr_count;
	int tension;

	Chord operator+ (Vector vec)
	{
		Chord sum(*this);
		for(int i = 0; i < t_size; ++i)
			sum[i] = (sum[i] + vec[i] + 12) % 12;
		sum.sort_delete();
		return sum;
	}

	void sort_delete()
	{
		bubble_sort(notes);
		for(int i = 0; i < t_size - 1; ++i)
			if(notes[i] == notes[i+1])
			{
				notes.erase(notes.begin() + i);
				--i;  --t_size;
			}
	}
	
	void next()
	{
		rec_id.clear();
		notes.clear();
		int copy = (++id), index = 1;
		notes.push_back(0);
		do{
			if(copy % 2 == 1)
				notes.push_back(index);
			copy /= 2;
			++index;
		}  while(copy != 0);
		t_size = notes.size();
	}
	// e.g. id = 26 = 2^1 + 2^3 + 2^4  =>  notes = {0, 2, 4, 5}

	void sort_chroma()
	{
		vector<double> chromas;
		int size = vecs.size();
		Chord new_chord;
		for(int i = 0; i < size; ++i)
		{
			new_chord = *this + vecs[i];
			chromas.push_back( get_chroma(notes, new_chord.notes) );
		}
		Vector v_temp;
		double k_temp;
		for(int i = size - 1; i >= 0; --i)
		{
			for(int j = 0; j < i; ++j)
			{
				if(chromas[j] < chromas[j+1])
				{
					v_temp = vecs[j];
					vecs[j] = vecs[j+1];
					vecs[j+1] = v_temp;
					k_temp = chromas[j];
					chromas[j] = chromas[j+1];
					chromas[j+1] = k_temp;
				}
			}
		}
	}
	
	void sort_tension()
	{
		vector<int> tensions;
		int size = vecs.size();
		Chord new_chord;
		for(int i = 0; i < size; ++i)
		{
			new_chord = *this + vecs[i];
			new_chord.set_tension();
			tensions.push_back(new_chord.tension);
		}
		Vector v_temp;
		int t_temp;
		for(int i = size - 1; i >= 0; --i)
		{
			for(int j = 0; j < i; ++j)
			{
				if(tensions[j] < tensions[j+1])
				{
					v_temp = vecs[j];
					vecs[j] = vecs[j+1];
					vecs[j+1] = v_temp;
					t_temp = tensions[j];
					tensions[j] = tensions[j+1];
					tensions[j+1] = t_temp;
				}
			}
		}
	}

	void rec_diff()
	{
		diff.clear();
		for(int i = 0; i < t_size - 1; ++i)
			diff.push_back(notes[i + 1] - notes[i]);
	}
	
	bool check_diff(vector<int> v)
	{
		diff.clear();
		rec_diff();
		vector<int> copy = diff;
		if(intersect(copy, v).size() != copy.size())
			return false;
		return true;
	}
	
	bool charming(int n)
	{
		do{
			next();
		}  while(t_size != n);
		vecs.clear();
		Vector vec(t_size);
		int max = vec.get_max_id();
		while(vec.get_id() < max)
		{
			do{
				vec.next();
			}  while( !vec.valid() );
			Chord new_chord;
			new_chord = *this + vec;
			if( check_diff(P) && new_chord.check_diff(R) && !include(notes, new_chord.notes) )
			{
				int new_id = 0;
				for(int i = 0; i < new_chord.t_size; ++i)
					new_id += (1 << new_chord[i]);
				if(find(rec_id, new_id) != -1)
				{
					vecs.push_back(vec);
					rec_id.push_back(new_id);
				}
			}
		}
		if(!vecs.empty())
			return true;
		return false;
	}

	void set_tension()
	{
		vector<int> d_all;
		for(int i = 0; i < t_size; ++i)
			for(int j = i + 1; j < t_size; ++j)
			{
				int temp = min(notes[j] - notes[i], 12 + notes[i] - notes[j]);
				d_all.push_back(temp);
			}

		count_vec.assign(6, 0);
		for(int i = 0; i < d_all.size(); ++i)
			++count_vec[d_all[i] - 1];

		tension = 0;
		for(int i = 0; i < 6; ++i)
			tension = tension + count_vec[i] * (1 << ( (9 - i) % 5 ));
	}

	void print(vector<Chord>& chords)
	{
		int size = chords.size();
		static int count = 0; 
		for(int i = 0; i < size; ++i)
		{
			chords[i].rec_diff();
			chords[i].set_tension();
			fout << "Original chord #" << (++count) << ":\n";
			fprint("set = ", chords[i].notes);
			fprint("d = "  , chords[i].diff);
			fprint("vec = ", chords[i].count_vec, "\0");
			fout<< "t = " << chords[i].tension << "\n";

			vector<Vector>& vecs = chords[i].vecs;
			int v_size = vecs.size();
			fout << v_size << " progression";
			if(v_size != 1)  fout << 's';
			fout << "\n\n";
			
			for(int j = 0; j < v_size; ++j)
			{
				Chord new_chord;
				new_chord = chords[i] + vecs[j];
				new_chord.rec_diff();
				new_chord.set_tension();

				fprint("v = ", vecs[j].get_components());
				fprint("set = ", new_chord.notes);
				fprint("d = "  , new_chord.diff);
				fprint("vec = ", new_chord.count_vec, "\0");
				fout << "t = " << new_chord.tension << ", "
					  << "k = " << get_chroma(chords[i].notes, new_chord.notes) << ", "
					  << "c = " << intersect(chords[i].notes, new_chord.notes, true).size() << "\n\n";

				if(!fout)
				{
					cerr << "Something went wrong while doing output.";
					exit(0);
				}
			}
			fout << "\n";	
		}
	}

public:
	Chord(): id(0), group_count(0), progr_count(0) {}
	Chord(const Chord& A)
	{
		t_size = A.t_size;
		notes  = A.notes;
		diff   = A.diff;
		vecs   = A.vecs;
		tension = A.tension;
		count_vec = A.count_vec;
	}
	
	const int& get_t_size() const  { return t_size; }
	const int& get_progr()  const  { return progr_count; }
	const int& get_group()  const  { return group_count; }
	
	const int& operator[] (int index) const
	{
		if(index >= 0 && index < t_size)
			return notes[index];
	}
	
	int& operator[] (int index)
	{
		if(index >= 0 && index < t_size)
			return notes[index];
	}
	
	void get_result(int n_min, int n_max)
	{
		vector<Chord> chords;
		for(int n = n_min; n <= n_max; ++n)
		{
			id = 0;
			chords.clear();
			while(id < 2047)
			{
				if(charming(n))
				{
					++group_count;
					progr_count += vecs.size();
					if(sort_by_tension)  sort_tension();
					else  sort_chroma();
					chords.push_back(*this);
				}
			}
			merge_sort(chords.begin(), chords.end(), _smaller);
			print(chords);
			cout << "\n > n = " << n << " finished.";
		}
	}
};

bool include(vector<int>& v1, vector<int>& v2)
{
	vector<int> intersection = intersect(v1, v2, true);
	if(intersection.size() == v1.size() ||
		intersection.size() == v2.size() )
		return true;
	return false;
}

void input_n(int& n_max, int& n_min)
{
	cout << "\n > Please input the range of n(the number of notes in a chord) in the form of 'min max': ";
	cin >> n_max >> n_min;
	cin.ignore();
	if(n_max < n_min)
	{
		int temp = n_min;
		n_min = n_max;
		n_max = temp;
	}
	fout << "n_min = " << n_min << ",  n_max = " << n_max << '\n';
	if(n_min <= 2)  n_min = 3;
}

void input_set(vector<int>& v, char name)
{
	int element;
	cout << "\n > Please input the elements of the set " << name << ", separated by a whitespace: ";
	fout << name << " = {";
	do{
		cin >> element;
		v.push_back(element);
		fout << element << ", ";
	}  while(cin.get() != '\n');
	fout.seekp(-2, ios::cur);
	fout << "}\n";
	bubble_sort(v);
}

void reduce(int& n_max)
{
	int P_min = P[0];
	for(int i = 0; i < P.size(); ++i)
	{
		if(P[i] < P_min)
			P_min = P[i];
	}
	int temp = ceil(12.0 / P_min);
	if(temp < n_max)
		n_max = temp;
}

double get_chroma(vector<int>& v1, vector<int>& v2)
{
	int len1 = v1.size();
	int len2 = v2.size();
	double sum1 = 0.0, sum2 = 0.0;
	for(int i = 0; i < len1; ++i)
		sum1 = sum1 + 6 - (5 * v1[i] + 6) % 12;
	for(int i = 0; i < len2; ++i)
		sum2 = sum2 + 6 - (5 * v2[i] + 6) % 12;
	return sum2 / len2 - sum1 / len1;
}

bool _smaller(const Chord& chord1, const Chord& chord2)
// We assume that chord1.t_size == chord2.t_size.
{
	int size = chord1.get_t_size();
	for(int i = 0; i < size; ++i)
	{
		if(chord1[i] < chord2[i])
			return true;
		if(chord1[i] > chord2[i])
			return false;
	}
	return false;
}

int main()
{
	cout << "[[  ChordNova v3.0 [Build: 2021.1.14]  ]]\n"
		  << "[[  (c) 2021 Wenge Chen, Ji-woon Sim.  ]]\n\n"
		  << " > Utility - Charming Chords:\n";
	cout << "\n > Please assign a name for the output file (the default extension is '.txt'): ";
	char output[100] = "\0";
	inputFilename(output, ".txt", false);
	fout.open(output, ios::trunc);

	fout << "Your input:\n";
	int n_max = 0, n_min = 0;
	input_n(n_max, n_min);
	input_set(P, 'p');
	input_set(Q, 'q');
	input_set(R, 'r');
	
	fout << "Sorting method: ";
	cout << "\n > Sorting method (Y - by tension, N - by chroma): ";
	char ch;  inputY_N(ch);
	if(ch == 'Y' || ch == 'y')
	{
		sort_by_tension = true;
		fout << "by tension\n\n"; 
	}
	else
	{
		sort_by_tension = false;
		fout << "by chroma\n\n"; 
	}
	
	cout << "\n > Please wait for the program to output the result to a file...\n";
	reduce(n_max);
	Chord chord;
	chord.get_result(n_min, n_max);
	fout << "There are " << chord.get_group() << " groups of charming chord progressions.\n"
		  << "The total number of charming chord progressions is " << chord.get_progr() <<  ".\n";
	fout.close();
	cout << "\n\n > Output finished. Now you can close the program.\n\n";

	system("pause"); 
	return 0;
}


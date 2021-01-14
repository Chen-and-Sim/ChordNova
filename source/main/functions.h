// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// functions.h

#ifndef FUNCTIONS
#define FUNCTIONS

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

extern ofstream fout, m_fout;
// 'fout' for text output; 'm_fout' for MIDI output
extern stringstream stream;
// for output in chord analysis
extern double INF;
extern double MINF;
extern int expansion_indexes[16][16][3432][15];
// One of our goals is to expand 'notes' to a size of 'm_max' by adding some notes from itself.
// We can create a vector 'expansion' (*) with size 'm_max' with elements from {0, 1, ..., notes.size() - 1}
// (sorted in ascending order), then the desired expansion is simply { notes[expansion[i]] }.
// 'expansion_indexes[min][max][i]' is the 'i'th alternative of expansion (*) for 'notes.size() = min' and 'm_max = max'.
extern int note_pos[12];
extern vector<int> omission[8];
// 'omission[i]' represents omission allowed for i-note chords.
// All elements in the vectors belong to {1, 3, 5, 7, 9, 11, 13}.
extern vector<int> chord_library;
// i.e. chord database (in integer form).
// Contains 'set_id' of all 12 transpositions of all 'note_set's in the chord database file.
extern vector<vector<int>> alignment_list;

struct Movement
{
	int amount;
	int instance = 0;
	double percentage;
};

// input from console
template<typename T>
void inputNum(T& num, const T& min, const T& max, const T& dflt)
// Input a number between 'min' and 'max'.
// If the user input 'Enter', the default value 'dflt' will be set.
{
	char ch;
	while(true)
	{
		if(dflt != MINF)
		// note the overloaded function below
		{
			cin.get(ch);
			if(ch == '\n')
			{
				num = dflt;
				break;
			}
			cin.putback(ch);
		}

		cin >> num;
		if(!cin || num < min || num > max)
		{
			cout << " > Invalid input. Please try again. ";
			if(!cin)
			{
				cin.clear();
				cin.ignore(10, '\n');
			}
		}
		else
		{
			cin.ignore(10, '\n');
			break;
		}
	}
}

template<typename T>
void inputNum(T& num, const T& min, const T& max)
// Input a number between 'min' and 'max'.
{
	inputNum(num, min, max, (const T)(MINF));
}

template<typename T>
void inputNums(T& num1, T& num2, const T& min, const T& max)
// Input two numbers between 'min' and 'max'.
// The first one should not be larger than the second.
{
	cin >> num1 >> num2;
	while(!cin || num1 < min || num1 > num2 || num2 > max)
	{
		cin.clear();
		cin.ignore(10, '\n');
		cout << " > Invalid input. Please try again. ";
		cin >> num1 >> num2;
	}
	cin.ignore(10, '\n');
}

extern void inputY_N(char&);
extern void inputFilename(char* str, const char* ext, bool find_file, const char* dflt = "\0");
extern void inputVec(vector<int>& vec, const int& min = MINF, const int& max = INF, bool organize = true);

// file reading
extern void ignore_path_ext(char*, char*);
extern void dbentry(const char*);
extern void read_alignment(const char*);

// output
extern void fprint(const char* begin, const vector<int>& v, const char* sep = ", ",
						 const char* end = "\n", bool is_decimal = true);
extern void sprint(const char* begin, const vector<int>& v, const char* sep = ", ",
						 const char* end = "\n", bool is_decimal = true);
extern void cprint(const char* begin, const vector<int>& v, const char* sep = ", ", const char* end = ", ");

// type conversion
extern int  nametonum(char* str);
extern int  chromatonum (int);
extern void chromatoname(int, char*);
extern void inttostring (int num, char* str, int base = 10);
extern void note_set_to_id(const vector<int>&, vector<int>&);
extern void id_to_notes (const int&, vector<int>&);

// mathematics
extern int    rand(const int&, const int&);
extern double rand(const double&, const double&);
extern int    sign(const int&);
extern int    sign(const double& x, const double& bound = 1E-5);
extern double round_double(const double&, const int&);
extern int    comb(const int&, const int&);

// set operation
extern vector<int> intersect(vector<int>& A, vector<int>& B, bool regular = false);
extern vector<int> get_union(const vector<int>&, const vector<int>&);
extern vector<int> get_complement(const vector<int>&, const vector<int>&);
extern vector<int> normal_form(vector<int>&);

// MIDI operation
extern int  swapInt(const int& value, const int& len = 4);
extern int  to_VLQ(int);
extern void midi_head(const int&, const int&);
extern void chord_to_midi(vector<int>& notes, int beat = 1);

// misc
extern bool different_name(const char*, const char*);
extern void next(vector<int>&, int&, bool);
extern int  find_root(vector<int>&);
extern void insert(int*, int*, int, int);
extern void set_expansion_indexes();

// comparison
extern bool smaller(const int&, const int&);
extern bool smallerVec(const vector<int>&, const vector<int>&);
extern bool larger_perc(const Movement&, const Movement&);
extern void remove_duplicate(vector<int>&);
extern void bubble_sort(vector<int>&);

template<typename T>
int find(const vector<T>& v, const int& begin, const int& end, const T& target)
// We assume that vec has been sorted in ascending order.
// if found, returns -1; otherwise, returns the position of the smallest element larger than "target"
{
	if(end - begin == 0)  return 0;
	if(end - begin == 1)
	{
		if(v[begin] == target)
			return -1;
		else if(v[begin] < target)
			return begin + 1;
		else  return begin;
	}

	const int middle = (end + begin) / 2;
	if(v[middle] == target)
		return -1;
	if(v[middle] < target)
		return find(v, middle, end, target);
	return find(v, begin, middle, target);
}

template<typename T>
int find(const vector<T>& v, const T& target)
{
	return find(v, 0, v.size(), target);
}

template<typename T, typename v_it>
void merge_sort(const v_it begin, const v_it end, bool (*_compare)(const T&, const T&))
// "end" is not included in the vector.
{
	if(end - begin <= 1)  return;

	const int half = (end - begin) / 2;
	v_it middle = begin + half;
	merge_sort(begin, middle, *_compare);
	merge_sort(middle, end, *_compare);

	vector<T> temp(begin, end);
	const v_it t_begin  = temp.begin();
	const v_it t_middle = temp.begin() + half;
	const v_it t_end    = temp.end();
	v_it t_pos1 = t_begin, t_pos2 = t_middle, pos = begin;

	while(t_pos1 < t_middle && t_pos2 < t_end)
	{
		if( (*_compare)(*t_pos1, *t_pos2) )
			*(pos++) = *(t_pos1++);
		else
			*(pos++) = *(t_pos2++);
	}
	while(pos < end)
	{
		if(t_pos1 < t_middle)
			*(pos++) = *(t_pos1++);
		else  // if(t_pos2 < t_end)
			*(pos++) = *(t_pos2++);
	}
}

#endif

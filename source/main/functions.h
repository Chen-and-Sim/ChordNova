// SmartChordGen v2.5 [Build: 2020.8.8]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// functions.h

#ifndef FUNCTIONS
#define FUNCTIONS

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

extern ofstream fout, m_fout;
extern double INF;
extern double MINF;
extern int expansion_indexes[16][16][3432][15];
extern int note_pos[12];
extern vector<int> omission[13];
extern vector<int> chord_library;
extern vector<vector<int>> alignment_list;

struct Movement
{
	int amount;
	int instance = 0;
	double percentage;
};

template<typename T>
void inputNum(T& num, const T& min, const T& max, const T& dflt)
{
	char ch;
	while(true)
	{
		if(dflt != MINF)
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
{
	inputNum(num, min, max, (const T)(MINF));
}

template<typename T>
void inputNums(T& num1, T& num2, const T& min, const T& max)
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

extern int  nametonum(char*);
extern void ignore_path_ext(char*, char*);
extern void next(vector<int>&, int&, bool);
extern int  find_root(vector<int>&);
extern void note_set_to_id (const vector<int>&, vector<int>&);
extern void dbentry(const char*);
extern void read_alignment(const char*);
extern int  rand   (const int&, const int&);
extern double rand (const double&, const double&);
extern void insert (int*, int*, int, int);
extern void set_expansion_indexes();
extern int  sign(const int&);
extern int  comb(const int&, const int&);
extern void fprint(const char* begin, const vector<int>& v, const char* sep = ", ", const char* end = "\n");
extern void cprint(const char* begin, const vector<int>& v, const char* sep = ", ", const char* end = ", ");
extern vector<int> intersect(vector<int>& A, vector<int>& B, bool regular = false);
extern vector<int> get_union(const vector<int>&, const vector<int>&);
extern vector<int> get_complement(const vector<int>&, const vector<int>&);
extern int  swapInt(const int& value, const int& len = 4);
extern int  to_VLQ(int);
extern void midi_head(const int&, const int&);
extern void chord_to_midi(vector<int>& notes, int beat = 1);

extern bool smaller(const int&, const int&);
extern bool smallerVec(const vector<int>&, const vector<int>&);
extern bool larger_perc(const Movement&, const Movement&);
extern void remove_duplicate(vector<int>&);
extern void bubble_sort(vector<int>&);

template<typename T>
int find(const vector<T>& v, const int& begin, const int& end, const T& target)
// We assume that vec has been sorted in ascending order.
// if found, return -1; otherwise, return the position of the smallest element larger than "target"
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

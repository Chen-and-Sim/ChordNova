// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// functions.cpp

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "functions.h"
using namespace std;

ofstream fout, m_fout;
double INF  =  1E9;
double MINF = -1E9;
int expansion_indexes[16][16][3432][15];
int note_pos[12] = {1, 9, 9, 3, 3, 11, 11, 5, 13, 13, 7, 7};
vector<int> omission[13];
vector<int> chord_library;
vector<vector<int>> alignment_list;

void inputY_N(char& ch)
{
	cin >> ch; 
	while(!cin || (ch != 'Y' && ch != 'y' && ch != 'N' && ch != 'n') )
	{
		cin.clear();
		cin.ignore(10, '\n');
		cout << " > Invalid input. Please try again. ";
		cin >> ch;
	}
	cin.ignore(10, '\n');
}

void inputFilename(char* str, const char* ext, bool find_file, const char* dflt)
{
	ifstream fin;
	do{
		cin.getline(str, 100, '\n');
		cin.clear();
		if(str[0] == '\0')
			strcpy(str, dflt);
			
		char* pc = strrchr(str, '.');
		if(pc == nullptr || *(pc + 1) == '\\')
		  strcat(str, ext);
		
		if(find_file)
		{
			fin.clear();
			if(fin.is_open())  fin.close();
			fin.open(str, ifstream::in);
			if(!fin)
				cout << " > The file does not exist. Please try again. ";
		}
	}  while(find_file && !fin);
	if(fin.is_open())  fin.close();
}

void inputVec(vector<int>& vec, const int& min, const int& max, bool organize)
{
	bool b;  char str[10];
	do{
		vec.clear();
		do{
			b = true;
			cin >> str;
			if(!cin)
			{
				cin.clear();
				cin.ignore(30, '\n');
				cout << " > Invalid input. Please try again.\n";
				b = false;
			}
			else
			{
				int val;
				if(str[0] >= '0' && str[0] <= '9')
					val = atoi(str);
				else  val = nametonum(str);
				if(val == -1 || val < min || val > max)
				{
					cin.clear();
					cin.ignore(30, '\n');
					cout << " > Invalid input. Please try again.\n";
					b = false;
					break;
				}
				else  vec.push_back(val);
			}
		}  while(cin.get() != '\n');
	}  while(!b);
	if(organize)
	{
		bubble_sort(vec);
		remove_duplicate(vec);
	}
}


int nametonum(char* str)
{
	int val1 = 0, val2 = 0, octave, pos;
	char str1[100], str2[100];
	strcpy(str1, str);
	strcpy(str2, str);

	{
		pos = 0;
		if(str1[0] >= 'a' && str1[0] <= 'z')
			str1[0] -= 32;
		switch(str1[pos])
		{
			case 'C':  val1 += 12;  break;
			case 'D':  val1 += 14;  break;
			case 'E':  val1 += 16;  break;
			case 'F':  val1 += 17;  break;
			case 'G':  val1 += 19;  break;
			case 'A':  val1 += 21;  break;
			case 'B':  val1 += 23;  break;
			default:   val1  = -1;
		}

		if(val1 != -1)
		{
			switch(str1[++pos])
			{
				case '#':  val1 += 1;  pos++;  break;
				case 'b':  val1 -= 1;  pos++;  break;
				default:   val1 = -1;
			}

			if(val1 != -1)
			{
				if(str1[pos] == '-')
					octave = (-1) * (str1[++pos] - 48);
				else
					octave = str1[pos] - 48;
				if(str1[pos + 1] != '\0' || str1[pos + 1] != ' ')
					val1 = -1;

				val1 += (12 * octave);
				if(val1 < 0 || val1 > 127)
					val1 = -1;
			}
		}
	}

	{
		pos = 0;
		switch(str2[pos])
		{
			case '#':  val2 += 1;  pos++;  break;
			case 'b':  val2 -= 1;  pos++;  break;
		}

		if(str2[1] >= 'a' && str2[1] <= 'z')
			str2[1] -= 32;
		switch(str2[pos])
		{
			case 'C':  val2 += 12;  break;
			case 'D':  val2 += 14;  break;
			case 'E':  val2 += 16;  break;
			case 'F':  val2 += 17;  break;
			case 'G':  val2 += 19;  break;
			case 'A':  val2 += 21;  break;
			case 'B':  val2 += 23;  break;
			default:   val2  = -1;
		}

		if(val2 != -1)
		{
			++pos;
			if(str2[pos] == '-')
				octave = (-1) * (str2[++pos] - 48);
			else
				octave = str2[pos] - 48;
			if(str2[pos + 1] != '\0' || str2[pos + 1] != ' ')
				val2 = -1;

			val2 += (12 * octave);
			if(val2 < 0 || val2 > 127)
				val2 = -1;
		}
	}

	if(val1 >= 0)  return val1;
	if(val2 >= 0)  return val2;
	return -1;
}

void ignore_path_ext(char* dest, char* source)
{
	char* pc = strrchr(source, '\\');
	if(pc == nullptr)
		pc = source;
	else  ++pc;
	int index = 0;
	while(*pc != '\0' && *pc!= '.')
	{
		dest[index] = *pc;
		++index;  ++pc;
	}
	dest[index] = '\0';
}

void next(vector<int>& v, int& id, bool first_zero)
{
	v.clear();
	int copy = (++id), index = 0;
	if(first_zero)
	{
		v.push_back(0);
		index = 1;
	}
	do{
		if(copy % 2 == 1)
			v.push_back(index);
		copy /= 2;
		++index;
	}  while(copy != 0);
}
// e.g. id = 26 = 2^1 + 2^3 + 2^4  =>  v = {0, 2, 4, 5} or {1, 3, 4}

int find_root(vector<int>& notes)
// reference: https://www.ux1.eiu.edu/~pdhesterman/old/analysis/chord_roots.html
// notes should be sorted in ascending order
{
	int interval_rank[12] = {11, 8, 6, 5, 3, 0, 10, 1, 2, 4, 7, 9}; 
	// int intervals[11] = {5, 7, 8, 4, 9, 3, 2, 10, 1, 11, 6};
	// interval_rank[interval] is the position of 'interval' in the array above
	// odd numbers represent that the lower note is the root, even numbers the opposite; -1 is a random value
	int root = *notes.rbegin();
	int t_size = notes.size();
	int best_interval = 6;
	for(int i = 0; i < t_size; ++i)
		for(int j = i + 1; j < t_size; ++j)
		{
			int interval = (notes[j] - notes[i]) % 12;
			int rank = interval_rank[interval];
			if(rank / 2 < interval_rank[best_interval] / 2)
			{
				root = notes[ (rank % 2) ? i : j ];
				best_interval = interval;
			}
		}
	return root % 12;
}

void note_set_to_id(const vector<int>& note_set, vector<int>& rec)
{
	for(int j = 0; j < 12; ++j)
	{
		int val = 0;
		for(int i = 0; i < (int)note_set.size(); ++i)
			val += (1 << ((note_set[i] + j) % 12));
		rec.push_back(val);
	}
	merge_sort(rec.begin(), rec.end(), smaller);
}

void dbentry(const char* filename)
{
	chord_library.clear();
	ifstream fin(filename, ifstream::in);
	char str[100];
	while(fin.peek() == '/' || fin.peek() == 't')
		fin.getline(str, 100, '\n');
	vector<int> note_set;
	int note;
	do{
		note_set.clear();
		do{
			fin >> note;
			note_set.push_back(note);
		}  while(!fin.eof() && fin.get() != '\n');
		
		int s_size = note_set.size();
		if(s_size == 0)  break;

		bubble_sort(note_set);
		int root = find_root(note_set);
		vector<int> omit_choice;
		for(int i = 0; i < s_size; ++i)
		{
			int diff = (note_set[i] - root) % 12;
			if(diff < 0)  diff += 12;
			if(find(omission[s_size], note_pos[diff]) == -1)
				omit_choice.push_back(note_set[i]);
		}
		bubble_sort(omit_choice);
		vector<int> indexes, subset, omitted;
		int id = -1, max_id = (1 << omit_choice.size()) - 1;
		while(id < max_id)
		{
			next(indexes, id, false);
			subset.clear();
			omitted.clear();
			for(int i = 0; i < (int)indexes.size(); ++i)
				subset.push_back( omit_choice[indexes[i]] );
			for(int i = 0; i < s_size; ++i)
			{
				if(find(subset, note_set[i]) != -1)
					omitted.push_back(note_set[i]);
			}
			if(omitted.size() != 0)
				note_set_to_id(omitted, chord_library);
		}
	}  while(!fin.eof());
	fin.close();
	remove_duplicate(chord_library);
}

void read_alignment(const char* filename)
{
	alignment_list.clear();
	ifstream fin(filename);
	char str[100];
	fin.getline(str, 100, '\n');
	fin.getline(str, 100, '\n');
	fin.getline(str, 100, '\n');
	fin.getline(str, 100, '\n');
	fin.getline(str, 100, '\n');
	vector<int> single_align;
	int num;
	do{
		single_align.clear();
		do{
			fin >> num;
			single_align.push_back(num);
		}  while(!fin.eof() && fin.get() != '\n');
		int len = single_align.size();
		if(len == 0)  break;
		for(int i = 0; i < len; ++i)
		{
			alignment_list.push_back(single_align);
			single_align.push_back(single_align[0]);
			single_align.erase(single_align.begin());
		}
	}  while(!fin.eof());
	fin.close();
}

int rand(const int& min, const int& max)
// min and max are included in the result
{
	return rand() % (max - min + 1) + min;
}

double rand(const double& min, const double& max)
{
	return (double) rand() / RAND_MAX * (max - min) + min;
}

void insert(int* ar, int* pos, int len1, int len2)
{
	for(int i = len2 - 1; i >= 0; --i)
	{
		for(int j = len1 - 1; j > pos[i]; --j)
			ar[j + 1] = ar[j];
		ar[pos[i] + 1] = pos[i];
		len1++;
	}
}
// e.g. ar = [0, 1, 2, 3], pos = [1, 1, 2]
// ar -> [0, 1, 2, 2, 3] -> [0, 1, 1, 2, 2, 3] -> [0, 1, 1, 1, 2, 2, 3]

void set_expansion_indexes()
{
	for(int min = 1; min <= 15; ++min)
		for(int max = min; max <= 15; ++max)
		{
			int diff = max - min;
			int size = comb(max - 1, min - 1);
			int index1 = 0, index2 = 0, index3 = 0;
			int pos[15];
			for(int i = 0; i < 15; ++i)
				pos[i] = 0;

			for(int i = 0; i < size; ++i)
				for(int j = 0; j < min; ++j)
					expansion_indexes[min][max][i][j] = j;

			while(index1 >= 0)
			{
				if(index2 >= min)
					index2 = pos[--index1] + 1;
				else if(index1 == diff)
				{
					int* ar = expansion_indexes[min][max][index3];
					insert(ar, pos, min, diff);
					--index1;  ++index2;  ++index3;
				}
				else
				{
					pos[index1] = index2;
					++index1;
				}
			}
		}
}

int sign(const int& n)
{
	if(n > 0) return 1;
	if(n < 0) return -1;
	return 0;
}

int comb(const int& n, const int& m)
{
	int res = 1;
	for(int i = 1; i <= m; ++i)
		res = res * (n + 1 - i) / i;
	return res;
}

void fprint(const char* begin, const vector<int>& v, const char* sep, const char* end)
{
	int size = v.size();
	fout << begin << "[";
	if(size != 0)  fout << v[0];
	for(int i = 1; i < size; ++i)
		fout << sep << v[i];
	fout << "]" << end;
}

void cprint(const char* begin, const vector<int>& v, const char* sep, const char* end)
{
	int size = v.size();
	cout << begin << "[";
	if(size != 0)  cout << v[0];
	for(int i = 1; i < size; ++i)
		cout << sep << v[i];
	cout << "]" << end;
}

vector<int> intersect(vector<int>& A, vector<int>& B, bool regular)
{
	if(!regular)
	{
		bubble_sort(A);
		bubble_sort(B);
		remove_duplicate(A);
		remove_duplicate(B);
	}

	int A_size = A.size(), B_size = B.size();
	vector<int> result;
	int i = 0, j = 0;
	while(i < A_size && j < B_size)
	{
		if(A[i] > B[j])  ++j;
		else if(A[i] < B[j]) ++i;
		else
		{
			result.push_back(A[i]);
			++i;  ++j; 
		} 
	}
	return result;
}

vector<int> get_union(const vector<int>& A, const vector<int>& B)
{
	vector<int> result(A);
	result.insert(result.end(), B.begin(), B.end());
	bubble_sort(result);
	remove_duplicate(result);
	return result;
}

vector<int> get_complement(const vector<int>& A, const vector<int>& B)
// We assume that sets U and A are sorted in ascending order; the result is A\B.
{
	int A_size = A.size(), B_size = B.size();
	vector<int> result;
	int i = 0, j = 0;
	while(i < A_size && j < B_size)
	{
		while(i < A_size && j < B_size && A[i] >  B[j])  ++j;
		while(i < A_size && j < B_size && A[i] <= B[j])
		{
			if(A[i] < B[j])
				result.push_back(A[i]);
			++i;
		}
	}
	for(int k = i; k < A_size; ++k)
		result.push_back(A[k]);
	return result;
}

int swapInt(const int& value, const int& len)
{
	switch(len)
	{
		case 1:  return value;
		case 2:  return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
		case 3:  return ((value & 0x0000FF) << 16) | (value & 0x00FF00) | ((value & 0xFF0000) >> 16);
		case 4:  return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) |
         				 ((value & 0x00FF0000) >> 8)  | ((value & 0xFF000000) >> 24);
		default:  exit(-1);
	}
}

int to_VLQ(int value)
{
	int count = 0, result = 0;
	while(value != 0)
	{
		result += ((value & 0x7F) << (8 * count));
		if(count != 0)
			result += (1 << ((8 * (count + 1)) - 1) );
		value >>= 7;
		++count;
	}
	return swapInt(result, count);
}

void midi_head(const int& chord_count, const int& note_count)
{
	char str1[19] = "\x4D\x54\x68\x64\x00\x00\x00\x06\x00\x00\x00\x01\x01\xE0\x4D\x54\x72\x6B";
	m_fout.write(str1, 18);
	int _len = swapInt(8 * note_count + chord_count + 74);
	m_fout.write((char*)&_len, sizeof(int));
	char str2[71] = { "\x00\xFF\x02\x21\x28\x63\x29\x20\x32\x30\x32\x30\x20\x57\x65\x6E\x67\x65\x20\x43\x68\x65\x6E"
							"\x2C\x20\x4A\x69\x2D\x77\x6F\x6F\x6E\x20\x53\x69\x6D\x2E\x00\xFF\x04\x05\x50\x69\x61\x6E\x6F"
							"\x00\xFF\x51\x03\x0F\x42\x40\x00\xFF\x58\x04\x04\x02\x18\x08\x00\xFF\x59\x02\x00\x00\x00\xC0\x00"};
	m_fout.write(str2, 70);
}

void chord_to_midi(vector<int>& notes, int beat)
{
	int size = notes.size();
	for(int i = 0; i < size; ++i)
	{
		m_fout.write("\x00\x90", 2);
		m_fout.put((char)notes[i]);
		m_fout.put('\x50');
	}
	
	for(int i = 0; i < size; ++i)
	{
		if(i == 0)
		{
			int vlq = to_VLQ(beat * 480);
			int len = (beat > 34) ? 3 : 2;
			m_fout.write((char*)&vlq, len);
			m_fout.put('\x80');
		}
		else  m_fout.write("\x00\x80", 2);
		m_fout.put((char)notes[i]);
		m_fout.put('\x40');
	}
}


bool smaller(const int& num1, const int& num2)
{ return num1 < num2; }

bool smallerVec(const vector<int>& v1, const vector<int>& v2)
{
	int size1 = v1.size(), size2 = v2.size();
	if(size1 < size2)  return true;
	if(size1 > size2)  return false;
	for(int i = 0; i < size1; ++i)
	{
		if(v1[i] < v2[i])  return true;
		if(v1[i] > v2[i])  return false;
	}
	return true;
}

bool larger_perc(const Movement& movement1, const Movement& movement2)
{ return movement1.percentage > movement2.percentage; }

void bubble_sort(vector<int>& vec)
{
	int len = vec.size();
	int temp;
	for(int i = 0; i < len; ++i)
		for(int j = len - 1; j > i; --j)
			if(vec[j - 1] > vec[j])
			{
				temp = vec[j];
				vec[j] = vec[j - 1];
				vec[j - 1] = temp;
			}
}

void remove_duplicate(vector<int>& vec)
// We assume that vec has been sorted.
{
	int size = vec.size();
	for(int i = size - 1; i > 0; --i)
		if(vec[i] == vec[i - 1])
			vec.erase(vec.begin() + i);
}

// SmartChordGen-utility-WriteAlignmentDatabase v1.5 [Build: 2020.7.2]
// (c) 2020 Wenge Chen, Ji-woon Sim.

#include <fstream>
#include <iostream>
#include <vector>
#include "../../main/functions.h"
#include "../../main/functions.cpp"

using namespace std;
vector<vector<int>> rec1, rec2;

void simple_mode(int low, int high)
{
	if(low == 1)
	{
		vector<int> v = {1};
		rec1.push_back(v);
		low = 3;
	}
	if(high == 1)  return;
	
	int index_low = (low - 3) / 2, index_high = (high - 3) / 2;
	int id = 0, len;
	vector<int> vec1, vec2, indexes;
	
	while(id < 63)
	{
		vec1.clear();
		next(indexes, id, false);
		if(*indexes.rbegin() < index_low || *indexes.rbegin() > index_high)
			continue;
		for(int i = 0; i < indexes.size(); ++i)
			vec1.push_back(indexes[i] * 2 + 3);
		
		vector<vector<int>> temp[6];
		temp[0].push_back( {vec1[0]} );
		for(len = 1; len < vec1.size(); ++len)
		{
			for(int i = 0; i < temp[len - 1].size(); ++i)
			{
				for(int j = 0; j <= temp[len - 1][i].size(); ++j)
				{
					vec2.assign(temp[len - 1][i].begin(), temp[len - 1][i].end());
					vec2.insert(vec2.begin() + j, vec1[len]);
					temp[len].push_back(vec2);
				}
			}
		}

		for(int i = 0; i < temp[len - 1].size(); ++i)
		{
			temp[len - 1][i].insert(temp[len - 1][i].begin(), 1);
			rec1.push_back(temp[len - 1][i]);
		}
	}
}

void no_omission()
{
	vector<int> avail = {1, 3, 5, 7, 9, 11, 13};
	for(int i = 0; i < rec1.size(); ++i)
	{
		int len = rec1[i].size();
		vector<int> temp(rec1[i]);
		bubble_sort(temp);
		if(*temp.rbegin() == avail[len - 1])
			rec2.push_back(rec1[i]);
	}
}

void omission_()
{
	cout << "\n > Please input omission condition in a list of 'a-b', press Enter for no omission: ";
	char str1[100], str2[100], ch = cin.peek();
	vector<vector<int>> list;
	int pos1 = 0, pos2;
	if(ch != '\n')
	{
		cin.getline(str1, 100);
		int len = strlen(str1);
		while(pos1 < len)
		{
			vector<int> temp = {0, 0};
			while(str1[pos1] == ' ')
				++pos1;
			if(pos1 >= len)  break;
			
			pos2 = 0;
			while(pos1 < len && str1[pos1] != '-')
			{
				str2[pos2] = str1[pos1];
				++pos1;  ++pos2;
			}
			str2[pos2] = '\0';
			temp[0] = atoi(str2);
			
			++pos1;  pos2 = 0;
			while(pos1 < len && str1[pos1] != ' ')
			{
				str2[pos2] = str1[pos1];
				++pos1;  ++pos2;
			}
			str2[pos2] = '\0';
			temp[1] = atoi(str2);
			list.push_back(temp);
		}
	}
	else
	{
		cin.get();
		return;
	}
	
	for(int pos = rec1.size() - 1; pos >= 0; --pos)
	{
		vector<int> temp(rec1[pos]);
		for(int i = 0; i < list.size(); ++i)
		{
			if(intersect(temp, list[i]).size() == 2)
			{
				rec1.erase(rec1.begin() + pos);
				break;
			}
		}
	}
}

void leap()
{
	cout << "\n > Please input desired adjacent leaps in 'min max', press Enter for '0 0': ";
	char ch = cin.peek();
	if(ch != '\n')
	{
		int min, max;
		inputNums(min, max, 0, 10);
		for(int pos = rec1.size() - 1; pos >= 0; --pos)
		{
			for(int i = 1; i < rec1[pos].size(); ++i)
			{
				int interval = abs(rec1[pos][i] - rec1[pos][i - 1]) / 2 - 1;
				if(interval < min || interval > max)
				{
					rec1.erase(rec1.begin() + pos);
					break;
				}
			}
		}
	}
	else
	{
		cin.get();
		return;
	}
}

void repetition()
{
	cout << "\n > Please input desired repetition in a list of 'x-y-z', press Enter for no repititions: ";
	char str1[100], str2[100], ch = cin.peek();
	vector<int> note, interval, count;
	int pos1 = 0, pos2;
	if(ch != '\n')
	{
		cin.getline(str1, 100);
		int len = strlen(str1);
		while(pos1 < len)
		{
			while(str1[pos1] == ' ')
				++pos1;
			if(pos1 >= len)  break;
			
			pos2 = 0;
			while(pos1 < len && str1[pos1] != '-')
			{
				str2[pos2] = str1[pos1];
				++pos1;  ++pos2;
			}
			str2[pos2] = '\0';
			note.push_back(atoi(str2));
			
			++pos1;  pos2 = 0;
			while(pos1 < len && str1[pos1] != '-')
			{
				str2[pos2] = str1[pos1];
				++pos1;  ++pos2;
			}
			str2[pos2] = '\0';
			interval.push_back(atoi(str2));
			
			++pos1;  pos2 = 0;
			while(pos1 < len && str1[pos1] != ' ')
			{
				str2[pos2] = str1[pos1];
				++pos1;  ++pos2;
			}
			str2[pos2] = '\0';
			count.push_back(atoi(str2));
		}
	}
	else
	{
		cin.get();
		rec2.assign(rec1.begin(), rec1.end());
		return;
	}

	cout << "\n > Always keep repetitions higher than the note (Y / N)? ";
	inputY_N(ch);
	bool higher = true;
	if(ch == 'N' || ch == 'n')
		higher = false;

	for(int i = 0; i < rec1.size(); ++i)
	{
		for(int j = 0; j < note.size(); ++j)
		{
			bool found = false;
			int pos;
			for(int k = 0; k < rec1[i].size(); ++k)
			{
				if(rec1[i][k] == note[j])
				{
					found = true;
					pos = k;
					break;
				}
			}
			
			if(found)
			{
				vector<vector<int>> rec_indexes;
				int min_index = (higher ? (pos + 1) : (pos - interval[j]));
				if(min_index < 1)   min_index = 1;
				if(min_index == pos)  ++min_index;
				int max_index = pos + interval[j] + 1;
				if(max_index > rec1[i].size())
					max_index = rec1[i].size();

				for(int len = 1; len <= count[j]; ++len)
				{
					int depth = 0, index = min_index;
					vector<int> indexes(len, (const int)MINF);
					while(depth >= 0)
					{
						if(depth == len)
						{
							rec_indexes.push_back(indexes);
							index = indexes[--depth] + 1;
							if(index == pos)  ++index;
						}
						else if(index > max_index)
						{
							index = indexes[--depth] + 1;
							if(index == pos)  ++index;
						}
						else
							indexes[depth++] = index;
					}
				}

				for(int k = 0; k < rec_indexes.size(); ++k)
				{
					vector<int> temp(rec1[i]);
					for(int l = rec_indexes[k].size() - 1; l >= 0; --l)
						temp.insert(temp.begin() + rec_indexes[k][l], rec1[i][pos]);
					rec2.push_back(temp);
				}
			}
		}
	}
}

void to_text()
{
	for(int i = 0; i < rec2.size(); ++i)
	{
		for(int j = 0; j < rec2[i].size() - 1; ++j)
			fout << rec2[i][j] << " ";
		fout << *rec2[i].rbegin() << "\n";
	}
	fout.close();
}


int main()
{
	cout << "[[  SmartChordGen v1.5 [Build: 2020.7.2]  ]]\n"
		  << "[[  (c) 2020 Wenge Chen, Ji-woon Sim.     ]]\n\n"
		  << " > Utility - Write Alignment Database:\n";
	
	cout << " > Please assign a name for the alignment database (.db): ";
	char _namedb[100] = "\0", namedb[100];
	inputFilename(_namedb, ".db", false);
	ignore_path_ext(namedb, _namedb);
	
	cout << "\n > Please input the name of the author of the database (.db): ";
	char nameauthor[100];
	cin.getline(nameauthor, 100, '\n');
	cin.clear();
	
	vector<int> avail = {1, 3, 5, 7, 9, 11, 13};
	int high, low;
	cout << "\n > Please input desired top note of the alignment (available: 1 3 5 7 9 11 13): ";
	while(true)
	{
		inputNum(high, 1, 13);
		if(find(avail, high) == -1)
			break;
		else  cout << " > Invalid input. Please try again.\n";
	}
	
	cout << "\n > Please input desired low-cutoff note (available: 1 3 5 7 9 11 13): ";
	while(true)
	{
		inputNum(low, 1, high);
		if(find(avail, low) == -1)
			break;
		else  cout << " > Invalid input. Please try again.\n";
	}

	simple_mode(low, high);
	
	cout << "\n > Would you like to specify more details about the alignment (Y / N)? ";
	char ch;  inputY_N(ch);
	if(ch == 'Y' || ch == 'y')
	{
		omission_();
		leap();
		repetition();
	}
	else
	{
		cout << "\n > Would you allow omission of notes in the alignment (Y / N)? ";
		inputY_N(ch);
		if(ch == 'N' || ch == 'n')
			no_omission();
		else
			rec2.assign(rec1.begin(), rec1.end());
	}

	fout.open(_namedb, ios::trunc);
	fout << "// SmartChordGen Chord Database\n"
		  << "// (c) 2020 " << nameauthor << ".\n"
		  << "// Database begins below.\n"
		  << "title_en = " << namedb << endl
		  << "title_ch = " << namedb << endl;
	
	merge_sort(rec2.begin(), rec2.end(), smallerVec);
	for(int i = rec2.size() - 1; i > 0; --i)
		if(rec2[i] == rec2[i - 1])
			rec2.erase(rec2.begin() + i);
	to_text();
	
	cout << "\n > Output finished. Now you can close the program.\n\n";
	system("pause"); 
	return 0;
}


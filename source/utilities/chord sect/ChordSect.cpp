// SmartChordGen-utility-ChordSect v1.5 [Build: 2020.7.2]
// (c) 2020 Wenge Chen, Ji-woon Sim.

#include <fstream>
#include <iostream>
#include <vector>

#include "../../main/functions.h"
#include "../../main/functions.cpp"
using namespace std;

int main()
{
	cout << "[[  SmartChordGen v1.5 [Build: 2020.7.2]  ]]\n"
		  << "[[  (c) 2020 Wenge Chen, Ji-woon Sim.     ]]\n\n"
		  << " > Utility - Chord sect:\n";
	
	cout << " > Please input the name of the chord data file (the default extension is '.txt'): ";
	char input[100] = "\0";
	inputFilename(input, ".txt", true);
	
	cout << "\n > Would you like to intersect or union adjacent chords (Y - intersect, N = union)? ";
	char ch;  inputY_N(ch);
	
	cout << "\n > Please assign a name for the output file (the default extension is '.txt'): ";
	char output[100] = "\0";
	inputFilename(output, ".txt", false);
	
	ifstream fin(input, ios::in);
	fout.open(output, ios::trunc);
	vector<int> notes[2], result;
	int count = 0, note;

	do{
		notes[count % 2].clear();
		do{
			fin >> note;
			notes[count % 2].push_back(note);
		}  while(!fin.eof() && fin.get() != '\n');
		if(notes[count % 2].size() == 0)  break;

		++count;
		if(count == 1)  continue;
		if(ch == 'Y' || ch == 'y')
			result = intersect(notes[0], notes[1]);
		else
			result = get_union(notes[0], notes[1]);

		for(int i = 0; i < result.size(); ++i)
			fout << result[i] << ' ';
		fout << endl;
	}  while(!fin.eof());

	cout << "\n > Output finished. Now you can close the program.\n\n";
	fin.close();
	fout.close();
	system("pause"); 
	return 0;
}


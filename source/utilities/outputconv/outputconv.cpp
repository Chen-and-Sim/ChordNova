// SmartChordGen-utility-OutputConversion v1.5 [Build: 2020.7.2]
// (c) 2020 Wenge Chen, Ji-woon Sim.

#include <fstream>
#include <iostream>
#include <vector>

#include "..\..\main\source\functions.h"
#include "..\..\main\source\functions.cpp"
using namespace std;

int main()
{
	cout << "[[  SmartChordGen v1.5 [Build: 2020.7.2]  ]]\n"
		  << "[[  (c) 2020 Wenge Chen, Ji-woon Sim.     ]]\n\n"
		  << " > Utility - Output Conversion:\n";
	
	cout << " > Please input the name of the chord data file you would like to convert (the default extension is '.txt): ";
	char input[100] = "\0";
	inputFilename(input, ".txt", true);
	ifstream fin(input, ios::in);
	
	cout << "\n > Please assign a name for the output file (the default extension is '.txt'): ";
	char output[100] = "\0";
	inputFilename(output, ".txt", false);
	fout.open(output, ios::trunc);

	char str[100] = "0";
	int note, count = 0;
	fin.getline(str, 100, '[');
	do{
		fin >> note;
		if(count != 0)  fout << ' ';
		fout << note;
		++count;
	}  while(fin.get() != ']');
	fout << endl;

	while(true)
	{
		do{
			fin.getline(str, 100, '\n');
		}  while(fin && !fin.eof() && fin.peek() != '-');
		if(!fin || fin.eof())  break;
		fin >> str;
		fin.get();  fin.get();
		count = 0;
		do{
			fin >> note;
			if(count != 0)  fout << ' ';
			fout << note;
			++count;
		}  while(fin.get() != ']');
		fout << endl;
	}

	cout << "\n > Output finished. Now you can close the program.\n\n";
	fout.close();
	system("pause"); 
	return 0;
}


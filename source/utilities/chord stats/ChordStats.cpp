// SmartChordGen-utility-ChordStats v1.5 [Build: 2020.7.2]
// (c) 2020 Wenge Chen, Ji-woon Sim.

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "..\..\main\source\chord.h"
#include "..\..\main\source\chord.cpp"
#include "..\..\main\source\chorddata.h"
#include "..\..\main\source\chorddata.cpp"
#include "..\..\main\source\functions.h"
#include "..\..\main\source\functions.cpp"

using namespace std;

int main()
{
	cout << "[[  SmartChordGen v1.5 [Build: 2020.7.2]  ]]\n"
		  << "[[  (c) 2020 Wenge Chen, Ji-woon Sim.     ]]\n\n"
		  << " > Utility - Chord stats:\n";
	
	cout << " > Please input the name of the chord data file (the default extension is '.txt'): ";
	char input[100] = "\0";
	inputFilename(input, ".txt", true);
	
	cout << "\n > Please input the max voice leading distance (Press Enter for default: 4): ";
	int vl_max;
	inputNum(vl_max, 0, 20, 4);
	
	cout << "\n > Results:\n" << " > Voice leading stats:\n" << " > Movement instances: ";
	vector<Movement> movement;
	movement.resize(2 * vl_max + 1);
	for(int i = 0; i < 2 * vl_max + 1; ++i)
		movement[i].amount = i - vl_max;

	ifstream fin(input, ios::in);
	vector<int> notes;
	int note, count = 0, cardinal_change = 0;
	vector<Chord> rec;

	do{
		notes.clear();
		do{
			fin >> note;
			notes.push_back(note);
		}  while(fin && !fin.eof() && fin.get() != '\n');
		if(!fin || notes.size() == 0)  break;

		Chord A(notes);
		rec.push_back(A);
		++count;
		if(count == 1)  continue;

		rec.rbegin() -> _set_vl_max(vl_max);
		(rec.rbegin() + 1) -> _set_vl_max(vl_max);
		(rec.rbegin() + 1) -> find_vec(*rec.rbegin());
		for(int i = 0; i < rec.rbegin() -> get_vec().size(); ++i)
		{
			int num = rec.rbegin() -> get_vec()[i];
			if(abs(num) > vl_max)
			{
				cerr << "\n\n > ERROR: chord ";
				cprint("\0", (rec.rbegin() + 1) -> get_notes(), ", ", "\0");
				cerr << " (@ #" << count - 1 << ") and/or chord ";
				cprint("\0", rec.rbegin() -> get_notes(), ", ", "\0");
				cerr << " (@ #" << count << ") do(es) not meet the requirements.\n\n";
				fout.close();
				system("pause"); 
				exit(0);
			}
			else
				++movement[num + vl_max].instance;
		}

		if(rec.rbegin() -> get_s_size() != (rec.rbegin() + 1) -> get_s_size())
			++cardinal_change;
	}  while(!fin.eof());
	fin.close();

	cout << "[0]  " << setw(5) << movement[vl_max].instance << endl << setw(25) << setfill(' ');
	for(int i = 1; i <= vl_max; ++i)
		cout << "[+" << i << "] " << setw(5) << movement[i + vl_max].instance << "  ";
	cout << endl << setw(24) << setfill(' ');
	for(int i = -1; i >= -vl_max; --i)
		cout << "["  << i << "] " << setw(5) << movement[i + vl_max].instance << "  ";

	int movement_instance = 0;
	for(int i = 0; i < 2 * vl_max + 1; ++i)
		movement_instance += movement[i].instance;
	for(int i = 0; i < 2 * vl_max + 1; ++i)
		movement[i].percentage = (double)movement[i].instance / movement_instance;

	vector<Movement> sorted;
	sorted.assign(movement.begin(), movement.end());
	merge_sort(sorted.begin(), sorted.end(), *larger_perc);
	cout << "\n > Movement percentage (sorted H -> L):\n   ";
	for(int i = 0; i < 2 * vl_max + 1; ++i)
	{
		cout << "[";
		if(sorted[i].amount > 0)  cout << "+";
		cout << sorted[i].amount << "] " << (int)(sorted[i].percentage * 100 + 0.5) << "%  ";
	}
	
	cout << "\n > Cardinal change instances: " << cardinal_change << ", "
		  << (int)((double)cardinal_change / (count - 1) * 100.0 + 0.5) << "%\n\n";
	
	int n_max = 0, n_min = 15,  n_max_index = 0, n_min_index = 0;
	double t_max = 0.0,  t_min = INF, k_max = 0.0, k_min = INF, g_max = 0.0, g_min = INF;
	int t_max_index = 0, t_min_index = 0, k_max_index = 0, k_min_index = 0, g_max_index = 0, g_min_index = 0;
	int h_max = 0, h_min = 50,  h_max_index = 0, h_min_index = 0;
	int c_max = 0, c_min = 15,  c_max_index = 0, c_min_index = 0;
	int r_max = 0, r_min = 11,  r_max_index = 0, r_min_index = 0;
	int s_max = 0, s_min = 100, s_max_index = 0, s_min_index = 0;
	int x_max = 0, x_min = 100, x_max_index = 0, x_min_index = 0;
	int temp1;  double temp2;

	for(int i = 0; i < count; ++i)
	{
		temp1 = rec[i].get_s_size();
		if(temp1 > n_max)  { n_max = temp1;  n_max_index = i; }
		if(temp1 < n_min)  { n_min = temp1;  n_min_index = i; }

		temp2 = rec[i].get_tension();
		if(temp2 > t_max)  { t_max = temp2;  t_max_index = i; }
		if(temp2 < t_min)  { t_min = temp2;  t_min_index = i; }

		temp2 = (rec[i].get_chroma() >= 0) ? rec[i].get_chroma() : (-rec[i].get_chroma());
		if(temp2 > k_max)  { k_max = temp2;  k_max_index = i; }
		if(temp2 < k_min)  { k_min = temp2;  k_min_index = i; }
		
		temp1 = rec[i].get_thickness();
		if(temp1 > h_max)  { h_max = temp1;  h_max_index = i; }
		if(temp1 < h_min)  { h_min = temp1;  h_min_index = i; }

		if(i != 0)
		{
			temp1 = intersect(rec[i].get_notes(), rec[i - 1].get_notes(), true).size();
			if(temp1 > c_max)  { c_max = temp1;  c_max_index = i; }
			if(temp1 < c_min)  { c_min = temp1;  c_min_index = i; }
		}
		
		temp1 = rec[i].get_root();
		if(temp1 > r_max)  { r_max = temp1;  r_max_index = i; }
		if(temp1 < r_min)  { r_min = temp1;  r_min_index = i; }

		temp2 = rec[i].get_g_center();
		if(temp2 > g_max)  { g_max = temp2;  g_max_index = i; }
		if(temp2 < g_min)  { g_min = temp2;  g_min_index = i; }
		
		if(i != 0)
		{
			temp1 = rec[i].get_sv();
			if(temp1 > s_max)  { s_max = temp1;  s_max_index = i; }
			if(temp1 < s_min)  { s_min = temp1;  s_min_index = i; }
		}
		
		if(i != 0)
		{
			temp1 = rec[i].get_similarity();
			if(temp1 > x_max)  { x_max = temp1;  x_max_index = i; }
			if(temp1 < x_min)  { x_min = temp1;  x_min_index = i; }
		}
	}

	cout << " > Other stats:\n" << setw(28) << left << " > Cardinal (n): "
		  << "highest - " << setw(6) << left << n_max << "(@ #" << setw(5) << left << n_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << n_min << "(@ #" << setw(5) << left << n_min_index + 1 << ");\n";
	cout << setw(28) << left << " > Tension (t): "
		  << "highest - " << setw(6) << left << t_max << "(@ #" << setw(5) << left << t_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << t_min << "(@ #" << setw(5) << left << t_min_index + 1 << ");\n";
	cout << setw(28) << left << " > Chroma (|k|): "
		  << "highest - " << setw(6) << left << k_max << "(@ #" << setw(5) << left << k_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << k_min << "(@ #" << setw(5) << left << k_min_index + 1 << ");\n";
	cout << setw(28) << left << " > Thickness (h): "
		  << "highest - " << setw(6) << left << h_max << "(@ #" << setw(5) << left << h_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << h_min << "(@ #" << setw(5) << left << h_min_index + 1 << ");\n";
	cout << setw(28) << left << " > Common notes (c): "
		  << "most    - " << setw(6) << left << c_max << "(@ #" << setw(5) << left << c_max_index + 1 << "); "
		  << "least  - "  << setw(6) << left << c_min << "(@ #" << setw(5) << left << c_min_index + 1 << ").\n";
	cout << setw(28) << left << " > Root (r): "
		  << "highest - " << setw(6) << left << r_max << "(@ #" << setw(5) << left << r_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << r_min << "(@ #" << setw(5) << left << r_min_index + 1 << ").\n";
	cout << setw(28) << left << " > Geomertic center (g): "
		  << "highest - " << setw(6) << left << g_max << "(@ #" << setw(5) << left << g_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << g_min << "(@ #" << setw(5) << left << g_min_index + 1 << ").\n";
	cout << setw(28) << left << " > Total voice leading (s): "
		  << "highest - " << setw(6) << left << s_max << "(@ #" << setw(5) << left << s_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << s_min << "(@ #" << setw(5) << left << s_min_index + 1 << ").\n";
	cout << setw(28) << left << " > Similarity (x%): "
		  << "highest - " << setw(6) << left << x_max << "(@ #" << setw(5) << left << x_max_index + 1 << "); "
		  << "lowest - "  << setw(6) << left << x_min << "(@ #" << setw(5) << left << x_min_index + 1 << ").\n";

	cout << "\n > Copy the stats to a file (Y / N)? ";
	char ch;  inputY_N(ch);
	if(ch == 'Y' || ch == 'y')
	{
		cout << "\n > Please assign a name for the output file (the default extension is \".txt\"): ";
		char output[100] = "\0";
		inputFilename(output, ".txt", false);

		fout.open(output, ios::trunc);
		fout << "Voice leading stats:\n";
		fout << "Movement instances: " << "[0]  " << movement[vl_max].instance << endl;
		for(int i = 1; i <= vl_max; ++i)
			fout << "[+" << i << "] " << movement[i + vl_max].instance << "  ";
		fout << endl;
		for(int i = -1; i >= -vl_max; --i)
			fout << "[" << i << "] " << movement[i + vl_max].instance << "  ";
		
		fout << "\nMovement percentage (sorted H -> L):\n";
		for(int i = 0; i < 2 * vl_max + 1; ++i)
		{
			fout << "[";
			if(sorted[i].amount > 0)  fout << "+";
			fout << sorted[i].amount << "] " << (int)(sorted[i].percentage * 100 + 0.5) << "%  ";
		}
		fout << "\nCardinal change instances: " << cardinal_change << ", "
			  << (int)((double)cardinal_change / (count - 1) * 100.0 + 0.5) << "%\n\n";

		fout << "Other stats:\n" << "Cardinal (n): "
			  << "highest - " << n_max << "(@ #" << n_max_index + 1 << "); "
			  << "lowest - "  << n_min << "(@ #" << n_min_index + 1 << ");\n";
		fout << "Tension (t): "
			  << "highest - " << t_max << "(@ #" << t_max_index + 1 << "); "
			  << "lowest - "  << t_min << "(@ #" << t_min_index + 1 << ");\n";
		fout << "Chroma (|k|): "
			  << "highest - " << k_max << "(@ #" << k_max_index + 1 << "); "
			  << "lowest - "  << k_min << "(@ #" << k_min_index + 1 << ");\n";
		fout << "Thickness (h): "
			  << "highest - " << h_max << "(@ #" << h_max_index + 1 << "); "
			  << "lowest - "  << h_min << "(@ #" << h_min_index + 1 << ");\n";
		fout << "Common notes (c): "
			  << "most - "    << c_max << "(@ #" << c_max_index + 1 << "); "
			  << "least - "   << c_min << "(@ #" << c_min_index + 1 << ").\n";
		fout << "Root (r): "
			  << "highest - " << r_max << "(@ #" << r_max_index + 1 << "); "
			  << "lowest - "  << r_min << "(@ #" << r_min_index + 1 << ").\n";
		fout << "Geomertic center (g): "
			  << "highest - " << g_max << "(@ #" << g_max_index + 1 << "); "
			  << "lowest - "  << g_min << "(@ #" << g_min_index + 1 << ").\n";
		fout << "Total voice leading (s): "
			  << "highest - " << s_max << "(@ #" << s_max_index + 1 << "); "
			  << "lowest - "  << s_min << "(@ #" << s_min_index + 1 << ").\n";
		fout << "Similarity (x%): "
			  << "highest - " << x_max << "(@ #" << x_max_index + 1 << "); "
			  << "lowest - "  << x_min << "(@ #" << x_min_index + 1 << ").\n";
	}

	cout << "\n > Output finished. Now you can close the program.\n\n";
	fout.close();
	system("pause"); 
	return 0;
}


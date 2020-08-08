// SmartChordGen-utility-ChordStats v1.5 [Build: 2020.7.2]
// (c) 2020 Wenge Chen, Ji-woon Sim.

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../../main/chord.h"
#include "../../main/chord.cpp"
#include "../../main/chorddata.h"
#include "../../main/chorddata.cpp"
#include "../../main/functions.h"
#include "../../main/functions.cpp"

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
	
	cout << "\n > Output language: English (Y) / Chinese (N): ";
	char ch;  inputY_N(ch);
	Language language;
	if(ch == 'Y' || ch == 'y')
		language = English;
	else  language = Chinese;
	
	if(language == English)
		cout << "\n > Results:\n" << " > Voice leading stats:\n" << " > Movement instances: ";
	else  cout << "\n > 结果：\n" << " > 声部进行统计：\n" << " > 声部动向及相应出现次数：";
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
				if(language == English)
					cerr << "\n\n > ERROR: chord ";
				else  cerr << "\n\n > 错误：和弦";
				cprint("\0", (rec.rbegin() + 1) -> get_notes(), ", ", "\0");
				if(language == English)
					cerr << " (@ #" << count - 1 << ") and/or chord ";
				else  cerr << " (@ #" << count - 1 << ") 与/或和弦";
				cprint("\0", rec.rbegin() -> get_notes(), ", ", "\0");
				if(language == English)
					cerr << " (@ #" << count << ") do(es) not meet the requirements.\n\n";
				else  cerr << " (@ #" << count << ") 不符合要求。\n\n";
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

	int width1 = ( (language == English) ? 24 : 28 );
	cout << "[0]  " << setw(5) << movement[vl_max].instance << endl << setw(width1 + 1) << setfill(' ');
	for(int i = 1; i <= vl_max; ++i)
		cout << "[+" << i << "] " << setw(5) << movement[i + vl_max].instance << "  ";
	cout << endl << setw(width1) << setfill(' ');
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
	if(language == English)
		cout << "\n > Movement percentage (sorted H -> L):\n   ";
	else  cout << "\n > 声部动向频次占比（从高到低）：\n   ";
	for(int i = 0; i < 2 * vl_max + 1; ++i)
	{
		cout << "[";
		if(sorted[i].amount > 0)  cout << "+";
		cout << sorted[i].amount << "] " << (int)(sorted[i].percentage * 100 + 0.5) << "%  ";
	}
	
	if(language == English)
		cout << "\n > Cardinal change instances: ";
	else  cout << "\n > 和弦音个数 (N) 发生变化的次数：";
	cout << cardinal_change << " (" << (int)((double)cardinal_change / (count - 1) * 100.0 + 0.5) << "%)\n\n";
	
	int n_max = 0, n_min = 15,  n_sum = 0, n_max_index = 0, n_min_index = 0;
	double t_max = 0.0, t_min = INF, k_max = 0.0, k_min = INF, t_sum = 0, k_sum = 0;
	double g_max = 0.0, g_min = INF, h_max = 0.0, h_min = 50,  g_sum = 0, h_sum = 0;
	int t_max_index = 0,  t_min_index = 0, k_max_index = 0, k_min_index = 0;
	int g_max_index = 0,  g_min_index = 0, h_max_index = 0, h_min_index = 0;
	int c_max = 0, c_min = 15,  c_sum = 0, c_max_index = 0, c_min_index = 0;
	int r_max = 0, r_min = 11,  r_sum = 0, r_max_index = 0, r_min_index = 0;
	int s_max = 0, s_min = 100, s_sum = 0, s_max_index = 0, s_min_index = 0;
	int x_max = 0, x_min = 100, x_sum = 0, x_max_index = 0, x_min_index = 0;
	int temp1;  double temp2;

	for(int i = 0; i < count; ++i)
	{
		temp1 = rec[i].get_s_size();
		if(temp1 > n_max)  { n_max = temp1;  n_max_index = i; }
		if(temp1 < n_min)  { n_min = temp1;  n_min_index = i; }
		n_sum += temp1;

		temp2 = rec[i].get_tension();
		if(temp2 > t_max)  { t_max = temp2;  t_max_index = i; }
		if(temp2 < t_min)  { t_min = temp2;  t_min_index = i; }
		t_sum += temp2;

		temp2 = (rec[i].get_chroma() >= 0) ? rec[i].get_chroma() : (-rec[i].get_chroma());
		if(temp2 > k_max)  { k_max = temp2;  k_max_index = i; }
		if(temp2 < k_min)  { k_min = temp2;  k_min_index = i; }
		k_sum += temp2;
		
		temp2 = rec[i].get_thickness();
		if(temp2 > h_max)  { h_max = temp2;  h_max_index = i; }
		if(temp2 < h_min)  { h_min = temp2;  h_min_index = i; }
		h_sum += temp1;

		if(i != 0)
		{
			temp1 = intersect(rec[i].get_notes(), rec[i - 1].get_notes(), true).size();
			if(temp1 > c_max)  { c_max = temp1;  c_max_index = i; }
			if(temp1 < c_min)  { c_min = temp1;  c_min_index = i; }
			c_sum += temp1;
		}
		
		temp1 = rec[i].get_root();
		if(temp1 > r_max)  { r_max = temp1;  r_max_index = i; }
		if(temp1 < r_min)  { r_min = temp1;  r_min_index = i; }
		r_sum += temp1;

		temp2 = rec[i].get_g_center();
		if(temp2 > g_max)  { g_max = temp2;  g_max_index = i; }
		if(temp2 < g_min)  { g_min = temp2;  g_min_index = i; }
		g_sum += temp2;
		
		if(i != 0)
		{
			temp1 = rec[i].get_sv();
			if(temp1 > s_max)  { s_max = temp1;  s_max_index = i; }
			if(temp1 < s_min)  { s_min = temp1;  s_min_index = i; }
			s_sum += temp1;
		}
		
		if(i != 0)
		{
			temp1 = rec[i].get_similarity();
			if(temp1 > x_max)  { x_max = temp1;  x_max_index = i; }
			if(temp1 < x_min)  { x_min = temp1;  x_min_index = i; }
			x_sum += temp1;
		}
	}

	int width2 = ( (language == English) ? 35 : 29 );
	if(language == English)
	{
		cout << " > Other stats:\n" << setw(width2) << left << " > Cardinal (n): "
			  << "most    - " << setw(6) << left << n_max << "(@ #" << setw(5) << left << n_max_index + 1 << "); "
			  << "least  - "  << setw(6) << left << n_min << "(@ #" << setw(5) << left << n_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)n_sum / count << ";\n";
		cout << setw(width2) << left << " > Tension (t): "
			  << "highest - " << setw(6) << left << t_max << "(@ #" << setw(5) << left << t_max_index + 1 << "); "
			  << "lowest - "  << setw(6) << left << t_min << "(@ #" << setw(5) << left << t_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << t_sum / count << ";\n";
		cout << setw(width2) << left << " > Absolute value of chroma (|k|): "
			  << "highest - " << setw(6) << left << k_max << "(@ #" << setw(5) << left << k_max_index + 1 << "); "
			  << "lowest - "  << setw(6) << left << k_min << "(@ #" << setw(5) << left << k_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << k_sum / count << ";\n";
		cout << setw(width2) << left << " > Thickness (h): "
			  << "highest - " << setw(6) << left << h_max << "(@ #" << setw(5) << left << h_max_index + 1 << "); "
			  << "lowest - "  << setw(6) << left << h_min << "(@ #" << setw(5) << left << h_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << h_sum / count << ";\n";
		cout << setw(width2) << left << " > Common notes (c): "
			  << "most    - " << setw(6) << left << c_max << "(@ #" << setw(5) << left << c_max_index + 1 << "); "
			  << "least  - "  << setw(6) << left << c_min << "(@ #" << setw(5) << left << c_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)c_sum / (count - 1) << ";\n";
		cout << setw(width2) << left << " > Root (r): "
			  << "highest - " << setw(6) << left << r_max << "(@ #" << setw(5) << left << r_max_index + 1 << "); "
			  << "lowest - "  << setw(6) << left << r_min << "(@ #" << setw(5) << left << r_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)r_sum / count << ";\n";
		cout << setw(width2) << left << " > Geomertic center (g%): "
			  << "highest - " << setw(6) << left << g_max << "(@ #" << setw(5) << left << g_max_index + 1 << "); "
			  << "lowest - "  << setw(6) << left << g_min << "(@ #" << setw(5) << left << g_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << g_sum / count << ";\n";
		cout << setw(width2) << left << " > Total voice leading (s): "
			  << "highest - " << setw(6) << left << s_max << "(@ #" << setw(5) << left << s_max_index + 1 << "); "
			  << "lowest - "  << setw(6) << left << s_min << "(@ #" << setw(5) << left << s_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)s_sum / (count - 1) << ";\n";
		cout << setw(width2) << left << " > Similarity (x%): "
			  << "highest - " << setw(6) << left << x_max << "(@ #" << setw(5) << left << x_max_index + 1 << "); "
			  << "lowest - "  << setw(6) << left << x_min << "(@ #" << setw(5) << left << x_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)x_sum / (count - 1) << ";\n";
	}
	else
	{
		cout << " > 其他统计：\n" << setw(width2) << left << " > 和弦音集音数 (n)："
			  << "最多 - " << setw(6) << left << n_max << "(@ #" << setw(5) << left << n_max_index + 1 << ")；"
			  << "最少 - " << setw(6) << left << n_min << "(@ #" << setw(5) << left << n_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)n_sum / count << "；\n";
		cout << setw(width2) << left << " > 紧张度 (t)："
			  << "最高 - " << setw(6) << left << t_max << "(@ #" << setw(5) << left << t_max_index + 1 << ")；"
			  << "最低 - " << setw(6) << left << t_min << "(@ #" << setw(5) << left << t_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << t_sum / count << "；\n";
		cout << setw(width2) << left << " > 和弦进行色差绝对值 (|k|)："
			  << "最高 - " << setw(6) << left << k_max << "(@ #" << setw(5) << left << k_max_index + 1 << ")；"
			  << "最低 - " << setw(6) << left << k_min << "(@ #" << setw(5) << left << k_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << k_sum / count << "；\n";
		cout << setw(width2) << left << " > 厚度 (h)："
			  << "最高 - " << setw(6) << left << h_max << "(@ #" << setw(5) << left << h_max_index + 1 << ")；"
			  << "最低 - " << setw(6) << left << h_min << "(@ #" << setw(5) << left << h_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << h_sum / count << "；\n";
		cout << setw(width2) << left << " > 共同音个数 (c)："
			  << "最多 - " << setw(6) << left << c_max << "(@ #" << setw(5) << left << c_max_index + 1 << ")；"
			  << "最少 - " << setw(6) << left << c_min << "(@ #" << setw(5) << left << c_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)c_sum / (count - 1) << "；\n";
		cout << setw(width2) << left << " > 根音键位 (r)："
			  << "最高 - " << setw(6) << left << r_max << "(@ #" << setw(5) << left << r_max_index + 1 << ")；"
			  << "最低 - " << setw(6) << left << r_min << "(@ #" << setw(5) << left << r_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)r_sum / count << "；\n";
		cout << setw(width2) << left << " > 几何中心位置占比 (g%)："
			  << "最高 - " << setw(6) << left << g_max << "(@ #" << setw(5) << left << g_max_index + 1 << ")；"
			  << "最低 - " << setw(6) << left << g_min << "(@ #" << setw(5) << left << g_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << g_sum / count << "；\n";
		cout << setw(width2) << left << " > 声部进行总大小 (s)："
			  << "最高 - " << setw(6) << left << s_max << "(@ #" << setw(5) << left << s_max_index + 1 << ")；"
			  << "最低 - " << setw(6) << left << s_min << "(@ #" << setw(5) << left << s_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)s_sum / (count - 1) << "；\n";
		cout << setw(width2) << left << " > 相邻和弦相似度 (x%)："
			  << "最高 - " << setw(6) << left << x_max << "(@ #" << setw(5) << left << x_max_index + 1 << ")；"
			  << "最低 - " << setw(6) << left << x_min << "(@ #" << setw(5) << left << x_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)x_sum / (count - 1) << "。\n";
	}

	cout << "\n > Copy the stats to a file (Y / N)? ";
	inputY_N(ch);
	if(ch == 'Y' || ch == 'y')
	{
		cout << "\n > Please assign a name for the output file (the default extension is \".txt\"): ";
		char output[100] = "\0";
		inputFilename(output, ".txt", false);

		fout.open(output, ios::trunc);
		if(language == English)
			fout << "Voice leading stats:\n"
				  << "Movement instances: " << "[0]  " << movement[vl_max].instance << endl;
		else  fout << "声部进行统计：\n声部动向及相应出现次数：[0]  " << movement[vl_max].instance << endl;
		for(int i = 1; i <= vl_max; ++i)
			fout << "[+" << i << "] " << movement[i + vl_max].instance << "  ";
		fout << endl;
		for(int i = -1; i >= -vl_max; --i)
			fout << "[" << i << "] " << movement[i + vl_max].instance << "  ";
		
		if(language == English)
			fout << "\nMovement percentage (sorted H -> L):\n";
		else  fout << "\n声部动向频次占比（从高到低）：\n";
		for(int i = 0; i < 2 * vl_max + 1; ++i)
		{
			fout << "[";
			if(sorted[i].amount > 0)  fout << "+";
			fout << sorted[i].amount << "] " << (int)(sorted[i].percentage * 100 + 0.5) << "%  ";
		}
		if(language == English)
			fout << "\nCardinal change instances: ";
		else  fout << "\n和弦音个数 (N) 发生变化的次数：";
		fout << cardinal_change << " (" << (int)((double)cardinal_change / (count - 1) * 100.0 + 0.5) << "%)\n\n";

		if(language == English)
		{
			fout << "Other stats:\n" << "Cardinal (n): "
				  << "most - "    << n_max << "(@ #" << n_max_index + 1 << "); "
				  << "least - "   << n_min << "(@ #" << n_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << (double)n_sum / count << ";\n";
			fout << "Tension (t): "
				  << "highest - " << t_max << "(@ #" << t_max_index + 1 << "); "
				  << "lowest - "  << t_min << "(@ #" << t_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << t_sum / count << ";\n";
			fout << "Absolute value of chroma (|k|): "
				  << "highest - " << k_max << "(@ #" << k_max_index + 1 << "); "
				  << "lowest - "  << k_min << "(@ #" << k_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << k_sum / count << ";\n";
			fout << "Thickness (h): "
				  << "highest - " << h_max << "(@ #" << h_max_index + 1 << "); "
				  << "lowest - "  << h_min << "(@ #" << h_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << h_sum / count << ";\n";
			fout << "Common notes (c): "
				  << "most - "    << c_max << "(@ #" << c_max_index + 1 << "); "
				  << "least - "   << c_min << "(@ #" << c_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << (double)c_sum / (count - 1) << ";\n";
			fout << "Root (r): "
				  << "highest - " << r_max << "(@ #" << r_max_index + 1 << "); "
				  << "lowest - "  << r_min << "(@ #" << r_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << (double)r_sum / count << ";\n";
			fout << "Geomertic center (g): "
				  << "highest - " << g_max << "(@ #" << g_max_index + 1 << "); "
				  << "lowest - "  << g_min << "(@ #" << g_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << g_sum / count << ";\n";
			fout << "Total voice leading (s): "
				  << "highest - " << s_max << "(@ #" << s_max_index + 1 << "); "
				  << "lowest - "  << s_min << "(@ #" << s_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << (double)s_sum / (count - 1) << ";\n";
			fout << "Similarity (x%): "
				  << "highest - " << x_max << "(@ #" << x_max_index + 1 << "); "
				  << "lowest - "  << x_min << "(@ #" << x_min_index + 1 << "); "
				  << "average - " << fixed << setprecision(2) << (double)x_sum / (count - 1) << ".\n";
		}
		else
		{
			fout << "其他统计：\n" << "和弦音集音数 (n)："
				  << "最多 - " << n_max << "(@ #" << n_max_index + 1 << ")；"
				  << "最少 - " << n_min << "(@ #" << n_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << (double)n_sum / count << "；\n";
			fout << "紧张度 (t)："
				  << "最高 - " << t_max << "(@ #" << t_max_index + 1 << ")；"
				  << "最低 - " << t_min << "(@ #" << t_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << t_sum / count << "；\n";
			fout << "和弦进行的色差绝对值 (|k|)："
				  << "最高 - " << k_max << "(@ #" << k_max_index + 1 << ")；"
				  << "最低 - " << k_min << "(@ #" << k_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << k_sum / count << "；\n";
			fout << "厚度 (h)："
				  << "最高 - " << h_max << "(@ #" << h_max_index + 1 << ")；"
				  << "最低 - " << h_min << "(@ #" << h_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << h_sum / count << "；\n";
			fout << "共同音个数 (c)："
				  << "最多 - " << c_max << "(@ #" << c_max_index + 1 << ")；"
				  << "最少 - " << c_min << "(@ #" << c_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << (double)c_sum / (count - 1) << "；\n";
			fout << "根音键位 (r)："
				  << "最高 - " << r_max << "(@ #" << r_max_index + 1 << ")；"
				  << "最低 - " << r_min << "(@ #" << r_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << (double)r_sum / count << "；\n";
			fout << "几何中心位置占比 (g%)："
				  << "最高 - " << g_max << "(@ #" << g_max_index + 1 << ")；"
				  << "最低 - " << g_min << "(@ #" << g_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << g_sum / count << "；\n";
			fout << "声部进行总大小 (s)："
				  << "最高 - " << s_max << "(@ #" << s_max_index + 1 << ")；"
				  << "最低 - " << s_min << "(@ #" << s_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << (double)s_sum / (count - 1) << "；\n";
			fout << "相邻和弦相似度 (x%)："
				  << "最高 - " << x_max << "(@ #" << x_max_index + 1 << ")；"
				  << "最低 - " << x_min << "(@ #" << x_min_index + 1 << ")；"
				  << "平均 - " << fixed << setprecision(2) << (double)x_sum / (count - 1) << "。\n";
		}
	}

	cout << "\n > Output finished. Now you can close the program.\n\n";
	fout.close();
	system("pause"); 
	return 0;
}


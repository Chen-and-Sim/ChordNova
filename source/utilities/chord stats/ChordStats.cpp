// SmartChordGen-utility-ChordStats v1.5 [Build: 2020.7.2]
// Generates statistics for voice leading and other indicators for a chord data file.
// Note: we get some of the indicators such as similarity from the data of adjacent chords.
// Thus this program is not suitable for the results of single mode in the main program (SmartChordGen).
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
	
	cout << "\n > Please assign a name for the output file (the default extension is \".txt\"): ";
	char output[100] = "\0";
	inputFilename(output, ".txt", false);
	fout.open(output, ios::trunc);
	
	cout << "\n > Do you want to output the details of chord progressions (Y / N) ? ";
	char ch;  inputY_N(ch);
	bool detail = false;
	if(ch == 'Y' || ch == 'y')
		detail = true;
	
	cout << "\n > Output language: English (Y) / Chinese (N): ";
	inputY_N(ch);
	Language language;
	if(ch == 'Y' || ch == 'y')
		language = English;
	else  language = Chinese;
	
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

		if(count == 0)
		{
			Chord A(notes);
			rec.push_back(A);
			++count;
			continue;
		}
		Chord A(notes, rec.rbegin() -> get_chroma_old());
		rec.push_back(A);
		++count;

		rec.rbegin() -> _set_vl_max(vl_max);
		(rec.rbegin() + 1) -> _set_vl_max(vl_max);
		(rec.rbegin() + 1) -> find_vec(*rec.rbegin());
		(rec.rbegin() + 1) -> _set_common_notes(*rec.rbegin());
		for(int i = 0; i < rec.rbegin() -> get_vec().size(); ++i)
		{
			int num = rec.rbegin() -> get_vec()[i];
			if(abs(num) > vl_max)
			{
				if(language == English)
					cerr << "\n > ERROR: chord ";
				else  cerr << "\n > 错误：和弦";
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

	if(detail)
	{
		if(language == English)
			fprint("Original chord: ", rec[0].get_notes(), " ", ", ");
		else  fprint("起始和弦：", rec[0].get_notes(), " ", ", ");
		rec[0].printInitial();
		for(int i = 1; i < rec.size(); ++i)
		{
			if(language == English)
				fout << "Progression #" << i << ":\n";
			else  fout << "和弦进行 #" << i << ":\n";
			rec[i - 1].print( static_cast<ChordData&>(rec[i]) );
		}

		if(language == Chinese)
		{			  
			fout << "分析报告结果指标说明：\n"
				  << "【每个和弦】(音名列表) - 系统判断和弦音名（从低到高）； t - 紧张度； s - 纯五跨度； vec - 音程涵量表； \n"
					  "d - 音程结构表； n - 和弦音集音数； m - 和弦声部数量； h - 厚度； g - 几何中心位置占比； r - 根音。\n"
				  << "【和弦进行】k - 色差（华萃康法）； c - 相邻和弦的共同音个数； ss - 合跨度（相邻和弦之并的纯五跨度）；\n"
					  "sv - 声部进行总大小； v - 声部运动方向及距离（半音个数）； \n"
					  "Q - 和弦进行的陈氏Q指标； x - 相邻和弦的相似度； dt, dr, dg, ds, dn - 相应各项指标的变化量。\n"
				  << "【星号注解】* - 等音记谱（色值溢出）； ** - 等音记谱（色差溢出）。\n\n";
		}
		fout << "==========\n\n";
	}

	if(language == English)
		fout << "Voice leading stats:\n"
			  << "Movement instances: " << "[0]  " << movement[vl_max].instance << endl;
	else  fout << "声部进行统计：\n声部动向及相应出现次数：[0]  " << movement[vl_max].instance << endl;
	for(int i = 1; i <= vl_max; ++i)
		fout << "[+" << i << "] " << movement[i + vl_max].instance << "  ";
	fout << endl;
	for(int i = -1; i >= -vl_max; --i)
		fout << "[" << i << "] " << movement[i + vl_max].instance << "  ";

	int movement_instance = 0;
	for(int i = 0; i < 2 * vl_max + 1; ++i)
		movement_instance += movement[i].instance;
	for(int i = 0; i < 2 * vl_max + 1; ++i)
		movement[i].percentage = (double)movement[i].instance / movement_instance;

	vector<Movement> sorted;
	sorted.assign(movement.begin(), movement.end());
	merge_sort(sorted.begin(), sorted.end(), *larger_perc);
	fout << fixed << setprecision(2);
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

	int _x_max = 0, _x_min = INF, x_sum = 0, x_max_index = 0, x_min_index = 0;
	int _c_max = 0, _c_min = INF, c_sum = 0, c_max_index = 0, c_min_index = 0;
	int _n_max = 0, _n_min = INF, n_sum = 0, n_max_index = 0, n_min_index = 0;
	int _m_max = 0, _m_min = INF, m_sum = 0, m_min_index = 0, m_max_index = 0;
	int _r_max = 0, _r_min = INF, r_sum = 0, r_max_index = 0, r_min_index = 0;
	int _g_max = 0, _g_min = INF, g_sum = 0, g_max_index = 0, g_min_index = 0;
	int _s_max = 0, _s_min = INF, s_sum = 0, s_max_index = 0, s_min_index = 0;
	int _ss_max = 0,  _ss_min = INF, ss_sum = 0, ss_max_index = 0, ss_min_index = 0;
	int _sv_max = 0,  _sv_min = INF, sv_sum = 0, sv_max_index = 0, sv_min_index = 0;
	int dr_max = MINF, dr_min = INF, dr_sum = 0, dr_max_index = 0, dr_min_index = 0;
	int dg_max = MINF, dg_min = INF, dg_sum = 0, dg_max_index = 0, dg_min_index = 0;
	int ds_max = MINF, ds_min = INF, ds_sum = 0, ds_max_index = 0, ds_min_index = 0;
	int dn_max = MINF, dn_min = INF, dn_sum = 0, dn_max_index = 0, dn_min_index = 0;
	double _t_max = 0.0, _t_min = INF,  t_sum = 0, _k_max = 0.0,  _k_min = INF,  k_sum = 0;
	double _h_max = 0.0, _h_min = INF,  h_sum = 0, _q_max = 0.0,  _q_min = INF,  q_sum = 0;
	double nm_max = 0.0, nm_min = 1.0, nm_sum = 0, dt_max = MINF, dt_min = INF, dt_sum = 0;
	int  t_max_index = 0,  t_min_index = 0,  k_max_index = 0,  k_min_index = 0;
	int  h_max_index = 0,  h_min_index = 0,  q_max_index = 0,  q_min_index = 0;
	int nm_max_index = 0, nm_min_index = 0, dt_max_index = 0, dt_min_index = 0;
	int temp1;  double temp2;

	for(int i = 0; i < count; ++i)
	{
		if(i != 0)
		{
			temp2 = abs(rec[i].get_chroma());
			if(temp2 > _k_max)  { _k_max = temp2;  k_max_index = i; }
			if(temp2 < _k_min)  { _k_min = temp2;  k_min_index = i; }
			k_sum += temp2;

			temp2 = rec[i].get_Q_indicator();
			if(temp2 > _q_max)  { _q_max = temp2;  q_max_index = i; }
			if(temp2 < _q_min)  { _q_min = temp2;  q_min_index = i; }
			q_sum += temp2;

			temp1 = rec[i].get_similarity();
			if(temp1 > _x_max)  { _x_max = temp1;  x_max_index = i; }
			if(temp1 < _x_min)  { _x_min = temp1;  x_min_index = i; }
			x_sum += temp1;

			temp1 = intersect(rec[i].get_notes(), rec[i - 1].get_notes(), true).size();
			if(temp1 > _c_max)  { _c_max = temp1;  c_max_index = i; }
			if(temp1 < _c_min)  { _c_min = temp1;  c_min_index = i; }
			c_sum += temp1;

			temp1 = rec[i].get_sspan();
			if(temp1 > _ss_max)  { _ss_max = temp1;  ss_max_index = i; }
			if(temp1 < _ss_min)  { _ss_min = temp1;  ss_min_index = i; }
			ss_sum += temp1;

			temp1 = rec[i].get_sv();
			if(temp1 > _sv_max)  { _sv_max = temp1;  sv_max_index = i; }
			if(temp1 < _sv_min)  { _sv_min = temp1;  sv_min_index = i; }
			sv_sum += temp1;
		}

		temp1 = rec[i].get_s_size();
		if(temp1 > _n_max)  { _n_max = temp1;  n_max_index = i; }
		if(temp1 < _n_min)  { _n_min = temp1;  n_min_index = i; }
		n_sum += temp1;

		temp1 = rec[i].get_t_size();
		if(temp1 > _m_max)  { _m_max = temp1;  m_max_index = i; }
		if(temp1 < _m_min)  { _m_min = temp1;  m_min_index = i; }
		m_sum += temp1;

		temp2 = (double) rec[i].get_s_size() / rec[i].get_t_size();
		if(temp2 > nm_max)  { nm_max = temp2;  nm_max_index = i; }
		if(temp2 < nm_min)  { nm_min = temp2;  nm_min_index = i; }
		nm_sum += temp2;

		temp2 = rec[i].get_thickness();
		if(temp2 > _h_max)  { _h_max = temp2;  h_max_index = i; }
		if(temp2 < _h_min)  { _h_min = temp2;  h_min_index = i; }
		h_sum += temp2;

		temp2 = rec[i].get_tension();
		if(temp2 > _t_max)  { _t_max = temp2;  t_max_index = i; }
		if(temp2 < _t_min)  { _t_min = temp2;  t_min_index = i; }
		t_sum += temp2;

		temp1 = rec[i].get_root();
		if(temp1 > _r_max)  { _r_max = temp1;  r_max_index = i; }
		if(temp1 < _r_min)  { _r_min = temp1;  r_min_index = i; }
		r_sum += temp1;

		temp1 = rec[i].get_g_center();
		if(temp1 > _g_max)  { _g_max = temp1;  g_max_index = i; }
		if(temp1 < _g_min)  { _g_min = temp1;  g_min_index = i; }
		g_sum += temp2;

		temp1 = rec[i].get_span();
		if(temp1 > _s_max)  { _s_max = temp1;  s_max_index = i; }
		if(temp1 < _s_min)  { _s_min = temp1;  s_min_index = i; }
		s_sum += temp1;
		
		if(i != 0)
		{
			temp2 = rec[i].get_tension() - rec[i - 1].get_tension();
			if(temp2 > dt_max)  { dt_max = temp2;  dt_max_index = i; }
			if(temp2 < dt_min)  { dt_min = temp2;  dt_min_index = i; }
			dt_sum += temp2;

			temp1 = rec[i].get_root() - rec[i - 1].get_root();
			if(temp1 > dr_max)  { dr_max = temp1;  dr_max_index = i; }
			if(temp1 < dr_min)  { dr_min = temp1;  dr_min_index = i; }
			dr_sum += temp1;

			temp1 = rec[i].get_g_center() - rec[i - 1].get_g_center();
			if(temp1 > dg_max)  { dg_max = temp1;  dg_max_index = i; }
			if(temp1 < dg_min)  { dg_min = temp1;  dg_min_index = i; }
			dg_sum += temp1;
			
			temp1 = rec[i].get_span() - rec[i - 1].get_span();
			if(temp1 > ds_max)  { ds_max = temp1;  ds_max_index = i; }
			if(temp1 < ds_min)  { ds_min = temp1;  ds_min_index = i; }
			ds_sum += temp1;

			temp1 = rec[i].get_s_size() - rec[i - 1].get_s_size();
			if(temp1 > dn_max)  { dn_max = temp1;  dn_max_index = i; }
			if(temp1 < dn_min)  { dn_min = temp1;  dn_min_index = i; }
			dn_sum += temp1;
		}
	}

	if(language == English)
	{
		fout << "Other stats:\n" << "Absolute value of chroma (|k|): "
			  << "highest = " << _k_max << "(@ #" << k_max_index + 1 << "); "
			  << "lowest = "  << _k_min << "(@ #" << k_min_index + 1 << "); "
			  << "average = " << k_sum / (count - 1) << ";\n";
		fout << "Chen's Q indicator (Q): "
			  << "highest = " << _q_max << "(@ #" << q_max_index + 1 << "); "
			  << "lowest = "  << _q_min << "(@ #" << q_min_index + 1 << "); "
			  << "average = " << (double)q_sum / (count - 1) << ";\n";
		fout << "Similarity (x%): "
			  << "highest = " << _x_max << "(@ #" << x_max_index + 1 << "); "
			  << "lowest = "  << _x_min << "(@ #" << x_min_index + 1 << "); "
			  << "average = " << (double)x_sum / (count - 1) << ";\n";
		fout << "Common notes (c): "
			  << "most = "    << _c_max << "(@ #" << c_max_index + 1 << "); "
			  << "least = "   << _c_min << "(@ #" << c_min_index + 1 << "); "
			  << "average = " << (double)c_sum / (count - 1) << ";\n";
		fout << "Span of the union of adjacent chords (ss): "
			  << "highest = " << _ss_max << "(@ #" << ss_max_index + 1 << "); "
			  << "lowest = "  << _ss_min << "(@ #" << ss_min_index + 1 << "); "
			  << "average = " << (double)ss_sum / (count - 1) << ";\n";
		fout << "Total voice leading (Σvec): "
			  << "highest = " << _sv_max << "(@ #" << sv_max_index + 1 << "); "
			  << "lowest = "  << _sv_min << "(@ #" << sv_min_index + 1 << "); "
			  << "average = " << (double)sv_sum / (count - 1) << ";\n";
		fout << "Number of notes (n): "
			  << "most = "    << _n_max << "(@ #" << n_max_index + 1 << "); "
			  << "least = "   << _n_min << "(@ #" << n_min_index + 1 << "); "
			  << "average = " << (double)n_sum / count << ";\n";
		fout << "Number of parts (m): "
			  << "most = "    << _m_max << "(@ #" << m_max_index + 1 << "); "
			  << "least = "   << _m_min << "(@ #" << m_min_index + 1 << "); "
			  << "average = " << (double)m_sum / count << ";\n";
		fout << "Number of notes / parts (n / m): "
			  << "highest = " << nm_max << "(@ #" << nm_max_index + 1 << "); "
			  << "lowest = "  << nm_min << "(@ #" << nm_min_index + 1 << "); "
			  << "average = " << (double)nm_sum / count << ";\n";
		fout << "Thickness (h): "
			  << "highest = " << _h_max << "(@ #" << h_max_index + 1 << "); "
			  << "lowest = "  << _h_min << "(@ #" << h_min_index + 1 << "); "
			  << "average = " << h_sum / count << ";\n";
		fout << "Tension (t): "
			  << "highest = " << _t_max << "(@ #" << t_max_index + 1 << "); "
			  << "lowest = "  << _t_min << "(@ #" << t_min_index + 1 << "); "
			  << "average = " << t_sum / count << ";\n";
		fout << "Root (r): "
			  << "highest = " << _r_max << "(@ #" << r_max_index + 1 << "); "
			  << "lowest = "  << _r_min << "(@ #" << r_min_index + 1 << "); "
			  << "average = " << (double)r_sum / count << ";\n";
		fout << "Geomertic center (g%): "
			  << "highest = " << _g_max << "(@ #" << g_max_index + 1 << "); "
			  << "lowest = "  << _g_min << "(@ #" << g_min_index + 1 << "); "
			  << "average = " << (double)g_sum / count << ";\n";
		fout << "Perfect fifth span (s): "
			  << "highest = " << _s_max << "(@ #" << s_max_index + 1 << "); "
			  << "lowest = "  << _s_min << "(@ #" << s_min_index + 1 << "); "
			  << "average = " << (double)s_sum / count << ";\n";
		fout << "Difference of tension (dt): "
			  << "highest = " << dt_max << "(@ #" << dt_max_index + 1 << "); "
			  << "lowest = "  << dt_min << "(@ #" << dt_min_index + 1 << "); "
			  << "average = " << dt_sum / (count - 1) << ";\n";
		fout << "Difference of root (dr): "
			  << "highest = " << dr_max << "(@ #" << dr_max_index + 1 << "); "
			  << "lowest = "  << dr_min << "(@ #" << dr_min_index + 1 << "); "
			  << "average = " << (double)dr_sum / (count - 1) << ";\n";
		fout << "Difference of geometric center (dg%): "
			  << "highest = " << dg_max << "(@ #" << dg_max_index + 1 << "); "
			  << "lowest = "  << dg_min << "(@ #" << dg_min_index + 1 << "); "
			  << "average = " << (double)dg_sum / (count - 1) << ";\n";
		fout << "Difference of perfect fifth span (ds): "
			  << "highest = " << ds_max << "(@ #" << ds_max_index + 1 << "); "
			  << "lowest = "  << ds_min << "(@ #" << ds_min_index + 1 << "); "
			  << "average = " << (double)ds_sum / (count - 1) << ";\n";
		fout << "Difference of number of notes (dn): "
			  << "highest = " << dn_max << "(@ #" << dn_max_index + 1 << "); "
			  << "lowest = "  << dn_min << "(@ #" << dn_min_index + 1 << "); "
			  << "average = " << (double)dn_sum / (count - 1) << ";\n";
	}
	else
	{
		fout << "其他统计：\n" << "和弦进行色差绝对值 (|k|): "
			  << "最高 = " << _k_max << "(@ #" << k_max_index + 1 << ")；"
			  << "最低 = " << _k_min << "(@ #" << k_min_index + 1 << ")；"
			  << "平均 = " << k_sum / (count - 1) << "；\n";
		fout << "和弦进行的陈氏Q指标 (Q): "
			  << "最高 = " << _q_max << "(@ #" << q_max_index + 1 << ")；"
			  << "最低 = " << _q_min << "(@ #" << q_min_index + 1 << ")；"
			  << "平均 = " << (double)q_sum / (count - 1) << "；\n";
		fout << "相邻和弦相似度 (x%): "
			  << "最高 = " << _x_max << "(@ #" << x_max_index + 1 << ")；"
			  << "最低 = " << _x_min << "(@ #" << x_min_index + 1 << ")；"
			  << "平均 = " << (double)x_sum / (count - 1) << "；\n";
		fout << "共同音个数 (c): "
			  << "最多 = " << _c_max << "(@ #" << c_max_index + 1 << ")；"
			  << "最少 = " << _c_min << "(@ #" << c_min_index + 1 << ")；"
			  << "平均 = " << (double)c_sum / (count - 1) << "；\n";
		fout << "相邻和弦之并的跨度 (ss): "
			  << "最高 = " << _ss_max << "(@ #" << ss_max_index + 1 << ")；"
			  << "最低 = " << _ss_min << "(@ #" << ss_min_index + 1 << ")；"
			  << "平均 = " << (double)ss_sum / (count - 1) << "；\n";
		fout << "声部进行总大小 (Σvec): "
			  << "最高 = " << _sv_max << "(@ #" << sv_max_index + 1 << ")；"
			  << "最低 = " << _sv_min << "(@ #" << sv_min_index + 1 << ")；"
			  << "平均 = " << (double)sv_sum / (count - 1) << "；\n";
		fout << "和弦音集音数 (n): "
			  << "最多 = " << _n_max << "(@ #" << n_max_index + 1 << ")；"
			  << "最少 = " << _n_min << "(@ #" << n_min_index + 1 << ")；"
			  << "平均 = " << (double)n_sum / count << "；\n";
		fout << "和弦声部数量 (m): "
			  << "最多 = " << _m_max << "(@ #" << m_max_index + 1 << ")；"
			  << "最少 = " << _m_min << "(@ #" << m_min_index + 1 << ")；"
			  << "平均 = " << (double)m_sum / count << "；\n";
		fout << "和弦音集音数 / 和弦声部数量 (n / m): "
			  << "最高 = " << nm_max << "(@ #" << nm_max_index + 1 << ")；"
			  << "最低 = " << nm_min << "(@ #" << nm_min_index + 1 << ")；"
			  << "平均 = " << (double)nm_sum / count << "；\n";
		fout << "厚度 (h): "
			  << "最高 = " << _h_max << "(@ #" << h_max_index + 1 << ")；"
			  << "最低 = " << _h_min << "(@ #" << h_min_index + 1 << ")；"
			  << "平均 = " << h_sum / count << "；\n";
		fout << "紧张度 (t): "
			  << "最高 = " << _t_max << "(@ #" << t_max_index + 1 << ")；"
			  << "最低 = " << _t_min << "(@ #" << t_min_index + 1 << ")；"
			  << "平均 = " << t_sum / count << "；\n";
		fout << "根音键位 (r): "
			  << "最高 = " << _r_max << "(@ #" << r_max_index + 1 << ")；"
			  << "最低 = " << _r_min << "(@ #" << r_min_index + 1 << ")；"
			  << "平均 = " << (double)r_sum / count << "；\n";
		fout << "几何中心位置占比 (g%): "
			  << "最高 = " << _g_max << "(@ #" << g_max_index + 1 << ")；"
			  << "最低 = " << _g_min << "(@ #" << g_min_index + 1 << ")；"
			  << "平均 = " << (double)g_sum / count << "；\n";
		fout << "和弦的纯五跨度 (s): "
			  << "最高 = " << _s_max << "(@ #" << ss_max_index + 1 << ")；"
			  << "最低 = " << _s_min << "(@ #" << ss_min_index + 1 << ")；"
			  << "平均 = " << (double)s_sum / count << "；\n";
		fout << "紧张度之差 (dt): "
			  << "最高 = " << dt_max << "(@ #" << dt_max_index + 1 << ")；"
			  << "最低 = " << dt_min << "(@ #" << dt_min_index + 1 << ")；"
			  << "平均 = " << dt_sum / (count - 1)<< "；\n";
		fout << "根音之差 (dr): "
			  << "最高 = " << dr_max << "(@ #" << dr_max_index + 1 << ")；"
			  << "最低 = " << dr_min << "(@ #" << dr_min_index + 1 << ")；"
			  << "平均 = " << (double)dr_sum / (count - 1) << "；\n";
		fout << "几何中心位置之差 (dg%): "
			  << "最高 = " << dg_max << "(@ #" << dg_max_index + 1 << ")；"
			  << "最低 = " << dg_min << "(@ #" << dg_min_index + 1 << ")；"
			  << "平均 = " << (double)dg_sum / (count - 1) << "；\n";
		fout << "几何中心位置之差 (ds): "
			  << "最高 = " << ds_max << "(@ #" << ds_max_index + 1 << ")；"
			  << "最低 = " << ds_min << "(@ #" << ds_min_index + 1 << ")；"
			  << "平均 = " << (double)ds_sum / (count - 1) << "；\n";
		fout << "和弦音集音数之差 (dn): "
			  << "最高 = " << dn_max << "(@ #" << dn_max_index + 1 << ")；"
			  << "最低 = " << dn_min << "(@ #" << dn_min_index + 1 << ")；"
			  << "平均 = " << (double)dn_sum / (count - 1) << "。\n";
	}

	cout << "\n > Output finished. Now you can close the program.\n\n";
	fout.close();
	system("pause"); 
	return 0;
}


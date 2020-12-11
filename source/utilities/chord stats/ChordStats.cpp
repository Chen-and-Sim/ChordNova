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
				else  cerr << "\n > ���󣺺���";
				cprint("\0", (rec.rbegin() + 1) -> get_notes(), ", ", "\0");
				if(language == English)
					cerr << " (@ #" << count - 1 << ") and/or chord ";
				else  cerr << " (@ #" << count - 1 << ") ��/�����";
				cprint("\0", rec.rbegin() -> get_notes(), ", ", "\0");
				if(language == English)
					cerr << " (@ #" << count << ") do(es) not meet the requirements.\n\n";
				else  cerr << " (@ #" << count << ") ������Ҫ��\n\n";
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
		else  fprint("��ʼ���ң�", rec[0].get_notes(), " ", ", ");
		rec[0].printInitial();
		for(int i = 1; i < rec.size(); ++i)
		{
			if(language == English)
				fout << "Progression #" << i << ":\n";
			else  fout << "���ҽ��� #" << i << ":\n";
			rec[i - 1].print( static_cast<ChordData&>(rec[i]) );
		}

		if(language == Chinese)
		{			  
			fout << "����������ָ��˵����\n"
				  << "��ÿ�����ҡ�(�����б�) - ϵͳ�жϺ����������ӵ͵��ߣ��� t - ���Ŷȣ� s - �����ȣ� vec - ���̺����� \n"
					  "d - ���̽ṹ�� n - �������������� m - �������������� h - ��ȣ� g - ��������λ��ռ�ȣ� r - ������\n"
				  << "�����ҽ��С�k - ɫ����Ϳ������� c - ���ں��ҵĹ�ͬ�������� ss - �Ͽ�ȣ����ں���֮���Ĵ����ȣ���\n"
					  "sv - ���������ܴ�С�� v - �����˶����򼰾��루������������ \n"
					  "Q - ���ҽ��еĳ���Qָ�ꣻ x - ���ں��ҵ����ƶȣ� dt, dr, dg, ds, dn - ��Ӧ����ָ��ı仯����\n"
				  << "���Ǻ�ע�⡿* - �������ף�ɫֵ������� ** - �������ף�ɫ���������\n\n";
		}
		fout << "==========\n\n";
	}

	if(language == English)
		fout << "Voice leading stats:\n"
			  << "Movement instances: " << "[0]  " << movement[vl_max].instance << endl;
	else  fout << "��������ͳ�ƣ�\n����������Ӧ���ִ�����[0]  " << movement[vl_max].instance << endl;
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
	else  fout << "\n��������Ƶ��ռ�ȣ��Ӹߵ��ͣ���\n";
	for(int i = 0; i < 2 * vl_max + 1; ++i)
	{
		fout << "[";
		if(sorted[i].amount > 0)  fout << "+";
		fout << sorted[i].amount << "] " << (int)(sorted[i].percentage * 100 + 0.5) << "%  ";
	}
	
	if(language == English)
		fout << "\nCardinal change instances: ";
	else  fout << "\n���������� (N) �����仯�Ĵ�����";
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
		fout << "Total voice leading (��vec): "
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
		fout << "����ͳ�ƣ�\n" << "���ҽ���ɫ�����ֵ (|k|): "
			  << "��� = " << _k_max << "(@ #" << k_max_index + 1 << ")��"
			  << "��� = " << _k_min << "(@ #" << k_min_index + 1 << ")��"
			  << "ƽ�� = " << k_sum / (count - 1) << "��\n";
		fout << "���ҽ��еĳ���Qָ�� (Q): "
			  << "��� = " << _q_max << "(@ #" << q_max_index + 1 << ")��"
			  << "��� = " << _q_min << "(@ #" << q_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)q_sum / (count - 1) << "��\n";
		fout << "���ں������ƶ� (x%): "
			  << "��� = " << _x_max << "(@ #" << x_max_index + 1 << ")��"
			  << "��� = " << _x_min << "(@ #" << x_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)x_sum / (count - 1) << "��\n";
		fout << "��ͬ������ (c): "
			  << "��� = " << _c_max << "(@ #" << c_max_index + 1 << ")��"
			  << "���� = " << _c_min << "(@ #" << c_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)c_sum / (count - 1) << "��\n";
		fout << "���ں���֮���Ŀ�� (ss): "
			  << "��� = " << _ss_max << "(@ #" << ss_max_index + 1 << ")��"
			  << "��� = " << _ss_min << "(@ #" << ss_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)ss_sum / (count - 1) << "��\n";
		fout << "���������ܴ�С (��vec): "
			  << "��� = " << _sv_max << "(@ #" << sv_max_index + 1 << ")��"
			  << "��� = " << _sv_min << "(@ #" << sv_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)sv_sum / (count - 1) << "��\n";
		fout << "������������ (n): "
			  << "��� = " << _n_max << "(@ #" << n_max_index + 1 << ")��"
			  << "���� = " << _n_min << "(@ #" << n_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)n_sum / count << "��\n";
		fout << "������������ (m): "
			  << "��� = " << _m_max << "(@ #" << m_max_index + 1 << ")��"
			  << "���� = " << _m_min << "(@ #" << m_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)m_sum / count << "��\n";
		fout << "������������ / ������������ (n / m): "
			  << "��� = " << nm_max << "(@ #" << nm_max_index + 1 << ")��"
			  << "��� = " << nm_min << "(@ #" << nm_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)nm_sum / count << "��\n";
		fout << "��� (h): "
			  << "��� = " << _h_max << "(@ #" << h_max_index + 1 << ")��"
			  << "��� = " << _h_min << "(@ #" << h_min_index + 1 << ")��"
			  << "ƽ�� = " << h_sum / count << "��\n";
		fout << "���Ŷ� (t): "
			  << "��� = " << _t_max << "(@ #" << t_max_index + 1 << ")��"
			  << "��� = " << _t_min << "(@ #" << t_min_index + 1 << ")��"
			  << "ƽ�� = " << t_sum / count << "��\n";
		fout << "������λ (r): "
			  << "��� = " << _r_max << "(@ #" << r_max_index + 1 << ")��"
			  << "��� = " << _r_min << "(@ #" << r_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)r_sum / count << "��\n";
		fout << "��������λ��ռ�� (g%): "
			  << "��� = " << _g_max << "(@ #" << g_max_index + 1 << ")��"
			  << "��� = " << _g_min << "(@ #" << g_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)g_sum / count << "��\n";
		fout << "���ҵĴ����� (s): "
			  << "��� = " << _s_max << "(@ #" << ss_max_index + 1 << ")��"
			  << "��� = " << _s_min << "(@ #" << ss_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)s_sum / count << "��\n";
		fout << "���Ŷ�֮�� (dt): "
			  << "��� = " << dt_max << "(@ #" << dt_max_index + 1 << ")��"
			  << "��� = " << dt_min << "(@ #" << dt_min_index + 1 << ")��"
			  << "ƽ�� = " << dt_sum / (count - 1)<< "��\n";
		fout << "����֮�� (dr): "
			  << "��� = " << dr_max << "(@ #" << dr_max_index + 1 << ")��"
			  << "��� = " << dr_min << "(@ #" << dr_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)dr_sum / (count - 1) << "��\n";
		fout << "��������λ��֮�� (dg%): "
			  << "��� = " << dg_max << "(@ #" << dg_max_index + 1 << ")��"
			  << "��� = " << dg_min << "(@ #" << dg_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)dg_sum / (count - 1) << "��\n";
		fout << "��������λ��֮�� (ds): "
			  << "��� = " << ds_max << "(@ #" << ds_max_index + 1 << ")��"
			  << "��� = " << ds_min << "(@ #" << ds_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)ds_sum / (count - 1) << "��\n";
		fout << "������������֮�� (dn): "
			  << "��� = " << dn_max << "(@ #" << dn_max_index + 1 << ")��"
			  << "��� = " << dn_min << "(@ #" << dn_min_index + 1 << ")��"
			  << "ƽ�� = " << (double)dn_sum / (count - 1) << "��\n";
	}

	cout << "\n > Output finished. Now you can close the program.\n\n";
	fout.close();
	system("pause"); 
	return 0;
}


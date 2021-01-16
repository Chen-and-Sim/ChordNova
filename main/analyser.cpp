// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// analyser.cpp

#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <vector>

#include "chord.h"
#include "functions.h"
using namespace std;

void Chord::analyse()
{
	Chord antechord(ante_notes, 0);
	Chord postchord(post_notes, antechord.chroma_old);
	antechord.find_vec(postchord, true, false);
	print_analysis( static_cast<ChordData&>(antechord), static_cast<ChordData&>(postchord),
						 str_ante_notes, str_post_notes, language );
}

void Chord::substitute()
{
	begin_sub = clock();
	set_param_center();
	set_param_range();
	set_sub_library();
	record_ante.clear();
	record_post.clear();

	char name1[200], name2[200];
	strcpy(name1, output_path);
	strcpy(name2, output_path);
	strcat(name1, ((QString)output_name_sub).toLocal8Bit().data());
	strcat(name2, ((QString)output_name_sub).toLocal8Bit().data());
	strcat(name1, ".txt");
	strcat(name2, ".mid");
	if(output_mode_sub != MidiOnly)
		fout.open(name1, ios::trunc);
	if(output_mode_sub != TextOnly)
		m_fout.open(name2, ios::trunc | ios::binary);

	Chord antechord(reduced_ante_notes, 0);
	Chord postchord(reduced_post_notes, 0);
	if(object == Postchord)
	{
		for(int i = 0; i < (1 << 12) - 1; ++i)
		{
			if(sub_library[i] == reduced_post_notes)
				continue;
			Chord new_postchord(sub_library[i], antechord.chroma_old);
			postchord.find_vec(new_postchord, false, true);
			new_postchord.sim_orig = set_similarity(postchord, new_postchord, true);
			if( valid_sub(new_postchord, antechord) )
				record_post.push_back( static_cast<ChordData>(new_postchord) );
		}
		sort_results(record_post, true);
		sub_size = record_post.size();
	}
	else if(object == Antechord)
	{
		for(int i = 0; i < (1 << 12) - 1; ++i)
		{
			if(sub_library[i] == reduced_ante_notes)
				continue;
			Chord new_antechord(sub_library[i], postchord.chroma_old);
			antechord.find_vec(new_antechord, false, true);
			new_antechord.sim_orig = set_similarity(antechord, new_antechord, true);
			if( valid_sub(new_antechord, postchord) )
				record_ante.push_back( static_cast<ChordData>(new_antechord) );
		}
		sort_results(record_ante, true);
		sub_size = record_ante.size();
	}
	else if(object == BothChords)
	{
		const int size = (test_all ? 16769025 : sample_size);
		// 16769025 = ( (1 << 12) - 1 ) ^ 2
		prgdialog_sub -> setMaximum(size - 1);
		begin_loop_sub = clock();
		int count = 0;

		for(int i = 0; i < size; ++i)
		{
			if(sub_library[2 * i] == reduced_ante_notes &&
				sub_library[2 * i + 1] == reduced_post_notes)
				continue;
			Chord chord1(sub_library[2 * i], 0);
			Chord chord2(sub_library[2 * i + 1], chord1.chroma_old);
			antechord.find_vec(chord1, false, true);
			postchord.find_vec(chord2, false, true);
			chord1.sim_orig = set_similarity(antechord, chord1, true);
			chord2.sim_orig = set_similarity(postchord, chord2, true);
			if( valid_sub(chord2, chord1) && valid_single_chord(chord1) )
			{
				chord2.orig_pos = (count++);
				record_ante.push_back( static_cast<ChordData>(chord1) );
				record_post.push_back( static_cast<ChordData>(chord2) );
			}

			if(i % 500 == 0)
			{
				labeltext_sub.clear();
				set_est_time(i, true);
				prgdialog_sub -> setLabelText(labeltext_sub);
				if(prgdialog_sub -> wasCanceled())  abort(true);
				prgdialog_sub -> setValue(i);
			}
		}
		sort_results(record_post, true);
		sub_size = record_post.size();
	}

	if(object == BothChords)
	{
		QStringList str = {"(Writing to file(s)...)", "（正在写入文件…）"};
		prgdialog_sub -> setLabelText(str[language]);
		if(prgdialog_sub -> wasCanceled())  abort(true);
		set_est_time(prgdialog_sub -> maximum(), true);
		prgdialog_sub -> setValue(prgdialog_sub -> maximum());
	}
	if(output_mode_sub != MidiOnly)  print_sub();
	if(output_mode_sub != TextOnly)  to_midi_sub();
	if(object == BothChords)
		prgdialog_sub -> close();
}

void Chord::set_param_center()
{
	reduced_ante_notes = ante_notes;
	for(int i = 0; i < (int)reduced_ante_notes.size(); ++i)
		reduced_ante_notes[i] %= 12;
	bubble_sort(reduced_ante_notes);
	remove_duplicate(reduced_ante_notes);
	for(int i = 0; i < (int)reduced_ante_notes.size(); ++i)
		reduced_ante_notes[i] += 72;

	reduced_post_notes = post_notes;
	for(int i = 0; i < (int)reduced_post_notes.size(); ++i)
		reduced_post_notes[i] %= 12;
	bubble_sort(reduced_post_notes);
	remove_duplicate(reduced_post_notes);
	for(int i = 0; i < (int)reduced_post_notes.size(); ++i)
		reduced_post_notes[i] += 72;

	Chord antechord(reduced_ante_notes, 0);
	Chord postchord(reduced_post_notes, antechord.get_chroma_old());
	antechord.find_vec(postchord, false, true);

	int *pReset[VAR_TOTAL]  = {&p_reset_value, &n_reset_value,  &t_reset_value,  &k_reset_value,
										&c_reset_value, &s_reset_value,  &ss_reset_value,  nullptr,
										 nullptr,        nullptr,        &sv_reset_value, &q_reset_value,
										&x_reset_value, &kk_reset_value, &r_reset_value,   nullptr};
	bool isDouble;
	int *param_int1, *param_int2;
	double *param_double1, *param_double2;
	double chroma_old_diff = postchord.chroma_old - postchord.prev_chroma_old;

	for(int i = 0; i < VAR_TOTAL - 1; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;

		if(i == 2 || i == 3 || i == 11 || i == 13)
			isDouble = true;
		else  isDouble = false;
		switch(i)
		{
			case 0:  param_int1 = &sim_orig;        param_int2 = &postchord.sim_orig;        break;
			case 1:  param_int1 = &s_size;          param_int2 = &postchord.s_size;          break;
			case 2:  param_double1 = &tension;      param_double2 = &postchord.tension;      break;
			case 3:  param_double1 = &chroma;       param_double2 = &postchord.chroma;       break;
			case 4:  param_int1 = &common_note;     param_int2 = &postchord.common_note;     break;
			case 5:  param_int1 = &span;            param_int2 = &postchord.span;            break;
			case 6:  param_int1 = &sspan;           param_int2 = &postchord.sspan;           break;
			case 10: param_int1 = &sv;              param_int2 = &postchord.sv;              break;
			case 11: param_double1 = &Q_indicator;  param_double2 = &postchord.Q_indicator;  break;
			case 12: param_int1 = &similarity;      param_int2 = &postchord.similarity;      break;
			case 13: param_double1 = &chroma_old;   param_double2 = &chroma_old_diff;        break;
			case 14: param_int1 = &root;            param_int2 = &postchord.root;            break;
		}

		if(isDouble)
		{
			if(strchr(reset_list, var[i]) != nullptr)
				*param_double1 = *pReset[i];
			else
				*param_double1 = *param_double2;
		}
		else
		{
			if(strchr(reset_list, var[i]) != nullptr)
				*param_int1 = *pReset[i];
			else
				*param_int1 = *param_int2;
		}
	}
	prev_chroma_old = 0;
	// These values will be overlapped in the main program.
}

void Chord::set_param_range()
{
	double *min_sub_ptr[VAR_TOTAL] = {&p_min_sub, &n_min_sub,  &t_min_sub,  &k_min_sub,
												 &c_min_sub, &s_min_sub,  &ss_min_sub,  nullptr,
												 nullptr,    nullptr,     &sv_min_sub, &q_min_sub,
												 &x_min_sub, &kk_min_sub, &r_min_sub,   nullptr};
	double *max_sub_ptr[VAR_TOTAL] = {&p_max_sub, &n_max_sub,  &t_max_sub,  &k_max_sub,
												 &c_max_sub, &s_max_sub,  &ss_max_sub, nullptr,
												 nullptr,    nullptr,     &sv_max_sub, &q_max_sub,
												 &x_max_sub, &kk_max_sub, &r_max_sub,  nullptr};
	int *pRadius[VAR_TOTAL] = {&p_radius, &n_radius,  &t_radius,  &k_radius,
										&c_radius, &s_radius,  &ss_radius, nullptr,
										nullptr,   nullptr,    &sv_radius, &q_radius,
										&x_radius, &kk_radius, &r_radius,  nullptr};

	double chroma_old_diff = chroma_old - prev_chroma_old;
	bool isDouble;
	int *param_int;
	double *param_double;

	for(int i = 0; i < VAR_TOTAL - 1; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;

		if(i == 2 || i == 3 || i == 11 || i == 13)
			isDouble = true;
		else  isDouble = false;
		switch(i)
		{
			case 0:  param_int = &sim_orig;            break;
			case 1:  param_int = &s_size;              break;
			case 2:  param_double = &tension;          break;
			case 3:  param_double = &chroma;           break;
			case 4:  param_int = &common_note;         break;
			case 5:  param_int = &span;                break;
			case 6:  param_int = &sspan;               break;
			case 10: param_int = &sv;                  break;
			case 11: param_double = &Q_indicator;      break;
			case 12: param_int = &similarity;          break;
			case 13: param_double = &chroma_old_diff;  break;
			case 14: param_int = &root;                break;
		}

		if(isDouble)
		{
			if(strchr(percentage_list, var[i]) != nullptr)
			{
				*min_sub_ptr[i] = (*param_double) * (1.0 - *pRadius[i] / 100.0);
				*max_sub_ptr[i] = (*param_double) * (1.0 + *pRadius[i] / 100.0);
			}
			else
			{
				*min_sub_ptr[i] = (*param_double) - (*pRadius[i]);
				*max_sub_ptr[i] = (*param_double) + (*pRadius[i]);
			}
		}
		else
		{
			if(strchr(percentage_list, var[i]) != nullptr)
			{
				*min_sub_ptr[i] = (*param_int) * (1.0 - *pRadius[i] / 100.0);
				*max_sub_ptr[i] = (*param_int) * (1.0 + *pRadius[i] / 100.0);
			}
			else
			{
				*min_sub_ptr[i] = (*param_int) - (*pRadius[i]);
				*max_sub_ptr[i] = (*param_int) + (*pRadius[i]);
			}
		}
	}
}

void Chord::set_sub_library()
{
	sub_library.clear();
	vector<int> _notes;
	if(object == BothChords)
	{
		QStringList str = {"(Please wait...)", "（请稍候…）"};
		prgdialog_sub -> setLabelText(str[language]);
		prgdialog_sub -> setMaximum(4095);

		if(test_all)
		{
			for(int i = 1; i < (1 << 12); ++i)
			{
				if(prgdialog_sub -> wasCanceled())  abort(true);
				prgdialog_sub -> setValue(i);

				for(int j = 1; j < (1 << 12); ++j)
				{
					id_to_notes(i, _notes);
					sub_library.push_back(_notes);
					id_to_notes(j, _notes);
					sub_library.push_back(_notes);
				}
			}
		}
		else
		{
			vector<int> rec_id;
			if(sample_size > 4095 * 4095)
				sample_size = 4095 * 4095;
			int quo = sample_size / ((1 << 12) - 1);
			int rem = sample_size % ((1 << 12) - 1);

			for(int j = 1; j < (1 << 12); ++j)
			{
				if(prgdialog_sub -> wasCanceled())  abort(true);
				prgdialog_sub -> setValue(j);

				int size = (j <= rem) ? (quo + 1) : quo;
				rec_id.clear();
				while((int)rec_id.size() < size)
				{
					int id = rand(1, (1 << 12) - 1);
					int pos = find(rec_id, id);
					if(pos != -1)
						rec_id.insert( rec_id.begin() + pos, id );
				}
				for(int i = 0; i < size; ++i)
				{
					id_to_notes(j, _notes);
					sub_library.push_back(_notes);
					id_to_notes(rec_id[i], _notes);
					sub_library.push_back(_notes);
				}
			}
		}
	}
	else
	{
		for(int id = 1; id < (1 << 12); ++id)
		{
			id_to_notes(id, _notes);
			sub_library.push_back(_notes);
		}
	}
}

bool Chord::valid_sub(Chord& chord1, Chord& chord2)
// progression direction: chord2 -> chord1; data is saved in chord1
// const char var[VAR_TOTAL] = {'P', 'N', 'T', 'K', 'C', 'a', 'A', 'm', 'h', 'g', 'S', 'Q', 'X', 'k', 'R', 'V'};
// name of parameters (a = S, A = SS, S = sv, k = KK)
{
	if( (strchr(sort_order_sub, var[0]) != nullptr) &&
		 (chord1.sim_orig < p_min_sub || chord1.sim_orig > p_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[1]) != nullptr) &&
		 (chord1.s_size < n_min_sub || chord1.s_size > n_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[2]) != nullptr) &&
		 (chord1.tension < t_min_sub || chord1.tension > t_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[14]) != nullptr) &&
		 (chord1.root < r_min_sub || chord1.root > r_max_sub) )
	return false;

	chord2.find_vec(chord1, false, true);
	if(object == Antechord)
		chord1.inverse_param();

	if( (strchr(sort_order_sub, var[3]) != nullptr) &&
		 (chord1.chroma < k_min_sub || chord1.chroma > k_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[4]) != nullptr) &&
		 (chord1.common_note < c_min_sub || chord1.common_note > c_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[5]) != nullptr) &&
		 (chord1.span < s_min_sub || chord1.span > s_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[6]) != nullptr) &&
		 (chord1.sspan < ss_min_sub || chord1.sspan > ss_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[10]) != nullptr) &&
		 (chord1.sv < sv_min_sub || chord1.sv > sv_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[11]) != nullptr) &&
		 (chord1.Q_indicator < q_min_sub || chord1.Q_indicator > q_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[12]) != nullptr) &&
		 (chord1.similarity < x_min_sub || chord1.similarity > x_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[13]) != nullptr) &&
		 ( (chord1.chroma_old - chord1.prev_chroma_old) < kk_min_sub ||
			(chord1.chroma_old - chord1.prev_chroma_old) > kk_max_sub ) )
	return false;

	if(strchr(sort_order_sub, var[15]) != nullptr &&
		enable_rm && rm_priority[chord1.root_movement] == -1)
	return false;

	return true;
}

bool Chord::valid_single_chord(Chord& chord)
{
	if( (strchr(sort_order_sub, var[0]) != nullptr) &&
		 (chord.sim_orig < p_min_sub || chord.sim_orig > p_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[1]) != nullptr) &&
		 (chord.s_size < n_min_sub || chord.s_size > n_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[2]) != nullptr) &&
		 (chord.tension < t_min_sub || chord.tension > t_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[5]) != nullptr) &&
		 (chord.span < s_min_sub || chord.span > s_max_sub) )
	return false;

	if( (strchr(sort_order_sub, var[14]) != nullptr) &&
		 (chord.root < r_min_sub || chord.root > r_max_sub) )
	return false;

	return true;
}

void Chord::print_sub()
{
	Chord antechord(reduced_ante_notes, 0);
	Chord postchord(reduced_post_notes, antechord.get_chroma_old());
	antechord.find_vec(postchord, false, true);
	fout << ((language == English) ? "Original: " : "原进行：");
	print_substitution(sort_order_sub, true, true, antechord, postchord, language);

	fout << ((language == English) ? "Substitute: " : "替代：");
	switch(object)
	{
		case Postchord:  fout << ((language == English) ? "Postchord\n"  : "后和弦\n");  break;
		case Antechord:  fout << ((language == English) ? "Antechord\n"  : "前和弦\n");  break;
		case BothChords: fout << ((language == English) ? "BothChords\n" : "两者\n");    break;
	}

	int *pReset[VAR_TOTAL]  = {&p_reset_value, &n_reset_value,  &t_reset_value,  &k_reset_value,
										&c_reset_value, &s_reset_value,  &ss_reset_value,  nullptr,
										 nullptr,        nullptr,        &sv_reset_value, &q_reset_value,
										&x_reset_value, &kk_reset_value, &r_reset_value,   nullptr};
	int *pRadius[VAR_TOTAL] = {&p_radius, &n_radius,  &t_radius,  &k_radius,
										&c_radius, &s_radius,  &ss_radius, nullptr,
										nullptr,   nullptr,   &sv_radius,  &q_radius,
										&x_radius, &kk_radius, &r_radius,  nullptr};
	const char _var[VAR_TOTAL][3] = {"P", "N", "T", "K", "C", "S", "SS", "M", "H", "G", "sv", "Q", "X", "KK", "R", "V"};
	int count = 0;

	fout << ((language == English) ? "Conditions: " : "条件：");
	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(strchr(sort_order_sub, var[i]) != nullptr)
		{
			if(count != 0)  fout << ",  ";
			fout << _var[i];
			if(i != 15)  // V
			{
				if(strchr(reset_list, var[i]) != nullptr)
					fout << " = " << *pReset[i];
				else  fout << " = val";
				fout << "±" << *pRadius[i];
				if(strchr(percentage_list, var[i]) != nullptr)
					fout << "%";
			}
			++count;
		}
	}

	fout << ((language == English) ? "\n\nSubstitutions:\n" : "\n\n替代结果：\n");
	if(sub_size == 0)
		fout << ((language == English) ? "\n\nNo substitutions found in this condition.\n" : "在该条件下未找到结果。\n");
	if(object == Postchord)
	{
		for(int i = 0; i < sub_size; ++i)
			print_substitution(sort_order_sub, false, true, antechord, record_post[i], language);
	}
	else if(object == Antechord)
	{
		for(int i = 0; i < sub_size; ++i)
			print_substitution(sort_order_sub, true, false, record_ante[i], postchord, language);
	}
	else if(object == BothChords)
	{
		for(int i = 0; i < sub_size; ++i)
		{
			int j = record_post[i].orig_pos;
			print_substitution(sort_order_sub, true, true, record_ante[j], record_post[i], language);
		}
	}

	if(language == Chinese)
	{
		fout << "\n分析报告结果指标说明：\n"
			  << "【每个和弦】(音名列表) - 系统判断和弦音名； t - 和弦紧张度； s - 和弦纯五跨度； n - 和弦音数； r - 和弦根音（欣氏法）。\n"
			  << "【和弦进行】k - 华氏色彩度； kk - 毛色彩度； c - 进行共同音个数；\n"
				  "ss - 相邻和弦的合跨度； sv - 声部运动总大小；\n"
				  "Q - 进行的陈氏Q指标； x - 横向相似度； dt, dr, ds, dn - 相应各项指标的变化量。\n"
			  << "【星号注解】* - 等音记谱（色值溢出）； ** - 等音记谱（色差溢出）。\n";
	}
	fout << "\n==========\n\n";
	print_stats_sub();
	end_sub = clock();
	double dur = (double) (end_sub - begin_sub) / CLOCKS_PER_SEC;
	if(language == English)
		fout << "\n" << sub_size << " substitution(s) found in " << fixed << setprecision(2) << dur << " seconds.";
	else  fout << "\n共找到" << sub_size << "个替代进行，耗时 " << fixed << setprecision(2) << dur << " 秒。";
	fout.close();
}

void Chord::print_stats_sub()
{
	if(sub_size == 0)
	{
		if(language == English)
			fout << "Voice leading stats:\nNo substitutions found in this condition.\n\n"
				  << "Other stats:\nNo substitutions found in this condition.\n";
		else
			fout << "声部进行统计：\n在该条件下未找到结果。\n\n"
				  << "其他统计：\n在该条件下未找到结果。\n";
		return;
	}

	vector<ChordData>* ptr;
	if(object == Antechord)
		ptr = &record_ante;
	else
		ptr = &record_post;

	vector<Movement> movement;
	movement.resize(13);
	for(int i = 0; i < 13; ++i)
		movement[i].amount = i - 6;

	Chord antechord(reduced_ante_notes, 0);
	Chord postchord(reduced_post_notes, antechord.get_chroma_old());
	int cardinal_change = 0;
	for(int i = 0; i < sub_size; ++i)
	{
		for(int j = 0; j < (int)(*ptr)[i].get_vec().size(); ++j)
		{
			int num = (*ptr)[i].get_vec()[j];
			++movement[num + 6].instance;
		}

		ChordData* ptr_;
		switch(object)
		{
			case Postchord:  ptr_ = &antechord;  break;
			case Antechord:  ptr_ = &postchord;  break;
			case BothChords: ptr_ = &(record_post[i]);  break;
		}
		if( (*ptr)[i].get_s_size() != ptr_ -> get_s_size())
			++cardinal_change;
	}

	if(language == English)
		fout << "Voice leading stats:\n"
			  << "Movement instances: " << "[0]  " << movement[6].instance << endl;
	else  fout << "声部进行统计：\n声部动向及相应出现次数：[0]  " << movement[6].instance << endl;
	for(int i = 1; i <= 6; ++i)
		fout << "[+" << i << "] " << movement[i + 6].instance << "  ";
	fout << endl;
	for(int i = -1; i >= -6; --i)
		fout << "[" << i << "] " << movement[i + 6].instance << "  ";

	int movement_instance = 0;
	for(int i = 0; i < 13; ++i)
		movement_instance += movement[i].instance;
	for(int i = 0; i < 13; ++i)
		movement[i].percentage = (double)movement[i].instance / movement_instance;

	vector<Movement> sorted;
	sorted.assign(movement.begin(), movement.end());
	merge_sort(sorted.begin(), sorted.end(), *larger_perc);
	if(language == English)
		fout << "\nMovement percentage (sorted H -> L):\n";
	else  fout << "\n声部动向频次占比（从高到低）：\n";
	for(int i = 0; i < 13; ++i)
	{
		fout << "[";
		if(sorted[i].amount > 0)  fout << "+";
		fout << sorted[i].amount << "] " << round(sorted[i].percentage * 100.0) << "%  ";
	}
	if(language == English)
		fout << "\nCardinal change instances: ";
	else  fout << "\n和弦音个数 (N) 发生变化的次数：";
	fout << cardinal_change << " (" << round((double)cardinal_change / sub_size * 100.0) << "%)\n\n";

	int _x_max = 0, _x_min = INF, x_sum = 0, x_max_index = 0, x_min_index = 0;
	int _p_max = 0, _p_min = INF, p_sum = 0, p_max_index = 0, p_min_index = 0;
	int _c_max = 0, _c_min = INF, c_sum = 0, c_max_index = 0, c_min_index = 0;
	int _n_max = 0, _n_min = INF, n_sum = 0, n_max_index = 0, n_min_index = 0;
	int _r_max = 0, _r_min = INF, r_sum = 0, r_max_index = 0, r_min_index = 0;
	int _s_max = 0, _s_min = INF, s_sum = 0, s_max_index = 0, s_min_index = 0;
	int _ss_max = 0,  _ss_min = INF, ss_sum = 0, ss_max_index = 0, ss_min_index = 0;
	int _sv_max = 0,  _sv_min = INF, sv_sum = 0, sv_max_index = 0, sv_min_index = 0;
	int dr_max = MINF, dr_min = INF, dr_sum = 0, dr_max_index = 0, dr_min_index = 0;
	int ds_max = MINF, ds_min = INF, ds_sum = 0, ds_max_index = 0, ds_min_index = 0;
	int dn_max = MINF, dn_min = INF, dn_sum = 0, dn_max_index = 0, dn_min_index = 0;
	double  _t_max = 0.0,  _t_min = INF,  t_sum = 0, _k_max = 0.0,  _k_min = INF,  k_sum = 0;
	double  _q_max = 0.0,  _q_min = INF,  q_sum = 0;
	double _kk_max = 0.0, _kk_min = INF, kk_sum = 0, dt_max = MINF, dt_min = INF, dt_sum = 0;
	int  t_max_index = 0,  t_min_index = 0,  k_max_index = 0,  k_min_index = 0;
	int  q_max_index = 0,  q_min_index = 0;
	int kk_max_index = 0, kk_min_index = 0, dt_max_index = 0, dt_min_index = 0;
	int temp1;  double temp2;

	for(int i = 0; i < sub_size; ++i)
	{
		ChordData* ptr_;
		switch(object)
		{
			case Postchord:  ptr_ = &antechord;  break;
			case Antechord:  ptr_ = &postchord;  break;
			case BothChords: ptr_ = &(record_post[i]);  break;
		}

		temp2 = abs((*ptr)[i].get_chroma());
		if(temp2 > _k_max)  { _k_max = temp2;  k_max_index = i; }
		if(temp2 < _k_min)  { _k_min = temp2;  k_min_index = i; }
		k_sum += temp2;

		temp2 = (*ptr)[i].get_Q_indicator();
		if(temp2 > _q_max)  { _q_max = temp2;  q_max_index = i; }
		if(temp2 < _q_min)  { _q_min = temp2;  q_min_index = i; }
		q_sum += temp2;

		temp1 = (*ptr)[i].get_similarity();
		if(temp1 > _x_max)  { _x_max = temp1;  x_max_index = i; }
		if(temp1 < _x_min)  { _x_min = temp1;  x_min_index = i; }
		x_sum += temp1;

		temp1 = (*ptr)[i].get_sim_orig();
		if(temp1 > _p_max)  { _p_max = temp1;  p_max_index = i; }
		if(temp1 < _p_min)  { _p_min = temp1;  p_min_index = i; }
		p_sum += temp1;

		temp1 = (*ptr)[i].get_common_note();
		if(temp1 > _c_max)  { _c_max = temp1;  c_max_index = i; }
		if(temp1 < _c_min)  { _c_min = temp1;  c_min_index = i; }
		c_sum += temp1;

		temp1 = (*ptr)[i].get_sspan();
		if(temp1 > _ss_max)  { _ss_max = temp1;  ss_max_index = i; }
		if(temp1 < _ss_min)  { _ss_min = temp1;  ss_min_index = i; }
		ss_sum += temp1;

		temp1 = (*ptr)[i].get_sv();
		if(temp1 > _sv_max)  { _sv_max = temp1;  sv_max_index = i; }
		if(temp1 < _sv_min)  { _sv_min = temp1;  sv_min_index = i; }
		sv_sum += temp1;

		temp1 = (*ptr)[i].get_s_size();
		if(temp1 > _n_max)  { _n_max = temp1;  n_max_index = i; }
		if(temp1 < _n_min)  { _n_min = temp1;  n_min_index = i; }
		n_sum += temp1;

		temp2 = (*ptr)[i].get_tension();
		if(temp2 > _t_max)  { _t_max = temp2;  t_max_index = i; }
		if(temp2 < _t_min)  { _t_min = temp2;  t_min_index = i; }
		t_sum += temp2;

		temp1 = (*ptr)[i].get_root();
		if(temp1 > _r_max)  { _r_max = temp1;  r_max_index = i; }
		if(temp1 < _r_min)  { _r_min = temp1;  r_min_index = i; }
		r_sum += temp1;

		temp1 = (*ptr)[i].get_span();
		if(temp1 > _s_max)  { _s_max = temp1;  s_max_index = i; }
		if(temp1 < _s_min)  { _s_min = temp1;  s_min_index = i; }
		s_sum += temp1;

		temp2 = (*ptr)[i].get_chroma_old() - ptr_ -> get_chroma_old();
		if(temp2 > dt_max)  { _kk_max = temp2;  kk_max_index = i; }
		if(temp2 < dt_min)  { _kk_min = temp2;  kk_min_index = i; }
		kk_sum += temp2;

		temp2 = (*ptr)[i].get_tension() - ptr_ -> get_tension();
		if(temp2 > dt_max)  { dt_max = temp2;  dt_max_index = i; }
		if(temp2 < dt_min)  { dt_min = temp2;  dt_min_index = i; }
		dt_sum += temp2;

		temp1 = (*ptr)[i].get_root() - ptr_ -> get_root();
		if(temp1 > dr_max)  { dr_max = temp1;  dr_max_index = i; }
		if(temp1 < dr_min)  { dr_min = temp1;  dr_min_index = i; }
		dr_sum += temp1;

		temp1 = (*ptr)[i].get_span() - ptr_ -> get_span();
		if(temp1 > ds_max)  { ds_max = temp1;  ds_max_index = i; }
		if(temp1 < ds_min)  { ds_min = temp1;  ds_min_index = i; }
		ds_sum += temp1;

		temp1 = (*ptr)[i].get_s_size() - ptr_ -> get_s_size();
		if(temp1 > dn_max)  { dn_max = temp1;  dn_max_index = i; }
		if(temp1 < dn_min)  { dn_min = temp1;  dn_min_index = i; }
		dn_sum += temp1;
	}

	QStringList str[22] = { {"Other stats:\n", "其他统计：\n"},
									{"Absolute value of chroma value (Hua) (|k|): ",  "华氏色彩度绝对值 (|k|): "},
									{"Absolute value of gross chroma value (|kk|): ", "华氏毛色彩度绝对值 (|kk|): "},
									{"Q-Indicator value (Chen) (Q): ",                "进行的陈氏Q指标 (Q): "},
									{"Lateral similarity (X%): ",                     "横向相似度 (X%): "},
									{"Chord similarity (P%): ",                       "和弦相似度 (P%): "},
									{"Number of common tones (C): ",                  "进行共同音个数 (C): "},
									{"Unioned chord span in fifths (SS): ",           "相邻和弦的合跨度 (SS): "},
									{"Total voice leading (Σvec): ",                  "声部运动总大小 (Σvec): "},
									{"Chord cardinality (N): ",                       "和弦音数 (n): "},
									{"Chord tension (T): ",                           "紧张度 (t): "},
									{"Root (r): ",                                    "和弦根音（欣氏法）(R): "},
									{"Chord span in fifths (S): ",                    "和弦纯五跨度 (s): "},
									{"Difference of tension (dt): ",                  "紧张度之差 (dt): "},
									{"Difference of root (dr): ",                     "根音之差 (dr): "},
									{"Difference of chord span in fifths (ds): ",     "和弦跨度之差 (ds): "},
									{"Difference of chord cardinality (dn): ",        "和弦音集音数之差 (dn): "},
									{"highest = %1 (@ #%2); ", "最高 = %1 (@ #%2)；"},
									{"lowest = %1 (@ #%2); ",  "最低 = %1 (@ #%2)；"},
									{"average = %1;\n",        "平均 = %1；\n"},
									{"most = %1 (@ #%2); ",    "最多 = %1 (@ #%2)；"},
									{"least = %1 (@ #%2); ",   "最少 = %1 (@ #%2)；"} };

	// const char var[VAR_TOTAL] = {'P', 'N', 'T', 'K', 'C', 'a', 'A', 'm', 'h', 'g', 'S', 'Q', 'X', 'k', 'R', 'V'};
	// name of parameters (a = S, A = SS, S = sv, k = KK)
	fout << str[0][language].toUtf8().data();

	if(strchr(sort_order_sub, var[3]) != nullptr)
	fout << str[1][language].toUtf8().data()
		  << str[17][language].arg(_k_max, 0, 'f', 1).arg(k_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_k_min, 0, 'f', 1).arg(k_min_index + 1).toUtf8().data()
		  << str[19][language].arg(k_sum / sub_size, 0, 'f', 1).toUtf8().data();

	if(strchr(sort_order_sub, var[13]) != nullptr)
	fout << str[2][language].toUtf8().data()
		  << str[17][language].arg(_kk_max, 0, 'f', 2).arg(kk_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_kk_min, 0, 'f', 2).arg(kk_min_index + 1).toUtf8().data()
		  << str[19][language].arg(kk_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[11]) != nullptr)
	fout << str[3][language].toUtf8().data()
		  << str[17][language].arg(_q_max, 0, 'f', 1).arg(q_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_q_min, 0, 'f', 1).arg(q_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)q_sum / sub_size, 0, 'f', 1).toUtf8().data();

	if(strchr(sort_order_sub, var[12]) != nullptr)
	fout << str[4][language].toUtf8().data()
		  << str[17][language].arg(_x_max).arg(x_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_x_min).arg(x_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)x_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[0]) != nullptr)
	fout << str[5][language].toUtf8().data()
		  << str[17][language].arg(_p_max).arg(p_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_p_min).arg(p_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)p_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[4]) != nullptr)
	fout << str[6][language].toUtf8().data()
		  << str[20][language].arg(_c_max).arg(c_max_index + 1).toUtf8().data()
		  << str[21][language].arg(_c_min).arg(c_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)c_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[6]) != nullptr)
	fout << str[7][language].toUtf8().data()
		  << str[17][language].arg(_ss_max).arg(ss_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_ss_min).arg(ss_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)ss_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[10]) != nullptr)
	fout << str[8][language].toUtf8().data()
		  << str[17][language].arg(_sv_max).arg(sv_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_sv_min).arg(sv_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)sv_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[1]) != nullptr)
	fout << str[9][language].toUtf8().data()
		  << str[20][language].arg(_n_max).arg(n_max_index + 1).toUtf8().data()
		  << str[21][language].arg(_n_min).arg(n_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)n_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[2]) != nullptr)
	fout << str[10][language].toUtf8().data()
		  << str[17][language].arg(_t_max, 0, 'f', 1).arg(t_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_t_min, 0, 'f', 1).arg(t_min_index + 1).toUtf8().data()
		  << str[19][language].arg(t_sum / sub_size, 0, 'f', 1).toUtf8().data();

	if(strchr(sort_order_sub, var[14]) != nullptr)
	fout << str[11][language].toUtf8().data()
		  << str[17][language].arg(_r_max).arg(r_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_r_min).arg(r_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)r_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[5]) != nullptr)
	fout << str[12][language].toUtf8().data()
		  << str[17][language].arg(_s_max).arg(s_max_index + 1).toUtf8().data()
		  << str[18][language].arg(_s_min).arg(s_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)s_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[2]) != nullptr)
	fout << str[13][language].toUtf8().data()
		  << str[17][language].arg(dt_max, 0, 'f', 1).arg(dt_max_index + 1).toUtf8().data()
		  << str[18][language].arg(dt_min, 0, 'f', 1).arg(dt_min_index + 1).toUtf8().data()
		  << str[19][language].arg(dt_sum / sub_size, 0, 'f', 1).toUtf8().data();

	if(strchr(sort_order_sub, var[14]) != nullptr)
	fout << str[14][language].toUtf8().data()
		  << str[17][language].arg(dr_max).arg(dr_max_index + 1).toUtf8().data()
		  << str[18][language].arg(dr_min).arg(dr_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)dr_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[5]) != nullptr)
	fout << str[15][language].toUtf8().data()
		  << str[17][language].arg(ds_max).arg(ds_max_index + 1).toUtf8().data()
		  << str[18][language].arg(ds_min).arg(ds_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)ds_sum / sub_size, 0, 'f', 2).toUtf8().data();

	if(strchr(sort_order_sub, var[1]) != nullptr)
	fout << str[16][language].toUtf8().data()
		  << str[17][language].arg(dn_max).arg(dn_max_index + 1).toUtf8().data()
		  << str[18][language].arg(dn_min).arg(dn_min_index + 1).toUtf8().data()
		  << str[19][language].arg((double)dn_sum / sub_size, 0, 'f', 2).toUtf8().data();
}

void Chord::to_midi_sub()
{
	Chord antechord(reduced_ante_notes, 0);
	Chord postchord(reduced_post_notes, antechord.get_chroma_old());
	int note_count = reduced_ante_notes.size() + reduced_post_notes.size();
	for(int i = 0; i < sub_size; ++i)
	{
		switch(object)
		{
			case Postchord:  note_count += (antechord.s_size + record_post[i].get_s_size());  break;
			case Antechord:  note_count += (record_ante[i].get_s_size() + postchord.s_size);  break;
			case BothChords: note_count += (record_ante[i].get_s_size() + record_post[i].get_s_size());  break;
		}
	}

	midi_head(2 * sub_size + 2, note_count);
	chord_to_midi(antechord.notes);
	chord_to_midi(postchord.notes);
	for(int i = 0; i < sub_size; ++i)
	{
		switch(object)
		{
			case Postchord:  chord_to_midi(antechord.notes);  chord_to_midi(record_post[i].get_notes());  break;
			case Antechord:  chord_to_midi(record_ante[i].get_notes());  chord_to_midi(postchord.notes);  break;
			case BothChords:
			{
				int j = record_post[i].orig_pos;
				chord_to_midi(record_ante[j].get_notes());
				chord_to_midi(record_post[i].get_notes());
				break;
			}
		}
	}
	m_fout.write("\x00\xFF\x2F\x00", 4);
	m_fout.close();
}

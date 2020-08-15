// SmartChordGen v2.5 [Build: 2020.8.8]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// chord.cpp

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "chord.h"
#include "functions.h"
using namespace std;

double _tension[12] = {0.0, 16.0, 8.0, 4.0, 2.0, 1.0, 16.0, 1.0, 2.0, 4.0, 8.0, 16.0};
int restriction[12] = {0, 53, 53, 51, 50, 51, 52, 39, 51, 50, 51, 52};
vector<int> overall_scale = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

void Chord::next(vector<int>& orig_vec)
// used for iteration in 'set_new_chords'
{
	int index = 0;
	while(index < m_max)
	{
		if(orig_vec[index] == vl_max)
			orig_vec[index++] = (-1) * vl_max;
		else
		{
			if(vl_min != 0 && orig_vec[index] == (-1) * vl_min)
				orig_vec[index] = vl_min;
			else  ++orig_vec[index];
			break;
		}
	}
}

void Chord::expand(Chord& expansion, const int& target_size, const int& index)
// expand 'notes' to 'target_size' by using expansion method #'index'
{
	expansion.notes.clear();
	for(int i = 0; i < target_size; ++i)
	{
		int note = notes[ expansion_indexes[t_size][target_size][index][i] ];
		expansion.notes.push_back(note);
	}
	expansion.t_size = target_size;
}

bool Chord::valid(Chord& new_chord)
// checks various conditions
{
	int i, pos = 1;
	for(i = 1; i < new_chord.t_size; ++i)
		if(new_chord.notes[i - 1] > new_chord.notes[i])
			return false;

	if(*new_chord.notes.begin() < lowest || *new_chord.notes.rbegin() > highest)
		return false;

	remove_duplicate(new_chord.notes);
	set_g_center(new_chord);
	if(new_chord.g_center > g_max || new_chord.g_center < g_min)
		return false;

	new_chord.set_param1();
	if(align_mode != Unlimited && !valid_alignment(new_chord))
		return false;
	if(enable_ex && !valid_exclusion(new_chord))
		return false;
	if(enable_pedal && continual && !include_pedal(new_chord))
		return false;
	if(new_chord.t_size < m_min || new_chord.t_size > m_max)
		return false;
	if(new_chord.s_size < n_min || new_chord.s_size > n_max)
		return false;
	if(new_chord.thickness > h_max || new_chord.thickness < h_min)
		return false;
	if(new_chord.root > r_max || new_chord.root < r_min)
		return false;

	vector<int> intersection = intersect(new_chord.note_set, overall_scale, true);
	if((int)intersection.size() < new_chord.s_size)  return false;

	new_chord.common_note = intersect(new_chord.notes, notes, true).size();
	if(continual && (new_chord.common_note > c_max || new_chord.common_note < c_min))
		return false;

	pos = find(bass_avail, new_chord.alignment[0]);
	if(pos != -1)  return false;
	pos = find(chord_library, new_chord.set_id);
	if(pos != -1)  return false;

	if(unique_mode == RemoveDupType)
	{
		pos = find(rec_id, new_chord.set_id);
		if(pos == -1)  return false;
	}

	find_vec(new_chord);
	if(!valid_vec(new_chord))
		return false;
	if(new_chord.sv < sv_min || new_chord.sv > sv_max)
		return false;
	if(enable_rm && rm_priority[new_chord.root_movement] == -1)
		return false;
	if(!valid_sim(new_chord))
		return false;

	set_vec_id(new_chord);
	pos = find(vec_ids, new_chord.vec_id);
	if(pos == -1)    return false;

	vec_ids.insert(vec_ids.begin() + pos, new_chord.vec_id);
	if(unique_mode == RemoveDupType && !continual)
		note_set_to_id(new_chord.note_set, rec_id);
	return true;
}

bool Chord::valid_alignment(Chord& chord)
{
	if(align_mode == List)
	{
		for(int i = 0; i < (int)alignment_list.size(); ++i)
			if(alignment_list[i] == chord.alignment)
				return true;
		return false;
	}
	else
	{
		if(chord.notes[1] - chord.notes[0] < i_low)  return false;
		if(chord.notes[t_size - 1] - chord.notes[t_size - 2] > i_high)
			return false;
		for(int i = 2; i <= t_size - 2; ++i)
		{
			int interval = chord.notes[i] - chord.notes[i - 1];
			if( interval > i_max || interval < i_min )
				return false;
		}
		return true;
	}
}

bool Chord::include_pedal(Chord& chord)
{
	if(in_bass)
	{
		for(int i = 0; i < (int)pedal_notes.size(); ++i)
			if(chord.notes[i] != pedal_notes[i])
				return false;
		return true;
	}
	else
	{
		vector<int> intersection;
		if(record.size() % period == 0)
		{
			intersection = intersect(pedal_notes_set, chord.note_set, true);
			if(intersection.size() != pedal_notes_set.size())
				return false;
			if(realign && record.size() != 0)
			{
				intersection = intersect(pedal_notes, chord.notes, true);
				if(intersection.size() == pedal_notes.size())
					return false;
			}
			return true;
		}
		else
		{
			intersection = intersect(pedal_notes, chord.notes, true);
			return (intersection.size() == pedal_notes.size());
		}
	}
}

bool Chord::valid_vec(Chord& chord)
{
	int v_size = chord.vec.size(), i;
	for(i = 0; i < v_size; ++i)
	{
		int temp = abs(chord.vec[i]);
		if(temp < vl_min || temp > vl_max)
			return false;
	}
	
	switch(vl_setting)
	{
		case Default:
			for(i = 1; i < v_size; ++i)
				if(chord.vec[i] == 0 || (sign(chord.vec[i - 1]) != sign(chord.vec[i])) )
					break;
			if(i == v_size)  return false;
			return true;

		case Number: return (chord.steady_count <= steady_max) && (chord.steady_count >= steady_min)
							  && (chord.ascending_count  <=  ascending_max) && (chord.ascending_count  >=  ascending_min)
							  && (chord.descending_count <= descending_max) && (chord.descending_count >= descending_min);

		case Percentage: return (chord.steady_count <= steady_max * chord.t_size)
									&& (chord.steady_count >= steady_min * chord.t_size)
									&& (chord.ascending_count <= ascending_max * chord.t_size)
									&& (chord.ascending_count >= ascending_min * chord.t_size)
									&& (chord.descending_count <= descending_max * chord.t_size)
									&& (chord.descending_count >= descending_min * chord.t_size);
	}
}

bool Chord::valid_exclusion(Chord& chord)
{
	if(intersect(chord.notes, exclusion_notes, true).size() != 0)
		return false;

	if(find(exclusion_roots, chord.root) == -1)
		return false;

	int size = exclusion_intervals.size();
	if(size > 0)
	{
		vector<int> diffs;
		for(int i = 0; i < chord.t_size; ++i)
		{
			for(int j = i + 1; j < chord.t_size; ++j)
			{
				int diff = chord.notes[j] - chord.notes[i];
				diffs.push_back(diff);
			}
		}

		int octave;
		vector<int> num(size, 0);
		for(int i = 0; i < (int)diffs.size(); ++i)
		{
			for(int j = 0; j < size; ++j)
			{
				int temp1 = diffs[i] - exclusion_intervals[j].interval;
				int temp2 = diffs[i] + exclusion_intervals[j].interval - 12;
				if(temp1 % 12 == 0)
				{
					octave = temp1 / 12;
					if(octave >= exclusion_intervals[j].octave_min && octave <= exclusion_intervals[j].octave_max)
						++num[j];
				}
				else if(temp2 % 12 == 0)
				{
					octave = temp2 / 12;
					if(octave >= exclusion_intervals[j].octave_min && octave <= exclusion_intervals[j].octave_max)
						++num[j];
				}
			}
		}

		for(int j = 0; j < size; ++j)
		{
			if(num[j] <= exclusion_intervals[j].num_max && num[j] >= exclusion_intervals[j].num_min)
				return false;
		}
	}
	return true;
}

bool Chord::valid_sim(Chord& chord)
{
	if(chord.similarity < x_min || chord.similarity > x_max)
		return false;

	if(enable_sim)
	{
		Chord copy1, copy2(chord);
		for(int i = 0; i < (int)sim_period.size(); ++i)
		{
			if((int)record.size() >= sim_period[i])
			{
				copy1.notes = (record.end() - sim_period[i]) -> get_notes();
				copy1.t_size = copy1.notes.size();
				copy1.root = (record.end() - sim_period[i]) -> get_root();
				copy1.find_vec(copy2);
				set_similarity(copy1, copy2, sim_period[i]);
				if(copy2.similarity < sim_min[i] || copy2.similarity > sim_max[i])
					return false;
			}
		}
	}
	return true;
}

void Chord::set_vec_id(Chord& chord)
{
	chord.vec_id = 0;
	long long exp = 1;
	for(int i = 0; i < (int)chord.vec.size(); ++i)
	{
		chord.vec_id += (chord.vec[i] + vl_max) * exp;
		exp *= (2 * vl_max + 1);
	}
}
// e.g. for vl_max = 4, (vl_min = 1), vec = [2, 3, -2, 0, 4],
//		  vec_id = 84276 in base 9 = 55635 in base 10.

void Chord::set_g_center(Chord& chord)
{
	double temp = 0;
	for(int i = 0; i < chord.t_size; ++i)
		temp += chord.notes[i];
	temp /= (double)chord.t_size;
	temp = (temp - lowest) / (highest - lowest);
	temp = round(temp * 100.0);
	chord.g_center = temp;
}

void Chord::set_similarity(Chord& chord1, Chord& chord2, const int& period = 1)
{
	double temp = vl_max * period * max(chord1.t_size, chord2.t_size);
	temp = pow(1.0 - chord2.sv / temp, period);
	if(chord1.root == chord2.root)
		temp = sqrt(temp);
	chord2.similarity = round(100.0 * temp);
}

void Chord::set_param1()
// these are parameters related only to the chord itself
{
	note_set.clear();
	t_size = notes.size();
	for(int i = 0; i < t_size; ++i)
		note_set.push_back(notes[i] % 12);
	bubble_sort(note_set);
	remove_duplicate(note_set);

	t_size = notes.size();
	s_size = note_set.size();
	root = find_root(notes);

	alignment.clear();
	for(int i = 0; i < t_size; ++i)
	{
		int diff = (notes[i] - root) % 12;
		if(diff < 0)  diff += 12;
		alignment.push_back(note_pos[diff]);
	}

	set_id = 0;
	for(int i = 0; i < s_size; ++i)
		set_id += (1 << note_set[i]);

	chroma = 0.0;
	for(int i = 0; i < s_size; ++i)
		chroma = chroma + 6 - (5 * note_set[i] + 6) % 12;
	// You can check the result for notes from 0 to 11.
	chroma /= (double)s_size;
	chroma = floor(chroma * 100) / 100.0;

	self_diff.clear();
	for(int i = 1; i < s_size; ++i)
		self_diff.push_back(note_set[i] - note_set[i - 1]);

	vector<int> d_all;
	for(int i = 0; i < s_size; ++i)
		for(int j = i + 1; j < s_size; ++j)
		{
			int temp = min(note_set[j] - note_set[i], 12 + note_set[i] - note_set[j]);
			d_all.push_back(temp);
		}

	count_vec.assign(6, 0);
	for(int i = 0; i < (int)d_all.size(); ++i)
		++count_vec[d_all[i] - 1];

	tension = 0.0;
	for(int i = 0; i < t_size; ++i)
		for(int j = i + 1; j < t_size; ++j)
		{
			int diff = notes[j] - notes[i];
			double temp = _tension[diff % 12] / ((diff / 12) + 1.0);
			if(notes[j] < restriction[diff % 12])
				temp = temp * restriction[diff % 12] / notes[j];
			tension += temp;
		}
	tension = floor(tension * 100) / 100.0;

	thickness = 0.0;
	for(int i = 0; i < t_size; ++i)
		for(int j = i + 1; j < t_size; ++j)
			if((notes[j] - notes[i]) % 12 == 0)
				thickness += ( 12.0 / (double)(notes[j] - notes[i]) );
}

void Chord::set_param2(Chord& chord)
// these are parameters related to both the chord itself and the generated chord
{
	chord.ascending_count = 0;
	chord.steady_count = 0;
	chord.descending_count = 0;
	for(int i = 0; i < (int)chord.vec.size(); ++i)
	{
		if(chord.vec[i] > 0)  ++chord.ascending_count;
		else if(chord.vec[i] == 0)  ++chord.steady_count;
		else if(chord.vec[i] < 0)   ++chord.descending_count;
	}

	chord.root_movement = (chord.root - root + 12) % 12;
	if(chord.root_movement > 6)
		chord.root_movement = 12 - chord.root_movement;

	set_similarity(*this, chord);
}

void Chord::set_new_chords(Chord& chord)
{
	vector<int> orig_vec;
	for(int i = 0; i < m_max; ++i)
		orig_vec.push_back(-vl_max);
	// This is the (movement) vector and from it we can get a new chord.
	// However, a new chord may correspond to multiple movement vectors.
	// 'orig_vec' may not be of the simplest form among all equivalent movement vectors,
	// so it can not be directly used.
	// The movement vector is determined in 'find_vec'.
#ifdef QT_CORE_LIB
	long long step = min(max_cnt / 100, (long long)1E7);
#else
	long long step = max_cnt / 100;
#endif

	for(long long count = 0; count < max_cnt; ++count)
	{
		Chord new_chord(chord);
		for(int i = 0; i < m_max; ++i)
			new_chord.notes[i] += orig_vec[i];
		if( valid(new_chord) )
		{
			new_chords.push_back( static_cast<ChordData>(new_chord) );
			++c_size;
		}
		next(orig_vec);

		if(count % step == 0)
		{
#ifdef QT_CORE_LIB
			if(continual)
			{
				QStringList str1 = {"Progression #", "进行 #"};
				QString str2;
				labeltext = str1[language] + str2.setNum(progr_count);
			}
			else  labeltext.clear();
			double temp = exp_count - 1 + (double)count / max_cnt;
			set_est_time(temp * 1000);
			prgdialog -> setLabelText(labeltext);
			if(prgdialog -> wasCanceled())  abort();
			prgdialog -> setValue(temp * 1000);
#else
			cout << "\b\b\b" << setw(2) << count / step / 5 << "%";
#endif
		}
	}
}

void Chord::set_max_count()
{
	int choice;
	max_cnt = 1;
	if(vl_min == 0)  choice = 2 * vl_max + 1;
	else  choice = 2 * (vl_max - vl_min + 1);
	for(int i = 0; i < m_max; ++i)
		max_cnt *= choice;
}

void Chord::init(ChordData& chord)
{
	set_max_count();
	c_size = 0;
	set_param1();
	set_g_center(*this);
	set_expansion_indexes();
	vec_ids.clear();
	if(enable_pedal)
	{
		if(continual && !in_bass && record.size() % period == 0)
		{
			pedal_notes.clear();
			for(int i = 0; i < (int)pedal_notes_set.size(); ++i)
			{
				for(int j = 0; j < t_size; ++j)
					if(notes[j] % 12 == pedal_notes_set[i])
					{
						pedal_notes.push_back(notes[j]);
						break;
					}
			}
			bubble_sort(pedal_notes);
		}
		chord.pedal_notes = pedal_notes;
	}
	record.push_back(chord);
	if(unique_mode == RemoveDupType)
		note_set_to_id(chord.get_note_set(), rec_id);
}

void Chord::get_progression()
{
	begin_progr = clock();
	new_chords.clear();
	int len = comb(m_max - 1, t_size - 1);
	// We will expand the chord to a size of 'm_max' by adding some notes from itself.
	// It can be proved that "len" equals to the number of different "expansions".

#ifdef QT_CORE_LIB
	prgdialog -> setMaximum(len * 1000);
	prgdialog -> setMinimumDuration(0);
#endif
	Chord expansion;
	for(exp_count = 1; exp_count <= len; ++exp_count)
	{
#ifndef QT_CORE_LIB
		cout << "\n" << exp_count << "/" << len << ":    ";
#endif
		expand(expansion, m_max, exp_count - 1);
		set_new_chords(expansion);
	}

#ifdef QT_CORE_LIB
	if(continual)
	{
		QStringList str1 = {"Progression #", "进行 #"};
		QString str2;
		labeltext = str1[language] + str2.setNum(progr_count) + " ";
	}
	else  labeltext.clear();
	QStringList str3 = {"(Writing to file(s)...)", "（正在写入文件…）"};
	labeltext += str3[language];
	prgdialog -> setLabelText(labeltext);
	if(prgdialog -> wasCanceled())  abort();
	set_est_time(prgdialog -> maximum());
	prgdialog -> setValue(prgdialog -> maximum());
#endif
	if(continual)  print2();
	else  print1();
}

void Chord::sort_results()
{
	for(int pos = strlen(sort_order) - 1; pos >= 0; --pos)
	{
		bool ascending = false;
		char ch = sort_order[pos];
		if(ch == '+')
		{
			ascending = true;
			ch = sort_order[--pos];
		}
		for(int i = 0; i < _TOTAL; ++i)
		{
			if(ch == var[i])
				merge_sort(new_chords.begin(), new_chords.end(), compare[i][ascending]);
		}
	}
}

void Chord::print1()
{
	merge_sort(new_chords.begin(), new_chords.end(), larger_chroma);
	int begin = (double)c_size * k_min / 100.0;
	int end   = (double)c_size * k_max / 100.0;
	if(end == c_size)  --end;
	double _k_min = new_chords[end].get_chroma();
	double _k_max = new_chords[begin].get_chroma();

	merge_sort(new_chords.begin(), new_chords.end(), larger_tension);
	begin = (double)c_size * t_min / 100.0;
	end   = (double)c_size * t_max / 100.0;
	if(end == c_size)  --end;
	double _t_min = new_chords[end].get_tension();
	double _t_max = new_chords[begin].get_tension();

	int i = 0;
	while(i < c_size)
	{
		if( new_chords[i].get_chroma()  < _k_min || new_chords[i].get_chroma()  > _k_max
		 || new_chords[i].get_tension() < _t_min || new_chords[i].get_tension() > _t_max)
		{
			new_chords.erase(new_chords.begin() + i);
			--c_size;
		}
		else  ++i;
	}

	if(c_size == 0)
	{
		if(language == English)
		{
			fout << "Results not found.\n\n";
			throw "ERROR - results not found under these conditions. Please check your conditions and try again.";
		}
		else
		{
			fout << "未找到结果。\n\n";
			throw "错误：在该条件下未找到结果。请检查条件后重试。";
		}
	}

	if(language == English)
		fout << c_size << " progression(s)\n\n";
	else  fout << c_size << " 种可能的和弦进行\n\n";
	sort_results();
	if(output_mode != MidiOnly)
	{
		for(int j = 0; j < c_size; ++j)
			print(new_chords[j]);
		print_end();
	}
}

void Chord::print2()
{
	if(c_size == 0)
	{
		if(output_mode != MidiOnly)
		{
			if(language == English)
				fout << "Results not found.\n\n";
			else  fout << "未找到结果。\n\n";
			print_end();
		}
		if(output_mode != TextOnly)  to_midi();
		if(progr_count == 1)
		{
			if(language == English)
				throw "ERROR - results not found under these conditions. Please check your conditions and try again.";
			else  throw "错误：在该条件下未找到结果。请检查条件后重试。";
		}
		else  throw progr_count;
	}

	merge_sort(new_chords.begin(), new_chords.end(), larger_chroma);
	int begin = (double)c_size * k_min / 100.0;
	int end   = (double)c_size * k_max / 100.0;
	if(end == c_size)  --end;
	double _k_min = new_chords[end].get_chroma();
	double _k_max = new_chords[begin].get_chroma();

	merge_sort(new_chords.begin(), new_chords.end(), larger_tension);
	begin = (double)c_size * t_min / 100.0;
	end   = (double)c_size * t_max / 100.0;
	if(end == c_size)  --end;
	
	vector<int> indexes;
	for(int i = begin; i <= end; ++i)
	{
		bool b = false;
		if( new_chords[i].get_chroma() >= _k_min && new_chords[i].get_chroma() <= _k_max)
		{
			b = true;
			if(unique_mode == RemoveDup)
				for(int j = 0; j < (int)record.size(); ++j)
					if(record[j].get_notes() == new_chords[i].get_notes())
					{ b = false;  break; }
		}
		if(b)  indexes.push_back(i);
	}

	if(indexes.empty())
	{
		if(output_mode != MidiOnly)
		{
			if(language == English)
				fout << "Results not found.\n\n";
			else  fout << "未找到结果。\n\n";
			print_end();
		}
		if(output_mode != TextOnly)  to_midi();
		if(progr_count == 1)
		{
			if(language == English)
				throw "ERROR - results not found under these conditions. Please check your conditions and try again.";
			else  throw "错误：在该条件下未找到结果。请检查条件后重试。";
		}
		else  throw progr_count;
	}
	int index = indexes[ rand(0, indexes.size() - 1) ];
	if(output_mode != MidiOnly)
		print(new_chords[index]);
	notes = new_chords[index].get_notes();
	init(new_chords[index]);
}

void Chord::print_stats()
{
	vector<ChordData>* ptr;
	if(continual)
		ptr = &record;
	else
		ptr = &new_chords;
	int count = ptr -> size();

	vector<Movement> movement;
	movement.resize(2 * vl_max + 1);
	for(int i = 0; i < 2 * vl_max + 1; ++i)
		movement[i].amount = i - vl_max;

	int cardinal_change = 0;
	for(int i = 0; i < count; ++i)
	{
		for(int j = 0; j < (int)(*ptr)[i].get_vec().size(); ++j)
		{
			int num = (*ptr)[i].get_vec()[j];
			++movement[num + vl_max].instance;
		}
		if(i != 0 && (*ptr)[i].get_s_size() != (*ptr)[i - 1].get_s_size())
			++cardinal_change;
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
	if(language == English)
		fout << "\nMovement percentage (sorted H -> L):\n";
	else  fout << "\n声部动向频次占比（从高到低）：\n";
	for(int i = 0; i < 2 * vl_max + 1; ++i)
	{
		fout << "[";
		if(sorted[i].amount > 0)  fout << "+";
		fout << sorted[i].amount << "] " << round(sorted[i].percentage * 10000.0) / 100.0 << "%  ";
	}
	if(language == English)
		fout << "\nCardinal change instances: ";
	else  fout << "\n和弦音个数 (N) 发生变化的次数：";
	fout << cardinal_change << " (" << round((double)cardinal_change / count * 10000.0) / 100.0 << "%)\n\n";


	int _n_max = 0, _n_min = 12,  n_sum = 0, n_max_index = 0, n_min_index = 0;
	int _m_max = 0, _m_min = 15,  m_sum = 0, m_min_index = 0, m_max_index = 0;
	double _t_max = 0.0, _t_min = INF, _k_max = 0.0, _k_min = INF, t_sum = 0, k_sum = 0;
	double _g_max = 0.0, _g_min = INF, _h_max = 0.0, _h_min = 50,  g_sum = 0, h_sum = 0;
	int t_max_index = 0,    t_min_index = 0, k_max_index = 0, k_min_index = 0;
	int g_max_index = 0,    g_min_index = 0, h_max_index = 0, h_min_index = 0;
	int _c_max = 0, _c_min = 15,  c_sum = 0, c_max_index = 0, c_min_index = 0;
	int _r_max = 0, _r_min = 11,  r_sum = 0, r_max_index = 0, r_min_index = 0;
	int _s_max = 0, _s_min = 100, s_sum = 0, s_max_index = 0, s_min_index = 0;
	int _x_max = 0, _x_min = 100, x_sum = 0, x_max_index = 0, x_min_index = 0;
	int temp1;  double temp2;

	for(int i = 0; i < count; ++i)
	{
		temp1 = (*ptr)[i].get_s_size();
		if(temp1 > _n_max)  { _n_max = temp1;  n_max_index = i; }
		if(temp1 < _n_min)  { _n_min = temp1;  n_min_index = i; }
		n_sum += temp1;

		temp1 = (*ptr)[i].get_t_size();
		if(temp1 > _m_max)  { _m_max = temp1;  m_max_index = i; }
		if(temp1 < _m_min)  { _m_min = temp1;  m_min_index = i; }
		m_sum += temp1;

		temp2 = (*ptr)[i].get_tension();
		if(temp2 > _t_max)  { _t_max = temp2;  t_max_index = i; }
		if(temp2 < _t_min)  { _t_min = temp2;  t_min_index = i; }
		t_sum += temp2;

		if(i != 0)
		{
			if(continual)
				temp2 = abs((*ptr)[i].get_chroma() - (*ptr)[i - 1].get_chroma());
			else  temp2 = abs((*ptr)[i].get_chroma() - (*ptr)[0].get_chroma());
			if(temp2 > _k_max)  { _k_max = temp2;  k_max_index = i; }
			if(temp2 < _k_min)  { _k_min = temp2;  k_min_index = i; }
			k_sum += temp2;
		}

		temp2 = (*ptr)[i].get_thickness();
		if(temp2 > _h_max)  { _h_max = temp2;  h_max_index = i; }
		if(temp2 < _h_min)  { _h_min = temp2;  h_min_index = i; }
		h_sum += temp2;

		if(i != 0)
		{
			temp1 = intersect((*ptr)[i].get_notes(), (*ptr)[i - 1].get_notes(), true).size();
			if(temp1 > _c_max)  { _c_max = temp1;  c_max_index = i; }
			if(temp1 < _c_min)  { _c_min = temp1;  c_min_index = i; }
			c_sum += temp1;
		}

		temp1 = (*ptr)[i].get_root();
		if(temp1 > _r_max)  { _r_max = temp1;  r_max_index = i; }
		if(temp1 < _r_min)  { _r_min = temp1;  r_min_index = i; }
		r_sum += temp1;

		temp2 = (*ptr)[i].get_g_center();
		if(temp2 > _g_max)  { _g_max = temp2;  g_max_index = i; }
		if(temp2 < _g_min)  { _g_min = temp2;  g_min_index = i; }
		g_sum += temp2;

		if(i != 0)
		{
			temp1 = (*ptr)[i].get_sv();
			if(temp1 > _s_max)  { _s_max = temp1;  s_max_index = i; }
			if(temp1 < _s_min)  { _s_min = temp1;  s_min_index = i; }
			s_sum += temp1;
		}

		if(i != 0)
		{
			temp1 = (*ptr)[i].get_similarity();
			if(temp1 > _x_max)  { _x_max = temp1;  x_max_index = i; }
			if(temp1 < _x_min)  { _x_min = temp1;  x_min_index = i; }
			x_sum += temp1;
		}
	}

	if(language == English)
	{
		fout << "Other stats:\n" << "Number of notes (n): "
			  << "most - "    << _n_max << "(@ #" << n_max_index + 1 << "); "
			  << "least - "   << _n_min << "(@ #" << n_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)n_sum / count << ";\n";
		fout << "Number of parts (m): "
			  << "most - "    << _m_max << "(@ #" << m_max_index + 1 << "); "
			  << "least - "   << _m_min << "(@ #" << m_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)m_sum / count << ";\n";
		fout << "Common notes (c): "
			  << "most - "    << _c_max << "(@ #" << c_max_index + 1 << "); "
			  << "least - "   << _c_min << "(@ #" << c_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)c_sum / (count - 1) << ";\n";
		fout << "Absolute value of chroma (|k|): "
			  << "highest - " << _k_max << "(@ #" << k_max_index + 1 << "); "
			  << "lowest - "  << _k_min << "(@ #" << k_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << k_sum / (count - 1) << ";\n";
		fout << "Similarity (x%): "
			  << "highest - " << _x_max << "(@ #" << x_max_index + 1 << "); "
			  << "lowest - "  << _x_min << "(@ #" << x_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)x_sum / (count - 1) << ";\n";
		fout << "Total voice leading (Σvec): "
			  << "highest - " << _s_max << "(@ #" << s_max_index + 1 << "); "
			  << "lowest - "  << _s_min << "(@ #" << s_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)s_sum / (count - 1) << ";\n";
		fout << "Root (r): "
			  << "highest - " << _r_max << "(@ #" << r_max_index + 1 << "); "
			  << "lowest - "  << _r_min << "(@ #" << r_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << (double)r_sum / count << ";\n";
		fout << "Tension (t): "
			  << "highest - " << _t_max << "(@ #" << t_max_index + 1 << "); "
			  << "lowest - "  << _t_min << "(@ #" << t_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << t_sum / count << ";\n";
		fout << "Thickness (h): "
			  << "highest - " << _h_max << "(@ #" << h_max_index + 1 << "); "
			  << "lowest - "  << _h_min << "(@ #" << h_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << h_sum / count << ";\n";
		fout << "Geomertic center (g%): "
			  << "highest - " << _g_max << "(@ #" << g_max_index + 1 << "); "
			  << "lowest - "  << _g_min << "(@ #" << g_min_index + 1 << "); "
			  << "average - " << fixed << setprecision(2) << g_sum / count << ".\n";
	}
	else
	{
		fout << "其他统计：\n" << "和弦音集音数 (n): "
			  << "最多 - " << _n_max << "(@ #" << n_max_index + 1 << ")；"
			  << "最少 - " << _n_min << "(@ #" << n_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)n_sum / count << ";\n";
		fout << "和弦声部数量 (m): "
			  << "最多 - " << _m_max << "(@ #" << m_max_index + 1 << ")；"
			  << "最少 - " << _m_min << "(@ #" << m_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)m_sum / count << ";\n";
		fout << "共同音个数 (c): "
			  << "最多 - " << _c_max << "(@ #" << c_max_index + 1 << ")；"
			  << "最少 - " << _c_min << "(@ #" << c_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)c_sum / (count - 1) << ";\n";
		fout << "和弦进行色差绝对值 (|k|): "
			  << "最高 - " << _k_max << "(@ #" << k_max_index + 1 << ")；"
			  << "最低 - " << _k_min << "(@ #" << k_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << k_sum / (count - 1) << ";\n";
		fout << "相邻和弦相似度 (x%): "
			  << "最高 - " << _x_max << "(@ #" << x_max_index + 1 << ")；"
			  << "最低 - " << _x_min << "(@ #" << x_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)x_sum / (count - 1) << ";\n";
		fout << "声部进行总大小 (Σvec): "
			  << "最高 - " << _s_max << "(@ #" << s_max_index + 1 << ")；"
			  << "最低 - " << _s_min << "(@ #" << s_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)s_sum / (count - 1) << ";\n";
		fout << "根音键位 (r): "
			  << "最高 - " << _r_max << "(@ #" << r_max_index + 1 << ")；"
			  << "最低 - " << _r_min << "(@ #" << r_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << (double)r_sum / count << ";\n";
		fout << "紧张度 (t): "
			  << "最高 - " << _t_max << "(@ #" << t_max_index + 1 << ")；"
			  << "最低 - " << _t_min << "(@ #" << t_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << t_sum / count << ";\n";
		fout << "厚度 (h): "
			  << "最高 - " << _h_max << "(@ #" << h_max_index + 1 << ")；"
			  << "最低 - " << _h_min << "(@ #" << h_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << h_sum / count << ";\n";
		fout << "几何中心位置占比 (g%): "
			  << "最高 - " << _g_max << "(@ #" << g_max_index + 1 << ")；"
			  << "最低 - " << _g_min << "(@ #" << g_min_index + 1 << ")；"
			  << "平均 - " << fixed << setprecision(2) << g_sum / count << "。\n";
	}
}

void Chord::print_end()
{
	if(language == Chinese)
	{
		fout << "（分析报告参数说明：）\n"
			  << "和弦进行：k - 色差（华萃康法）； x - 相邻和弦的相似度； v - 各声部运动方向及距离（半音个数）； c - 相邻和弦的共同音个数\n"
			  << "每个和弦：r - 根音； t - 紧张度； vec - 音程涵量表； d - 音程结构表； h - 厚度； g - 几何中心位置占比\n\n";
	}
	fout << "==========\n";
	print_stats();
	end = clock();
	double dur = (double) (end - begin) / CLOCKS_PER_SEC;
	if(language == English)
		fout << "\nGeneration completed in " << dur << " seconds.";
	else  fout << "\n本次生成耗时 " << dur << " 秒。";
	fout.close();
}

void Chord::to_midi()
// writes the results to a MIDI file
// For single mode or continual mode with 'connect_pedal' == false, the midi file is written in one track;
// for continual mode with 'connect_pedal' == true, it is written in three tracks.
// The first track contains some information including title, tempo and copyright,
// the second track contains non-pedal notes, and the third track contains pedal notes.
{
	int chord_count, note_count = 0;
	if(continual)
	{
		if(!enable_pedal || !connect_pedal)
		{
			chord_count = record.size();
			for(int i = 0; i < chord_count; ++i)
				note_count += record[i].get_t_size();
			midi_head(chord_count, note_count);
			for(int i = 0; i < chord_count; ++i)
				chord_to_midi(record[i].get_notes());
		}
		else
		{
			char str1[19] = "\x4D\x54\x68\x64\x00\x00\x00\x06\x00\x01\x00\x03\x01\xE0\x4D\x54\x72\x6B";
			m_fout.write(str1, 18);
			int _len = swapInt(74);
			m_fout.write((char*)&_len, sizeof(int));
			char str2[79] = { "\x00\xFF\x02\x21\x28\x63\x29\x20\x32\x30\x32\x30\x20\x57\x65\x6E\x67\x65\x20\x43\x68\x65\x6E\x2C\x20\x4A"
									"\x69\x2D\x77\x6F\x6F\x6E\x20\x53\x69\x6D\x2E\x00\xFF\x04\x05\x50\x69\x61\x6E\x6F\x00\xFF\x51\x03\x0F\x42\x40"
									"\x00\xFF\x58\x04\x04\x02\x18\x08\x00\xFF\x59\x02\x00\x00\x00\xC0\x00\x00\xFF\x2F\x00\x4D\x54\x72\x6B"};
			m_fout.write(str2, 78);

			chord_count = record.size();
			for(int i = 0; i < chord_count; ++i)
				note_count += (record[i].get_t_size() - record[i].pedal_notes.size());
			_len = swapInt(8 * note_count + chord_count + 4);
			m_fout.write((char*)&_len, sizeof(int));
	
			for(int i = 0; i < chord_count; ++i)
			{
				vector<int> non_pedal = get_complement(record[i].get_notes(), record[i].pedal_notes);
				chord_to_midi(non_pedal);
			}
			m_fout.write("\x00\xFF\x2F\x00\x4D\x54\x72\x6B", 8);
			
			// Please refer to the function 'chord_to_midi' in the file 'functions'.
			if(in_bass)  period = chord_count;
			int group_count = ceil(record.size() / period);
			int addition = ((period > 34) ? 2 : 1) * group_count;
			if(period > 34 && chord_count % period <= 34)  --addition;
			note_count = 0;
			for(int i = 0; i < chord_count; i += period)
				note_count += record[i].pedal_notes.size();
			_len = swapInt(8 * note_count + addition + 4);
			m_fout.write((char*)&_len, sizeof(int));

			for(int i = 0; i < chord_count; i += period)
			{
				int beat = period;
				if(i >= chord_count - period)
					beat = (chord_count - 1) % period + 1;
				chord_to_midi(record[i].pedal_notes, beat);
			}
		}
	}
	else
	{
		if(interlace)
		{
			chord_count = 2 * c_size;
			for(int i = 0; i < c_size; ++i)
				note_count += new_chords[i].get_t_size();
			note_count += (c_size * t_size);
			midi_head(chord_count, note_count);
			for(int i = 0; i < c_size; ++i)
			{
				chord_to_midi(notes);
				chord_to_midi(new_chords[i].get_notes());
			}
		}  
		else
		{
			chord_count = c_size + 1;
			for(int i = 0; i < c_size; ++i)
				note_count += new_chords[i].get_t_size();
			note_count += t_size;
			midi_head(chord_count, note_count);
			chord_to_midi(notes);
			for(int i = 0; i < c_size; ++i)
				chord_to_midi(new_chords[i].get_notes());
		}
	}
	m_fout.write("\x00\xFF\x2F\x00", 4);
	m_fout.close();
}


void Chord::check_initial()
// check the initial chord the user input
{
	t_size = notes.size();
	if(t_size < m_min || t_size > m_max)
	{
		if(language == English)
			throw "The number of parts in the chord is not in the range you set. Please try again.";
		else  throw "和弦声部数量不在您设置的范围内。请重试。";
	}

	set_g_center(*this);
	if(g_center > g_max || g_center < g_min)
	{
		if(language == English)
			throw "The geometric center of the chord is not in the range you set. Please try again.";
		else  throw "和弦的几何中心不在您设置的范围内。请重试。";
	}

	set_param1();
	if(s_size < n_min || s_size > n_max)
	{
		if(language == English)
			throw "The number of notes in the chord is not in the range you set. Please try again.";
		else  throw "和弦音集音数不在您设置的范围内。请重试。";
	}
	if(notes[0] < lowest || *notes.rbegin() > highest)
	{
		if(language == English)
			throw "The chord you have input is not in the range of notes. Please try again.";
		else  throw "和弦的音高超过了音域。请重试。";
	}
	if(root > r_max || root < r_min)
	{
		if(language == English)
			throw "The root of the chord is not in the range you set. Please try again.";
		else  throw "和弦的根音不在您设置的范围内。请重试。";
	}
	if(thickness > h_max || thickness < h_min)
	{
		if(language == English)
			throw "The thickness of the chord is not in the range you set. Please try again.";
		else  throw "和弦的厚度不在您设置的范围内。请重试。";
	}

	if(align_mode != Unlimited && !valid_alignment(*this))
	{
		if(language == English)
			throw "The alignment of the chord is not valid. Please try again.\n";
		else  throw "和弦的排列方式不正确。请重试。";
	}
	if(enable_ex && !valid_exclusion(*this))
	{
		if(language == English)
			throw "The chord does not exclude the notes/intervals you have set. Please try again.";
		else  throw "和弦未排除您指定的音/音程。请重试。";
	}
	if(enable_pedal && continual && !include_pedal(*this))
	{
		if(language == English)
			throw "The chord does not include pedal notes or pedal notes are not in bass. Please try again.";
		else  throw "和弦不包含持续音或者持续音不在低音处。请重试。";
	}

	int pos1 = find(bass_avail, alignment[0]), pos2 = find(chord_library, set_id);
	if(pos1 != -1)
	{
		if(language == English)
			throw "The bass of the chord does not meet the requirements you have set. Please try again.";
		else  throw "和弦的低音不符合您设置的要求。请重试。";
	}
	if(pos2 != -1)
	{
		if(language == English)
			throw "The chord you have input is not in the chord library. Please try again.";
		else  throw "您输入的和弦不在和弦库中。请重试。";
	}

	vector<int> intersection = intersect(note_set, overall_scale, true);
	if((int)intersection.size() < s_size)
	{
		if(language == English)
			throw "The chord you have input is not in the overall scale. Please try again.";
		else  throw "您输入的和弦不在整体音阶中。请重试。";
	}
}

void Chord::choose_initial()
// automatically chooses an initial chord that satisfies the conditions
{
	vector<int> intersection;
	while(true)
	{
		notes = pedal_notes;
		int size = rand(m_min, m_max);
		int _lowest = lowest;
		if(enable_pedal && continual && in_bass)
			_lowest = *pedal_notes.rbegin() + 1;
		do{
			int note = rand(_lowest, highest);
			notes.push_back(note);
			bubble_sort(notes);
			remove_duplicate(notes);
			t_size = notes.size();
		}  while(t_size != size);
		set_param1();
		set_g_center(*this);
		intersection = intersect(note_set, overall_scale, true);
		bool b = (thickness <= h_max) && (thickness >= h_min) && (root <= r_max) && (root >= r_min)
				&& (g_center <= g_max) && (g_center >= g_min) && (s_size <= n_max) && (s_size >= n_min)
				&& (find(chord_library, set_id) == -1) && (find(bass_avail, alignment[0]) == -1)
				&& (align_mode == Unlimited || valid_alignment(*this)) && ( !(enable_pedal && continual && !include_pedal(*this))
				&& (!(enable_ex && !valid_exclusion(*this))) && ((int)intersection.size() == s_size));
		if(b)  break;
	}
}

Chord::Chord() = default;
Chord::Chord(const vector<int>& _notes)
// used in utilities
{
	notes = _notes;
	remove_duplicate(notes);
	bubble_sort(notes);
	m_min = 1;   m_max = 15;
	n_min = 1;   n_max = 12;
	lowest = 0;  highest = 127;
	sv_min = 0;  sv_max = 100;
	init( static_cast<ChordData&>(*this) );
}
Chord::Chord(const Chord& A): ChordData(A) {};

int& Chord::get_set_id()  { return set_id; }

void Chord::Main()
{
	begin = clock();
	char name1[200] = "../output/", name2[200] = "../output/";
	char name3[200];
#ifdef QT_CORE_LIB
	strcpy(name3, ((QString)output_name).toLocal8Bit().data());
#else
	strcpy(name3, output_name);
#endif
	strcat(name1, name3);
	strcat(name2, name3);
	strcat(name1, ".txt");
	strcat(name2, ".mid");
	fout.open(name1, ios::trunc);
	m_fout.open(name2, ios::trunc | ios::binary);
	record.clear();
	rec_id.clear();

	similarity = MINF;
	sv = MINF;
	common_note = MINF;
	init( static_cast<ChordData&>(*this) );
	if(language == English)
		fprint("original_chord: ", notes);
	else  fprint("起始和弦：", notes);
	fout << "r = " << root << ",  "
		  << "t = " << tension << ",  ";
	fprint("vec = ", count_vec, "\0", ",  ");
	fprint("d = ", self_diff, ", ", ",  ");
	fout << "h = " << thickness << ",  "
		  << "g = " << g_center << "%\n\n";
	if(language == English)
		fout << "Results:\n";
	else  fout << "生成结果：\n";

	if(continual)
	{
		for(progr_count = 1; progr_count <= loop_count; ++progr_count)
		{
#ifndef QT_CORE_LIB
			cout << "\nProgression #" << progr_count << ":\n";
#endif
			if(language == English)
				fout << "Progression #" << progr_count << ":\n";
			else  fout << "和弦进行 #" << progr_count << ":\n";
			get_progression();
		}
	}
	else  get_progression();
	if(fout.is_open())
		print_end();
	if(output_mode != TextOnly)  to_midi();
}

void Chord::_set_vl_max(const int & _vl_max)
{ vl_max = _vl_max; }

void Chord::find_vec(Chord& new_chord)
// First we expand the smaller one (refer to size) of two chords to the same size of another one.
// Then we iterate through all expansions and get 'vec' and 'sv'.
// The one with the smallest 'sv' will be chosen.
// Notice: set_param2 is called.
{
	int diff, min_diff = 1000, min_index = 0, len;
	Chord expansion;
	if(new_chord.t_size > t_size)
	{
		len = comb(new_chord.t_size - 1, t_size - 1);
		for(int i = 0; i < len; ++i)
		{
			expand(expansion, new_chord.t_size, i);
			diff = 0;
			for(int j = 0; j < new_chord.t_size; ++j)
				diff += abs(new_chord.notes[j] - expansion.notes[j]);
			if(diff < min_diff)
			{
				min_diff = diff;
				min_index = i;
			}
		}
		expand(expansion, new_chord.t_size, min_index);
		for(int i = 0; i < new_chord.t_size; ++i)
			new_chord.vec.push_back(new_chord.notes[i] - expansion.notes[i]);
		new_chord.sv = min_diff;
	}

	else
	{
		len = comb(t_size - 1, new_chord.t_size - 1);
		for(int i = 0; i < len; ++i)
		{
			new_chord.expand(expansion, t_size, i);
			diff = 0;
			for(int j = 0; j < t_size; ++j)
				diff += abs(expansion.notes[j] - notes[j]);

			if(diff < min_diff)
			{
				min_diff = diff;
				min_index = i;
			}
		}
		new_chord.expand(expansion, t_size, min_index);
		for(int i = 0; i < t_size; ++i)
			new_chord.vec.push_back(expansion.notes[i] - notes[i]);
		new_chord.sv = min_diff;
	}
	set_param2(new_chord);
}

#ifdef QT_CORE_LIB
void Chord::set_est_time(const int& value)
{
	static double  prev[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	const double weight[9] = {1, 1, 1.5, 1.5, 1.5, 2, 2, 2, 1.5};
	// To avoid huge fluctuation, we will calculate weighted mean with previous remaining times.
	// However, the array itself is given somewhat randomly.
	if(value == prgdialog -> maximum())
	{
		for(int i = 0; i < 9; ++i)
			prev[i] = 0;
		return;
	}

	end = clock();
	double dur = (end - begin_progr) / CLOCKS_PER_SEC;
	double rem = (double)dur / (value + 1.0) * (prgdialog -> maximum() - value)
					 * exp2(1.0 - (double)value / prgdialog -> maximum());
	// The last term is a modification, at the beginning it is 2 and it gradually drops to 1.
	// It is set because the speed of the program will decrease as the size of 'new_chords' grows up.
	double weighted_rem = rem;
	for(int i = 0; i < 9; ++i)
	{
		if(prev[i] == 0)
			weighted_rem += rem * weight[i];
		else  weighted_rem += prev[i] * weight[i];
	}

	weighted_rem /= 15.0;
	for(int i = 0; i <= 7; ++i)
		prev[i] = prev[i + 1];
	prev[8] = weighted_rem;

	long long _rem = floor(weighted_rem);
	int sec = _rem % 60;
	int min =(_rem / 60) % 60;
	long long hour = _rem / 3600;

	QString time, str1;
	QStringList str_sec  = {" sec", " 秒"};
	QStringList str_min  = {" min", " 分"};
	QStringList str_hour = {" hr", " 小时"};
	if(hour >= 24)
	{
		QStringList str2 = {"> 24 hours", "> 24 小时"};
		time = str2[language];
	}
	else if(hour == 0)
	{
		if(min == 0)
		{
			time = str1.setNum(sec) + str_sec[language];
			if(language == English && sec > 1)  time += 's';
		}
		else
		{
			time = str1.setNum(min) + str_min[language];
			if(language == English && min  > 1)  time += 's';
			time = time + ' ' + str1.setNum(sec) + str_sec[language];
			if(language == English && sec > 1)  time += 's';
		}
	}
	else
	{
		time = str1.setNum(hour) + str_hour[language];
		if(language == English && hour > 1)  time += 's';
		time = time + ' ' + str1.setNum(min) + str_min[language];
		if(language == English && min  > 1)  time += 's';
	}

	QStringList str3 = {" (Estimated remaining time: ", " （预计剩余时间："};
	QStringList str4 = {")", "）"};
	labeltext = labeltext + str3[language] + time + str4[language];
}

void Chord::abort()
{
	set_est_time(prgdialog -> maximum());
	if(language == English)
		throw "The generation is aborted.";
	else  throw "生成已中止。";
}
#endif

// ChordNova v3.0 [Build: 2021.1.14]
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

const double _tension[12] = {0.0, 11.0, 8.0, 6.0, 5.0, 3.0, 7.0, 3.0, 5.0, 6.0, 8.0, 11.0};
const int restriction[12] = {0, 53, 53, 51, 50, 51, 52, 39, 51, 50, 51, 52};
vector<int> overall_scale = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

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
	c_size = 0;
	set_param1();
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

	vector<int> normal = normal_form(note_set);
	self_diff.clear();
	for(int i = 1; i < s_size; ++i)
		self_diff.push_back(normal[i] - normal[i - 1]);

	set_id = 0;
	for(int i = 0; i < s_size; ++i)
		set_id += (1 << note_set[i]);

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
	tension /= 10.0;

	thickness = 0.0;
	for(int i = 0; i < t_size; ++i)
		for(int j = i + 1; j < t_size; ++j)
			if((notes[j] - notes[i]) % 12 == 0)
				thickness += ( 12.0 / (double)(notes[j] - notes[i]) );

	if(t_size == 1)  g_center = 50;
	else
	{
		double temp = 0;
		for(int i = 0; i < t_size; ++i)
			temp += notes[i];
		temp /= (double)t_size;
		temp = (temp - notes[0]) / (notes[t_size - 1] - notes[0]);
		temp = round(temp * 100.0);
		g_center = temp;
	}
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
	if(prgdialog -> wasCanceled())  abort(false);
	set_est_time(prgdialog -> maximum(), false);
	prgdialog -> setValue(prgdialog -> maximum());
#endif
	if(continual)  print_continual();
	else  print_single();
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
			set_est_time(temp * 1000, false);
			prgdialog -> setLabelText(labeltext);
			if(prgdialog -> wasCanceled())  abort(false);
			prgdialog -> setValue(temp * 1000);
#else
			cout << "\b\b\b" << setw(2) << count / step / 5 << "%";
#endif
		}
	}
}

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
	if(new_chord.g_center > g_max || new_chord.g_center < g_min)
		return false;

	vector<int> intersection = intersect(new_chord.note_set, overall_scale, true);
	if((int)intersection.size() < new_chord.s_size)  return false;

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
	if(new_chord.common_note > c_max || new_chord.common_note < c_min)
		return false;
	if(new_chord.sv < sv_min || new_chord.sv > sv_max)
		return false;
	if(enable_rm && rm_priority[new_chord.root_movement] == -1)
		return false;
	if(!valid_sim(new_chord))
		return false;
	if(new_chord.span < s_min || new_chord.span > s_max)
		return false;
	if(new_chord.sspan < ss_min || new_chord.sspan > ss_max)
		return false;
	if(new_chord.Q_indicator < q_min || new_chord.Q_indicator > q_max)
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

void Chord::_find_vec(Chord& new_chord)
// First we expand the smaller one (refer to size) of two chords to the same size of another one.
// Then we iterate through all expansions and get 'vec' and 'sv'.
// The one with the smallest 'sv' will be chosen.
{
	new_chord.vec.clear();
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
}

void Chord::set_param2(Chord& chord, bool in_analyser, bool in_substitution)
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

	if(in_analyser)
	{
		vl_max = 0, vl_min = 0;
		for(int i = 0; i < (int)chord.vec.size(); ++i)
		{
			if(abs(chord.vec[i]) > vl_max)
				vl_max = abs(chord.vec[i]);
		}
		if(vl_max == 0)  vl_max = 1;
	}
	if(in_substitution)
	{
		vl_max = 6;
		vl_min = 0;
	}

	chord.common_note = intersect(chord.notes, notes, true).size();
	chord.similarity = set_similarity(*this, chord, in_substitution);
	set_span(chord, false);
	chord.prev_chroma_old = chroma_old;
	chord.set_chroma_old();
	set_chroma(chord);
	chord.set_name();
	chord.Q_indicator = chord.chroma * (tension + chord.tension)
							/ 2.0 / (double)max(t_size, chord.t_size);
}

int Chord::set_similarity(Chord& chord1, Chord& chord2, bool in_substitution, const int& period)
{
	double temp;
	if(in_substitution)  temp = 36; // Maximum value of sv in chord substitution.
	else  temp = vl_max * period * max(chord1.t_size, chord2.t_size);
	temp = pow(1.0 - chord2.sv / temp, period);
	if(chord1.root == chord2.root)
		temp = sqrt(temp);
	return round(100.0 * temp);
}

void Chord::set_span(Chord& chord, bool initial)
{
	chord.single_chroma.clear();
	for(int i = 0; i < chord.t_size; ++i)
		chord.single_chroma.push_back( 6 - (5 * (chord.notes[i] % 12) + 6) % 12 );
	vector<int> copy(chord.single_chroma);
	bubble_sort(copy);

	int diff1, min_diff1 = copy[chord.t_size - 1] - copy[0];
	int bound, min_bound = max( abs(copy[0]), abs(copy[chord.t_size - 1]) );
	int index = 0;
	if(initial)
	{
		for(int i = 1; i < chord.t_size; ++i)
		{
			diff1 = copy[i - 1] + 12 - copy[i];
			if(diff1 < min_diff1)
			{
				min_diff1 = diff1;
				min_bound = max( abs(copy[i - 1] + 12), abs(copy[i]) );
				index = i;
			}
			else if(diff1 == min_diff1)
			{
				bound = max( abs(copy[i - 1] + 12), abs(copy[i]) );
				if(bound < min_bound)
				{
					min_bound = bound;
					index = i;
				}
			}
		}
		chord.span = min_diff1;
	}
	else
	{
		int diff2, min_diff2;
		vector<int> merged_single_chroma = get_union(single_chroma, copy);
		min_diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
		for(int i = 1; i <= chord.t_size; ++i)
		{
			copy[i - 1] += 12;
			diff1 = copy[i - 1] - copy[i % chord.t_size];
			if(diff1 < min_diff1)
			{
				min_diff1 = diff1;
				merged_single_chroma = get_union(single_chroma, copy);
				min_diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
				min_bound = max( abs(copy[i - 1]), abs(copy[i % chord.t_size]) );
				index = i;
			}
			else if(diff1 == min_diff1)
			{
				merged_single_chroma = get_union(single_chroma, copy);
				diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
				if(diff2 < min_diff2)
				{
					min_diff2 = diff2;
					min_bound = max( abs(copy[i - 1]), abs(copy[i % chord.t_size]) );
					index = i;
				}
				else if(diff2 == min_diff2)
				{
					bound = max( abs(copy[i - 1]), abs(copy[i % chord.t_size]) );
					if(bound < min_bound)
					{
						min_bound = bound;
						index = i;
					}
				}
			}
		}

		copy.assign(chord.single_chroma.begin(), chord.single_chroma.end());
		bubble_sort(copy);
		for(int i = chord.t_size; i > 0; --i)
		{
			int j = (i - 2 + chord.t_size) % chord.t_size;  // i.e. j = i - 2
			copy[i - 1] -= 12;
			diff1 = copy[j] - copy[i - 1];
			if(diff1 < min_diff1)
			{
				min_diff1 = diff1;
				merged_single_chroma = get_union(single_chroma, copy);
				min_diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
				min_bound = max( abs(copy[j]), abs(copy[i - 1]) );
				index = -i;
			}
			else if(diff1 == min_diff1)
			{
				merged_single_chroma = get_union(single_chroma, copy);
				diff2 = *(merged_single_chroma.rbegin()) - *(merged_single_chroma.begin());
				if(diff2 < min_diff2)
				{
					min_diff2 = diff2;
					min_bound = max( abs(copy[j]), abs(copy[i - 1]) );
					index = -i;
				}
				else if(diff2 == min_diff2)
				{
					bound = max( abs(copy[j]), abs(copy[i - 1]) );
					if(bound < min_bound)
					{
						min_bound = bound;
						index = -i;
					}
				}
			}
		}
		chord.span = min_diff1;
		chord.sspan = min_diff2;
	}

	copy.assign(chord.single_chroma.begin(), chord.single_chroma.end());
	bubble_sort(copy);
	if(index > 0)
	{
		for(int i = 0; i < chord.t_size; ++i)
		{
			if(chord.single_chroma[i] <= copy[index - 1])
				chord.single_chroma[i] += 12;
		}
	}
	else if(index < 0)
	{
		for(int i = 0; i < chord.t_size; ++i)
		{
			if(chord.single_chroma[i] >= copy[- index - 1])
				chord.single_chroma[i] -= 12;
		}
	}
}

void Chord::set_chroma_old()
{
	vector<int> copy(single_chroma);
	bubble_sort(copy);
	remove_duplicate(copy);
	chroma_old = 0.0;
	for(int i = 0; i < s_size; ++i)
		chroma_old += copy[i];
	chroma_old /= (double)s_size;
	chroma_old = floor(chroma_old * 100) / 100.0;

	int val = 0;
	if(chroma_old - prev_chroma_old < -18.0)
		val = 24;
	else if(chroma_old - prev_chroma_old < -6.0)
		val = 12;
	else if(chroma_old - prev_chroma_old > 18.0)
		val = -24;
	else if(chroma_old - prev_chroma_old > 6.0)
		val = -12;

	if(val != 0)
	{
		for(int i = 0; i < t_size; ++i)
			single_chroma[i] += val;
		chroma_old += val;
		overflow_state = Total;
	}
	else  overflow_state = NoOverflow;
}

void Chord::set_chroma(Chord& chord)
{
	vector<int> A(single_chroma);
	vector<int> B(chord.single_chroma);
	bubble_sort(A);
	bubble_sort(B);
	remove_duplicate(A);
	remove_duplicate(B);
	vector<int> A_unique = get_complement(A, B);
	vector<int> B_unique = get_complement(B, A);
	int val = 0;
	for(int i = 0; i < (int)A_unique.size(); ++i)
		for(int j = 0; j < (int)B_unique.size(); ++j)
			val += abs(A_unique[i] - B_unique[j]);
	int _sign = sign(chord.chroma_old - chroma_old);
	chord.chroma = _sign * 2.0 / 3.1416 * atan(val / 54.0) * 100.0;
}

void Chord::set_name()
{
	vector<int> copy(single_chroma);
	bubble_sort(copy);

	overflow_amount = 0;
	if(copy[t_size - 1] < -6)
		overflow_amount = -12;
	else if(copy[0] > 6)
		overflow_amount = 12;
	else if(copy[t_size - 1] >= 13 && copy[0] >= 4)  // The name includes 'x' and does not include 'bb'.
		overflow_amount = 12;
	else if(copy[0] <= -9 && copy[t_size - 1] <= 0)  // The name includes 'bb' and does not include '#'.
		overflow_amount = -12;

	for(int i = 0; i < t_size; ++i)
		single_chroma[i] -= overflow_amount;
	if(overflow_state == NoOverflow && overflow_amount != 0)
		overflow_state = Single;
	chroma_old -= overflow_amount;
	prev_chroma_old -= overflow_amount;

	strcpy(name, "\0");
	strcpy(name_with_octave, "\0");
	for(int i = 0; i < t_size; ++i)
	{
		char _name[5];
		chromatoname(single_chroma[i], _name);
		strcat(name, _name);

		strcat(name_with_octave, _name);
		int octave = (notes[i] - chromatonum(single_chroma[i]) ) / 12 - 1;
		char _octave[5];
		inttostring(octave, _octave);
		strcat(name_with_octave, _octave);

		if(i < t_size - 1)
		{
			strcat(name, " ");
			strcat(name_with_octave, " ");
		}
	}

	int position = 0;
	for(int i = 0; i < t_size; ++i)
	{
		if((notes[i] - root) % 12 == 0)
		{
			position = i;
			break;
		}
	}
	strcpy(root_name, "\0");
	chromatoname(single_chroma[position], root_name);
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
				copy2.similarity = set_similarity(copy1, copy2, false, sim_period[i]);
				if(copy2.similarity < sim_min[i] || copy2.similarity > sim_max[i])
					return false;
			}
		}
	}
	return true;
}

void Chord::sort_results(vector<ChordData>& chords, bool in_substitution)
{
	char _sort_order[50];
	if(in_substitution)
		strcpy(_sort_order, sort_order_sub);
	else  strcpy(_sort_order, sort_order);

	for(int pos = strlen(_sort_order) - 1; pos >= 0; --pos)
	{
		bool ascending = false;
		char ch = _sort_order[pos];
		if(ch == '+')
		{
			ascending = true;
			ch = _sort_order[--pos];
		}
		for(int i = 0; i < VAR_TOTAL; ++i)
		{
			if(ch == var[i])
				merge_sort(chords.begin(), chords.end(), compare[i][ascending]);
		}
	}
}

void Chord::print_single()
{
	merge_sort(new_chords.begin(), new_chords.end(), larger_chroma);
	int begin = (double)c_size * k_min / 100.0;
	int end   = (double)c_size * k_max / 100.0;
	if(end == c_size)  --end;
	double _k_min = new_chords[end].get_chroma();
	double _k_max = new_chords[begin].get_chroma();

	merge_sort(new_chords.begin(), new_chords.end(), larger_chroma_old);
	begin = (double)c_size * kk_min / 100.0;
	end   = (double)c_size * kk_max / 100.0;
	if(end == c_size)  --end;
	double _kk_min = new_chords[end].get_chroma_old();
	double _kk_max = new_chords[begin].get_chroma_old();

	merge_sort(new_chords.begin(), new_chords.end(), larger_tension);
	begin = (double)c_size * t_min / 100.0;
	end   = (double)c_size * t_max / 100.0;
	if(end == c_size)  --end;
	double _t_min = new_chords[end].get_tension();
	double _t_max = new_chords[begin].get_tension();

	int i = 0;
	while(i < c_size)
	{
		if( new_chords[i].get_chroma()     <  _k_min || new_chords[i].get_chroma()     >  _k_max
		 || new_chords[i].get_chroma_old() < _kk_min || new_chords[i].get_chroma_old() > _kk_max
		 || new_chords[i].get_tension()    <  _t_min || new_chords[i].get_tension()    >  _t_max )
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
	sort_results(new_chords, false);
	if(output_mode != MidiOnly)
	{
		for(int j = 0; j < c_size; ++j)
			print(new_chords[j], language);
		print_end();
	}
}

void Chord::print_continual()
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

	merge_sort(new_chords.begin(), new_chords.end(), larger_chroma_old);
	begin = (double)c_size * kk_min / 100.0;
	end   = (double)c_size * kk_max / 100.0;
	if(end == c_size)  --end;
	double _kk_min = new_chords[end].get_chroma_old();
	double _kk_max = new_chords[begin].get_chroma_old();

	merge_sort(new_chords.begin(), new_chords.end(), larger_tension);
	begin = (double)c_size * t_min / 100.0;
	end   = (double)c_size * t_max / 100.0;
	if(end == c_size)  --end;
	
	vector<int> indexes;
	for(int i = begin; i <= end; ++i)
	{
		bool b = false;
		if( new_chords[i].get_chroma()     >=  _k_min && new_chords[i].get_chroma()     <=  _k_max
		 && new_chords[i].get_chroma_old() >= _kk_min && new_chords[i].get_chroma_old() <= _kk_max )
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
		print(new_chords[index], language);
	notes = new_chords[index].get_notes();
	single_chroma = new_chords[index].get_single_chroma();
	prev_chroma_old = chroma_old;
	chroma_old = new_chords[index].get_chroma_old();
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
	int count_ = (continual ? (count - 1) : count);

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

		if(continual && i != 0 && (*ptr)[i].get_s_size() != (*ptr)[i - 1].get_s_size())
			++cardinal_change;
		if(!continual && (*ptr)[i].get_s_size() != s_size)
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
		fout << sorted[i].amount << "] " << round(sorted[i].percentage * 100.0) << "%  ";
	}
	if(language == English)
		fout << "\nCardinal change instances: ";
	else  fout << "\n和弦音个数 (N) 发生变化的次数：";
	fout << cardinal_change << " (" << round((double)cardinal_change / count_ * 100.0) << "%)\n\n";

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
	double _kk_max = 0.0, _kk_min = INF, kk_sum = 0;
	double nm_max = 0.0, nm_min = 1.0, nm_sum = 0, dt_max = MINF, dt_min = INF, dt_sum = 0;
	int  t_max_index = 0,  t_min_index = 0,  k_max_index = 0,  k_min_index = 0;
	int  h_max_index = 0,  h_min_index = 0,  q_max_index = 0,  q_min_index = 0;
	int kk_max_index = 0, kk_min_index = 0;
	int nm_max_index = 0, nm_min_index = 0, dt_max_index = 0, dt_min_index = 0;
	int temp1;  double temp2;

	for(int i = 0; i < count; ++i)
	{
		if( !(continual && i == 0) )
		{
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
		}

		temp1 = (*ptr)[i].get_s_size();
		if(temp1 > _n_max)  { _n_max = temp1;  n_max_index = i; }
		if(temp1 < _n_min)  { _n_min = temp1;  n_min_index = i; }
		n_sum += temp1;

		temp1 = (*ptr)[i].get_t_size();
		if(temp1 > _m_max)  { _m_max = temp1;  m_max_index = i; }
		if(temp1 < _m_min)  { _m_min = temp1;  m_min_index = i; }
		m_sum += temp1;

		temp2 = (double) (*ptr)[i].get_s_size() / (*ptr)[i].get_t_size();
		if(temp2 > nm_max)  { nm_max = temp2;  nm_max_index = i; }
		if(temp2 < nm_min)  { nm_min = temp2;  nm_min_index = i; }
		nm_sum += temp2;

		temp2 = (*ptr)[i].get_thickness();
		if(temp2 > _h_max)  { _h_max = temp2;  h_max_index = i; }
		if(temp2 < _h_min)  { _h_min = temp2;  h_min_index = i; }
		h_sum += temp2;

		temp2 = (*ptr)[i].get_tension();
		if(temp2 > _t_max)  { _t_max = temp2;  t_max_index = i; }
		if(temp2 < _t_min)  { _t_min = temp2;  t_min_index = i; }
		t_sum += temp2;

		temp1 = (*ptr)[i].get_root();
		if(temp1 > _r_max)  { _r_max = temp1;  r_max_index = i; }
		if(temp1 < _r_min)  { _r_min = temp1;  r_min_index = i; }
		r_sum += temp1;

		temp1 = (*ptr)[i].get_g_center();
		if(temp1 > _g_max)  { _g_max = temp1;  g_max_index = i; }
		if(temp1 < _g_min)  { _g_min = temp1;  g_min_index = i; }
		g_sum += temp1;

		temp1 = (*ptr)[i].get_span();
		if(temp1 > _s_max)  { _s_max = temp1;  s_max_index = i; }
		if(temp1 < _s_min)  { _s_min = temp1;  s_min_index = i; }
		s_sum += temp1;

		if( !(continual && i == 0) )
		{
			if(continual)
				temp2 = (*ptr)[i].get_chroma_old() - (*ptr)[i - 1].get_chroma_old();
			else  temp2 = (*ptr)[i].get_chroma_old() - chroma_old;
			if(temp2 > dt_max)  { _kk_max = temp2;  kk_max_index = i; }
			if(temp2 < dt_min)  { _kk_min = temp2;  kk_min_index = i; }
			kk_sum += temp2;

			if(continual)
				temp2 = (*ptr)[i].get_tension() - (*ptr)[i - 1].get_tension();
			else  temp2 = (*ptr)[i].get_tension() - tension;
			if(temp2 > dt_max)  { dt_max = temp2;  dt_max_index = i; }
			if(temp2 < dt_min)  { dt_min = temp2;  dt_min_index = i; }
			dt_sum += temp2;

			if(continual)
				temp1 = (*ptr)[i].get_root() - (*ptr)[i - 1].get_root();
			else  temp1 = (*ptr)[i].get_root() - root;
			if(temp1 > dr_max)  { dr_max = temp1;  dr_max_index = i; }
			if(temp1 < dr_min)  { dr_min = temp1;  dr_min_index = i; }
			dr_sum += temp1;

			if(continual)
				temp1 = (*ptr)[i].get_g_center() - (*ptr)[i - 1].get_g_center();
			else  temp1 = (*ptr)[i].get_g_center() - g_center;
			if(temp1 > dg_max)  { dg_max = temp1;  dg_max_index = i; }
			if(temp1 < dg_min)  { dg_min = temp1;  dg_min_index = i; }
			dg_sum += temp1;

			if(continual)
				temp1 = (*ptr)[i].get_span() - (*ptr)[i - 1].get_span();
			else  temp1 = (*ptr)[i].get_span() - span;
			if(temp1 > ds_max)  { ds_max = temp1;  ds_max_index = i; }
			if(temp1 < ds_min)  { ds_min = temp1;  ds_min_index = i; }
			ds_sum += temp1;

			if(continual)
				temp1 = (*ptr)[i].get_s_size() - (*ptr)[i - 1].get_s_size();
			else  temp1 = (*ptr)[i].get_s_size() - s_size;
			if(temp1 > dn_max)  { dn_max = temp1;  dn_max_index = i; }
			if(temp1 < dn_min)  { dn_min = temp1;  dn_min_index = i; }
			dn_sum += temp1;
		}
	}

	fout << fixed << setprecision(2);
	if(language == English)
	{
		fout << "Other stats:\n" << "Absolute value of chroma value (Hua) (|k|): "
			  << "highest = " << _k_max << "(@ #" << k_max_index + 1 << "); "
			  << "lowest = "  << _k_min << "(@ #" << k_min_index + 1 << "); "
			  << "average = " << k_sum / count_ << ";\n";
		fout << "Absolute value of gross chroma value (|kk|): "
			  << "highest = " << _kk_max << "(@ #" << kk_max_index + 1 << "); "
			  << "lowest = "  << _kk_min << "(@ #" << kk_min_index + 1 << "); "
			  << "average = " << kk_sum / count_ << ";\n";
		fout << "Q-Indicator value (Chen) (Q): "
			  << "highest = " << _q_max << "(@ #" << q_max_index + 1 << "); "
			  << "lowest = "  << _q_min << "(@ #" << q_min_index + 1 << "); "
			  << "average = " << (double)q_sum / count_ << ";\n";
		fout << "Lateral similarity (X%): "
			  << "highest = " << _x_max << "(@ #" << x_max_index + 1 << "); "
			  << "lowest = "  << _x_min << "(@ #" << x_min_index + 1 << "); "
			  << "average = " << (double)x_sum / count_ << ";\n";
		fout << "Number of common tones (C): "
			  << "most = "    << _c_max << "(@ #" << c_max_index + 1 << "); "
			  << "least = "   << _c_min << "(@ #" << c_min_index + 1 << "); "
			  << "average = " << (double)c_sum / count_ << ";\n";
		fout << "Unioned chord span in fifths (SS) "
			  << "highest = " << _ss_max << "(@ #" << ss_max_index + 1 << "); "
			  << "lowest = "  << _ss_min << "(@ #" << ss_min_index + 1 << "); "
			  << "average = " << (double)ss_sum / count_ << ";\n";
		fout << "Total voice leading (Σvec): "
			  << "highest = " << _sv_max << "(@ #" << sv_max_index + 1 << "); "
			  << "lowest = "  << _sv_min << "(@ #" << sv_min_index + 1 << "); "
			  << "average = " << (double)sv_sum / count_ << ";\n";
		fout << "Chord cardinality (N) "
			  << "most = "    << _n_max << "(@ #" << n_max_index + 1 << "); "
			  << "least = "   << _n_min << "(@ #" << n_min_index + 1 << "); "
			  << "average = " << (double)n_sum / count << ";\n";
		fout << "Chord voice cardinality (M): "
			  << "most = "    << _m_max << "(@ #" << m_max_index + 1 << "); "
			  << "least = "   << _m_min << "(@ #" << m_min_index + 1 << "); "
			  << "average = " << (double)m_sum / count << ";\n";
		fout << "Chord cardinality / Chord voice cardinality (N / M): "
			  << "highest = " << nm_max << "(@ #" << nm_max_index + 1 << "); "
			  << "lowest = "  << nm_min << "(@ #" << nm_min_index + 1 << "); "
			  << "average = " << (double)nm_sum / count << ";\n";
		fout << "Chord thickness (H): "
			  << "highest = " << _h_max << "(@ #" << h_max_index + 1 << "); "
			  << "lowest = "  << _h_min << "(@ #" << h_min_index + 1 << "); "
			  << "average = " << h_sum / count << ";\n";
		fout << "Chord tension (T): "
			  << "highest = " << _t_max << "(@ #" << t_max_index + 1 << "); "
			  << "lowest = "  << _t_min << "(@ #" << t_min_index + 1 << "); "
			  << "average = " << t_sum / count << ";\n";
		fout << "Chord root (Hindemith) (R):  "
			  << "highest = " << _r_max << "(@ #" << r_max_index + 1 << "); "
			  << "lowest = "  << _r_min << "(@ #" << r_min_index + 1 << "); "
			  << "average = " << (double)r_sum / count << ";\n";
		fout << "Chord geometric center (G%): "
			  << "highest = " << _g_max << "(@ #" << g_max_index + 1 << "); "
			  << "lowest = "  << _g_min << "(@ #" << g_min_index + 1 << "); "
			  << "average = " << (double)g_sum / count << ";\n";
		fout << "Chord span in fifths (S): "
			  << "highest = " << _s_max << "(@ #" << s_max_index + 1 << "); "
			  << "lowest = "  << _s_min << "(@ #" << s_min_index + 1 << "); "
			  << "average = " << (double)s_sum / count << ";\n";
		fout << "Difference of chord tension (dt): "
			  << "highest = " << dt_max << "(@ #" << dt_max_index + 1 << "); "
			  << "lowest = "  << dt_min << "(@ #" << dt_min_index + 1 << "); "
			  << "average = " << dt_sum / count_ << ";\n";
		fout << "Difference of chord root (dr): "
			  << "highest = " << dr_max << "(@ #" << dr_max_index + 1 << "); "
			  << "lowest = "  << dr_min << "(@ #" << dr_min_index + 1 << "); "
			  << "average = " << (double)dr_sum / count_ << ";\n";
		fout << "Difference of chord geometric center (dg%): "
			  << "highest = " << dg_max << "(@ #" << dg_max_index + 1 << "); "
			  << "lowest = "  << dg_min << "(@ #" << dg_min_index + 1 << "); "
			  << "average = " << (double)dg_sum / count_ << ";\n";
		fout << "Difference of chord span in fifths (ds): "
			  << "highest = " << ds_max << "(@ #" << ds_max_index + 1 << "); "
			  << "lowest = "  << ds_min << "(@ #" << ds_min_index + 1 << "); "
			  << "average = " << (double)ds_sum / count_ << ";\n";
		fout << "Difference of chord cardinality (dn): "
			  << "highest = " << dn_max << "(@ #" << dn_max_index + 1 << "); "
			  << "lowest = "  << dn_min << "(@ #" << dn_min_index + 1 << "); "
			  << "average = " << (double)dn_sum / count_ << ";\n";
	}
	else
	{
		fout << "其他统计：\n" << "华氏色彩度绝对值 (|k|): "
			  << "最高 = " << _k_max << "(@ #" << k_max_index + 1 << ")；"
			  << "最低 = " << _k_min << "(@ #" << k_min_index + 1 << ")；"
			  << "平均 = " << k_sum / count_ << "；\n";
		fout << "华氏毛色彩度绝对值 (|kk|): "
			  << "最高 = " << _kk_max << "(@ #" << kk_max_index + 1 << ")；"
			  << "最低 = " << _kk_min << "(@ #" << kk_min_index + 1 << ")；"
			  << "平均 = " << kk_sum / count_ << "；\n";
		fout << "陈氏Q指标数值 (Q): "
			  << "最高 = " << _q_max << "(@ #" << q_max_index + 1 << ")；"
			  << "最低 = " << _q_min << "(@ #" << q_min_index + 1 << ")；"
			  << "平均 = " << (double)q_sum / count_ << "；\n";
		fout << "横向相似度 (X%): "
			  << "最高 = " << _x_max << "(@ #" << x_max_index + 1 << ")；"
			  << "最低 = " << _x_min << "(@ #" << x_min_index + 1 << ")；"
			  << "平均 = " << (double)x_sum / count_ << "；\n";
		fout << "进行共同音个数 (C): "
			  << "最多 = " << _c_max << "(@ #" << c_max_index + 1 << ")；"
			  << "最少 = " << _c_min << "(@ #" << c_min_index + 1 << ")；"
			  << "平均 = " << (double)c_sum / count_ << "；\n";
		fout << "相邻和弦的合跨度 (SS): "
			  << "最高 = " << _ss_max << "(@ #" << ss_max_index + 1 << ")；"
			  << "最低 = " << _ss_min << "(@ #" << ss_min_index + 1 << ")；"
			  << "平均 = " << (double)ss_sum / count_ << "；\n";
		fout << "声部运动总大小 (Σvec): "
			  << "最高 = " << _sv_max << "(@ #" << sv_max_index + 1 << ")；"
			  << "最低 = " << _sv_min << "(@ #" << sv_min_index + 1 << ")；"
			  << "平均 = " << (double)sv_sum / count_ << "；\n";
		fout << "和弦音数 (n): "
			  << "最多 = " << _n_max << "(@ #" << n_max_index + 1 << ")；"
			  << "最少 = " << _n_min << "(@ #" << n_min_index + 1 << ")；"
			  << "平均 = " << (double)n_sum / count << "；\n";
		fout << "和弦声部数 (M): "
			  << "最多 = " << _m_max << "(@ #" << m_max_index + 1 << ")；"
			  << "最少 = " << _m_min << "(@ #" << m_min_index + 1 << ")；"
			  << "平均 = " << (double)m_sum / count << "；\n";
		fout << "和弦音数 / 和弦声部数 (n / m): "
			  << "最高 = " << nm_max << "(@ #" << nm_max_index + 1 << ")；"
			  << "最低 = " << nm_min << "(@ #" << nm_min_index + 1 << ")；"
			  << "平均 = " << (double)nm_sum / count << "；\n";
		fout << "和弦八度厚度 (H): "
			  << "最高 = " << _h_max << "(@ #" << h_max_index + 1 << ")；"
			  << "最低 = " << _h_min << "(@ #" << h_min_index + 1 << ")；"
			  << "平均 = " << h_sum / count << "；\n";
		fout << "和弦紧张度 (T): "
			  << "最高 = " << _t_max << "(@ #" << t_max_index + 1 << ")；"
			  << "最低 = " << _t_min << "(@ #" << t_min_index + 1 << ")；"
			  << "平均 = " << t_sum / count << "；\n";
		fout << "和弦根音 (欣氏法) (R): "
			  << "最高 = " << _r_max << "(@ #" << r_max_index + 1 << ")；"
			  << "最低 = " << _r_min << "(@ #" << r_min_index + 1 << ")；"
			  << "平均 = " << (double)r_sum / count << "；\n";
		fout << "和弦几何中心位置 (G%): "
			  << "最高 = " << _g_max << "(@ #" << g_max_index + 1 << ")；"
			  << "最低 = " << _g_min << "(@ #" << g_min_index + 1 << ")；"
			  << "平均 = " << (double)g_sum / count << "；\n";
		fout << "和弦纯五跨度 (S): "
			  << "最高 = " << _s_max << "(@ #" << ss_max_index + 1 << ")；"
			  << "最低 = " << _s_min << "(@ #" << ss_min_index + 1 << ")；"
			  << "平均 = " << (double)s_sum / count << "；\n";
		fout << "和弦紧张度之差 (dt): "
			  << "最高 = " << dt_max << "(@ #" << dt_max_index + 1 << ")；"
			  << "最低 = " << dt_min << "(@ #" << dt_min_index + 1 << ")；"
			  << "平均 = " << dt_sum / count_<< "；\n";
		fout << "和弦根音之差 (dr): "
			  << "最高 = " << dr_max << "(@ #" << dr_max_index + 1 << ")；"
			  << "最低 = " << dr_min << "(@ #" << dr_min_index + 1 << ")；"
			  << "平均 = " << (double)dr_sum / count_ << "；\n";
		fout << "和弦几何中心位置之差 (dg%): "
			  << "最高 = " << dg_max << "(@ #" << dg_max_index + 1 << ")；"
			  << "最低 = " << dg_min << "(@ #" << dg_min_index + 1 << ")；"
			  << "平均 = " << (double)dg_sum / count_ << "；\n";
		fout << "和弦纯五跨度之差 (ds): "
			  << "最高 = " << ds_max << "(@ #" << ds_max_index + 1 << ")；"
			  << "最低 = " << ds_min << "(@ #" << ds_min_index + 1 << ")；"
			  << "平均 = " << (double)ds_sum / count_ << "；\n";
		fout << "和弦音数之差 (dn): "
			  << "最高 = " << dn_max << "(@ #" << dn_max_index + 1 << ")；"
			  << "最低 = " << dn_min << "(@ #" << dn_min_index + 1 << ")；"
			  << "平均 = " << (double)dn_sum / count_ << "。\n";
	}
}

void Chord::print_end()
{
	if(language == Chinese)
	{
		fout << "分析报告结果指标说明：\n"
			  << "【每个和弦】(音名列表) - 系统判断和弦音名（从低到高）； t - 和弦紧张度； s - 和弦纯五跨度； vec - 音程涵量表； \n"
				  "d - 音程结构表； n - 和弦音数； m - 和弦声部数； h - 和弦八度厚度； g - 几何中心位置； r - 和弦根音（欣氏法）。\n"
			  << "【和弦进行】k - 华氏色彩度； kk - 毛色彩度； c - 进行共同音个数；\n"
				  "ss - 相邻和弦的合跨度； sv - 声部运动总大小； v - 声部运动方向及距离（半音个数）； \n"
				  "Q - 陈氏Q指标数值； x - 横向相似度； dt, dr, dg, ds, dn - 相应各项指标的变化量。\n"
			  << "【星号注解】* - 等音记谱（色值溢出）； ** - 等音记谱（色差溢出）。\n\n";
	}
	fout << "==========\n";
	print_stats();
	end = clock();
	double dur = (double) (end - begin) / CLOCKS_PER_SEC;
	if(language == English)
		fout << "\nGeneration completed in " << fixed << setprecision(2) << dur << " seconds.";
	else  fout << "\n本次生成耗时 " << fixed << setprecision(2) << dur << " 秒。";
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
	if(g_center > g_max || g_center < g_min)
	{
		if(language == English)
			throw "The geometric center of the chord is not in the range you set. Please try again.";
		else  throw "和弦的几何中心不在您设置的范围内。请重试。";
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

	set_span(*this, true);
	if(span > s_max || span < s_min)
	{
		if(language == English)
			throw "The span of the chord is not in the range you set. Please try again.";
		else  throw "和弦的纯五跨度不在您设置的范围内。请重试。";
	}
	set_chroma_old();
	set_name();
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
		intersection = intersect(note_set, overall_scale, true);
		set_span(*this, true);
		set_chroma_old();
		set_name();
		bool b = (thickness <= h_max) && (thickness >= h_min) && (root <= r_max) && (root >= r_min)
				&& (g_center <= g_max) && (g_center >= g_min) && (s_size <= n_max) && (s_size >= n_min)
				&& (span <= s_max) && (span >= s_min)
				&& (find(chord_library, set_id) == -1) && (find(bass_avail, alignment[0]) == -1)
				&& (align_mode == Unlimited || valid_alignment(*this))
				&& ( !(enable_pedal && continual && !include_pedal(*this))
				&& ( !(enable_ex && !valid_exclusion(*this)) ) && ((int)intersection.size() == s_size));
		if(b)  break;
	}
}

Chord::Chord() = default;
Chord::Chord(const vector<int>& _notes, double _chroma_old)
// construct a chord with a vector ('notes'); used in utilities and analyser
{
	notes = _notes;
	remove_duplicate(notes);
	bubble_sort(notes);
	enable_pedal = false;
	unique_mode = Disabled;
	prev_chroma_old = _chroma_old;
	sim_orig = 100;

//
	m_min = 1;   m_max = 15;
	n_min = 1;   n_max = 12;
	lowest = 0;  highest = 127;
	sv_min = 0;  sv_max = 100;
//

	init( static_cast<ChordData&>(*this) );
	set_span(*this, true);
	set_chroma_old();
	set_name();
}
Chord::Chord(const Chord& A): ChordData(A) {};

int& Chord::get_set_id()  { return set_id; }

void Chord::_set_vl_max(const int & _vl_max)
{ vl_max = _vl_max; }

void Chord::Main()
{
	begin = clock();
	char name1[200], name2[200], name3[200];
	strcpy(name1, output_path);
	strcpy(name2, output_path);
#ifdef QT_CORE_LIB
	strcpy(name3, ((QString)output_name).toLocal8Bit().data());
#else
	strcpy(name3, output_name);
#endif
	strcat(name1, name3);
	strcat(name2, name3);
	strcat(name1, ".txt");
	strcat(name2, ".mid");
	if(output_mode != MidiOnly)
		fout.open(name1, ios::trunc);
	if(output_mode != TextOnly)
		m_fout.open(name2, ios::trunc | ios::binary);
	record.clear();
	rec_id.clear();

	similarity = MINF;
	sv = MINF;
	common_note = MINF;
	set_max_count();
	set_expansion_indexes();
	init( static_cast<ChordData&>(*this) );
	printInitial(language);
	if(language == English)
		fout << "Results:\n";
	else  fout << "生成结果：\n";

	if(continual)
	{
		for(progr_count = 1; progr_count <= loop_count; ++progr_count)
		{
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

void Chord::find_vec(Chord& new_chord, bool in_analyser, bool in_substitution)
// If in substitution mode, we will iterate through all inversions of 'new_chord' and get 'vec' and 'sv'.
// The one with the smallest 'sv' will be chosen.
// Remark: set_param2 is called.
{
	if(!in_substitution)
		_find_vec(new_chord);
	else
	{
		int min_index = 0, min_sv = INF;
		vector<int> min_vec;
		int size = new_chord.t_size;
		vector<int> orig_notes = new_chord.notes;
		vector<int> inversion;
		Chord copy(new_chord);

		for(int i = 0; i <= 2 * size; ++i)
		{
			inversion.clear();
			for(int j = 0; j < size; ++j)
				inversion.push_back( orig_notes[ (j + i) % size ] + ((j + i) / size - 1) * 12 );
			// For i = 0, the whole 'new_chord' is flipped down an octave;
			// for i = 2 * size, it is flipped up an octave.
			copy.notes = inversion; // We do not care other parameters of a chord.
			_find_vec(copy);
			int size_ = copy.vec.size();
			bool b = true;
			for(int j = 0; j < size_; ++j)
				if(abs(copy.vec[j]) > 6)
				{
					b = false;
					break;
				}
			if(b && copy.sv < min_sv)
			{
				min_sv = copy.sv;
				min_index = i;
				min_vec = copy.vec;
			}
		}

		inversion.clear();
		for(int j = 0; j < size; ++j)
			inversion.push_back( orig_notes[ (j + min_index) % size ] + ((j + min_index) / size - 1) * 12 );
		new_chord.notes = inversion;
		new_chord.sv = min_sv;
		new_chord.vec = min_vec;
	}
	set_param2(new_chord, in_analyser, in_substitution);
}

#ifdef QT_CORE_LIB
void Chord::set_est_time(const int& value, bool in_substitution)
{	
	const int MAX = in_substitution ? prgdialog_sub -> maximum() : prgdialog -> maximum();
	static double  prev[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	const double weight[9] = {1, 1, 1.5, 1.5, 1.5, 2, 2, 2, 1.5};
	// To avoid huge fluctuation, we will calculate weighted mean with previous remaining times.
	// However, the array itself is given somewhat randomly.
	if(value == MAX)
	{
		for(int i = 0; i < 9; ++i)
			prev[i] = 0;
		return;
	}

	double dur;
	if(in_substitution)
	{
		end_sub = clock();
		dur = (end_sub - begin_loop_sub) / CLOCKS_PER_SEC;
	}
	else
	{
		end = clock();
		dur = (end - begin_progr) / CLOCKS_PER_SEC;
	}

	double rem = (double)dur / (value + 1.0) * (MAX - value)
					 * ( ( value + 2 * MAX ) / ( 2 * value + MAX ) );
	// The last term is a modification. At first it is 2 and it gradually drops to 1.
	// This is set because the speed of the program becomes slower when the size of 'new_chord' grows up.
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
	QStringList str4 = {") ", "） "};
	if(in_substitution)
		labeltext_sub = labeltext_sub + str3[language]+ time + str4[language];
	else  labeltext = labeltext + str3[language]+ time + str4[language];
}

void Chord::abort(bool in_substitution)
{
	if(in_substitution)
		set_est_time(prgdialog_sub -> maximum(), true);
	else  set_est_time(prgdialog -> maximum(), false);
	if(language == English)
		throw "The generation is aborted.";
	else  throw "生成已中止。";
}
#endif

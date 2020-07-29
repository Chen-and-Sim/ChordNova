// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// chord.h

#ifndef CHORD
#define CHORD

#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "chorddata.h"
#ifdef QT_CORE_LIB
	#include <QApplication>
	#include <QProgressDialog>
#endif

using std::vector;

enum Language   {English, Chinese};
enum OutputMode {Both, MidiOnly, TextOnly};
enum UniqueMode {Disabled, RemoveDup, RemoveDupType};
enum AlignMode  {Interval, List, Unlimited};
enum VLSetting  {Percentage, Number, Default};

struct intervalData
{
	int interval;
	int octave_min;
	int octave_max;
	int num_min;
	int num_max;
};

class Chord: public ChordData
{
protected:
	Language language;
	char output_name[100];
	bool continual;
	OutputMode output_mode;
	int  loop_count;
	bool m_unchanged;
	bool nm_same;
	char database[2][100];
	char database_filename[100];
	int  database_size;
	bool enable_pedal;
	bool enable_more_rules;
	bool automatic;
	bool connect_pedal;
	bool interlace;
	char str_notes[100];
	UniqueMode unique_mode;
	vector<int> bass_avail;
	char align_db[2][100];
	char align_db_filename[100];
	int  align_db_size;
	AlignMode align_mode;
	bool in_bass;
	bool realign;
	int  period;
	VLSetting  vl_setting;
	bool enable_steady;
	bool enable_ascending;
	bool enable_descending;
	bool custom_vl_range;
	bool enable_rm;
	bool enable_ex;
	bool enable_sim;
	char exclusion[200];
	char str_sim[100];
	vector<int> exclusion_notes;
	vector<int> exclusion_roots;
	vector<intervalData> exclusion_intervals;
	vector<int> sim_period;
	vector<int> sim_min;
	vector<int> sim_max;
	char sort_order[30];

	double k_min, k_max;
	double t_min, t_max;
	int    c_min, c_max;
	int   sv_min, sv_max;
	int    m_min, m_max;
	int    n_min, n_max;
	int    r_min, r_max;
	double h_min, h_max;
	double g_min, g_max;
	int    x_min, x_max;
	int  highest, lowest;
	int   vl_min, vl_max;
	int    i_min, i_max, i_high, i_low;
	double steady_min, steady_max;
	double ascending_min, ascending_max;
	double descending_min, descending_max;

	clock_t begin, begin_progr, end;
	int exp_count;
	int progr_count;
	int c_size; // size of new_chords
	int set_id;
	long long vec_id;
	long long max_cnt;
	vector<int> rec_id;
	vector<long long> vec_ids;
	vector<ChordData> record;
	vector<ChordData> new_chords;

	void next(vector<int>&);
	void expand(Chord&, const int&, const int&);
	bool valid(Chord&);
	bool valid_alignment(Chord&);
	bool include_pedal(Chord&);
	bool valid_vec(Chord&);
	bool valid_exclusion(Chord&);
	bool valid_sim(Chord&);
	void set_vec_id(Chord&);
	void set_g_center(Chord&);
	void set_similarity(Chord&, Chord&, const int&);
	void set_param1();
	void set_param2(Chord&);
	void set_new_chords(Chord&);
	void set_max_count();
	void init(ChordData&);
	void get_progression();
	void sort_results();
	void print1 ();
	void print2 ();
	void print_stats();
	void to_midi();
	void check_original();
	void choose_original();

public:
	Chord();
	Chord(const vector<int>&);
	Chord(const Chord&);
	
	int& get_set_id();
	void Main();
	void _set_vl_max(const int&);
	void find_vec(Chord&);

#ifdef QT_CORE_LIB
protected:
	QProgressDialog* prgdialog;
	QString labeltext;
	void set_est_time(const int&);
	void abort();
#endif
};

extern double _tension[12];
extern int restriction[12];
extern vector<int> overall_scale;

#endif

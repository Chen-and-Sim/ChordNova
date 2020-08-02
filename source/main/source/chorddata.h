// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// chorddata.h

#ifndef CHORDDATA
#define CHORDDATA

#include <vector>
using std::vector;

class ChordData
{
protected:
	int t_size; // n  // size of notes
	int s_size; // m  // size of note_set
	double tension; // t
	double thickness; // h
	int root; // r
	double g_center; // g
	double chroma; // k
	int common_note; // c
	int sv; // s
	int similarity;
	int steady_count;
	int ascending_count;
	int descending_count;
	int root_movement;
	
	vector<int> notes;
	vector<int> note_set; // set, not necessarily beginning with 0
	vector<int> vec; // v
	vector<int> self_diff; // d
	vector<int> count_vec; // vec
	vector<int> alignment;

	void print(const ChordData&);

public:
	vector<int> pedal_notes_set;
	vector<int> pedal_notes;
	
	ChordData() = default;;
	ChordData(const ChordData& data) = default;
	
	int& get_t_size()           { return t_size; }
	int& get_s_size()           { return s_size; }
	double& get_tension()       { return tension; }
	double& get_thickness()     { return thickness; }
	int& get_root()             { return root; }
	double& get_g_center()      { return g_center; }
	double& get_chroma()        { return chroma; }
	int& get_common_note()      { return common_note; }
	int& get_sv()               { return sv; }
	int& get_similarity()       { return similarity; }
	int& get_steady_count()     { return steady_count; }
	int& get_ascending_count()  { return ascending_count; }
	int& get_descending_count() { return descending_count; }
	int& get_root_movement()    { return root_movement; }
	
	vector<int>& get_notes()           { return notes; }
	vector<int>& get_note_set()        { return note_set; }
	vector<int>& get_vec()             { return vec; }
	vector<int>& get_self_diff()       { return self_diff; }
	vector<int>& get_count_vec()       { return count_vec; }
	vector<int>& get_alignment()       { return alignment; } 
	vector<int>& get_pedal_notes()     { return pedal_notes; }
	vector<int>& get_pedal_notes_set() { return pedal_notes_set; }
	
	friend bool larger_chroma     (const ChordData&, const ChordData&);
	friend bool larger_tension    (const ChordData&, const ChordData&);
	friend bool larger_common     (const ChordData&, const ChordData&);
	friend bool larger_sv         (const ChordData&, const ChordData&);
	friend bool larger_t_size     (const ChordData&, const ChordData&);
	friend bool larger_s_size     (const ChordData&, const ChordData&);
	friend bool larger_root       (const ChordData&, const ChordData&);
	friend bool larger_thickness  (const ChordData&, const ChordData&);
	friend bool larger_g_center   (const ChordData&, const ChordData&);
	friend bool larger_similarity (const ChordData&, const ChordData&);
	friend bool superior_rm       (const ChordData&, const ChordData&);

	friend bool smaller_chroma    (const ChordData&, const ChordData&);
	friend bool smaller_tension   (const ChordData&, const ChordData&);
	friend bool smaller_common    (const ChordData&, const ChordData&);
	friend bool smaller_sv        (const ChordData&, const ChordData&);
	friend bool smaller_t_size    (const ChordData&, const ChordData&);
	friend bool smaller_s_size    (const ChordData&, const ChordData&);
	friend bool smaller_root      (const ChordData&, const ChordData&);
	friend bool smaller_thickness (const ChordData&, const ChordData&);
	friend bool smaller_g_center  (const ChordData&, const ChordData&);
	friend bool smaller_similarity(const ChordData&, const ChordData&);
	friend bool inferior_rm       (const ChordData&, const ChordData&);
};

extern bool larger_chroma     (const ChordData&, const ChordData&);
extern bool larger_tension    (const ChordData&, const ChordData&);
extern bool larger_common     (const ChordData&, const ChordData&);
extern bool larger_sv         (const ChordData&, const ChordData&);
extern bool larger_t_size     (const ChordData&, const ChordData&);
extern bool larger_s_size     (const ChordData&, const ChordData&);
extern bool larger_root       (const ChordData&, const ChordData&);
extern bool larger_thickness  (const ChordData&, const ChordData&);
extern bool larger_g_center   (const ChordData&, const ChordData&);
extern bool larger_similarity (const ChordData&, const ChordData&);
extern bool superior_rm       (const ChordData&, const ChordData&);

extern bool smaller_chroma    (const ChordData&, const ChordData&);
extern bool smaller_tension   (const ChordData&, const ChordData&);
extern bool smaller_common    (const ChordData&, const ChordData&);
extern bool smaller_sv        (const ChordData&, const ChordData&);
extern bool smaller_t_size    (const ChordData&, const ChordData&);
extern bool smaller_s_size    (const ChordData&, const ChordData&);
extern bool smaller_root      (const ChordData&, const ChordData&);
extern bool smaller_thickness (const ChordData&, const ChordData&);
extern bool smaller_g_center  (const ChordData&, const ChordData&);
extern bool smaller_similarity(const ChordData&, const ChordData&);
extern bool inferior_rm       (const ChordData&, const ChordData&);

const int _TOTAL = 11;
const char var[_TOTAL] = {'K', 'T', 'C', 'S', 'M', 'N', 'R', 'H', 'G', 'X', 'V'};
extern bool (*compare[_TOTAL][2]) (const ChordData&, const ChordData&);
extern vector<int> rm_priority;

#endif

// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// chorddata.cpp

#include <vector>
#include "chorddata.h"
#include "functions.h"

vector<int> rm_priority;

void ChordData::print(const ChordData& chord)
{
	fprint("new_chord = ", chord.notes, ", ", ",  ");
	fout << "k = " << chord.chroma - chroma << ",  "
		  << "x = " << chord.similarity << "%,  ";
	fprint("v = ", chord.vec, ", ", ",  ");
	fout << "c = " << chord.common_note << ",\n"
		  << "r = " << chord.root << ",  "
		  << "t = " << chord.tension << ",  ";
	fprint("vec = ", chord.count_vec, "\0", ",  ");
	fprint("d = ", chord.self_diff, ", ", ",  ");
	fout << "h = " << chord.thickness << ",  "
		  << "g = " << chord.g_center << "%\n\n";
}

bool larger_t_size(const ChordData& data1, const ChordData& data2)
{ return data1.t_size >= data2.t_size; }

bool larger_s_size(const ChordData& data1, const ChordData& data2)
{ return data1.s_size >= data2.s_size; }

bool larger_tension(const ChordData& data1, const ChordData& data2)
{ return data1.tension >= data2.tension; }

bool larger_thickness(const ChordData& data1, const ChordData& data2)
{ return data1.thickness >= data2.thickness; }

bool larger_root(const ChordData& data1, const ChordData& data2)
{ return data1.root >= data2.root; }

bool larger_g_center(const ChordData& data1, const ChordData& data2)
{ return data1.g_center >= data2.g_center; }

bool larger_chroma(const ChordData& data1, const ChordData& data2)
{ return data1.chroma >= data2.chroma; }

bool larger_common(const ChordData& data1, const ChordData& data2)
{ return data1.common_note >= data2.common_note; }

bool larger_sv(const ChordData& data1, const ChordData& data2)
{ return data1.sv >= data2.sv; }

bool larger_similarity(const ChordData& data1, const ChordData& data2)
{ return data1.similarity >= data2.similarity; }

bool superior_rm(const ChordData& data1, const ChordData& data2)
{ return rm_priority[data1.root_movement] <= rm_priority[data2.root_movement]; }


bool smaller_t_size(const ChordData& data1, const ChordData& data2)
{ return data1.t_size <= data2.t_size; }

bool smaller_s_size(const ChordData& data1, const ChordData& data2)
{ return data1.s_size <= data2.s_size; }

bool smaller_tension(const ChordData& data1, const ChordData& data2)
{ return data1.tension <= data2.tension; }

bool smaller_thickness(const ChordData& data1, const ChordData& data2)
{ return data1.thickness <= data2.thickness; }

bool smaller_root(const ChordData& data1, const ChordData& data2)
{ return data1.root <= data2.root; }

bool smaller_g_center(const ChordData& data1, const ChordData& data2)
{ return data1.g_center <= data2.g_center; }

bool smaller_chroma(const ChordData& data1, const ChordData& data2)
{ return data1.chroma <= data2.chroma; }

bool smaller_common(const ChordData& data1, const ChordData& data2)
{ return data1.common_note <= data2.common_note; }

bool smaller_sv(const ChordData& data1, const ChordData& data2)
{ return data1.sv <= data2.sv; }

bool smaller_similarity(const ChordData& data1, const ChordData& data2)
{ return data1.similarity <= data2.similarity; }

bool inferior_rm(const ChordData& data1, const ChordData& data2)
{ return rm_priority[data1.root_movement] >= rm_priority[data2.root_movement]; }


bool (*compare[_TOTAL][2]) (const ChordData&, const ChordData&) =
{ {larger_chroma, smaller_chroma}, {larger_tension, smaller_tension}, {larger_common, smaller_common},
  {larger_sv, smaller_sv},         {larger_t_size, smaller_t_size},   {larger_s_size, smaller_s_size},
  {larger_root, smaller_root}, {larger_thickness, smaller_thickness}, {larger_g_center, smaller_g_center},
  {larger_similarity, smaller_similarity}, {superior_rm, inferior_rm} };

// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// chorddata.cpp

#include <cstring>
#include <vector>
#include "chorddata.h"
#include "functions.h"

vector<int> rm_priority;

void ChordData::inverse_param()
{
	double temp = prev_chroma_old;
	prev_chroma_old = chroma_old;
	chroma_old = temp;
	chroma = -chroma;
	Q_indicator = -Q_indicator;
}

void ChordData::printInitial(Language language)
{
	if(language == English)
		fprint("Initial chord: ", notes, " ", ", ");
	else  fprint("起始和弦：", notes, " ", ", ");
	fout << "(" << name << ")\n"
		  << "t = " << fixed << setprecision(1) << round_double(tension, 1) << ",  "
		  << "s = " << span << ",  ";
	fprint("vec = ", count_vec, "\0", ",  ", false);
	fprint("d = ", self_diff);
	fout << "n = " << s_size << ",  "
		  << "m = " << t_size << ",  "
		  << "n/m = " << fixed << setprecision(2) << round_double((double)s_size / t_size, 2) << ",  "
		  << "h = " << fixed << setprecision(2) << round_double(thickness, 2) << ",  "
		  << "g = " << g_center << "%,  "
		  << (language == Chinese ? "根音：" : "root: ") << root_name
		  << " (r = " << root << ")\n\n";
}

void ChordData::print(const ChordData& chord, Language language)
{
	fprint("-> ", chord.notes, " ", ", ");
	fout << "(" << chord.name << ")";
	switch(chord.overflow_state)
	{
		case Single:     fout << "*\n";   break;
		case Total:      fout << "**\n";  break;
		case NoOverflow: fout << "\n";    break;
	}
	fout << "k = " << fixed << setprecision(1) << round_double(chord.chroma, 1) << ",  "
		  << "kk = " << fixed << setprecision(2) << round_double(chord.chroma_old - chord.prev_chroma_old, 2) << ",  "
		  << "c = " << chord.common_note << ",  "
		  << "ss = " << chord.sspan << ",  "
		  << "sv = " << chord.sv << ",  ";
	fprint("v = ", chord.vec);
	fout << "t = " << fixed << setprecision(1) << round_double(chord.tension, 1) << ",  "
		  << "s = " << chord.span << ",  ";
	fprint("vec = ", chord.count_vec, "\0", ",  ", false);
	fprint("d = ", chord.self_diff);
	fout << "n = " << chord.s_size << ",  "
		  << "m = " << chord.t_size << ",  "
		  << "n/m = " << fixed << setprecision(2) << round_double((double)chord.s_size / chord.t_size, 2) << ",  "
		  << "h = " << fixed << setprecision(2) << round_double(chord.thickness, 2) << ",  "
		  << "g = " << chord.g_center << "%,  "
		  << (language == Chinese ? "根音：" : "root: ") << chord.root_name
		  << " (r = " << chord.root << ")\n";
	fout << "Q = " << fixed << setprecision(1) << round_double(chord.Q_indicator, 1) << ",  "
		  << "x = " << chord.similarity << "%,  "
		  << "dr = " << chord.root - root << ",  "
		  << "dn = " << chord.s_size - s_size << ",  "
		  << "dt = " << fixed << setprecision(1) << round_double(chord.tension - tension, 1) << ",  "
		  << "ds = " << chord.span - span << ",  "
		  << "dg = " << chord.g_center - g_center << "%\n\n";
}

void ChordData::print_analysis(const ChordData& antechord, const ChordData& postchord,
										 const char* str_ante, const char* str_post, Language language)
{
	if(hide_octave)
		stream << "(" << antechord.name << ") -> (" << postchord.name << ")";
	else
		stream << "(" << antechord.name_with_octave << ") -> (" << postchord.name_with_octave << ")";
	switch(postchord.overflow_state)
	{
		case Single:     stream << "*\n";   break;
		case Total:      stream << "**\n";  break;
		case NoOverflow: stream << "\n";    break;
	}
	bool b = ( different_name(str_ante, antechord.name) || different_name(str_post, postchord.name) );
	if(b)  stream << "( (" << str_ante << ") -> (" << str_post << ") )\n";
	sprint("", antechord.notes, " ", " -> ");
	sprint("", postchord.notes, " ", "\n\n");

	stream << (language == Chinese ? "前和弦：\n" : "Antechord: \n");
	stream << "t = " << fixed << setprecision(1) << round_double(antechord.tension, 1) << ",  "
			 << "s = " << antechord.span << ",  ";
	sprint("vec = ", antechord.count_vec, "\0", ",  ", false);
	sprint("d = ", antechord.self_diff);
	stream << "n = " << antechord.s_size << ",  "
			 << "m = " << antechord.t_size << ",  "
			 << "n/m = " << fixed << setprecision(2) << round_double((double)antechord.s_size / antechord.t_size, 2) << ",  "
			 << "h = " << fixed << setprecision(2) << round_double(antechord.thickness, 2) << ",  "
			 << "g = " << antechord.g_center << "%,  "
			 << (language == Chinese ? "根音：" : "root: ") << antechord.root_name
			 << " (r = " << antechord.root << ")\n\n";

	stream << (language == Chinese ? "后和弦：\n" : "Postchord: \n");
	stream << "t = " << fixed << setprecision(1) << round_double(postchord.tension, 1) << ",  "
			 << "s = " << postchord.span << ",  ";
	sprint("vec = ", postchord.count_vec, "\0", ",  ", false);
	sprint("d = ", postchord.self_diff);
	stream << "n = " << postchord.s_size << ",  "
			 << "m = " << postchord.t_size << ",  "
			 << "n/m = " << fixed << setprecision(2) << round_double((double)postchord.s_size / postchord.t_size, 2) << ",  "
			 << "h = " << fixed << setprecision(2) << round_double(postchord.thickness, 2) << ",  "
			 << "g = " << postchord.g_center << "%,  "
			 << (language == Chinese ? "根音：" : "root: ") << postchord.root_name
			 << " (r = " << postchord.root << ")\n\n";

	stream << (language == Chinese ? "和弦进行：\n" : "Chord progression: \n");
	stream << "k = " << fixed << setprecision(1) << round_double(postchord.chroma, 1) << ",  "
			 << "kk = " << fixed << setprecision(2) << round_double(postchord.chroma_old - postchord.prev_chroma_old, 2) << ",  "
			 << "c = " << postchord.common_note << ",  "
			 << "ss = " << postchord.sspan << ",  "
			 << "sv = " << postchord.sv << ",  ";
	sprint("v = ", postchord.vec);
	stream << "Q = " << fixed << setprecision(1) << round_double(postchord.Q_indicator, 1) << ",  "
			 << "x = " << postchord.similarity << "%,  "
			 << "dr = " << postchord.root - antechord.root << ",  "
			 << "dn = " << postchord.s_size - antechord.s_size << ",  "
			 << "dt = " << fixed << setprecision(1) << round_double(postchord.tension - antechord.tension, 1) << ",  "
			 << "ds = " << postchord.span - antechord.span << ",  "
			 << "dg = " << postchord.g_center - antechord.g_center << "%";
}

void ChordData::print_substitution(const char* param, bool print_ante, bool print_post,
											  const ChordData& antechord, const ChordData& postchord, Language language)
// const char var[VAR_TOTAL] = {'P', 'N', 'T', 'K', 'C', 'a', 'A', 'm', 'h', 'g', 'S', 'Q', 'X', 'k', 'R', 'V'};
// name of parameters (a = S, A = SS, S = sv, k = KK)
{
	if( !(print_ante || print_post) )  return;

	fout << "(" << antechord.name << ") -> (" << postchord.name << ")";
	switch(postchord.overflow_state)
	{
		case Single:     fout << "*";   break;
		case Total:      fout << "**";  break;
		case NoOverflow: fout << "";    break;
	}

	const ChordData* pChord = (print_post ? &postchord : &antechord);

	if( !(print_ante && print_post) )
	{
		if(strchr(param, var[2]) != nullptr)
			fout << ",  t = " << fixed << setprecision(1) << round_double(pChord -> tension, 1);
		if(strchr(param, var[5]) != nullptr)
			fout << ",  s = " << pChord -> span;
		if(strchr(param, var[1]) != nullptr)
			fout << ",  n = " << pChord -> s_size;
		if(strchr(param, var[14]) != nullptr || strchr(param, var[15]) != nullptr)
			fout << (language == Chinese ? ",  根音：" : ",  root: ") << pChord -> root_name
				  << " (r = " << pChord -> root << ")";
	}
	else
	{
		if(strchr(param, var[2]) != nullptr)
			fout << ",  t_a = " << fixed << setprecision(1) << round_double(antechord.tension, 1)
				  << ",  t_b = " << fixed << setprecision(1) << round_double(postchord.tension, 1);
		if(strchr(param, var[5]) != nullptr)
			fout << ",  s_a = " << antechord.span << ",  s_b = " << postchord.span;
		if(strchr(param, var[1]) != nullptr)
			fout << ",  n_a = " << antechord.s_size << ", n_b = " << postchord.s_size;
		if(strchr(param, var[14]) != nullptr || strchr(param, var[15]) != nullptr)
			fout << (language == Chinese ? ",  根音（前）：" : ",  root_a: ") << antechord.root_name
				  << " (r_a = " << antechord.root << ")"
				  << (language == Chinese ? ",  根音（后）：" : ",  root_b: ") << postchord.root_name
				  << " (r_b = " << postchord.root << ")";
	}

	if(strchr(param, var[3]) != nullptr)
		fout << ",  k = " << fixed << setprecision(1) << round_double(pChord -> chroma, 1);
	if(strchr(param, var[13]) != nullptr)
		fout << ",  kk = " << fixed << setprecision(2) << round_double(pChord -> chroma_old - pChord -> prev_chroma_old, 2);
	if(strchr(param, var[4]) != nullptr)
		fout << ",  c = " << pChord -> common_note;
	if(strchr(param, var[5]) != nullptr)
		fout << ",  ss = " << pChord -> sspan;
	if(strchr(param, var[10]) != nullptr)
		fout << ",  sv = " << pChord -> sv;
	if(strchr(param, var[11]) != nullptr)
		fout << ",  Q = " << fixed << setprecision(1) << round_double(pChord -> Q_indicator, 1);
	if(strchr(param, var[12]) != nullptr)
		fout << ",  x = " << pChord -> similarity << "%";
	if(strchr(param, var[0]) != nullptr)
		fout << ",  p = " << pChord -> sim_orig << "%";

	if(strchr(param, var[14]) != nullptr)
		fout << ",  dr = " << postchord.root - antechord.root;
	if(strchr(param, var[1]) != nullptr)
		fout << ",  dn = " << postchord.s_size - antechord.s_size;
	if(strchr(param, var[2]) != nullptr)
		fout << ",  dt = " << fixed << setprecision(1) << round_double(postchord.tension - antechord.tension, 1);
	if(strchr(param, var[5]) != nullptr)
		fout << ",  ds = " << postchord.span - antechord.span;
	fout << endl;
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

bool larger_span(const ChordData& data1, const ChordData& data2)
{ return data1.span >= data2.span; }

bool larger_sspan(const ChordData& data1, const ChordData& data2)
{ return data1.sspan >= data2.sspan; }

bool larger_g_center(const ChordData& data1, const ChordData& data2)
{ return data1.g_center >= data2.g_center; }

bool larger_chroma(const ChordData& data1, const ChordData& data2)
{ return data1.chroma >= data2.chroma; }

bool larger_chroma_old(const ChordData& data1, const ChordData& data2)
{ return data1.chroma_old >= data2.chroma_old; }

bool larger_common(const ChordData& data1, const ChordData& data2)
{ return data1.common_note >= data2.common_note; }

bool larger_sv(const ChordData& data1, const ChordData& data2)
{ return data1.sv >= data2.sv; }

bool larger_similarity(const ChordData& data1, const ChordData& data2)
{ return data1.similarity >= data2.similarity; }

bool larger_sim_orig(const ChordData& data1, const ChordData& data2)
{ return data1.sim_orig >= data2.sim_orig; }

bool larger_Q_indicator(const ChordData& data1, const ChordData& data2)
{ return data1.Q_indicator >= data2.Q_indicator; }

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

bool smaller_span(const ChordData& data1, const ChordData& data2)
{ return data1.span <= data2.span; }

bool smaller_sspan(const ChordData& data1, const ChordData& data2)
{ return data1.sspan <= data2.sspan; }

bool smaller_g_center(const ChordData& data1, const ChordData& data2)
{ return data1.g_center <= data2.g_center; }

bool smaller_chroma(const ChordData& data1, const ChordData& data2)
{ return data1.chroma <= data2.chroma; }

bool smaller_chroma_old(const ChordData& data1, const ChordData& data2)
{ return data1.chroma_old <= data2.chroma_old; }

bool smaller_common(const ChordData& data1, const ChordData& data2)
{ return data1.common_note <= data2.common_note; }

bool smaller_sv(const ChordData& data1, const ChordData& data2)
{ return data1.sv <= data2.sv; }

bool smaller_similarity(const ChordData& data1, const ChordData& data2)
{ return data1.similarity <= data2.similarity; }

bool smaller_sim_orig(const ChordData& data1, const ChordData& data2)
{ return data1.sim_orig <= data2.sim_orig; }

bool smaller_Q_indicator(const ChordData& data1, const ChordData& data2)
{ return data1.Q_indicator <= data2.Q_indicator; }

bool inferior_rm(const ChordData& data1, const ChordData& data2)
{ return rm_priority[data1.root_movement] >= rm_priority[data2.root_movement]; }


bool (*compare[VAR_TOTAL][2]) (const ChordData&, const ChordData&) =
{ {larger_sim_orig, smaller_sim_orig},     {larger_s_size, smaller_s_size},
  {larger_tension, smaller_tension},		 {larger_chroma, smaller_chroma},
  {larger_common, smaller_common},			 {larger_span, smaller_span},
  {larger_sspan, smaller_sspan},				 {larger_t_size, smaller_t_size},
  {larger_thickness, smaller_thickness},   {larger_g_center, smaller_g_center},
  {larger_sv, smaller_sv},						 {larger_Q_indicator, smaller_Q_indicator},
  {larger_similarity, smaller_similarity}, {larger_chroma_old, smaller_chroma_old},
  {larger_root, smaller_root},				 {superior_rm, inferior_rm} };

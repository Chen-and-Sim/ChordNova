// SmartChordGen-utility-ChordCheck v3.0 [Build: 2020.11.27]
// Lists some information of two given chords and checks whether the progression
// from one to another meets the requirements of the main program (SmartChordGen).
// (c) 2020 Wenge Chen, Ji-woon Sim.

#include <fstream>
#include <iostream>
#include <vector>

#include "../../main/chord.h"
#include "../../main/chord.cpp"
#include "../../main/chorddata.h"
#include "../../main/chorddata.cpp"
#include "../../main/functions.h"
#include "../../main/functions.cpp"

using namespace std;

void isSmart(Chord& A, Chord& B, const int& omit)
{
	int i;
	int t_size1 = A.get_notes().size(), t_size2 = B.get_notes().size();
	for(i = 0; i < t_size1; ++i)
		if(A.get_notes()[i] > 127 || A.get_notes()[i] < 0)
			throw "each note in chord A should be between 0 and 127.";
	for(i = 0; i < t_size2; ++i)
		if(B.get_notes()[i] > 127 || B.get_notes()[i] < 0)
			throw "each note in chord B should be between 0 and 127.";

	vector<int> intersection = intersect(A.get_note_set(), overall_scale);
	if(intersection.size() < A.get_s_size())
		throw "chord A is not in the overall scale.";
	intersection = intersect(B.get_note_set(), overall_scale);
	if(intersection.size() < B.get_s_size())
		throw "chord B is not in the overall scale.";

	int pos1 = find(chord_library, A.get_set_id());
	if(pos1 != -1)
		throw "chord A is not in the chord library.";
	int pos2 = find(chord_library, B.get_set_id());
	if(pos2 != -1)
		throw "chord B is not in the chord library.";

	A.find_vec(B);
	int v_size = B.get_vec().size();
	for(i = 0; i < v_size; ++i)
		if(abs(B.get_vec()[i]) > 4)
			throw "there exists at least a part movement larger than a major third.";
	
	for(i = 1; i < v_size; ++i)
		if(B.get_vec()[0] == 0 || sign(B.get_vec()[i - 1]) != sign(B.get_vec()[i]) )
			break;
	if(i == v_size)  throw "the two chords are parallel.";
}

int main()
{
	cout << "[[  SmartChordGen v3.0 [Build: 2020.11.27]  ]]\n"
		  << "[[  (c) 2020 Wenge Chen, Ji-woon Sim.       ]]\n\n"
		  << " > Utility - Chord check:\n";

	vector<int> notes1, notes2;
	cout << "\n > Please input chord A, separated by a whitespace: ";
	inputVec(notes1);
	cout << "\n > Please input chord B, separated by a whitespace: ";
	inputVec(notes2);

	Chord A(notes1), B(notes2);
	B.find_vec(A);
	cout << "\n > Result:\n\n   ";
	cprint("d(A) = ",   A.get_self_diff(), ", ");
	cprint("vec(A) = ", A.get_count_vec(), "\0");
	cout << "t(A) = " << A.get_tension() << ", "
		  << "h(A) = " << A.get_thickness() << ", "
		  << "r(A) = " << A.get_root() << ", "
		  << "g(A) = " << A.get_g_center() << "%\n   ";
	
	cprint("d(B) = ",   B.get_self_diff(), ", ");
	cprint("vec(B) = ", B.get_count_vec(), "\0");
	cout << "t(B) = " << B.get_tension() << ", "
		  << "h(B) = " << B.get_thickness() << ", "
		  << "r(B) = " << B.get_root() << ", "
		  << "g(B) = " << B.get_g_center() << "%\n\n   ";
	
	cout << "n(A) = " << A.get_s_size() << ", "
		  << "n(B) = " << B.get_s_size() << ", "
		  << "diff(n(A, B)) = " << A.get_s_size() - B.get_s_size() << "\n   ";

	cout << "h(A) = " << A.get_thickness() << ", "
		  << "h(B) = " << B.get_thickness() << ", "
		  << "diff(h(A, B)) = " << A.get_thickness() - B.get_thickness() << "\n\n   ";
	
	cout << "k(A->B) = " << B.get_chroma() - A.get_chroma() << ", "
		  << "c(A, B) = " << intersect(A.get_notes(), B.get_notes(), true).size() << ", "
		  << "s(A, B) = " << A.get_sv() << "\n\n   ";

	vector<int> intersection = intersect(notes1, notes2);
	Chord I(intersection);
	for(int i = 0; i < I.get_note_set().size(); ++i)
		I.get_note_set()[i] -= I.get_note_set()[0];
	cprint("A^B = ", I.get_notes(), ", ");
	cprint("set(A^B) = ", I.get_note_set(), ", ", ",\n   ");
	cprint("d(A^B) = ",   I.get_self_diff(), ", ");
	cprint("vec(A^B) = ", I.get_count_vec(), "\0");
	cout<< "t(A^B) = " << I.get_tension() << ",\n\n   ";

	vector<int> _union = get_union(notes1, notes2);
	Chord U(_union);
	for(int i = 0; i < U.get_note_set().size(); ++i)
		U.get_note_set()[i] -= U.get_note_set()[0];
	cprint("AvB = ", U.get_notes(), ", ");
	cprint("set(AvB) = ", U.get_note_set(), ", ", ",\n   ");
	cprint("d(AvB) = ",   U.get_self_diff(), ", ");
	cprint("vec(AvB) = ", U.get_count_vec(), "\0");
	cout<< "t(AvB) = " << U.get_tension() << "\n\n";

	cout << " > Check isSmart (Y / N)? ";
	char ch;  inputY_N(ch);
	if(ch == 'Y' || ch == 'y')
	{
		int omit;
		cout << "\n > Allow note ommission (the default setting) (Y / N)? ";
		inputY_N(ch);
		if(ch == 'Y' || ch == 'y')
			omit = true;
		else  omit = false;

		char str1[100] = "\0", str2[100];
		cout << "\n > Please input the name of the database (.db) file (Press Enter for default - Pop and Jazz): ";
		inputFilename(str1, ".db", true, "../db/chord/00-pop-and-jazz");
		ignore_path_ext(str2, str1); 
		dbentry(str1);

		cout << "\n > Would you like to constraint the overall scale (Y / N)? ";
		inputY_N(ch);
		if(ch == 'Y' || ch == 'y')	
		{
			cout << "\n > Please input the desired constraining scale set (0 - 11, with space): ";
			inputVec(overall_scale);
		}

		try{
			isSmart(A, B, omit);
		}
		catch(const char* msg)
		{
			cout << "\n > Result: isSmart(" << str2 << ") == false\n"
				  << " > Detail: " << msg << "\n\n";
			system("pause");
			return 0;
		}
		cout << "\n > Result: isSmart(" << str2 << ") == true\n\n"; 
	}
	system("pause"); 
	return 0;
}


// SmartChordGen v2.5 [Build: 2020.8.8]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// savesettingsgui.cpp

#include "Interface.h"

void Interface::saveSettingsGui()
{
	save_dialog = new QDialog(this);
	save_dialog -> setFont(font);
	save_dialog -> setWindowModality(Qt::WindowModal);
	save_dialog -> setWindowFlag(Qt::Dialog, true);
	save_dialog -> setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	QStringList str1 = {"Save/Save as", "保存/另存为"};
	save_dialog -> setWindowTitle(str1[language]);

	QGridLayout* grid = new QGridLayout(save_dialog);
	QStringList str2 = {"Save current settings", "保存当前设置"};
	QStringList str3 = {"Save current settings as a new preset", "当前设置另存为新预设"};
	btn_save = new QRadioButton(str2[language], save_dialog);
	btn_save_as = new QRadioButton(str3[language], save_dialog);
	grid -> addWidget(btn_save, 0, 0, 1, 2);
	grid -> addWidget(btn_save_as, 1, 0, 1, 2);
	connect(btn_save, &QRadioButton::clicked, this, &Interface::set_save_mode);
	connect(btn_save_as, &QRadioButton::clicked, this, &Interface::set_save_mode);

	QButtonGroup* btns = new QButtonGroup(save_dialog);
	btns -> addButton(btn_save);
	btns -> addButton(btn_save_as);
	btn_save_as -> setChecked(true);
	save_mode = SaveAs;

	QStringList str4 = {"Plase input Chinese title: ", "请输入中文名："};
	QLabel* label1 = new QLabel(str4[language], save_dialog);
	grid -> addWidget(label1, 2, 0);

	edit_Chinese = new QLineEdit(save_dialog);
	grid -> addWidget(edit_Chinese, 2, 1);
	connect(edit_Chinese, &QLineEdit::editingFinished, this, &Interface::set_preset_Chinese);

	QStringList str5 = {"Plase input English title: ", "请输入英文名："};
	QLabel* label2 = new QLabel(str5[language], save_dialog);
	grid -> addWidget(label2, 3, 0);

	edit_English = new QLineEdit(save_dialog);
	grid -> addWidget(edit_English, 3, 1);
	connect(edit_English, &QLineEdit::editingFinished, this, &Interface::set_preset_English);

	QStringList str6 = {"Plase input the filename: \n(omit extension)", "请输入文件名：\n（省略扩展名）"};
	QLabel* label3 = new QLabel(str6[language], save_dialog);
	grid -> addWidget(label3, 4, 0);

	edit_filename = new QLineEdit(save_dialog);
	grid -> addWidget(edit_filename, 4, 1);
	connect(edit_filename, &QLineEdit::editingFinished, this, &Interface::set_preset_filename);

	QStringList str7 = {"Ok", "确定"};
	QPushButton* btn = new QPushButton(str7[language], save_dialog);
	grid -> addWidget(btn, 5, 0, 1, 2, Qt::AlignCenter);
	btn -> setFixedWidth(60);
	connect(btn, &QPushButton::clicked, this, &Interface::write_preset);
	save_dialog -> show();
}

void Interface::set_save_mode(bool state)
{
	Q_UNUSED(state);
	if(btn_save -> isChecked())
	{
		save_mode = Save;
		edit_Chinese  -> clear();
		edit_English  -> clear();
		edit_filename -> clear();
		edit_Chinese  -> setDisabled(true);
		edit_English  -> setDisabled(true);
		edit_filename -> setDisabled(true);
		preset[Chinese] = cur_preset[Chinese];
		preset[English] = cur_preset[English];
		preset_filename = cur_preset_filename;
	}
	else if(btn_save_as -> isChecked())
	{
		save_mode = SaveAs;
		edit_Chinese  -> setEnabled(true);
		edit_English  -> setEnabled(true);
		edit_filename -> setEnabled(true);
		preset[Chinese].clear();
		preset[English].clear();
		preset_filename.clear();
	}
}

void Interface::set_preset_Chinese()
{ preset[Chinese] = edit_Chinese -> text(); }

void Interface::set_preset_English()
{ preset[English] = edit_English -> text(); }

void Interface::set_preset_filename()
{ preset_filename = edit_filename -> text() + ".preset"; }

void Interface::write_preset()
// Format: name + " = " + value + ";"
// '\n' is optional
{
	if(save_mode == SaveAs)
		cur_preset_path = "../presets/user-presets/" + preset_filename;
	fout.open(cur_preset_path.toLocal8Bit().data(), ios::trunc);

	fout << "// SmartChordGen Preset File\n"
		  << "// Preset data begins below.\n"
		  << "title_en = " << preset[English].toLatin1().data() << ";\n"
		  << "title_ch = " << preset[Chinese].toUtf8().data() << ";\n";
	if(language == Chinese)
		fout << "language = Chinese;\n\n";
	else  fout << "language = English;\n\n";

	fout << "output name = " << output_name << ";\n";
	if(continual)  fout << "output mode = continual;\n";
	else  fout << "output mode = single;\n";
	switch(output_mode)
	{
		case Both:      fout << "output format = Both;\n\n";  break;
		case MidiOnly:	 fout << "output format = MidiOnly;\n\n";  break;
		case TextOnly:  fout << "output format = TextOnly;\n\n";  break;
	}

	if(continual)
		fout << "number of progressions = " << loop_count << ";\n";
	fout << "range of notes: min = " << lowest << "; max = " << highest << ";\n"
		  << "number of parts: min = " << m_min << "; max = " << m_max << ";\n"
		  << "number of notes: min = " << n_min << "; max = " << n_max << ";\n"
		  << "keep number of parts unchanged = " << boolalpha << m_unchanged << ";\n"
		  << "ditto = " << boolalpha << nm_same << ";\n\n";

	fout << "chord database filename = " << database_filename << ";\n"
		  << "title of chord database (English) = " << database[English] << ";\n"
		  << "title of chord database (Chinese) = " << database[Chinese] << ";\n"
		  << "size of chord database = " << database_size << ";\n\n";

	if(automatic)
		fout << "input mode = automatic;\n";
	else
	{
		fout << "input mode = manual;\n"
			  << "initial chord = " << str_notes << ";\n";
	}
	fout << "connect pedal notes = " << boolalpha << connect_pedal << ";\n"
		  << "interlace = " << boolalpha << interlace << ";\n";
	switch(unique_mode)
	{
		case RemoveDupType: fout << "unique mode = RemoveDupType;\n\n";  break;
		case RemoveDup:	  fout << "unique mode = RemoveDup;\n\n";  break;
		case Disabled:      fout << "unique mode = Disabled;\n\n";  break;
	}

	fout << "have set overall scale = " << boolalpha << have_set_overall_scale << ";\n";
	fprint("overall scale = ", overall_scale, " ", ";\n\n");

	for(int i = 3; i <= 7; ++i)
	{
		fout << "omission for " << i << "-note chords = ";
		fprint("", omission[i], " ", ";\n");
	}
	fprint("\nbass note allowed = ", bass_avail, " ", ";\n\n");

	switch(align_mode)
	{
		case Interval:  fout << "align mode = Interval;\n"
									<< "intervals: i_low = " << i_low << "; i_high = " << i_high
									<< "; i_min = " << i_min << "; i_max = " << i_max << ";\n\n";  break;

		case List:	    fout << "align mode = List;\n"
									<< "alignment database filename = " << align_db_filename << ";\n"
									<< "title of alignment database (English) = " << align_db[English] << ";\n"
									<< "title of alignment database (Chinese) = " << align_db[Chinese] << ";\n"
									<< "size of alignment database = " << align_db_size << ";\n\n";  break;

		case Unlimited: fout << "align mode = Unlimited;\n\n";  break;
	}

	fout << "enable pedal notes = " << boolalpha << enable_pedal << ";\n"
		  << "in bass = " << boolalpha << in_bass << ";\n";
	if(in_bass)  fprint("pedal notes = ", pedal_notes, " ", ";\n");
	else  fprint("pedal notes = ", pedal_notes_set, " ", ";\n");
	fout << "realign = " << boolalpha << realign << ";\n"
		  << "period = " << period << ";\n\n";

	switch(vl_setting)
	{
		case Default:    fout << "voice leading setting = Default;\n";     break;
		case Number:	  fout << "voice leading setting = Number;\n";      break;
		case Percentage: fout << "voice leading setting = Percentage;\n";  break;
	}
	fout << "enable steady voices = " << boolalpha << enable_steady << ";\n"
		  << "enable ascending voices = " << boolalpha << enable_ascending << ";\n"
		  << "enable descending voices = " << boolalpha << enable_descending << ";\n"
		  << "steady voices: min = " << steady_min << "; max = " << steady_max << ";\n"
		  << "ascending voices: min = " << ascending_min << "; max = " << ascending_max << ";\n"
		  << "descending voices: min = " << descending_min << "; max = " << descending_max << ";\n"
		  << "range of voice leading: custom = " << boolalpha << custom_vl_range << "; min = " << vl_min << "; max = " << vl_max << ";\n"
		  << "custom root movement = " << boolalpha << enable_rm << ";\n";
	if(enable_rm)  fprint("priority = ", rm_priority, " ", ";\n");
	fout << "custom exclusion = " << boolalpha << enable_ex << ";\n";
	if(enable_ex)  fout << "exclusion = " << exclusion << ";\n";
	fout << "custom similarity = " << boolalpha << enable_sim << ";\n";
	if(enable_sim)  fout << "similarity = " << str_sim << ";\n\n";
	else  fout << "\n";

	fout << "K: min = " << k_min << "; max = " << k_max << ";\n"
		  << "T: min = " << t_min << "; max = " << t_max << ";\n"
		  << "C: min = " << c_min << "; max = " << c_max << ";\n"
		  <<"sv: min = " <<sv_min << "; max = " <<sv_max << ";\n"
		  << "R: min = " << r_min << "; max = " << r_max << ";\n"
		  << "H: min = " << h_min << "; max = " << h_max << ";\n"
		  << "G: min = " << g_min << "; max = " << g_max << ";\n"
		  << "X: min = " << x_min << "; max = " << x_max << ";\n"
		  << "sort order = " << sort_order << ";\n";

	fout.close();
	save_dialog -> close();
	cur_preset[Chinese] = preset[Chinese];
	cur_preset[English] = preset[English];
	cur_preset_filename = preset_filename;
	if(language == Chinese)
		set_to_Chinese();
	else  set_to_English();
}

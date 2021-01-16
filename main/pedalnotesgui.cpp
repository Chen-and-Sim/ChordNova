// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// pedalnotesgui.cpp

#include "interface.h"

void Interface::pedalNotesGui()
{
	pedal_notes_window = new QWidget(this);
	pedal_notes_window -> installEventFilter(this);
	pedal_notes_window -> setWindowModality(Qt::WindowModal);
	pedal_notes_window -> setWindowFlag(Qt::Window, true);
	pedal_notes_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	if(language == Chinese)
		pedal_notes_window -> setFixedSize(560 * hscale, 360 * vscale);
	else  pedal_notes_window -> setFixedSize(640 * hscale, 380 * vscale);
	QStringList str1 = {"Set pedal notes", "设置持续音"};
	pedal_notes_window -> setWindowTitle(str1[language]);
	pedal_notes_window -> setFont(font);

	QStringList str2 = {"Apply pedal notes", "应用持续音"};
	cb_enable_pedal = new QCheckBox(str2[language], pedal_notes_window);
	cb_enable_pedal -> setLayoutDirection(Qt::LeftToRight);
	connect(cb_enable_pedal, &QCheckBox::clicked, this, &Interface::set_enable_pedal_notes);

	QStringList str3 = {"Pedal note(s) is/are always in lowest part(s)", "持续音总位于和弦最低的（一个或多个）声部"};
	cb_in_bass = new QCheckBox(str3[language], pedal_notes_window);
	cb_in_bass -> setLayoutDirection(Qt::LeftToRight);
	connect(cb_in_bass, &QCheckBox::stateChanged, this, &Interface::set_in_bass);

	QVBoxLayout* vbox1 = new QVBoxLayout(pedal_notes_window);
	vbox1 -> setContentsMargins(30 * hscale, 30 * vscale, 30 * hscale, 30 * vscale);
	vbox1 -> addWidget(cb_enable_pedal, 0, Qt::AlignTop);
	vbox1 -> addWidget(cb_in_bass, 0, Qt::AlignTop);

	QButtonGroup* btns = new QButtonGroup(pedal_notes_window);
	QStringList str4 = {"List of notes: ", "音高列表："};
	QStringList str5 = {"Note set: ", "音集："};
	btn_pitch_mode = new QRadioButton(str4[language], pedal_notes_window);
	btn_set_mode   = new QRadioButton(str5[language], pedal_notes_window);
	btns -> addButton(btn_pitch_mode);
	btns -> addButton(btn_set_mode);
	connect(btn_set_mode, SIGNAL(toggled(bool)), this, SLOT(select_set_pitch(bool)));

	QVBoxLayout* vbox3 = new QVBoxLayout();
	edit_pitch_mode = new QLineEdit(pedal_notes_window);
	if(language == Chinese)
		edit_pitch_mode -> setMinimumWidth(300 * hscale);
	else  edit_pitch_mode -> setMinimumWidth(330 * hscale);
	edit_pitch_mode -> setDisabled(true);
	vbox3 -> addWidget(edit_pitch_mode, 0, Qt::AlignHCenter);
	connect(edit_pitch_mode, &QLineEdit::editingFinished, this, &Interface::input_pedal_notes);

	QStringList str6 = {"(automatically judges MIDI note numbers/pitch names)\n", "（自动判别MIDI键位号/音符输入方式）\n"};
	label_pitch_mode = new QLabel(str6[language], this);
	label_pitch_mode -> setDisabled(true);
	label_pitch_mode -> setMinimumHeight(30 * vscale);
	vbox3 -> addWidget(label_pitch_mode, 0, Qt::AlignHCenter);

	QHBoxLayout* hbox2 = new QHBoxLayout();
	hbox2 -> addWidget(btn_pitch_mode, 1, Qt::AlignLeft | Qt::AlignTop);
	hbox2 -> addLayout(vbox3, 0);

	QHBoxLayout* hbox3 = new QHBoxLayout();
	hbox3 -> addWidget(btn_set_mode, 0, Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* vbox[12];
	for(int i = 0; i < 12; ++i)
	{
		QString str;
		label_set_mode[i] = new QLabel(str.setNum(i), pedal_notes_window);
		cb_set_mode[i] = new QCheckBox(pedal_notes_window);
		vbox[i] = new QVBoxLayout();
		vbox[i] -> addWidget(label_set_mode[i], 0, Qt::AlignTop | Qt::AlignHCenter);
		vbox[i] -> addWidget(cb_set_mode[i], 0, Qt::AlignTop | Qt::AlignHCenter);
		hbox3 -> addLayout(vbox[i], 0);
		connect(cb_set_mode[i], &QCheckBox::clicked, this, &Interface::select_pedal_notes);
	}

	QVBoxLayout* vbox2 = new QVBoxLayout();
	vbox2 -> addLayout(hbox2, 0);
	vbox2 -> setSpacing(10 * vscale);
	vbox2 -> addLayout(hbox3, 0);

	QStringList str7 = {"Pedal note format: ", "持续音形式："};
	label_pedal_format = new QLabel(str7[language], pedal_notes_window);
	QHBoxLayout* hbox1 = new QHBoxLayout();
	vbox1 -> setSpacing(20 * vscale);
	hbox1 -> addWidget(label_pedal_format, 0, Qt::AlignTop);
	hbox1 -> addLayout(vbox2, 1);
	vbox1 -> addLayout(hbox1, 0);

	QStringList str8 = {"Always realign pedal notes(only apply to set input mode)", "强制换列（只适用于音集形式的持续音）"};
	cb_realign = new QCheckBox(str8[language], pedal_notes_window);
	cb_realign -> setMinimumWidth(300 * hscale);
	cb_realign -> setLayoutDirection(Qt::LeftToRight);
	connect(cb_realign, &QCheckBox::clicked, this, &Interface::set_realign);

	QStringList str9 = {"Realignment period (number of chords in an interval): ", "换列周期（每隔和弦数）:"};
	label_period = new QLabel(str9[language], pedal_notes_window);
	edit_period = new QLineEdit(pedal_notes_window);
	connect(edit_period, &QLineEdit::editingFinished, this, &Interface::set_period);

	if(language == Chinese)
	{
		QHBoxLayout* hbox4 = new QHBoxLayout();
		hbox4 -> addWidget(cb_realign, 0);
		hbox4 -> addSpacing(60 * hscale);
		hbox4 -> addWidget(label_period, 0);
		hbox4 -> addWidget(edit_period, 0);
		vbox1 -> addSpacing(10 * vscale);
		vbox1 -> addLayout(hbox4, 0);
		vbox1 -> setSpacing(20 * vscale);
	}
	else
	{
		QHBoxLayout* hbox4 = new QHBoxLayout();
		hbox4 -> setSpacing(10 * hscale);
		hbox4 -> setContentsMargins(0, 0, 0, 0);
		hbox4 -> addWidget(label_period, 0, Qt::AlignRight);
		hbox4 -> addWidget(edit_period);
		vbox1 -> addSpacing(15 * vscale);
		vbox1 -> addWidget(cb_realign, 1, Qt::AlignLeft);
		vbox1 -> addLayout(hbox4, 1);
		vbox1 -> setSpacing(15 * vscale);
	}
	edit_period -> setFixedWidth(60 * hscale);

	QStringList str10 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str10[language], pedal_notes_window);
	connect(btn, &QPushButton::clicked, this, &Interface::closePedalNotes);
	vbox1 -> addWidget(btn, 0, Qt::AlignRight);

	initPedalNotes();
	pedal_notes_window -> show();
}

void Interface::initPedalNotes()
{
	cb_enable_pedal -> setChecked(enable_pedal);
	if(enable_pedal)
	{
		cb_in_bass -> setEnabled(true);
		cb_in_bass -> setChecked(in_bass);
		cb_realign -> setChecked(realign);
		set_in_bass( cb_in_bass -> checkState() );
		set_realign();
		QString str;
		edit_period -> setText(str.setNum(period));
		if(in_bass)
		{
			for(int i = 0; i < (int)pedal_notes.size(); ++i)
			{
				edit_pitch_mode -> setText( edit_pitch_mode -> text() + str.setNum(pedal_notes[i]) );
				if(i != (int)pedal_notes.size())
					edit_pitch_mode -> setText( edit_pitch_mode -> text() + ' ' );
			}
		}
		else
		{
			for(int i = 0; i < (int)pedal_notes_set.size(); ++i)
				cb_set_mode[ pedal_notes_set[i] ] -> setChecked(true);
		}
	}
	else
	{
		cb_in_bass -> setDisabled(true);
		cb_realign -> setDisabled(true);
		label_pedal_format -> setDisabled(true);
		label_pitch_mode -> setDisabled(true);
		edit_pitch_mode -> setDisabled(true);
		btn_pitch_mode -> setDisabled(true);
		btn_set_mode -> setDisabled(true);
		for(int i = 0; i < 12; ++i)
		{
			label_set_mode[i] -> setDisabled(true);
			cb_set_mode[i] -> setDisabled(true);
		}
		label_period -> setDisabled(true);
		edit_period -> setDisabled(true);
	}
}

void Interface::set_enable_pedal_notes(int state)
{
	cb_connect_pedal -> setEnabled((bool)state);
	cb_connect_pedal -> setChecked((bool)state);
	enable_pedal = (bool)state;
	connect_pedal = (bool)state;
	initPedalNotes();
}

void Interface::set_in_bass(int state)
{
	if(state == Qt::Checked)
	{
		in_bass = true;
		btn_pitch_mode -> setChecked(true);
		btn_pitch_mode -> setEnabled(true);
		btn_set_mode -> setDisabled(true);
		select_set_pitch(false);
	}
	else
	{
		in_bass = false;
		btn_set_mode -> setChecked(true);
		btn_set_mode -> setEnabled(true);
		btn_pitch_mode -> setDisabled(true);
		select_set_pitch(true);
	}
}

void Interface::select_set_pitch(bool state)
{
	if(state == true)
	// set input mode is selected
	{
		pedal_notes.clear();
		edit_pitch_mode -> clear();
		for(int i = 0; i < 12; ++i)
		{
			label_set_mode[i] -> setEnabled(true);
			cb_set_mode[i] -> setEnabled(true);
		}
		edit_pitch_mode -> setDisabled(true);
		label_pitch_mode -> setDisabled(true);
		cb_realign -> setEnabled(true);
		label_period -> setEnabled(true);
		edit_period -> setEnabled(true);
	}
	else
	{
		pedal_notes_set.clear();
		for(int i = 0; i < 12; ++i)
		{
			label_set_mode[i] -> setDisabled(true);
			cb_set_mode[i] -> setDisabled(true);
			cb_set_mode[i] -> setChecked(false);
		}
		edit_pitch_mode -> setEnabled(true);
		label_pitch_mode -> setEnabled(true);
		cb_realign -> setChecked(false);
		cb_realign -> setDisabled(true);
		label_period -> setDisabled(true);
		edit_period -> setDisabled(true);
	}
}

void Interface::input_pedal_notes()
{
	pedal_notes.clear();
	char _note[50];
	int note;
	QString text = edit_pitch_mode -> text();
	int pos1 = 0, pos2 = 0, len = text.size();
	if(len >= 45)
	{
		pedal_notes.clear();
		edit_pitch_mode -> clear();
		return;
	}
	while(pos1 < len)
	{
		pos2 = 0;
		while(pos1 < len && text[pos1] == ' ')
			++pos1;
		if(pos1 == len)  break;
		while(pos1 < len && text[pos1] != ' ')
		{
			_note[pos2] = text[pos1].toLatin1();
			++pos1;  ++pos2;
		}
		_note[pos2] = '\0';
		if(_note[0] >= '0' && _note[0] <= '9')
			note = atoi(_note);
		else  note = nametonum(_note);
		if(note < lowest || note > highest)
		{
			pedal_notes.clear();
			edit_pitch_mode -> clear();
			return;
		}
		pedal_notes.push_back(note);
	}
	bubble_sort(pedal_notes);
	remove_duplicate(pedal_notes);
	if((int)pedal_notes.size() > m_max)
	{
		pedal_notes.clear();
		edit_initial -> clear();
	}
}

void Interface::select_pedal_notes(bool state)
{
	Q_UNUSED(state);
	pedal_notes_set.clear();
	for(int i = 0; i < 12; ++i)
	{
		if(cb_set_mode[i] -> isChecked())
			pedal_notes_set.push_back(i);
	}
}

void Interface::set_realign()
{
	if(cb_realign -> isChecked())
		realign = true;
	else  realign = false;
}

void Interface::set_period()
{
	QString str = edit_period -> text();
	period = str.toInt();
	if(period <= 0 || period > 999)
	{
		period = 1;
		edit_period -> setText("1");
	}
}

void Interface::closePedalNotes()
{
	pedal_notes_window -> close();
	if(enable_pedal)
	{
		QStringList str1 = {"(Set)", "（已设置）"};
		label_pedal_notes -> setText(str1[language]);
		label_pedal_notes -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		QStringList str2 = {"(Not set)", "（未设置）"};
		label_pedal_notes -> setText(str2[language]);
		label_pedal_notes -> setStyleSheet("color: #606060;");
	}
	label_pedal_notes -> setFont(font);
}

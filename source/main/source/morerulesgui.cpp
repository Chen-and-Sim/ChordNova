// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// morerulesgui.cpp

#include "Interface.h"

void Interface::moreRulesGui()
{
	more_rules_window = new QWidget(this);
	more_rules_window -> setWindowModality(Qt::WindowModal);
	more_rules_window -> setWindowFlag(Qt::Window, true);
	more_rules_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	if(language == Chinese)
		more_rules_window -> setFixedSize(640 * scale, 510 * _scale);
	else  more_rules_window -> setFixedSize(640 * scale, 550 * _scale);
	QStringList str1 = {"Set more rules", "设置更多规则"};
	more_rules_window -> setWindowTitle(str1[language]);
	more_rules_window -> setFont(font);

	QVBoxLayout* vbox = new QVBoxLayout(more_rules_window);
	vbox -> setSpacing(10 * _scale);
	vbox -> setContentsMargins(30 * _scale, 30 * scale, 30 * _scale, 30 * scale);

	QHBoxLayout* hbox1 = new QHBoxLayout();
	QStringList str2 = {"Number of voice leading: ", "声部进行数量："};
	cb_vl_number = new QCheckBox(str2[language], more_rules_window);
	hbox1 -> addWidget(cb_vl_number);
	connect(cb_vl_number, &QCheckBox::clicked, this, &Interface::enable_vl_num);

	QStringList str3 = {"Number", "数量"};
	QStringList str4 = {"Percentage of the number of chord notes (M)", "占和弦构成音数目(M)的百分比"};
	btn_vl_number = new QRadioButton(str3[language], more_rules_window);
	btn_vl_percentage = new QRadioButton(str4[language], more_rules_window);
	hbox1 -> addWidget(btn_vl_number, 1);
	hbox1 -> addWidget(btn_vl_percentage, 1);
	connect(btn_vl_number, SIGNAL(toggled(bool)), this, SLOT(change_vl_setting(bool)));

	QButtonGroup* btns1 = new QButtonGroup(more_rules_window);
	btns1 -> addButton(btn_vl_number);
	btns1 -> addButton(btn_vl_percentage);

	if(language == Chinese)
	{
		QLabel* label0 = new QLabel("", more_rules_window);
		label0 -> setFixedWidth(200 * scale);
		hbox1 -> addWidget(label0, 0);
	}
	vbox -> addLayout(hbox1);

	QGridLayout* grid1 = new QGridLayout();
	QLabel* label1 = new QLabel("", more_rules_window);
	label1 -> setFixedWidth(10);
	grid1 -> addWidget(label1, 0, 0);
	grid1 -> addWidget(label1, 1, 0);
	grid1 -> addWidget(label1, 2, 0);

	QStringList str5 = {"Steady voices (v=0): ", "保持不动的声部 (v=0):"};
	cb_steady = new QCheckBox(str5[language], more_rules_window);
	grid1 -> addWidget(cb_steady, 0, 1);
	connect(cb_steady, &QCheckBox::clicked, this, &Interface::enable_vl_direction);

	QStringList str6 = {"Ascending voices (v>0): ", "向上运动的声部 (v>0):"};
	cb_ascending = new QCheckBox(str6[language], more_rules_window);
	grid1 -> addWidget(cb_ascending, 1, 1);
	connect(cb_ascending, &QCheckBox::clicked, this, &Interface::enable_vl_direction);

	QStringList str7 = {"Descending voices (v<0): ", "向下运动的声部 (v<0):"};
	cb_descending = new QCheckBox(str7[language], more_rules_window);
	grid1 -> addWidget(cb_descending, 2, 1);
	connect(cb_descending, &QCheckBox::clicked, this, &Interface::enable_vl_direction);

	edit_steady_min = new QLineEdit(more_rules_window);
	grid1 -> addWidget(edit_steady_min, 0, 2);
	connect(edit_steady_min, &QLineEdit::textChanged, this, &Interface::set_steady_min);

	edit_ascending_min = new QLineEdit(more_rules_window);
	grid1 -> addWidget(edit_ascending_min, 1, 2);
	connect(edit_ascending_min, &QLineEdit::textChanged, this, &Interface::set_ascending_min);

	edit_descending_min = new QLineEdit(more_rules_window);
	grid1 -> addWidget(edit_descending_min, 2, 2);
	connect(edit_descending_min, &QLineEdit::textChanged, this, &Interface::set_descending_min);

	QStringList str8 = {"to", "至"};
	label_steady = new QLabel(str8[language], more_rules_window);
	label_ascending = new QLabel(str8[language], more_rules_window);
	label_descending = new QLabel(str8[language], more_rules_window);
	grid1 -> addWidget(label_steady, 0, 3);
	grid1 -> addWidget(label_ascending, 1, 3);
	grid1 -> addWidget(label_descending, 2, 3);

	edit_steady_max = new QLineEdit(more_rules_window);
	grid1 -> addWidget(edit_steady_max, 0, 4);
	connect(edit_steady_max, &QLineEdit::textChanged, this, &Interface::set_steady_max);

	edit_ascending_max = new QLineEdit(more_rules_window);
	grid1 -> addWidget(edit_ascending_max, 1, 4);
	connect(edit_ascending_max, &QLineEdit::textChanged, this, &Interface::set_ascending_max);

	edit_descending_max = new QLineEdit(more_rules_window);
	grid1 -> addWidget(edit_descending_max, 2, 4);
	connect(edit_descending_max, &QLineEdit::textChanged, this, &Interface::set_descending_max);
	vbox -> addLayout(grid1);
	vbox -> addSpacing(5);

	QHBoxLayout* hbox2 = new QHBoxLayout();
	QStringList str9 = {"Range of voice leading: ", "声部进行幅度："};
	cb_vl_range = new QCheckBox(str9[language], more_rules_window);
	hbox2 -> addWidget(cb_vl_range);
	connect(cb_vl_range, &QCheckBox::clicked, this, &Interface::enable_vl_range);

	edit_vl_min = new QLineEdit(more_rules_window);
	hbox2 -> addWidget(edit_vl_min);
	connect(edit_vl_min, &QLineEdit::textChanged, this, &Interface::set_vl_min);

	QStringList str10 = {"≤ voice leading of any part (|v|) ≤", "≤ 任一声部的运动幅度 (|v|) ≤"};
	label_vl = new QLabel(str10[language], more_rules_window);
	hbox2 -> addWidget(label_vl);

	edit_vl_max = new QLineEdit(more_rules_window);
	hbox2 -> addWidget(edit_vl_max);
	connect(edit_vl_max, &QLineEdit::textChanged, this, &Interface::set_vl_max);
	vbox -> addLayout(hbox2);
	vbox -> addSpacing(10 * _scale);

	QGridLayout* grid2 = new QGridLayout();
	QStringList str11 = {"Root movement and its priority: ", "根音运动音程及优先顺序："};
	QLabel* label2 = new QLabel(str11[language], more_rules_window);
	grid2 -> addWidget(label2, 0, 0);

	QStringList str12 = {"Default (unlimited)", "默认（无限制）"};
	QStringList str13 = {"Custom: ", "自定义："};
	btn_default_rm = new QRadioButton(str12[language], more_rules_window);
	btn_custom_rm  = new QRadioButton(str13[language], more_rules_window);
	connect(btn_custom_rm, SIGNAL(toggled(bool)), this, SLOT(enable_custom_rm(bool)));
	grid2 -> addWidget(btn_default_rm, 0, 1, 1, 2, Qt::AlignLeft);
	grid2 -> addWidget(btn_custom_rm , 1, 1, Qt::AlignLeft);

	QButtonGroup* btns2 = new QButtonGroup(more_rules_window);
	btns2 -> addButton(btn_default_rm);
	btns2 -> addButton(btn_custom_rm);

	edit_custom_rm = new QLineEdit(more_rules_window);
	edit_custom_rm -> setFixedWidth(200 * scale);
	grid2 -> addWidget(edit_custom_rm, 1, 2, Qt::AlignLeft);
	connect(edit_custom_rm, &QLineEdit::editingFinished, this, &Interface::input_rm);

	QStringList str14 = {"Exclude chords with certain notes/intervals: ", "排除含特定音/音程的和弦："};
	QLabel* label3 = new QLabel(str14[language], more_rules_window);
	grid2 -> addWidget(label3, 2, 0);

	QStringList str15 = {"Default (no exclusion)", "默认（不排除）"};
	QStringList str16 = {"Custom: ", "自定义："};
	btn_default_ex = new QRadioButton(str15[language], more_rules_window);
	btn_custom_ex  = new QRadioButton(str16[language], more_rules_window);
	connect(btn_custom_ex, SIGNAL(toggled(bool)), this, SLOT(enable_custom_ex(bool)));
	grid2 -> addWidget(btn_default_ex, 2, 1, 1, 2, Qt::AlignLeft);
	grid2 -> addWidget(btn_custom_ex , 3, 1, Qt::AlignLeft);

	QButtonGroup* btns3 = new QButtonGroup(more_rules_window);
	btns3 -> addButton(btn_default_ex);
	btns3 -> addButton(btn_custom_ex);

	edit_custom_ex = new QLineEdit(more_rules_window);
	edit_custom_ex -> setFixedWidth(200 * scale);
	grid2 -> addWidget(edit_custom_ex, 3, 2, Qt::AlignLeft);
	connect(edit_custom_ex, &QLineEdit::editingFinished, this, &Interface::input_ex);

	QStringList str17 = {"Similarity of chords (X): \n(covers the settings in 'set more parameters')",
								"和弦相似度 (X)：\n（覆盖在“更多指标设置”\n处的设置）"};
	QLabel* label4 = new QLabel(str17[language], more_rules_window);
	grid2 -> addWidget(label4, 4, 0, 2, 1, Qt::AlignTop);

	btn_default_sim = new QRadioButton(str12[language], more_rules_window);
	btn_custom_sim  = new QRadioButton(str13[language], more_rules_window);
	connect(btn_custom_sim, SIGNAL(toggled(bool)), this, SLOT(enable_custom_sim(bool)));
	grid2 -> addWidget(btn_default_sim, 4, 1, 1, 2, Qt::AlignLeft);
	grid2 -> addWidget(btn_custom_sim , 5, 1, Qt::AlignLeft);

	QButtonGroup* btns4 = new QButtonGroup(more_rules_window);
	btns4 -> addButton(btn_default_sim);
	btns4 -> addButton(btn_custom_sim);

	edit_custom_sim = new QLineEdit(more_rules_window);
	edit_custom_sim -> setFixedWidth(200 * scale);
	grid2 -> addWidget(edit_custom_sim, 5, 2, Qt::AlignLeft);
	connect(edit_custom_sim, &QLineEdit::editingFinished, this, &Interface::input_sim);

	grid2 -> setColumnStretch(1, 0);
	grid2 -> setColumnStretch(2, 1);
	vbox -> addLayout(grid2);
	vbox -> addSpacing(15 * _scale);

	QString str18 = "<p style=\"line-height:150%\">%1<p>";
	QStringList str19 = {"Warning: All settings above is restricted by the range of notes;<br>"
								"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
								"inconsistent or highly strict conditions may result in no solutions.",
								"注意：以上设置都受音域设置的制约；不符或过严的条件可能导致无解。"};
	QLabel* label5 = new QLabel(str18.arg(str19[language]), more_rules_window);
	label5 -> setFont(QFont("Microsoft Yahei", 11, QFont::Bold));
	label5 -> setStyleSheet("color: #FF0000;");
	if(language == Chinese)
		label5 -> setFixedHeight(30 * _scale);
	else  label5 -> setFixedHeight(60 * _scale);
	vbox -> addWidget(label5, 0, Qt::AlignLeft);

	QStringList str20 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str20[language], more_rules_window);
	btn -> setFixedWidth(80 * scale);
	vbox -> addWidget(btn, 1, Qt::AlignRight);
	connect(btn, SIGNAL(clicked()), more_rules_window, SLOT(close()));

	initMoreRules();
	more_rules_window -> show();
}

void Interface::initMoreRules()
{
	switch(vl_setting)
	{
		case Default:     enable_vl_num(false);      break;
		case Number:	   cb_vl_number -> setChecked(true);  btn_vl_number -> setChecked(true);  break;
		case Percentage:  cb_vl_number -> setChecked(true);  btn_vl_percentage -> setChecked(true);  break;
	}
	if(vl_setting != Default)
	{
		cb_steady -> setChecked(enable_steady);
		cb_ascending -> setChecked(enable_ascending);
		cb_descending -> setChecked(enable_descending);
		enable_vl_num(true);
		enable_vl_direction(0);
	}
	QString str;
	edit_steady_min -> setText(str.setNum(steady_min));
	edit_steady_max -> setText(str.setNum(steady_max));
	edit_ascending_min -> setText(str.setNum(ascending_min));
	edit_ascending_max -> setText(str.setNum(ascending_max));
	edit_descending_min -> setText(str.setNum(descending_min));
	edit_descending_max -> setText(str.setNum(descending_max));
	cb_vl_range -> setChecked(custom_vl_range);
	enable_vl_range(cb_vl_range -> isChecked());
	edit_vl_min -> setText(str.setNum(vl_min));
	edit_vl_max -> setText(str.setNum(vl_max));
	if(enable_rm)
	{
		btn_custom_rm -> setChecked(true);
		for(int i = 0; i < (int)rm_priority.size(); ++i)
		{
			edit_custom_rm -> setText( edit_custom_rm -> text() + str.setNum(rm_priority[i]) );
			if(i != (int)rm_priority.size() - 1)
				edit_custom_rm -> setText(edit_custom_rm -> text() + ' ');
		}
	}
	else
	{
		btn_default_rm -> setChecked(true);
		enable_custom_rm(false);
	}
	if(enable_ex)
	{
		btn_custom_ex -> setChecked(true);
		edit_custom_ex -> setText(exclusion);
		set_ex();
	}
	else
	{
		btn_default_ex -> setChecked(true);
		enable_custom_ex(false);
	}
	if(!continual)
	{
		btn_custom_sim -> setDisabled(true);
		btn_default_sim -> setDisabled(true);
		edit_custom_sim -> setDisabled(true);
	}
	else
	{
		if(enable_sim)
		{
			btn_custom_sim -> setChecked(true);
			edit_custom_sim -> setText(str_sim);
			set_sim();
		}
		else
		{
			btn_default_sim -> setChecked(true);
			enable_custom_sim(false);
		}
	}
}

void Interface::enable_vl_num(bool state)
{
	if(state == true)
	{
		if(btn_vl_number -> isChecked())
			vl_setting = Number;
		else if(btn_vl_percentage -> isChecked())
			vl_setting = Percentage;
		else
		{
			btn_vl_number -> setChecked(true);
			vl_setting = Number;
		}
		btn_vl_number -> setEnabled(true);
		btn_vl_percentage -> setEnabled(true);
		cb_steady -> setEnabled(true);
		cb_ascending -> setEnabled(true);
		cb_descending -> setEnabled(true);
		if(cb_steady -> isChecked())
		{
			edit_steady_min -> setEnabled(true);
			edit_steady_max -> setEnabled(true);
			label_steady -> setEnabled(true);
		}
		if(cb_ascending -> isChecked())
		{
			edit_ascending_min -> setEnabled(true);
			edit_ascending_max -> setEnabled(true);
			label_ascending -> setEnabled(true);
		}
		if(cb_descending -> isChecked())
		{
			edit_descending_min -> setEnabled(true);
			edit_descending_max -> setEnabled(true);
			label_descending -> setEnabled(true);
		}
	}
	else
	{
		vl_setting = Default;
		btn_vl_number -> setDisabled(true);
		btn_vl_percentage -> setDisabled(true);
		cb_steady -> setDisabled(true);
		cb_ascending -> setDisabled(true);
		cb_descending -> setDisabled(true);
		edit_steady_min -> setDisabled(true);
		edit_steady_max -> setDisabled(true);
		edit_ascending_min -> setDisabled(true);
		edit_ascending_max -> setDisabled(true);
		edit_descending_min -> setDisabled(true);
		edit_descending_max -> setDisabled(true);
		label_steady -> setDisabled(true);
		label_ascending -> setDisabled(true);
		label_descending -> setDisabled(true);
	}
}

void Interface::change_vl_setting(bool state)
{
	if(state == true)
	// 'btn_vl_number' is selected
		vl_setting = Number;
	else  vl_setting = Percentage;
}

void Interface::enable_vl_direction(bool state)
{
	Q_UNUSED(state);
	enable_steady = cb_steady -> isChecked();
	enable_ascending = cb_ascending -> isChecked();
	enable_descending = cb_descending -> isChecked();
	if(cb_steady -> isChecked())
	{
		edit_steady_min -> setEnabled(true);
		edit_steady_max -> setEnabled(true);
		label_steady -> setEnabled(true);
	}
	else
	{
		edit_steady_min -> setDisabled(true);
		edit_steady_max -> setDisabled(true);
		label_steady -> setDisabled(true);
		edit_steady_min -> setText("0");
		if(btn_vl_number -> isChecked())
			edit_steady_max -> setText("15");
		else  edit_steady_max -> setText("100");
	}
	if(cb_ascending -> isChecked())
	{
		edit_ascending_min -> setEnabled(true);
		edit_ascending_max -> setEnabled(true);
		label_ascending -> setEnabled(true);
	}
	else
	{
		edit_ascending_min -> setDisabled(true);
		edit_ascending_max -> setDisabled(true);
		label_ascending -> setDisabled(true);
		edit_ascending_min -> setText("0");
		if(btn_vl_number -> isChecked())
			edit_ascending_max -> setText("15");
		else  edit_ascending_max -> setText("100");
	}
	if(cb_descending -> isChecked())
	{
		edit_descending_min -> setEnabled(true);
		edit_descending_max -> setEnabled(true);
		label_descending -> setEnabled(true);
	}
	else
	{
		edit_descending_min -> setDisabled(true);
		edit_descending_max -> setDisabled(true);
		label_descending -> setDisabled(true);
		edit_descending_min -> setText("0");
		if(btn_vl_number -> isChecked())
			edit_descending_max -> setText("15");
		else  edit_descending_max -> setText("100");
	}
	if(!cb_steady -> isChecked() && !cb_ascending -> isChecked() && !cb_descending -> isChecked())
	{
		cb_vl_number -> setChecked(false);
		cb_steady -> setDisabled(true);
		cb_ascending -> setDisabled(true);
		cb_descending -> setDisabled(true);
	}
}

void Interface::set_steady_min()
{
	QString text = edit_steady_min -> text();
	steady_min = text.toInt();
	if(steady_min > steady_max)
	{
		steady_min = steady_max;
		edit_steady_min -> setText(edit_steady_max -> text());
	}
	else if(steady_min <= 0)
	{
		steady_min = 0;
		edit_steady_min -> setText("0");
	}
	if(steady_min > 0)
	{
		custom_vl_range = true;
		cb_vl_range -> setDisabled(true);
		cb_vl_range -> setChecked(true);
		enable_vl_range(true);
		if(vl_min == 0)
		{
			vl_min = 1;
			edit_vl_min -> setText("1");
		}
	}
	else  cb_vl_range -> setEnabled(true);
}

void Interface::set_steady_max()
{
	QString text = edit_steady_max -> text();
	steady_max = text.toInt();
	if(steady_max < steady_min)
	{
		steady_max = steady_min;
		edit_i_max -> setText(edit_steady_min -> text());
	}
	else
	{
		if(vl_setting == Number && steady_max > 15)
		{
			steady_max = 15;
			edit_steady_max -> setText("15");
		}
		else if(vl_setting == Percentage && steady_max > 100)
		{
			steady_max = 100;
			edit_steady_max -> setText("100");
		}
	}
}

void Interface::set_ascending_min()
{
	QString text = edit_ascending_min -> text();
	ascending_min = text.toInt();
	if(ascending_min > ascending_max)
	{
		ascending_min = ascending_max;
		edit_ascending_min -> setText(edit_ascending_max -> text());
	}
	else if(ascending_min <= 0)
	{
		ascending_min = 0;
		edit_ascending_min -> setText("0");
	}
}

void Interface::set_ascending_max()
{
	QString text = edit_ascending_max -> text();
	ascending_max = text.toInt();
	if(ascending_max < ascending_min)
	{
		ascending_max = ascending_min;
		edit_i_max -> setText(edit_ascending_min -> text());
	}
	else
	{
		if(vl_setting == Number && ascending_max > 15)
		{
			ascending_max = 15;
			edit_ascending_max -> setText("15");
		}
		else if(vl_setting == Percentage && ascending_max > 100)
		{
			ascending_max = 100;
			edit_ascending_max -> setText("100");
		}
	}
}

void Interface::set_descending_min()
{
	QString text = edit_descending_min -> text();
	descending_min = text.toInt();
	if(descending_min > descending_max)
	{
		descending_min = descending_max;
		edit_descending_min -> setText(edit_descending_max -> text());
	}
	else if(descending_min <= 0)
	{
		descending_min = 0;
		edit_descending_min -> setText("0");
	}
}

void Interface::set_descending_max()
{
	QString text = edit_descending_max -> text();
	descending_max = text.toInt();
	if(descending_max < descending_min)
	{
		descending_max = descending_min;
		edit_i_max -> setText(edit_descending_min -> text());
	}
	else
	{
		if(vl_setting == Number && descending_max > 15)
		{
			descending_max = 15;
			edit_descending_max -> setText("15");
		}
		else if(vl_setting == Percentage && descending_max > 100)
		{
			descending_max = 100;
			edit_descending_max -> setText("100");
		}
	}
}

void Interface::enable_vl_range(bool state)
{
	if(state == true)
	{
		custom_vl_range = true;
		edit_vl_min -> setEnabled(true);
		edit_vl_max -> setEnabled(true);
		label_vl -> setEnabled(true);
	}
	else
	{
		custom_vl_range = false;
		edit_vl_min -> setDisabled(true);
		edit_vl_max -> setDisabled(true);
		label_vl -> setDisabled(true);
		edit_vl_min -> setText("0");
		edit_vl_max -> setText("4");
		vl_min = 0;
		vl_max = 4;
	}
}

void Interface::set_vl_min()
{
	QString text = edit_vl_min -> text();
	vl_min = text.toInt();
	if(vl_min > vl_max)
	{
		vl_min = vl_max;
		edit_vl_min -> setText(edit_vl_max -> text());
	}
	else if(vl_min <= 0)
	{
		vl_min = 0;
		edit_vl_min -> setText("0");
	}
	if(steady_min > 0 && vl_min == 0)
	{
		vl_min = 1;
		edit_vl_min -> setText("1");
	}
}

void Interface::set_vl_max()
{
	QString text = edit_vl_max -> text(), str;
	vl_max = text.toInt();
	if(vl_max < vl_min)
	{
		vl_max = vl_min;
		edit_i_max -> setText(edit_vl_min -> text());
	}
	else if(vl_max > 12)
	{
		vl_max = 12;
		edit_vl_max -> setText("12");
	}
}

void Interface::enable_custom_rm(bool state)
{
	enable_rm = state;
	if(state == true)
	// custom root movement is selected
	{
		edit_custom_rm -> setEnabled(true);
	}
	else
	{
		edit_custom_rm -> setDisabled(true);
		edit_custom_rm -> clear();
	}
}

void Interface::input_rm()
{
	rm_priority.clear();
	char str_rm[25];
	int rm;
	QString text = edit_custom_rm -> text(), str;
	int pos1 = 0, pos2 = 0, len = text.size();
	if(len >= 20)
	{
		edit_custom_rm -> clear();
		rm_priority.clear();
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
			str_rm[pos2] = text[pos1].toLatin1();
			++pos1;  ++pos2;
		}
		str_rm[pos2] = '\0';
		str = str_rm;
		rm = str.toInt();
		if(rm < 0 || rm > 6)
		{
			edit_custom_rm -> clear();
			rm_priority.clear();
			return;
		}
		bool b = true;
		for(int i = 0; i < (int)rm_priority.size(); ++i)
			if(rm_priority[i] == rm)
			{
				b = false;
				break;
			}
		if(b)  rm_priority.push_back(rm);
	}
	edit_custom_rm -> clear();
	for(int i = 0; i < (int)rm_priority.size(); ++i)
	{
		edit_custom_rm -> setText( edit_custom_rm -> text() + str.setNum(rm_priority[i]) );
		if(i != (int)rm_priority.size() - 1)
			edit_custom_rm -> setText(edit_custom_rm -> text() + ' ');
	}
}


void Interface::enable_custom_ex(bool state)
{
	enable_ex = state;
	if(state == true)
	// custom exclusion is selected
	{
		edit_custom_ex -> setEnabled(true);
	}
	else
	{
		edit_custom_ex -> setDisabled(true);
		edit_custom_ex -> clear();
	}
}

void Interface::input_ex()
{
	strcpy(exclusion, edit_custom_ex -> text().toLatin1().data());
	set_ex();
}

void Interface::set_ex()
{
	exclusion_notes.clear();
	exclusion_roots.clear();
	exclusion_intervals.clear();
	char str[5], ch;
	QString text = exclusion;
	int num, pos1 = 0, pos2 = 0, len = text.size();
	if(len >= 100)
	{
		edit_custom_ex -> clear();
		return;
	}
	while(pos1 < len)
	{
		ch = text[pos1].toLatin1();
		if(ch == ' ')
		{
			++pos1;
			continue;
		}
		else if(ch > '0' && ch < '9')
		{
			pos2 = 0;
			while(pos1 < len && text[pos1] != ' ')
			{
				str[pos2] = text[pos1].toLatin1();
				++pos1;  ++pos2;
			}
			str[pos2] = '\0';
			num = ((QString)str).toInt();
			exclusion_notes.push_back(num);
		}
		else if(ch == 'r')
		{
			++pos1;  pos2 = 0;
			while(pos1 < len && text[pos1] != ' ')
			{
				str[pos2] = text[pos1].toLatin1();
				++pos1;  ++pos2;
			}
			str[pos2] = '\0';
			num = ((QString)str).toInt();
			exclusion_roots.push_back(num);
		}
		else if(ch == '\\')
		{
			ch = text[++pos1].toLatin1();
			if(ch == '\\')
			{
				++pos1;  pos2 = 0;
				while(pos1 < len && text[pos1] != ' ' && text[pos1] != '(' && text[pos1] != '[')
				{
					str[pos2] = text[pos1].toLatin1();
					++pos1;  ++pos2;
				}
				str[pos2] = '\0';
				num = ((QString)str).toInt();
				intervalData data = {0, 0, 10, 1, 200};
				data.interval = num;
				int temp1, temp2;
				for(int k = 0; k < 2; ++k)
				{
					if(text[pos1] == '(')
					{
						++pos1;  pos2 = 0;
						while(pos1 < len && text[pos1] != '-' && text[pos1] != ')')
						{
							str[pos2] = text[pos1].toLatin1();
							++pos1;  ++pos2;
						}
						str[pos2] = '\0';
						temp1 = ((QString)str).toInt();
						if(text[pos1] == '-')
						{
							++pos1;  pos2 = 0;
							while(pos1 < len && text[pos1] != ')')
							{
								str[pos2] = text[pos1].toLatin1();
								++pos1;  ++pos2;
							}
							str[pos2] = '\0';
							temp2 = ((QString)str).toInt();
							data.num_min = temp1;
							data.num_max = temp2;
						}
						else  data.num_min = temp1;
						++pos1;
					}
					else if(text[pos1] == '[')
					{
						++pos1;  pos2 = 0;
						while(pos1 < len && text[pos1] != '-' && text[pos1] != ']')
						{
							str[pos2] = text[pos1].toLatin1();
							++pos1;  ++pos2;
						}
						str[pos2] = '\0';
						temp1 = ((QString)str).toInt();
						if(text[pos1] == '-')
						{
							++pos1;  pos2 = 0;
							while(pos1 < len && text[pos1] != ']')
							{
								str[pos2] = text[pos1].toLatin1();
								++pos1;  ++pos2;
							}
							str[pos2] = '\0';
							temp2 = ((QString)str).toInt();
							data.octave_min = temp1;
							data.octave_max = temp2;
						}
						else  data.octave_max = temp1;
						++pos1;
					}
				}
				exclusion_intervals.push_back(data);
			}
			else
			{
				pos2 = 0;
				while(pos1 < len && text[pos1] != ' ' && text[pos1] != '(')
				{
					str[pos2] = text[pos1].toLatin1();
					++pos1;  ++pos2;
				}
				str[pos2] = '\0';
				num = ((QString)str).toInt();
				intervalData data = {0, 0, 0, 1, 200};
				data.interval = num;
				int temp1, temp2;
				if(text[pos1] == '(')
				{
					++pos1;  pos2 = 0;
					while(pos1 < len && text[pos1] != '-' && text[pos1] != ')')
					{
						str[pos2] = text[pos1].toLatin1();
						++pos1;  ++pos2;
					}
					str[pos2] = '\0';
					temp1 = ((QString)str).toInt();
					if(text[pos1] == '-')
					{
						++pos1;  pos2 = 0;
						while(pos1 < len && text[pos1] != ')')
						{
							str[pos2] = text[pos1].toLatin1();
							++pos1;  ++pos2;
						}
						str[pos2] = '\0';
						temp2 = ((QString)str).toInt();
						data.num_min = temp1;
						data.num_max = temp2;
					}
					else  data.num_min = temp1;
				}
				++pos1;
				exclusion_intervals.push_back(data);
			}
		}
		else
		{
			pos2 = 0;
			str[0] = ch;
			while(pos1 < len && text[pos1] != ' ')
			{
				str[pos2] = text[pos1].toLatin1();
				++pos1;  ++pos2;
			}
			str[pos2] = '\0';
			num = nametonum(str);
			if(num > 0)
				exclusion_notes.push_back(num);
		}
	}
	bubble_sort(exclusion_notes);
	bubble_sort(exclusion_roots);
}

void Interface::enable_custom_sim(bool state)
{
	enable_sim = state;
	if(state == true)
	// custom total voice leading is selected
	{
		edit_custom_sim -> setEnabled(true);
	}
	else
	{
		edit_custom_sim -> setDisabled(true);
		edit_custom_sim -> clear();
	}
}

void Interface::input_sim()
{
	strcpy(str_sim, edit_custom_sim -> text().toLatin1().data());
	set_sim();
}

void Interface::set_sim()
{
	sim_min.clear();
	sim_max.clear();
	sim_period.clear();
	char str[5], ch;
	QString text = str_sim;
	int num, pos1 = 0, pos2 = 0, len = text.size();
	if(len >= 100)
	{
		edit_custom_sim -> clear();
		return;
	}
	while(pos1 < len)
	{
		ch = text[pos1].toLatin1();
		if(ch == ' ')
		{
			++pos1;
			continue;
		}
		else if(ch > '0' && ch < '9')
		{
			pos2 = 0;
			while(pos1 < len && text[pos1] != '-')
			{
				str[pos2] = text[pos1].toLatin1();
				++pos1;  ++pos2;
			}
			str[pos2] = '\0';
			num = ((QString)str).toInt();
			sim_period.push_back(num);

			++pos1;  pos2 = 0;
			while(pos1 < len && text[pos1] != '-')
			{
				str[pos2] = text[pos1].toLatin1();
				++pos1;  ++pos2;
			}
			str[pos2] = '\0';
			num = ((QString)str).toInt();
			sim_min.push_back(num);

			++pos1;  pos2 = 0;
			while(pos1 < len && text[pos1] != ' ')
			{
				str[pos2] = text[pos1].toLatin1();
				++pos1;  ++pos2;
			}
			str[pos2] = '\0';
			num = ((QString)str).toInt();
			sim_max.push_back(num);
		}
		else
		{
			sim_min.clear();
			sim_max.clear();
			sim_period.clear();
			return;
		}
	}
}

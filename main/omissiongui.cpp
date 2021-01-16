// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// omissiongui.cpp

#include "interface.h"

void Interface::omissionGui()
{
	omission_window = new QWidget(this);
	omission_window -> installEventFilter(this);
	omission_window -> setWindowModality(Qt::WindowModal);
	omission_window -> setWindowFlag(Qt::Window, true);
	omission_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	omission_window -> setFixedSize(640 * hscale, 370 * vscale);
	QStringList str0 = {"Set omission", "设置省略音"};
	omission_window -> setWindowTitle(str0[language]);
	omission_window -> setFont(font);

	QGridLayout* grid = new QGridLayout(omission_window);
	grid -> setContentsMargins(20 * hscale, 30 * vscale, 20 * hscale, 25 * vscale);
	grid -> setRowMinimumHeight(7, 40 * vscale);
	grid -> setVerticalSpacing(15 * vscale);

	QStringList str1[2] = { {"Root", "3rd",  "5th",  "7th", " 9th/2nd ",  " 11th/4th ", " 13th/6th "},
									{"根音", "三音", "五音", "七音", "9（附2）音", "11（附4）音", "13（附6）音"} };
	QLabel* label1[7];
	for(int i = 0; i < 7; ++i)
	{
		label1[i] = new QLabel(str1[language][i], omission_window);
		grid -> addWidget(label1[i], 0, i + 1, Qt::AlignCenter);
	}

	QStringList str2[2] = { {"Reserve for 3-note chords", "Reserve for 4-note chords", "Reserve for 5-note chords",
									 "Reserve for 6-note chords", "Reserve for 7-note chords"},
									{"对 3-音 和弦保留：", "对 4-音 和弦保留：", "对 5-音 和弦保留：", "对 6-音 和弦保留：", "对 7-音 和弦保留："} };
	QLabel* label2[7];
	for(int i = 0; i < 5; ++i)
	{
		label2[i] = new QLabel(str2[language][i], omission_window);
		grid -> addWidget(label2[i], i + 1, 0, Qt::AlignRight);
	}

	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 7; ++j)
		{
			cb_reserve[i][j] = new QCheckBox(omission_window);
			grid -> addWidget(cb_reserve[i][j], i + 1, j + 1, Qt::AlignCenter);
			connect(cb_reserve[i][j], &QCheckBox::clicked, this, &Interface::set_omission);
		}
	}

	QString str3 = "<p style=\"line-height:150%\">%1<p>";
	QStringList str4 = {"*&nbsp;&nbsp;The notes selected are reserved, and the others are allowed to be omitted.<br>"
							  "**&nbsp;Names such as '3rd' and '5th' is defined by the interval between the root and the note.<br>"
							  "&nbsp;&nbsp;&nbsp;root = 0, 3rd = 3 or 4, 5th = 7, 7th = 10 or 11, 9th/2nd = 1 or 2, 11th/4th = 5 or 6, 13th/6th = 8 or 9.",
							  "*&nbsp;&nbsp;以上所勾选为保留的音，没有勾选的音即为可以省略的音。<br>"
							  "** \"三音、五音\"等称呼以该音与和弦的Hindemith根音相差音程的音集序号定义。<br>"
							  "&nbsp;&nbsp;&nbsp;根音 = 0，三音 = 3或4，五音 = 7，七音 = 10或11，九（附二）音 = 1或2，<br>"
							  "&nbsp;&nbsp;&nbsp;十一（附四）音 = 5或6，十三（附六）音 = 8或9。"};
	QLabel* label3 = new QLabel(str3.arg(str4[language]), omission_window);
	label3 -> setFixedHeight(120 * vscale);
	grid -> addWidget(label3, 6, 0, 1, 8, Qt::AlignLeft);

	QStringList str5 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str5[language], omission_window);
	grid -> addWidget(btn, 7, 0, 1, 8, Qt::AlignRight | Qt::AlignBottom);
	connect(btn, &QPushButton::clicked, this, &Interface::closeOmission);

	initOmission();
	omission_window -> show();
}

void Interface::initOmission()
{
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 7; ++j)
			cb_reserve[i][j] -> setChecked(true);
		for(int j = 0; j < (int)omission[i + 3].size(); ++j)
			cb_reserve[i][ (omission[i + 3][j] - 1) / 2 ] -> setChecked(false);
	}
}

void Interface::set_omission(bool state)
{
	Q_UNUSED(state);
	for(int i = 3; i <= 7; ++i)
	{
		omission[i].clear();
		for(int j = 0; j < 7; ++j)
		{
			if(!cb_reserve[i - 3][j] -> isChecked())
				omission[i].push_back(2 * j + 1);
		}
	}
}

void Interface::closeOmission()
{
	have_set_omission = true;
	omission_window -> close();
	QStringList str = {"(Set)", "（已设置）"};
	label_omission -> setText(str[language]);
	label_omission -> setStyleSheet("color: #0084FF;");
	label_omission -> setFont(font);
}

// SmartChordGen v3.0 [Build: 2020.11.27]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// inversiongui.cpp

#include "Interface.h"

void Interface::inversionGui()
{
	inversion_window = new QWidget(this);
	inversion_window -> setWindowModality(Qt::WindowModal);
	inversion_window -> setWindowFlag(Qt::Window, true);
	inversion_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	inversion_window -> setWindowFlag(Qt::WindowCloseButtonHint, false);
	if(language == Chinese)
		inversion_window -> setFixedSize(640 * scale, 250 * _scale);
	else  inversion_window -> setFixedSize(640 * scale, 225 * _scale);
	QStringList str0 = {"Set inversion", "设置转位"};
	inversion_window -> setWindowTitle(str0[language]);
	inversion_window -> setFont(font);

	QGridLayout* grid = new QGridLayout(inversion_window);
	grid -> setContentsMargins(20 * _scale, 30 * scale, 20 * _scale, 30 * scale);

	QStringList str1 = {"Bass note allowed: ", "允许作为低音："};
	QLabel* label1 = new QLabel(str1[language], inversion_window);
	grid -> addWidget(label1, 1, 0);

	QStringList str2[2] = { {"Root", "3rd",  "5th",  "7th", "  9th/2nd  ", "  11th/4th  ",  "  13th/6th  " },
									{"根音", "三音", "五音", "七音", "九（附二）音", "十一（附四）音", "十三（附六）音"} };
	QLabel** label = new QLabel*[7];
	for(int i = 0; i < 7; ++i)
	{
		label[i] = new QLabel(str2[language][i], inversion_window);
		cb_note[i] = new QCheckBox(inversion_window);
		grid -> addWidget(label[i], 0, i + 1, Qt::AlignHCenter);
		grid -> addWidget(cb_note[i], 1, i + 1, Qt::AlignHCenter);
	}
	for(int i = 0; i < 7; ++i)
		connect(cb_note[i], &QCheckBox::clicked, this, &Interface::set_inversion);

	QString str3 = "<p style=\"line-height:150%\">%1<p>";
	QStringList str4 = {"<br>* Names such as '3rd' and '5th' is defined by the interval between the root and the note.<br>"
							  "&nbsp;&nbsp;root = 0, 3rd = 3 or 4, 5th = 7, 7th = 10 or 11, 9th/2nd = 1 or 2, 11th/4th = 5 or 6, 13th/6th = 8 or 9.",
							  "<br>* \"三音、五音\"等称呼以该音与和弦的Hindemith根音相差音程的音集序号定义。<br>"
							  "&nbsp;&nbsp;根音 = 0，三音 = 3或4，五音 = 7，七音 = 10或11，九（附二）音 = 1或2，<br>"
							  "&nbsp;&nbsp;十一（附四）音 = 5或6，十三（附六）音 = 8或9。"};
	QLabel* label2 = new QLabel(str3.arg(str4[language]), inversion_window);
	grid -> addWidget(label2, 2, 0, 1, 8, Qt::AlignHCenter);

	QStringList str5 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str5[language], inversion_window);
	grid -> addWidget(btn, 3, 7);
	connect(btn, &QPushButton::clicked, this, &Interface::closeInversion);

	grid -> setColumnMinimumWidth(0, 80 * scale);
	grid -> setColumnMinimumWidth(1, 50 * scale);
	grid -> setColumnMinimumWidth(2, 50 * scale);
	grid -> setColumnMinimumWidth(3, 50 * scale);
	grid -> setColumnMinimumWidth(4, 60 * scale);
	grid -> setRowStretch(2, 1);

	initInversion();
	inversion_window -> show();
}

void Interface::initInversion()
{
	for(size_t i = 0; i < bass_avail.size(); ++i)
		cb_note[ (bass_avail[i] - 1) / 2 ] -> setCheckState(Qt::Checked);
}

void Interface::set_inversion(bool state)
{
	Q_UNUSED(state);
	bass_avail.clear();
	for(int i = 0; i < 7; ++i)
		if(cb_note[i] -> isChecked())
			bass_avail.push_back(2 * i + 1);
}

void Interface::closeInversion()
{
	have_set_inversion = true;
	inversion_window -> close();
	QStringList str = {"(Set)", "（已设置）"};
	label_inversion -> setText(str[language]);
	label_inversion -> setStyleSheet("color: #0084FF;");
	label_inversion -> setFont(font);
}

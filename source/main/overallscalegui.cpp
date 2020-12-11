// SmartChordGen v3.0 [Build: 2020.11.27]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// overallscalegui.cpp

#include "Interface.h"

void Interface::overallScaleGui()
{
	overall_scale_window = new QWidget(this);
	overall_scale_window -> setWindowModality(Qt::WindowModal);
	overall_scale_window -> setWindowFlag(Qt::Window, true);
	overall_scale_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	overall_scale_window -> setWindowFlag(Qt::WindowCloseButtonHint, false);
	overall_scale_window -> setFixedSize(560 * scale, 170 * _scale);
	QStringList str1 = {"Set overall scale", "设置整体音阶"};
	overall_scale_window -> setWindowTitle(str1[language]);
	overall_scale_window -> setFont(font);

	QGridLayout* grid1 = new QGridLayout(overall_scale_window);
	grid1 -> setContentsMargins(30 * _scale, 30 * scale, 30 * _scale, 30 * scale);
	grid1 -> setVerticalSpacing(10 * scale);

	QStringList str2 = {"Overall scale: ", "整体音阶："};
	QLabel* label1 = new QLabel(str2[language], overall_scale_window);
	grid1 -> addWidget(label1, 0, 0);

	QStringList str3 = {"Default (no restrictions)", "默认（无限制）"};
	QStringList str4 = {"Custom: ", "自定义："};
	btn_default_overall_scale = new QRadioButton(str3[language], overall_scale_window);
	btn_custom_overall_scale  = new QRadioButton(str4[language], overall_scale_window);
	connect(btn_default_overall_scale, SIGNAL(toggled(bool)), this, SLOT(set_default_overall_scale(bool)));
	grid1 -> addWidget(btn_default_overall_scale, 0, 1, 1, 2, Qt::AlignLeft);
	grid1 -> addWidget(btn_custom_overall_scale , 1, 1, Qt::AlignLeft);

	QButtonGroup* btns = new QButtonGroup(overall_scale_window);
	btns -> addButton(btn_default_overall_scale);
	btns -> addButton(btn_custom_overall_scale);

	QGridLayout* grid2 = new QGridLayout();
	grid2 -> setVerticalSpacing(5 * scale);
	QLabel* label_0_11[12];
	for(int i = 0; i < 12; ++i)
	{
		QString str;
		label_0_11[i] = new QLabel(str.setNum(i), this);
		cb_0_11[i] = new QCheckBox(this);
		grid2 -> addWidget(label_0_11[i], 0, i, Qt::AlignCenter);
		grid2 -> addWidget(cb_0_11[i], 1, i, Qt::AlignCenter);
		connect(cb_0_11[i], &QCheckBox::clicked, this, &Interface::set_overall_scale);
		grid2 -> setColumnMinimumWidth(i, 30);
	}
	grid1 -> addLayout(grid2, 1, 2, Qt::AlignLeft);

	QStringList str5 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str5[language], overall_scale_window);
	grid1 -> addWidget(btn, 7, 0, 1, 8, Qt::AlignRight | Qt::AlignBottom);
	connect(btn, &QPushButton::clicked, this, &Interface::closeOverallScale);

	initOverallScale();
	overall_scale_window -> show();
}

void Interface::initOverallScale()
{
	if(have_set_overall_scale)
		btn_custom_overall_scale  -> setChecked(true);
	else
		btn_default_overall_scale -> setChecked(true);
	for(int i = 0; i < (int)overall_scale.size(); ++i)
		cb_0_11[overall_scale[i]] -> setChecked(true);
}

void Interface::set_default_overall_scale(bool state)
{
	if(state)  // use default overall scale
	{
		have_set_overall_scale = false;
		overall_scale.clear();
		for(int i = 0; i < 12; ++i)
		{
			cb_0_11[i] -> setDisabled(true);
			cb_0_11[i] -> setChecked(true);
			overall_scale.push_back(i);
		}
	}
	else
	{
		have_set_overall_scale = true;
		for(int i = 0; i < 12; ++i)
			cb_0_11[i] -> setEnabled(true);
	}
}

void Interface::set_overall_scale(int state)
{
	Q_UNUSED(state);
	overall_scale.clear();
	for(int i = 0; i < 12; ++i)
	{
		if(cb_0_11[i] -> isChecked())
			overall_scale.push_back(i);
	}
}

void Interface::closeOverallScale()
{
	overall_scale_window -> close();
	QStringList str1 = {"(Set)", "（已设置）"};
	QStringList str2 = {"(Default)", "（默认）"};
	if(have_set_overall_scale)
	{
		label_overall_scale -> setText(str1[language]);
		label_overall_scale -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_overall_scale -> setText(str2[language]);
		label_overall_scale -> setStyleSheet("color: #606060;");
	}
	label_overall_scale -> setFont(font);
}

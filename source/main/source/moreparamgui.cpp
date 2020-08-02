// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// moreparamgui.cpp

#include "Interface.h"

void (Interface::*set_min[_TOTAL]) () =
	{&Interface::set_k_min, &Interface::set_t_min, &Interface::set_c_min, &Interface::set_sv_min, nullptr, nullptr,
	 &Interface::set_r_min, &Interface::set_h_min, &Interface::set_g_min, &Interface::set_x_min,  nullptr};
void (Interface::*set_max[_TOTAL]) () =
	{&Interface::set_k_max, &Interface::set_t_max, &Interface::set_c_max, &Interface::set_sv_max, nullptr, nullptr,
	 &Interface::set_r_max, &Interface::set_h_max, &Interface::set_g_max, &Interface::set_x_max,  nullptr};

void Interface::moreParamGui()
{
	more_param_window = new QWidget(this);
	more_param_window -> setWindowModality(Qt::WindowModal);
	more_param_window -> setWindowFlag(Qt::Window, true);
	more_param_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	more_param_window -> setWindowFlag(Qt::WindowCloseButtonHint, false);
	more_param_window -> setFixedSize(640 * scale, 540 * _scale);
	QStringList str0 = {"Set more parameters and sort priorties", "更多指标设置及排序设置"};
	more_param_window -> setWindowTitle(str0[language]);
	more_param_window -> setFont(font);

	QGridLayout* grid = new QGridLayout(more_param_window);
	grid -> setContentsMargins(20 * _scale, 30 * scale, 20 * _scale, 20 * scale);
	grid -> setColumnStretch(2, 0);
	if(language == Chinese)
		grid -> setHorizontalSpacing(20 * scale);

	QStringList str1 = {"Range of parameters", "指 标 范 围"};
	QLabel* label1 = new QLabel(str1[language], more_param_window);
	label1 -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	label1 -> setStyleSheet("color: #3F48CC;");
	grid -> addWidget(label1, 0, 0, 1, 4, Qt::AlignCenter);

	QStringList str2 = {"Sorting and its order", "排序及次序"};
	QLabel* label2 = new QLabel(str2[language], more_param_window);
	label2 -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	label2 -> setStyleSheet("color: #3F48CC;");
	grid -> addWidget(label2, 0, 4, 1, 2, Qt::AlignCenter);

	QLabel* label_param[_TOTAL];
	QStringList str_param[_TOTAL] = { {"≤ Percentage of Cuikang Hua's chroma difference (K%) ≤",
												  "≤  华萃康氏色差值占比 (K%)  ≤"},
												 {"≤ Percentage of tension measured by the intervals (T%) ≤",
												  "≤ 从音程涵量得的紧张度 (T%)≤"},
												 {"≤      Number of common notes in a progression (C)     ≤",
												  "≤  和弦进行的共同音个数 (C) ≤"},
												 {"≤                      Total voice leading (Σvec)                     ≤",
												  "≤    音集进行总大小 (Σvec)   ≤"},
												 {"Number of notes in a chord (M)", "和弦构成音个数 (M)"},
												 {"Number of notes in a chord in terms of set (N)", "和弦音集构成音个数 (N)"},
												 {"≤                  Hindemith root of a chord (R)                   ≤",
												  "≤  和弦的Hindemith根音 (R) ≤"},
												 {"≤               Weighted thickness of a chord (H)               ≤",
												  "≤ 和弦的重复音加权厚度 (H) ≤"},
												 {"≤                Geometric center of a chord (G%)               ≤",
												  "≤     和弦的几何中心 (G%)    ≤"},
												 {"≤               Similarity of adjacent chords (X%)                ≤",
												  "≤     相邻和弦相似度 (X%)     ≤"},
												 {"Root movement in a progression (V)", "和弦进行的根音运动音程 (V)"} };
	QStringList str3 = {"Ascending", "升序"};
	for(int i = 0; i < _TOTAL; ++i)
	{
		if((i != 4) && (i != 5) && (i != 10))
		{
			edit_min[i] = new QLineEdit(more_param_window);
			grid -> addWidget(edit_min[i], i + 1, 1);
			edit_max[i] = new QLineEdit(more_param_window);
			grid -> addWidget(edit_max[i], i + 1, 3);
		}

		cb_param[i] = new QCheckBox(more_param_window);
		grid -> addWidget(cb_param[i], i + 1, 0);

		label_param[i] = new QLabel(str_param[i][language], more_param_window);
		grid -> addWidget(label_param[i], i + 1, 2, Qt::AlignCenter);

		cb_ascending_order[i] = new QCheckBox(str3[language], more_param_window);
		grid -> addWidget(cb_ascending_order[i], i + 1, 4);

		edit_order[i] = new QLineEdit(more_param_window);
		grid -> addWidget(edit_order[i], i + 1, 5);
	}

	for(int i = 0; i < _TOTAL; ++i)
	{
		connect(cb_param[i], &QCheckBox::clicked, this, &Interface::enable_param);
		if((i != 4) && (i != 5) && (i != 10))
		{
			connect(edit_min[i], &QLineEdit::editingFinished, this, set_min[i]);
			connect(edit_max[i], &QLineEdit::editingFinished, this, set_max[i]);
		}
	}

	QStringList str4 = {"<p style=\"line-height:120%\">%1<p>", "<p style=\"line-height:150%\">%1<p>"};
	QStringList str5 = {"<br>*&nbsp;&nbsp;Not selecting a parameter means not applying its conditions and keep it in disorder."
							  "<br>&nbsp;&nbsp;&nbsp;The default order is descending order.<br>"
							  "**&nbsp;Please set the range of M, N in the main Interface and set the conditions of V in 'Apply more rules'.<br>"
							  "***Please fill in the order correctly (1, 2, 3...).",
							  "<br>*&nbsp;&nbsp;不勾选则不应用该项指标的筛选条件，且保持乱序，默认排序为降序排列。 <br>"
							  "**&nbsp;请于主界面设置M和N的范围，于“更多规则”处设置V的条件。<br>"
							  "***请于排序次序框内正确填写序号(1、2、3……)。"};
	QLabel* label3 = new QLabel(str4[language].arg(str5[language]), more_param_window);
	grid -> addWidget(label3, 12, 0, 1, 6, Qt::AlignLeft);

	QStringList str6 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str6[language], more_param_window);
	grid -> addWidget(btn, 13, 4, 1, 2, Qt::AlignRight);
	connect(btn, &QPushButton::clicked, this ,&Interface::closeMoreParam);

	initMoreParam();
	more_param_window -> show();
}

void Interface::initMoreParam()
{
	QString str;
	QStringList str_min = {str.setNum(k_min), str.setNum(t_min), str.setNum(c_min), str.setNum(sv_min), "-1", "-1",
								  str.setNum(r_min), str.setNum(h_min), str.setNum(g_min), str.setNum(x_min),  "-1"};
	QStringList str_max = {str.setNum(k_max), str.setNum(t_max), str.setNum(c_max), str.setNum(sv_max), "-1", "-1",
								  str.setNum(r_max), str.setNum(h_max), str.setNum(g_max), str.setNum(x_max),  "-1"};
	for(int i = 0; i < _TOTAL; ++i)
	{
		if((i != 4) && (i != 5) && (i != 10))
		{
			edit_min[i] -> setText(str_min[i]);
			edit_max[i] -> setText(str_max[i]);
		}
	}
	int pos = 0, len = strlen(sort_order), count = 0;
	while(pos < len)
	{
		int i;
		for(i = 0; i < _TOTAL; ++i)
			if(var[i] == sort_order[pos])  break;
		++pos;  ++count;
		cb_param[i] -> setChecked(true);
		edit_order[i] -> setText(str.setNum(count));
		if(pos < len && sort_order[pos] == '+')
		{
			cb_ascending_order[i] -> setChecked(true);
			++pos;
		}
	}
	enable_param(0);
}

void Interface::set_default_param(int index)
{
	QStringList str_min = {  "0",   "0",  "0",    "0", "-1", "-1",  "0",  "0",   "0",   "0", "-1"};
	QStringList str_max = {"100", "100", "15", "2000", "-1", "-1", "11", "50", "100", "100", "-1"};
	vector<int> val_min = {  0,   0,  0,    0, -1, -1,  0,  0,   0,   0, -1};
	vector<int> val_max = {100, 100, 15, 2000, -1, -1, 11, 50, 100, 100, -1};
	edit_min[index] -> setText(str_min[index]);
	edit_max[index] -> setText(str_max[index]);

	bool isDouble;
	int *pt1_min, *pt1_max;
	double *pt2_min, *pt2_max;
	if(index == 2 || index == 3 || index == 6 || index == 9)
		isDouble = false;
	else  isDouble = true;
	switch(index)
	{
		case 0:  pt2_min = &k_min;  pt2_max = &k_max;  break;
		case 1:  pt2_min = &t_min;  pt2_max = &t_max;  break;
		case 2:  pt1_min = &c_min;  pt1_max = &c_max;  break;
		case 3:  pt1_min = &sv_min; pt1_max = &sv_max; break;
		case 6:  pt1_min = &r_min;  pt1_max = &r_max;  break;
		case 7:  pt2_min = &h_min;  pt2_max = &h_max;  break;
		case 8:  pt2_min = &g_min;  pt2_max = &g_max;  break;
		case 9:  pt1_min = &x_min;  pt1_max = &x_max;  break;
	}
	if(isDouble)
	{
		*pt2_min = val_min[index];
		*pt2_max = val_max[index];
	}
	else
	{
		*pt1_min = val_min[index];
		*pt1_max = val_max[index];
	}
}

void Interface::set_param_min(int index, int min)
{
	bool isDouble;
	int *pt1_min, *pt1_max;
	double *pt2_min, *pt2_max;
	if(index == 2 || index == 3 || index == 6 || index == 9)
		isDouble = false;
	else  isDouble = true;
	switch(index)
	{
		case 0:  pt2_min = &k_min;  pt2_max = &k_max;  break;
		case 1:  pt2_min = &t_min;  pt2_max = &t_max;  break;
		case 2:  pt1_min = &c_min;  pt1_max = &c_max;  break;
		case 3:  pt1_min = &sv_min; pt1_max = &sv_max; break;
		case 6:  pt1_min = &r_min;  pt1_max = &r_max;  break;
		case 7:  pt2_min = &h_min;  pt2_max = &h_max;  break;
		case 8:  pt2_min = &g_min;  pt2_max = &g_max;  break;
		case 9:  pt1_min = &x_min;  pt1_max = &x_max;  break;
	}

	QString text = edit_min[index] -> text(), str;
	if(isDouble)
	{
		*pt2_min = text.toDouble();
		if(*pt2_min > *pt2_max)
		{
			*pt2_min = *pt2_max;
			edit_min[index] -> setText(edit_max[index] -> text());
		}
		else if(*pt2_min <= min)
		{
			*pt2_min = min;
			edit_min[index] -> setText(str.setNum(min));
		}
	}
	else
	{
		*pt1_min = text.toInt();
		if(*pt1_min > *pt1_max)
		{
			*pt1_min = *pt1_max;
			edit_min[index] -> setText(edit_max[index] -> text());
		}
		else if(*pt1_min <= min)
		{
			*pt1_min = min;
			edit_min[index] -> setText(str.setNum(min));
		}
	}
}

void Interface::set_param_max(int index, int max)
{
	bool isDouble;
	int *pt1_min, *pt1_max;
	double *pt2_min, *pt2_max;
	if(index == 2 || index == 3 || index == 6 || index == 9)
		isDouble = false;
	else  isDouble = true;
	switch(index)
	{
		case 0:  pt2_min = &k_min;  pt2_max = &k_max;  break;
		case 1:  pt2_min = &t_min;  pt2_max = &t_max;  break;
		case 2:  pt1_min = &c_min;  pt1_max = &c_max;  break;
		case 3:  pt1_min = &sv_min; pt1_max = &sv_max; break;
		case 6:  pt1_min = &r_min;  pt1_max = &r_max;  break;
		case 7:  pt2_min = &h_min;  pt2_max = &h_max;  break;
		case 8:  pt2_min = &g_min;  pt2_max = &g_max;  break;
		case 9:  pt1_min = &x_min;  pt1_max = &x_max;  break;
	}

	QString text = edit_max[index] -> text(), str;
	if(isDouble)
	{
		*pt2_max = text.toDouble();
		if(*pt2_max < *pt2_min)
		{
			*pt2_max = *pt2_min;
			edit_max[index] -> setText(edit_min[index] -> text());
		}
		else if(*pt2_max > max)
		{
			*pt2_max = max;
			edit_max[index] -> setText(str.setNum(max));
		}
	}
	else
	{
		*pt1_max = text.toInt();
		if(*pt1_max < *pt1_min)
		{
			*pt1_max = *pt1_min;
			edit_max[index] -> setText(edit_min[index] -> text());
		}
		else if(*pt1_max > max)
		{
			*pt1_max = max;
			edit_max[index] -> setText(str.setNum(max));
		}
	}
}

void Interface::closeMoreParam()
{
	if(continual)
	{
		int pos = 0;
		for(int i = 0; i < _TOTAL; ++i)
		{
			if(cb_param[i] -> isChecked())
				sort_order[pos++] = var[i];
		}
		sort_order[pos] = '\0';
		more_param_window -> close();
	}
	else
	{
		vector<int> orders, indexes;
		for(int i = 0; i < _TOTAL; ++i)
		{
			if(cb_param[i] -> isChecked())
			{
				int order = edit_order[i] -> text().toInt();
				indexes.push_back(i);
				orders.push_back(order);
			}
		}
		bool valid = true;
		int pos = 0, j = 0;
		for(int i = 1; i <= (int)indexes.size(); ++i)
		{
			for(j = 0; j < (int)indexes.size(); ++j)
			{
				if(orders[j] == i)
				{
					sort_order[pos++] = var[indexes[j]];
					if(cb_ascending_order[indexes[j]] -> isChecked())
						sort_order[pos++] = '+';
					break;
				}
			}
			if(j == (int)indexes.size())
			{
				valid = false;
				break;
			}
		}

		if(valid)
		{
			sort_order[pos] = '\0';
			more_param_window -> close();
		}
		else
		{
			sort_order[0] = '\0';
			QStringList str1 = {"Warning", "警告"};
			QStringList str2 = {"Sorting orders are not correct - please check and try again.",
									  "排序次序填写不正确，请检查。"};
			QMessageBox::warning(this, str1[language], str2[language], QMessageBox::Close);
		}
	}
}

void Interface::enable_param(bool state)
{
	Q_UNUSED(state);
	for(int i = 0; i < _TOTAL; ++i)
	{
		if(cb_param[i] -> isChecked())
		{
			if(i != 4 && i != 5 && i != 10)
			{
				edit_min[i] -> setEnabled(true);
				edit_max[i] -> setEnabled(true);
			}
			if(!continual)
			{
				cb_ascending_order[i] -> setEnabled(true);
				edit_order[i] -> setEnabled(true);
			}
			else
			{
				cb_ascending_order[i] -> setChecked(false);
				cb_ascending_order[i] -> setDisabled(true);
				edit_order[i] -> setDisabled(true);
				edit_order[i] -> setText("0");
			}
		}
		else
		{
			if(i != 4 && i != 5 && i != 10)
			{
				set_default_param(i);
				edit_min[i] -> setDisabled(true);
				edit_max[i] -> setDisabled(true);
			}
			cb_ascending_order[i] -> setDisabled(true);
			edit_order[i] -> setDisabled(true);
			edit_order[i] -> setText("0");
		}
	}
	if(!continual)
	{
		int max_order = 0;
		QString str;
		for(int i = 0; i < _TOTAL; ++i)
		{
			if(edit_order[i] -> text().toInt() > max_order)
				max_order = edit_order[i] -> text().toInt();
		}
		for(int i = 0; i < _TOTAL; ++i)
		{
			if(cb_param[i] -> isChecked() && edit_order[i] -> text().toInt() == 0)
				edit_order[i] -> setText(str.setNum(++max_order));
		}
	}
}

void Interface::set_k_min()  { set_param_min(0, 0); }
void Interface::set_k_max()  { set_param_max(0, 100); }
void Interface::set_t_min()  { set_param_min(1, 0); }
void Interface::set_t_max()  { set_param_max(1, 100); }
void Interface::set_c_min()  { set_param_min(2, 0); }
void Interface::set_c_max()  { set_param_max(2, 15); }
void Interface::set_sv_min() { set_param_min(3, 0); }
void Interface::set_sv_max() { set_param_max(3, 2000); }
void Interface::set_r_min()  { set_param_min(6, 0); }
void Interface::set_r_max()  { set_param_max(6, 11); }
void Interface::set_h_min()  { set_param_min(7, 0); }
void Interface::set_h_max()  { set_param_max(7, 50); }
void Interface::set_g_min()  { set_param_min(8, 0); }
void Interface::set_g_max()  { set_param_max(8, 100); }
void Interface::set_x_min()  { set_param_min(9, 0); }
void Interface::set_x_max()  { set_param_max(9, 100); }

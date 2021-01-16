// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// moreparamgui.cpp

#include "interface.h"

void (Interface::*set_min[VAR_TOTAL]) () =
	{ nullptr,               nullptr,               &Interface::set_t_min,  &Interface::set_k_min,
	 &Interface::set_c_min,	&Interface::set_s_min,  &Interface::set_ss_min,  nullptr,
	 &Interface::set_h_min, &Interface::set_g_min,  &Interface::set_sv_min, &Interface::set_q_min,
	 &Interface::set_x_min, &Interface::set_kk_min, &Interface::set_r_min,   nullptr };
void (Interface::*set_max[VAR_TOTAL]) () =
	{ nullptr,               nullptr,               &Interface::set_t_max,  &Interface::set_k_max,
	 &Interface::set_c_max,	&Interface::set_s_max,  &Interface::set_ss_max,  nullptr,
	 &Interface::set_h_max, &Interface::set_g_max,  &Interface::set_sv_max, &Interface::set_q_max,
	 &Interface::set_x_max, &Interface::set_kk_max, &Interface::set_r_max,   nullptr };

void Interface::moreParamGui()
{
	more_param_window = new QWidget(this);
	more_param_window -> setWindowModality(Qt::WindowModal);
	more_param_window -> setWindowFlag(Qt::Window, true);
	more_param_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	more_param_window -> setWindowFlag(Qt::WindowCloseButtonHint, false);
	QStringList str0 = {"Set more parameters and sort priorties", "更多指标设置及排序设置"};
	more_param_window -> setWindowTitle(str0[language]);
	more_param_window -> setFont(font);

	int width, height;
#if __WIN32
	height = 640;
#else
	height = 685;
#endif
	if(language == Chinese)
		width = 550;
	else  width = 650;
	more_param_window -> setFixedSize(width * hscale, height * vscale);

	QGridLayout* grid = new QGridLayout(more_param_window);
	grid -> setContentsMargins(30 * hscale, 30 * vscale, 30 * hscale, 20 * vscale);
	grid -> setHorizontalSpacing(20 * hscale);
	grid -> setColumnStretch(2, 0);
	grid -> setColumnStretch(3, 1);

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

	QLabel* label_param[VAR_TOTAL];
	QStringList str_param[VAR_TOTAL] =
		{ {"", ""},
		  {"Chord cardinality (N)",						 "和弦音数 (N)"},
		  {"≤  Chord tension (T%)  ≤",					 "≤  和弦紧张度占比 (T%)  ≤"},
		  {"≤  Chroma value (Hua) (K%)  ≤",				 "≤  华氏色彩度占比 (K%)  ≤"},
		  {"≤  Number of common tones (C)  ≤",			 "≤  进行共同音个数 (C)  ≤"},
		  {"≤  Chord span in fifths (S)  ≤",          "≤  和弦纯五跨度 (S)  ≤"},
		  {"≤  Unioned chord span in fifths (SS)  ≤", "≤  相邻和弦的合跨度 (SS)  ≤"},
		  {"Chord voice cardinality (M)",				 "和弦声部数 (M)"},
		  {"≤  Chord thickness (H)  ≤",					 "≤  和弦八度厚度 (H)  ≤"},
		  {"≤  Chord geometric center (G%)  ≤",		 "≤  和弦几何中心位置 (G%)  ≤"},
		  {"≤  Total voice leading (Σvec)  ≤",			 "≤  声部运动总大小 (Σvec)  ≤"},
		  {"≤  Q-Indicator value (Chen) (Q)  ≤",		 "≤  陈氏Q指标数值 (Q)  ≤"},
		  {"≤  Lateral similarity (X%)  ≤",				 "≤  横向相似度 (X%)  ≤"},
		  {"≤  Gross chroma value (KK%)  ≤",			 "≤  华氏毛色彩度占比 (KK%)  ≤"},
		  {"≤  Chord root (Hindemith) (R)  ≤",        "≤  和弦根音（欣氏法）(R)  ≤"},
		  {"Chord root movement interval (V)",			 "进行根音运动音程 (V)"} };
	QStringList str3 = {"Ascending", "升序"};
	int row = 1;
	for(int i = 1; i < VAR_TOTAL; ++i)
	{
		if((i != 1) && (i != 7) && (i != 15))  // N, M, V
		{
			edit_min[i] = new QLineEdit(more_param_window);
			edit_min[i] -> setFixedWidth(60 * hscale);
			grid -> addWidget(edit_min[i], row, 1);

			edit_max[i] = new QLineEdit(more_param_window);
			edit_max[i] -> setFixedWidth(60 * hscale);
			grid -> addWidget(edit_max[i], row, 3, Qt::AlignLeft);
		}

		cb_param[i] = new QCheckBox(more_param_window);
		grid -> addWidget(cb_param[i], row, 0);

		label_param[i] = new QLabel(str_param[i][language], more_param_window);
		grid -> addWidget(label_param[i], row, 2, Qt::AlignCenter);

		cb_ascending_order[i] = new QCheckBox(str3[language], more_param_window);
		grid -> addWidget(cb_ascending_order[i], row, 4);

		edit_order[i] = new QLineEdit(more_param_window);
		edit_order[i] -> setFixedWidth(40 * hscale);
		grid -> addWidget(edit_order[i], row, 5);

		++row;
	}

	for(int i = 1; i < VAR_TOTAL; ++i)
	{
		connect(cb_param[i], &QCheckBox::clicked, this, &Interface::enable_param);
		if((i != 1) && (i != 7) && (i != 15))
		{
			connect(edit_min[i], &QLineEdit::editingFinished, this, set_min[i]);
			connect(edit_max[i], &QLineEdit::editingFinished, this, set_max[i]);
		}
	}

	QStringList str4 = {"<p style=\"line-height:120%\">%1<p>", "<p style=\"line-height:150%\">%1<p>"};
	QStringList str5 = {"<br>*&nbsp;&nbsp;If a parameter is unchecked, the corresponding conditions will not be applied"
							  "<br>&nbsp;&nbsp;&nbsp;and it will be kept in disorder. The default order is descending order.<br>"
							  "**&nbsp;Please set the range of M, N in the main Interface and set the conditions of V in 'Apply more rules'.<br>"
							  "***Please fill in the order correctly (1, 2, 3...).",
							  "<br>*&nbsp;&nbsp;不勾选则不应用该项指标的筛选条件，且保持乱序，默认排序为降序。 <br>"
							  "**&nbsp;请于主界面设置M和N的范围，于“更多规则”处设置V的条件。<br>"
							  "***请于排序次序框内正确填写序号(1、2、3……)。"};
	QLabel* label3 = new QLabel(str4[language].arg(str5[language]), more_param_window);
	grid -> addWidget(label3, 16, 0, 1, 6, Qt::AlignLeft);

	QStringList str6 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str6[language], more_param_window);
	grid -> addWidget(btn, 17, 4, 1, 2, Qt::AlignRight);
	connect(btn, &QPushButton::clicked, this ,&Interface::closeMoreParam);

	initMoreParam();
	more_param_window -> show();
}

void Interface::initMoreParam()
{
	QString str;
	QStringList str_min = {"-1",              "-1",               str.setNum(t_min),  str.setNum(k_min),
								  str.setNum(c_min), str.setNum(s_min),  str.setNum(ss_min), "-1",
								  str.setNum(h_min), str.setNum(g_min),  str.setNum(sv_min), str.setNum(q_min),
								  str.setNum(x_min), str.setNum(kk_min), str.setNum(r_min),  "-1"};
	QStringList str_max = {"-1",              "-1",               str.setNum(t_max),  str.setNum(k_max),
								  str.setNum(c_max), str.setNum(s_max),  str.setNum(ss_max), "-1",
								  str.setNum(h_max), str.setNum(g_max),  str.setNum(sv_max), str.setNum(q_max),
								  str.setNum(x_max), str.setNum(kk_max), str.setNum(r_max),  "-1"};
	for(int i = 1; i < VAR_TOTAL; ++i)
	{
		if((i != 1) && (i != 7) && (i != 15))
		{
			edit_min[i] -> setText(str_min[i]);
			edit_max[i] -> setText(str_max[i]);
		}
	}
	int pos = 0, len = strlen(sort_order), count = 0;
	while(pos < len)
	{
		int i;
		for(i = 0; i < VAR_TOTAL; ++i)
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
	QStringList str_min = {"-1",  "-1",    "0",    "0",   "0",   "0",  "0", "-1",
									"0",   "0",    "0", "-500",   "0",   "0",  "0", "-1"};
	QStringList str_max = {"-1",  "-1",  "100",  "100",  "15",  "12", "12", "-1",
								  "50", "100", "2000",  "500", "100", "100", "11", "-1"};
	vector<int> val_min = {-1, -1,   0,   0,  0,  0,  0, -1,  0,   0,    0, -500,   0,   0,  0, -1};
	vector<int> val_max = {-1, -1, 100, 100, 15, 12, 12, -1, 50, 100, 2000,  500, 100, 100, 11, -1};
	edit_min[index] -> setText(str_min[index]);
	edit_max[index] -> setText(str_max[index]);

	bool isDouble;
	int *pt1_min, *pt1_max;
	double *pt2_min, *pt2_max;
	if(index == 2 || index == 3 || index == 8 || index == 11 || index == 13)
		isDouble = true;
	else  isDouble = false;
	switch(index)
	{
		case 2:  pt2_min = &t_min;  pt2_max = &t_max;  break;
		case 3:  pt2_min = &k_min;  pt2_max = &k_max;  break;
		case 4:  pt1_min = &c_min;  pt1_max = &c_max;  break;
		case 5:  pt1_min = &s_min;  pt1_max = &s_max;  break;
		case 6:  pt1_min = &ss_min; pt1_max = &ss_max; break;
		case 8:  pt2_min = &h_min;  pt2_max = &h_max;  break;
		case 9:  pt1_min = &g_min;  pt1_max = &g_max;  break;
		case 10: pt1_min = &sv_min; pt1_max = &sv_max; break;
		case 11: pt2_min = &q_min;  pt2_max = &q_max;  break;
		case 12: pt1_min = &x_min;  pt1_max = &x_max;  break;
		case 13: pt2_min = &kk_min; pt2_max = &kk_max; break;
		case 14: pt1_min = &r_min;  pt1_max = &r_max;  break;
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
// Sets the value of parameter #'index' (i.e. the parameter represented by 'var[index]').
// If the value is smaller than 'min', it will be set equal to 'min'.
{
	bool isDouble;
	int *pt1_min, *pt1_max;
	double *pt2_min, *pt2_max;
	if(index == 2 || index == 3 || index == 8 || index == 11 || index == 13)
		isDouble = true;
	else  isDouble = false;
	switch(index)
	{
		case 2:  pt2_min = &t_min;  pt2_max = &t_max;  break;
		case 3:  pt2_min = &k_min;  pt2_max = &k_max;  break;
		case 4:  pt1_min = &c_min;  pt1_max = &c_max;  break;
		case 5:  pt1_min = &s_min;  pt1_max = &s_max;  break;
		case 6:  pt1_min = &ss_min; pt1_max = &ss_max; break;
		case 8:  pt2_min = &h_min;  pt2_max = &h_max;  break;
		case 9:  pt1_min = &g_min;  pt1_max = &g_max;  break;
		case 10: pt1_min = &sv_min; pt1_max = &sv_max; break;
		case 11: pt2_min = &q_min;  pt2_max = &q_max;  break;
		case 12: pt1_min = &x_min;  pt1_max = &x_max;  break;
		case 13: pt2_min = &kk_min; pt2_max = &kk_max; break;
		case 14: pt1_min = &r_min;  pt1_max = &r_max;  break;
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
// Sets the value of parameter #'index' (i.e. the parameter represented by 'var[index]').
// If the value is smaller than 'max', it will be set equal to 'max'.
{
	bool isDouble;
	int *pt1_min, *pt1_max;
	double *pt2_min, *pt2_max;
	if(index == 2 || index == 3 || index == 8 || index == 11 || index == 13)
		isDouble = true;
	else  isDouble = false;
	switch(index)
	{
		case 2:  pt2_min = &t_min;  pt2_max = &t_max;  break;
		case 3:  pt2_min = &k_min;  pt2_max = &k_max;  break;
		case 4:  pt1_min = &c_min;  pt1_max = &c_max;  break;
		case 5:  pt1_min = &s_min;  pt1_max = &s_max;  break;
		case 6:  pt1_min = &ss_min; pt1_max = &ss_max; break;
		case 8:  pt2_min = &h_min;  pt2_max = &h_max;  break;
		case 9:  pt1_min = &g_min;  pt1_max = &g_max;  break;
		case 10: pt1_min = &sv_min; pt1_max = &sv_max; break;
		case 11: pt2_min = &q_min;  pt2_max = &q_max;  break;
		case 12: pt1_min = &x_min;  pt1_max = &x_max;  break;
		case 13: pt2_min = &kk_min; pt2_max = &kk_max; break;
		case 14: pt1_min = &r_min;  pt1_max = &r_max;  break;
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

void Interface::enable_param(bool state)
{
	Q_UNUSED(state);
	for(int i = 1; i < VAR_TOTAL; ++i)
	{
		if(cb_param[i] -> isChecked())
		{
			if(i != 1 && i != 7 && i != 15)
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
			if(i != 1 && i != 7 && i != 15)
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
		for(int i = 1; i < VAR_TOTAL; ++i)
		{
			if(edit_order[i] -> text().toInt() > max_order)
				max_order = edit_order[i] -> text().toInt();
		}
		for(int i = 1; i < VAR_TOTAL; ++i)
		{
			if(cb_param[i] -> isChecked() && edit_order[i] -> text().toInt() == 0)
				edit_order[i] -> setText(str.setNum(++max_order));
		}

		vector<int> orders;
		for(int i = 1; i < VAR_TOTAL; ++i)
		{
			if(cb_param[i] -> isChecked())
			{
				int num = edit_order[i] -> text().toInt();
				if(num != 0)
					orders.push_back(num);
			}
		}
		bubble_sort(orders);
		for(int j = 0; j < (int)orders.size(); ++j)
		{
			for(int i = 1; i < VAR_TOTAL; ++i)
			{
				if(edit_order[i] -> text().toInt() == orders[j])
					edit_order[i] -> setText(str.setNum(j + 1));
			}
		}
	}
}

void Interface::set_t_min()  { set_param_min(2, 0); }
void Interface::set_t_max()  { set_param_max(2, 100); }
void Interface::set_k_min()  { set_param_min(3, 0); }
void Interface::set_k_max()  { set_param_max(3, 100); }
void Interface::set_c_min()  { set_param_min(4, 0); }
void Interface::set_c_max()  { set_param_max(4, 15); }
void Interface::set_s_min()  { set_param_min(5, 0); }
void Interface::set_s_max()  { set_param_max(5, 12); }
void Interface::set_ss_min() { set_param_min(6, 0); }
void Interface::set_ss_max() { set_param_max(6, 12); }
void Interface::set_h_min()  { set_param_min(8, 0); }
void Interface::set_h_max()  { set_param_max(8, 50); }
void Interface::set_g_min()  { set_param_min(9, 0); }
void Interface::set_g_max()  { set_param_max(9, 100); }
void Interface::set_sv_min() { set_param_min(10, 0); }
void Interface::set_sv_max() { set_param_max(10, 2000); }
void Interface::set_q_min()  { set_param_min(11, -500); }
void Interface::set_q_max()  { set_param_max(11, 500); }
void Interface::set_x_min()  { set_param_min(12, 0); }
void Interface::set_x_max()  { set_param_max(12, 100); }
void Interface::set_kk_min() { set_param_min(13, 0); }
void Interface::set_kk_max() { set_param_max(13, 100); }
void Interface::set_r_min()  { set_param_min(14, 0); }
void Interface::set_r_max()  { set_param_max(14, 11); }

void Interface::closeMoreParam()
{
	if(continual)
	{
		int pos = 0;
		for(int i = 1; i < VAR_TOTAL; ++i)
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
		for(int i = 1; i < VAR_TOTAL; ++i)
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

	if(sort_order[0] != '\0')
	{
		QStringList str1 = {"(Set)", "（已设置）"};
		label_more_param -> setText(str1[language]);
		label_more_param -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		QStringList str2 = {"(Not set)", "（未设置）"};
		label_more_param -> setText(str2[language]);
		label_more_param -> setStyleSheet("color: #606060;");
	}
	label_more_param -> setFont(font);
}

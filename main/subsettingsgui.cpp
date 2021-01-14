// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// subsettingsgui.cpp

#include "interface.h"

void (Interface::*reset[VAR_TOTAL]) () =
	{&Interface::reset_p, &Interface::reset_n,  &Interface::reset_t,  &Interface::reset_k,
	 &Interface::reset_c, &Interface::reset_s,  &Interface::reset_ss,  nullptr,
	  nullptr,             nullptr,             &Interface::reset_sv, &Interface::reset_q,
	 &Interface::reset_x, &Interface::reset_kk, &Interface::reset_r,   nullptr};
void (Interface::*reset_value[VAR_TOTAL]) () =
	{&Interface::reset_value_p, &Interface::reset_value_n,  &Interface::reset_value_t,  &Interface::reset_value_k,
	 &Interface::reset_value_c, &Interface::reset_value_s,  &Interface::reset_value_ss,  nullptr,
	  nullptr,                   nullptr,                   &Interface::reset_value_sv, &Interface::reset_value_q,
	 &Interface::reset_value_x, &Interface::reset_value_kk, &Interface::reset_value_r,   nullptr};
void (Interface::*plus_minus[VAR_TOTAL]) () =
	{&Interface::plus_minus_p, &Interface::plus_minus_n,  &Interface::plus_minus_t,  &Interface::plus_minus_k,
	 &Interface::plus_minus_c, &Interface::plus_minus_s,  &Interface::plus_minus_ss,  nullptr,
	  nullptr,                  nullptr,                  &Interface::plus_minus_sv, &Interface::plus_minus_q,
	 &Interface::plus_minus_x, &Interface::plus_minus_kk, &Interface::plus_minus_r,   nullptr};

void Interface::subSettingsGui()
{
	sub_settings_window = new QWidget(this);
	sub_settings_window -> setWindowModality(Qt::WindowModal);
	sub_settings_window -> setWindowFlag(Qt::Window, true);
	sub_settings_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	sub_settings_window -> setWindowFlag(Qt::WindowCloseButtonHint, false);
	QStringList str0 = {"Advanced settings for chord substitution", "和弦替代高级设置"};
	sub_settings_window -> setWindowTitle(str0[language]);
	sub_settings_window -> setFont(font);

	int width, height;
#if __WIN32
	height = 615;
#else
	height = 655;
#endif
	if(language == Chinese)
		width = 580;
	else  width = 700;
	sub_settings_window -> setFixedSize(width * hscale, height * vscale);

	QGridLayout* grid = new QGridLayout(sub_settings_window);
	grid -> setContentsMargins(30 * hscale, 30 * vscale, 30 * hscale, 20 * vscale);
	grid -> setHorizontalSpacing(20 * hscale);
	grid -> setColumnStretch(5, 1);

	QStringList str1 = {"Range of parameters", "指 标 范 围"};
	QLabel* label1 = new QLabel(str1[language], sub_settings_window);
	label1 -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	label1 -> setStyleSheet("color: #3F48CC;");
	grid -> addWidget(label1, 0, 0, 1, 6, Qt::AlignCenter);

	QStringList str2 = {"Sorting and its order", "排序及次序"};
	QLabel* label2 = new QLabel(str2[language], sub_settings_window);
	label2 -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	label2 -> setStyleSheet("color: #3F48CC;");
	grid -> addWidget(label2, 0, 6, 1, 2, Qt::AlignCenter);

	QStringList str3 = {"Reset?", "重设？"};
	QLabel* label3 = new QLabel(str3[language], sub_settings_window);
	grid -> addWidget(label3, 1, 2, Qt::AlignCenter);

	QStringList str4 = {"Reset value", "重设数值"};
	QLabel* label4 = new QLabel(str4[language], sub_settings_window);
	grid -> addWidget(label4, 1, 3, Qt::AlignCenter);

	QLabel* label5 = new QLabel("±", sub_settings_window);
	grid -> addWidget(label5, 1, 4, Qt::AlignCenter);
	QLabel* label6 = new QLabel("%", sub_settings_window);
	grid -> addWidget(label6, 1, 5, Qt::AlignLeft);

	QLabel* label_param[VAR_TOTAL];
	QStringList str_param[VAR_TOTAL] =
		{ {"Chord similarity (P%)", "和弦相似度 (P%)"},
		  {"Chord cardinality (N)", "和弦音数 (N)"},
		  {"Chord tension (T)", "和弦紧张度 (T)"},
		  {"Chroma value (Hua) (K)", "华氏色彩度 (K)"},
		  {"Number of common tones (C)", "进行共同音个数 (C)"},
		  {"Chord span in fifths (S)", "和弦纯五跨度 (S)"},
		  {"Unioned chord span in fifths (SS)", "相邻和弦的合跨度 (SS)"},
		  {"", ""}, {"", ""}, {"", ""},
		  {"Total voice leading (Σvec)", "声部运动总大小 (Σvec)"},
		  {"Q-Indicator value (Chen) (Q)", "进行的陈氏Q指标 (Q)"},
		  {"Lateral similarity (X%)", "横向相似度 (X%)"},
		  {"Gross chroma value (KK)", "毛色彩度 (KK)"},
		  {"Chord root (Hindemith) (R)", "和弦根音（欣氏法）(R)"},
		  {"Chord root movement interval (V)", "进行根音运动音程 (V)"} };
	QStringList str5 = {"Ascending", "升序"};

	int row = 2;
	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9) // m, h, g
			continue;

		cb_param_sub[i] = new QCheckBox(sub_settings_window);
		grid -> addWidget(cb_param_sub[i], row, 0);

		label_param[i] = new QLabel(str_param[i][language], sub_settings_window);
		grid -> addWidget(label_param[i], row, 1, Qt::AlignCenter);

		if(i != VAR_TOTAL - 1)
		{
			cb_reset[i] = new QCheckBox(sub_settings_window);
			grid -> addWidget(cb_reset[i], row, 2, Qt::AlignCenter);

			edit_reset_value[i] = new QLineEdit(sub_settings_window);
			edit_reset_value[i] -> setFixedWidth(40 * hscale);
			grid -> addWidget(edit_reset_value[i], row, 3, Qt::AlignCenter);

			edit_plus_minus[i] = new QLineEdit(sub_settings_window);
			edit_plus_minus[i] -> setFixedWidth(40 * hscale);
			grid -> addWidget(edit_plus_minus[i], row, 4);

			cb_percentage[i] = new QCheckBox(sub_settings_window);
			grid -> addWidget(cb_percentage[i], row, 5, Qt::AlignLeft);
		}

		cb_ascending_order_sub[i] = new QCheckBox(str5[language], sub_settings_window);
		grid -> addWidget(cb_ascending_order_sub[i], row, 6);

		edit_order_sub[i] = new QLineEdit(sub_settings_window);
		edit_order_sub[i] -> setFixedWidth(40 * hscale);
		grid -> addWidget(edit_order_sub[i], row, 7);

		++row;
	}

	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;
		connect(cb_param_sub[i], &QCheckBox::clicked, this, &Interface::enable_param_sub);
		if(i != VAR_TOTAL - 1)
		{
			connect(cb_reset[i], &QCheckBox::clicked, this, reset[i]);
			connect(edit_reset_value[i], &QLineEdit::editingFinished, this, reset_value[i]);
			connect(edit_plus_minus[i], &QLineEdit::editingFinished, this, plus_minus[i]);
		}
	}

	QStringList str6 = {"<p style=\"line-height:120%\">%1<p>", "<p style=\"line-height:150%\">%1<p>"};
	QStringList str7 = {"<br>*&nbsp;&nbsp;If a parameter is unchecked, the corresponding conditions will not be applied"
							  "<br>&nbsp;&nbsp;&nbsp;and it will be kept in disorder. The default order is descending order.<br>"
							  "**&nbsp;Please set the conditions of V in 'Apply more rules'.<br>"
							  "***Please fill in the order correctly (1, 2, 3...).",
							  "<br>*&nbsp;&nbsp;不勾选则不应用该项指标的筛选条件，且保持乱序，默认排序为降序排列。 <br>"
							  "**&nbsp;请于“更多规则”处设置V的条件。<br>"
							  "***请于排序次序框内正确填写序号(1、2、3……)。"};
	QLabel* label7 = new QLabel(str6[language].arg(str7[language]), sub_settings_window);
	grid -> addWidget(label7, 15, 0, 1, 8, Qt::AlignLeft);

	QStringList str8 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str8[language], sub_settings_window);
	grid -> addWidget(btn, 16, 6, 1, 2, Qt::AlignRight);
	connect(btn, &QPushButton::clicked, this ,&Interface::closeSubSettings);

	initSubSettings();
	sub_settings_window -> show();
}

void Interface::initSubSettings()
{
	QString str;
	int pos = 0, len = strlen(sort_order_sub), count = 0;
	while(pos < len)
	{
		int i;
		for(i = 0; i < VAR_TOTAL; ++i)
			if(var[i] == sort_order_sub[pos])  break;
		++pos;  ++count;
		cb_param_sub[i] -> setChecked(true);
		edit_order_sub[i] -> setText(str.setNum(count));
		if(pos < len && sort_order_sub[pos] == '+')
		{
			cb_ascending_order_sub[i] -> setChecked(true);
			++pos;
		}
	}

	int len1 = strlen(percentage_list);
	for(pos = 0; pos < len1; ++pos)
	{
		int i;
		for(i = 0; i < VAR_TOTAL - 1; ++i)
			if(var[i] == percentage_list[pos])  break;
		cb_percentage[i] -> setChecked(true);
	}

	int len2 = strlen(reset_list);
	for(pos = 0; pos < len2; ++pos)
	{
		int i;
		for(i = 0; i < VAR_TOTAL - 1; ++i)
			if(var[i] == reset_list[pos])  break;
		cb_reset[i] -> setChecked(true);
	}

	int *pReset[VAR_TOTAL]  = {&p_reset_value, &n_reset_value,  &t_reset_value,  &k_reset_value,
										&c_reset_value, &s_reset_value,  &ss_reset_value,  nullptr,
										 nullptr,        nullptr,        &sv_reset_value, &q_reset_value,
										&x_reset_value, &kk_reset_value, &r_reset_value,   nullptr};
	int *pRadius[VAR_TOTAL] = {&p_radius, &n_radius,  &t_radius,  &k_radius,
										&c_radius, &s_radius,  &ss_radius, nullptr,
										nullptr,   nullptr,   &sv_radius,  &q_radius,
										&x_radius, &kk_radius, &r_radius,  nullptr};

	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;

		bool b1 = cb_param_sub[i] -> isChecked();
		if(i != VAR_TOTAL - 1)
		{
			edit_reset_value[i] -> setText( str.setNum( *pReset[i] ) );
			edit_plus_minus[i]  -> setText( str.setNum( *pRadius[i] ) );
			cb_reset[i] -> setEnabled(b1);
			edit_plus_minus[i] -> setEnabled(b1);
			cb_percentage[i]  -> setEnabled(b1);

			bool b2 = cb_reset[i] -> isChecked();
			edit_reset_value[i] -> setEnabled(b2);
		}
		cb_ascending_order_sub[i] -> setEnabled(b1);
		edit_order_sub[i] -> setEnabled(b1);
		if(!b1)  edit_order_sub[i] -> setText("0");
	}
}

void Interface::enable_param_sub(bool state)
{
	int *pReset[VAR_TOTAL]  = {&p_reset_value, &n_reset_value,  &t_reset_value,  &k_reset_value,
										&c_reset_value, &s_reset_value,  &ss_reset_value,  nullptr,
										 nullptr,        nullptr,        &sv_reset_value, &q_reset_value,
										&x_reset_value, &kk_reset_value, &r_reset_value,   nullptr};
	int *pRadius[VAR_TOTAL] = {&p_radius, &n_radius,  &t_radius,  &k_radius,
										&c_radius, &s_radius,  &ss_radius,  nullptr,
										 nullptr,   nullptr,   &sv_radius, &q_radius,
										&x_radius, &kk_radius, &r_radius,   nullptr};
	Q_UNUSED(state);
	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;

		bool b = (cb_param_sub[i] -> isChecked());
		if(i != VAR_TOTAL - 1)
		{
			cb_reset[i] -> setEnabled(b);
			edit_plus_minus[i] -> setEnabled(b);
			cb_percentage[i]  -> setEnabled(b);
		}
		cb_ascending_order_sub[i] -> setEnabled(b);
		edit_order_sub[i] -> setEnabled(b);

		if(!b)
		{
			if(i != VAR_TOTAL - 1)
			{
				cb_reset[i] -> setChecked(false);
				cb_percentage[i] -> setChecked(false);
				edit_reset_value[i] -> setEnabled(false);
				edit_reset_value[i] -> setText("0");
				edit_plus_minus[i] -> setText("0");
				*pReset[i] = 0;
				*pRadius[i] = 0;
			}
			cb_ascending_order_sub[i] -> setChecked(false);
			edit_order_sub[i] -> setText("0");
		}
	}

	int max_order = 0;
	QString str;
	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;
		if(edit_order_sub[i] -> text().toInt() > max_order)
			max_order = edit_order_sub[i] -> text().toInt();
	}
	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;
		if(cb_param_sub[i] -> isChecked() && edit_order_sub[i] -> text().toInt() == 0)
			edit_order_sub[i] -> setText(str.setNum(++max_order));
	}

	vector<int> orders;
	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;
		if(cb_param_sub[i] -> isChecked())
		{
			int num = edit_order_sub[i] -> text().toInt();
			if(num != 0)
				orders.push_back(num);
		}
	}
	bubble_sort(orders);
	for(int j = 0; j < (int)orders.size(); ++j)
	{
		for(int i = 0; i < VAR_TOTAL; ++i)
		{
			if(i == 7 || i == 8 || i == 9)
				continue;
			if(edit_order_sub[i] -> text().toInt() == orders[j])
				edit_order_sub[i] -> setText(str.setNum(j + 1));
		}
	}
}

void Interface::_reset(int index)
{
	int *pReset[VAR_TOTAL]  = {&p_reset_value, &n_reset_value,  &t_reset_value,  &k_reset_value,
										&c_reset_value, &s_reset_value,  &ss_reset_value,  nullptr,
										 nullptr,        nullptr,        &sv_reset_value, &q_reset_value,
										&x_reset_value, &kk_reset_value, &r_reset_value,   nullptr};
	if(cb_reset[index] -> isChecked())
		edit_reset_value[index] -> setEnabled(true);
	else
	{
		edit_reset_value[index] -> setEnabled(false);
		edit_reset_value[index] -> setText("0");
		*pReset[index] = 0;
	}
}

void Interface::_reset_value(int index)
{
	int *pReset[VAR_TOTAL]  = {&p_reset_value, &n_reset_value,  &t_reset_value,  &k_reset_value,
										&c_reset_value, &s_reset_value,  &ss_reset_value,  nullptr,
										 nullptr,        nullptr,        &sv_reset_value, &q_reset_value,
										&x_reset_value, &kk_reset_value, &r_reset_value,   nullptr};
	int val = edit_reset_value[index] -> text().toInt();
	if(val == 0)
	{
		edit_reset_value[index] -> setText("0");
		*pReset[index] = 0;
	}
	else  *pReset[index] = val;
}

void Interface::_plus_minus(int index)
{
	int *pRadius[VAR_TOTAL] = {&p_radius, &n_radius,  &t_radius,  &k_radius,
										&c_radius, &s_radius,  &ss_radius, nullptr,
										nullptr,   nullptr,   &sv_radius,  &q_radius,
										&x_radius, &kk_radius, &r_radius,  nullptr};
	int val = edit_plus_minus[index] -> text().toInt();
	if(val <= 0)
	{
		edit_plus_minus[index] -> setText("0");
		*pRadius[index] = 0;
	}
	else  *pRadius[index] = val;
}

void Interface::reset_p()  { _reset(0); }
void Interface::reset_n()  { _reset(1); }
void Interface::reset_t()  { _reset(2); }
void Interface::reset_k()  { _reset(3); }
void Interface::reset_c()  { _reset(4); }
void Interface::reset_s()  { _reset(5); }
void Interface::reset_ss() { _reset(6); }
void Interface::reset_sv() { _reset(10); }
void Interface::reset_q()  { _reset(11); }
void Interface::reset_x()  { _reset(12); }
void Interface::reset_kk() { _reset(13); }
void Interface::reset_r()  { _reset(14); }
void Interface::reset_value_p()  { _reset_value(0); }
void Interface::reset_value_n()  { _reset_value(1); }
void Interface::reset_value_t()  { _reset_value(2); }
void Interface::reset_value_k()  { _reset_value(3); }
void Interface::reset_value_c()  { _reset_value(4); }
void Interface::reset_value_s()  { _reset_value(5); }
void Interface::reset_value_ss() { _reset_value(6); }
void Interface::reset_value_sv() { _reset_value(10); }
void Interface::reset_value_q()  { _reset_value(11); }
void Interface::reset_value_x()  { _reset_value(12); }
void Interface::reset_value_kk() { _reset_value(13); }
void Interface::reset_value_r()  { _reset_value(14); }
void Interface::plus_minus_p()  { _plus_minus(0); }
void Interface::plus_minus_n()  { _plus_minus(1); }
void Interface::plus_minus_t()  { _plus_minus(2); }
void Interface::plus_minus_k()  { _plus_minus(3); }
void Interface::plus_minus_c()  { _plus_minus(4); }
void Interface::plus_minus_s()  { _plus_minus(5); }
void Interface::plus_minus_ss() { _plus_minus(6); }
void Interface::plus_minus_sv() { _plus_minus(10); }
void Interface::plus_minus_q()  { _plus_minus(11); }
void Interface::plus_minus_x()  { _plus_minus(12); }
void Interface::plus_minus_kk() { _plus_minus(13); }
void Interface::plus_minus_r()  { _plus_minus(14); }

void Interface::closeSubSettings()
{
	int pos1 = 0, pos2 = 0;
	for(int i = 0; i < VAR_TOTAL - 1; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;
		if(cb_percentage[i] -> isChecked())
			percentage_list[pos1++] = var[i];
		if(cb_reset[i] -> isChecked())
			reset_list[pos2++] = var[i];
	}
	percentage_list[pos1] = '\0';
	reset_list[pos2] = '\0';

	vector<int> orders, indexes;
	for(int i = 0; i < VAR_TOTAL; ++i)
	{
		if(i == 7 || i == 8 || i == 9)
			continue;
		if(cb_param_sub[i] -> isChecked())
		{
			int order = edit_order_sub[i] -> text().toInt();
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
				sort_order_sub[pos++] = var[indexes[j]];
				if(cb_ascending_order_sub[indexes[j]] -> isChecked())
					sort_order_sub[pos++] = '+';
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
		sort_order_sub[pos] = '\0';
		sub_settings_window -> close();
	}
	else
	{
		sort_order_sub[0] = '\0';
		QStringList str1 = {"Warning", "警告"};
		QStringList str2 = {"Sorting orders are not correct - please check and try again.",
								  "排序次序填写不正确，请检查。"};
		QMessageBox::warning(this, str1[language], str2[language], QMessageBox::Close);
	}
}

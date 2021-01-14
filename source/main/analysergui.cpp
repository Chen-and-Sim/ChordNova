// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// analysergui.cpp

#include "interface.h"

void Interface::analyserGui()
{
	analyser_window = new QWidget(this);
	analyser_window -> installEventFilter(this);
	analyser_window -> setWindowModality(Qt::WindowModal);
	analyser_window -> setWindowFlag(Qt::Window, true);
	analyser_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	if(language == Chinese)
		analyser_window -> setFixedSize(385 * hscale, 550 * vscale);
	else  analyser_window -> setFixedSize(440 * hscale, 585 * vscale);
	QStringList str1 = {"Chord analysis/substitution", "和弦进行速查/替代"};
	analyser_window -> setWindowTitle(str1[language]);
	analyser_window -> setFont(font);

	QVBoxLayout* vbox = new QVBoxLayout(analyser_window);
	vbox -> setContentsMargins(20 * hscale, 20 * vscale, 20 * hscale, 25 * vscale);
	vbox -> setSpacing(10 * vscale);

	QGridLayout* grid1 = new QGridLayout();
	grid1 -> setVerticalSpacing(10 * vscale);
	QStringList str2 = {"Enter antechord and postchord to calculate stats and\n"
							  "substitutions, or you can: ",
							  "输入前后和弦以测量指标、计算替代和弦，或可："};
	QLabel* label1 = new QLabel(str2[language], analyser_window);
	grid1 -> addWidget(label1, 0, 0, 1, 2, Qt::AlignVCenter);

	QStringList str3 = {"Fill in\nrandomly", "随机填入"};
	QPushButton* btn1 = new QPushButton(str3[language], analyser_window);
	connect(btn1, &QPushButton::clicked, this, &Interface::random_chords);
	grid1 -> addWidget(btn1, 0, 2);

	QStringList str4 = {"Antechord: ", "前和弦："};
	QStringList str5 = {"Postchord: ", "后和弦："};
	QLabel* label2 = new QLabel(str4[language], analyser_window);
	QLabel* label3 = new QLabel(str5[language], analyser_window);
	grid1 -> addWidget(label2, 1, 0);
	grid1 -> addWidget(label3, 2, 0);

	edit_ante = new QLineEdit(analyser_window);
	edit_post = new QLineEdit(analyser_window);
	connect(edit_ante, &QLineEdit::editingFinished, this, &Interface::set_antechord);
	connect(edit_post, &QLineEdit::editingFinished, this, &Interface::set_postchord);
	grid1 -> addWidget(edit_ante, 1, 1);
	grid1 -> addWidget(edit_post, 2, 1);

	QStringList str6 = {"Swap ↑↓", "交换 ↑↓"};
	QPushButton* btn2 = new QPushButton(str6[language], analyser_window);
	btn2 -> setFixedHeight(50 * vscale);
	connect(btn2, &QPushButton::clicked, this, &Interface::swap_chords);
	grid1 -> addWidget(btn2, 1, 2, 2, 1);
	vbox -> addLayout(grid1);

	QHBoxLayout* hbox1 = new QHBoxLayout();
	hbox1 -> setSpacing(10 * hscale);
	QStringList str7 = {"Results: ", "结果："};
	QLabel* label4 = new QLabel(str7[language], analyser_window);
	label4 -> setAlignment(Qt::AlignLeft);
	hbox1 -> addWidget(label4, 0, Qt::AlignVCenter | Qt::AlignLeft);

	QStringList str8 = {"Calc. stats and\ncopy to clipboard", "测指标并复制到粘贴板"};
	QPushButton* btn3 = new QPushButton(str8[language], analyser_window);
	btn3 -> setMinimumWidth(160 * hscale);
	btn3 -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	connect(btn3, &QPushButton::clicked, this, &Interface::analyse);
	hbox1 -> addWidget(btn3, 1, Qt::AlignLeft);

	QStringList str9 = {"Do not show octave\nin pitch name", "不显示音名八度"};
	cb_hide_octave = new QCheckBox(str9[language], analyser_window);
	connect(cb_hide_octave, &QPushButton::clicked, this, &Interface::set_hide_octave);
	hbox1 -> addWidget(cb_hide_octave, 0, Qt::AlignRight);
	vbox -> addLayout(hbox1);

	edit_stats = new QTextEdit(analyser_window);
	edit_stats -> setFixedHeight(100 * vscale);
	edit_stats -> setStyleSheet("QTextEdit { background: #F3EBC2 }");
	edit_stats -> setReadOnly(true);
	vbox -> addWidget(edit_stats);

	QGridLayout* grid2 = new QGridLayout();
	grid2 -> setContentsMargins(0, 20 * vscale, 0, 0);
	grid2 -> setVerticalSpacing(10 * vscale);
	grid2 -> setColumnStretch(0, 0);
	grid2 -> setColumnStretch(1, 1);
	grid2 -> setColumnStretch(2, 1);

	QStringList str10 = {"Substitute object: ", "和弦替代对象："};
	QLabel* label5 = new QLabel(str10[language], analyser_window);
	grid2 -> addWidget(label5, 0, 0);

	QStringList str11 = {"Postchord", "后和弦"};
	QStringList str12 = {"Antechord", "前和弦"};
	QStringList str13 = {"Both", "两者"};
	btn_post = new QRadioButton(str11[language], analyser_window);
	btn_ante = new QRadioButton(str12[language], analyser_window);
	btn_both = new QRadioButton(str13[language], analyser_window);

	QButtonGroup* btns1 = new QButtonGroup(analyser_window);
	btns1 -> addButton(btn_post);
	btns1 -> addButton(btn_ante);
	btns1 -> addButton(btn_both);
	connect(btn_post, SIGNAL(toggled(bool)), this, SLOT(set_substitute_obj(bool)));
	connect(btn_ante, SIGNAL(toggled(bool)), this, SLOT(set_substitute_obj(bool)));
	connect(btn_both, SIGNAL(toggled(bool)), this, SLOT(set_substitute_obj(bool)));

	QHBoxLayout* hbox2 = new QHBoxLayout();
	hbox2 -> addWidget(btn_post);
	hbox2 -> addWidget(btn_ante, 1, Qt::AlignCenter);
	hbox2 -> addWidget(btn_both, 0, Qt::AlignRight);
	grid2 -> addLayout(hbox2, 0, 1, 1, 2);

	QStringList str0 = {"Performance option: ", "性能时间选项："};
	QLabel* label0 = new QLabel(str0[language], analyser_window);
	grid2 -> addWidget(label0, 1, 0);

	QStringList str14 = {"Test all", "检测全部"};
	QStringList str15 = {"Test sample size: ", "检测样本数："};
	btn_test_all  = new QRadioButton(str14[language], analyser_window);
	btn_test_part = new QRadioButton(str15[language], analyser_window);

	QButtonGroup* btns2 = new QButtonGroup(analyser_window);
	btns2 -> addButton(btn_test_all);
	btns2 -> addButton(btn_test_part);
	connect(btn_test_all , &QRadioButton::clicked, this, &Interface::set_test_all);
	connect(btn_test_part, &QRadioButton::clicked, this, &Interface::set_test_all);

	QHBoxLayout* hbox3 = new QHBoxLayout();
	hbox3 -> setSpacing(5 * hscale);
	hbox3 -> addWidget(btn_test_all, Qt::AlignVCenter);
	hbox3 -> addSpacing(20 * hscale);
	hbox3 -> addWidget(btn_test_part, Qt::AlignVCenter);

	edit_sample_size = new QLineEdit(analyser_window);
	edit_sample_size -> setFixedWidth(40 * hscale);
	connect(edit_sample_size, &QLineEdit::editingFinished, this, &Interface::set_sample_size);
	hbox3 -> addWidget(edit_sample_size, Qt::AlignCenter);

	QStringList str16= {"M", "百万"};
	label_sample_size = new QLabel(str16[language], analyser_window);
	label_sample_size -> setAlignment(Qt::AlignVCenter);
	hbox3 -> addWidget(label_sample_size, Qt::AlignCenter);
	grid2 -> addLayout(hbox3, 1, 1, 1, 2);

	QStringList str17= {"Substitute reference: ", "和弦替代依据："};
	QLabel* label7 = new QLabel(str17[language], analyser_window);
	grid2 -> addWidget(label7, 2, 0);

	QStringList str18 = {"Default (equal k values)", "默认（k值相等）"};
	QStringList str19 = {"Customized", "定制"};
	btn_def_ref = new QRadioButton(str18[language], analyser_window);
	btn_cus_ref = new QRadioButton(str19[language], analyser_window);

	QButtonGroup* btns3 = new QButtonGroup(analyser_window);
	btns3 -> addButton(btn_def_ref);
	btns3 -> addButton(btn_cus_ref);
	connect(btn_def_ref, SIGNAL(toggled(bool)), this, SLOT(set_substitute_ref(bool)));
	connect(btn_cus_ref, SIGNAL(toggled(bool)), this, SLOT(set_substitute_ref(bool)));

	QStringList str20 = {"Details...", "定 制 详 细..."};
	btn_detail_ref = new QPushButton(str20[language], analyser_window);
	btn_detail_ref -> setDisabled(true);
	btn_detail_ref -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	if(language == Chinese)
		btn_detail_ref -> setFixedWidth(190 * hscale);
	else  btn_detail_ref -> setFixedWidth(165 * hscale);
	connect(btn_detail_ref, &QPushButton::clicked, this, &Interface::subSettingsGui);

	QHBoxLayout* hbox4 = new QHBoxLayout();
	hbox4 -> setSpacing(15 * hscale);
	hbox4 -> addWidget(btn_cus_ref, 0);
	hbox4 -> addWidget(btn_detail_ref, 1, Qt::AlignLeft);
	grid2 -> addWidget(btn_def_ref, 2, 1, 1, 2);
	grid2 -> addLayout(hbox4, 3, 1, 1, 2);

	QStringList str21 = {"Output filename:\n(omit extension)", "输出文件名：\n（省略扩展名）"};
	QLabel* label8 = new QLabel(str21[language], analyser_window);
	grid2 -> addWidget(label8, 4, 0);

	QPixmap pic1 = QPixmap("icons/settings.png");
	QPushButton* btn4 = new QPushButton(analyser_window);
	btn4 -> setIcon(pic1);
	btn4 -> setFixedSize(QSize(25 * hscale, 25 * hscale));
	btn4 -> setIconSize (QSize(20 * hscale, 20 * hscale));
	connect(btn4, &QPushButton::clicked, this, &Interface::set_output_path_sub);

	edit_output_name_sub = new QLineEdit(analyser_window);
	if(language == Chinese)
		edit_output_name_sub -> setFixedWidth(217 * hscale);
	else  edit_output_name_sub -> setFixedWidth(235 * hscale);
	connect(edit_output_name_sub, &QLineEdit::textChanged, this, &Interface::set_output_name_sub);

	QHBoxLayout* hbox5 = new QHBoxLayout();
	hbox5 -> addWidget(btn4, 0);
	hbox5 -> addWidget(edit_output_name_sub, 1, Qt::AlignLeft);
	grid2 -> addLayout(hbox5, 4, 1, 1, 2);

	QStringList str22 = {"Output format: ", "输出格式："};
	QLabel* label9 = new QLabel(str22[language], analyser_window);
	label9 -> setAlignment(Qt::AlignTop);
	grid2 -> addWidget(label9, 5, 0);

	QStringList str23 = {"Default", "默认"};
	QStringList str24 = {"MIDI only", "仅MIDI"};
	QStringList str25 = {"Report only (.txt)", "仅分析报告(.txt)"};
	btn_def_sub  = new QRadioButton(str23[language], analyser_window);
	btn_midi_sub = new QRadioButton(str24[language], analyser_window);
	btn_text_sub = new QRadioButton(str25[language], analyser_window);

	QButtonGroup* btns4 = new QButtonGroup(analyser_window);
	btns4 -> addButton(btn_def_sub);
	btns4 -> addButton(btn_midi_sub);
	btns4 -> addButton(btn_text_sub);
	connect(btn_def_sub , SIGNAL(toggled(bool)), this, SLOT(set_output_format_sub(bool)));
	connect(btn_midi_sub, SIGNAL(toggled(bool)), this, SLOT(set_output_format_sub(bool)));
	connect(btn_text_sub, SIGNAL(toggled(bool)), this, SLOT(set_output_format_sub(bool)));

	QVBoxLayout* vbox_ = new QVBoxLayout();
	vbox_ -> addWidget(btn_def_sub);
	vbox_ -> addWidget(btn_midi_sub);
	vbox_ -> addWidget(btn_text_sub);
	grid2 -> addLayout(vbox_, 5, 1);

	QPixmap pic2 = QPixmap("icons/go.png");
	QPushButton* btn5 = new QPushButton(analyser_window);
	btn5 -> setIcon(pic2);
	btn5 -> setFixedSize(QSize(86 * hscale, 56 * hscale));
	btn5 -> setIconSize (QSize(80 * hscale, 50 * hscale));
	connect(btn5, &QPushButton::clicked, this, &Interface::run_sub);
	grid2 -> addWidget(btn5, 5, 2, Qt::AlignRight);
	vbox -> addLayout(grid2);

	initAnalyser();
	analyser_window -> show();
}

void Interface::initAnalyser()
{
	QString str;
	edit_ante -> setText(str_ante_notes);
	edit_post -> setText(str_post_notes);
	set_antechord();
	set_postchord();
	if(hide_octave)
		cb_hide_octave -> setChecked(true);

	switch(object)
	{
		case Postchord:  btn_post -> setChecked(true);  break;
		case Antechord:  btn_ante -> setChecked(true);  break;
		case BothChords: btn_both -> setChecked(true);	break;
	}
	set_substitute_obj(0);
	if(test_all)
		btn_test_all -> setChecked(true);
	else  btn_test_part -> setChecked(true);
	edit_sample_size -> setText(str.setNum(sample_size / 1E6));

	if(detailed_ref)
		btn_cus_ref -> setChecked(true);
	else
	{
		btn_def_ref -> setChecked(true);
		btn_detail_ref -> setDisabled(true);
	}

	edit_output_name_sub -> setText(output_name_sub);
	switch(output_mode_sub)
	{
		case Both:      btn_def_sub  -> setChecked(true);  break;
		case MidiOnly:  btn_midi_sub -> setChecked(true);  break;
		case TextOnly:  btn_text_sub -> setChecked(true);  break;
	}
}

void Interface::random_chords()
{
	const int ante_size = rand(2, 5);
	const int post_size = rand(2, 5);
	int cur_ante_size, cur_post_size;

	ante_notes.clear();
	do{
		int note = rand(0, 127);
		ante_notes.push_back(note);
		bubble_sort(ante_notes);
		remove_duplicate(ante_notes);
		cur_ante_size = ante_notes.size();
	}  while(cur_ante_size != ante_size);

	post_notes.clear();
	do{
		int note = rand(0, 127);
		post_notes.push_back(note);
		bubble_sort(post_notes);
		remove_duplicate(post_notes);
		cur_post_size = post_notes.size();
	}  while(cur_post_size != post_size);

	QString str;
	strcpy(str_ante_notes, "");
	for(int i = 0; i < ante_size; ++i)
	{
		strcat(str_ante_notes, str.setNum(ante_notes[i]).toLatin1().data() );
		strcat(str_ante_notes, " ");
	}
	edit_ante -> setText((QString)str_ante_notes);

	strcpy(str_post_notes, "");
	for(int i = 0; i < post_size; ++i)
	{
		strcat(str_post_notes, str.setNum(post_notes[i]).toLatin1().data() );
		strcat(str_post_notes, " ");
	}
	edit_post -> setText((QString)str_post_notes);
}

void Interface::set_antechord()
{
	strcpy(str_ante_notes, edit_ante -> text().toLatin1().data());
	set_notes(ante_notes, edit_ante);
}

void Interface::set_postchord()
{
	strcpy(str_post_notes, edit_post -> text().toLatin1().data());
	set_notes(post_notes, edit_post);
}

void Interface::swap_chords()
{
	QString temp = edit_ante -> text();
	edit_ante -> setText(edit_post -> text());
	edit_post -> setText(temp);
	set_antechord();
	set_postchord();
}

void Interface::set_hide_octave()
{
	hide_octave = (cb_hide_octave -> isChecked());
	analyse();
}

void Interface::analyse()
{
	if(ante_notes.empty() || post_notes.empty())
		return;
	Chord::analyse();
	char result[1000];
	stream.getline(result, 1000, '\0');
	stream.clear();
	stream << flush;
	edit_stats -> setText(result);
	edit_stats -> selectAll();
	edit_stats -> copy();
}

void Interface::set_substitute_obj(bool b)
{
	Q_UNUSED(b);
	if(btn_post -> isChecked())
	{
		object = Postchord;
		edit_sample_size -> setDisabled(true);
		btn_test_all -> setDisabled(true);
		btn_test_part -> setDisabled(true);
		label_sample_size -> setDisabled(true);
	}
	else if(btn_ante -> isChecked())
	{
		object = Antechord;
		edit_sample_size -> setDisabled(true);
		btn_test_all -> setDisabled(true);
		btn_test_part -> setDisabled(true);
		label_sample_size -> setDisabled(true);
	}
	else
	{
		object = BothChords;
		btn_test_all -> setEnabled(true);
		btn_test_part -> setEnabled(true);
		label_sample_size -> setEnabled(true);
		edit_sample_size -> setDisabled(test_all);
	}
}

void Interface::set_test_all()
{
	if(btn_test_all -> isChecked())
	{
		edit_sample_size -> setDisabled(true);
		test_all = true;
	}
	else if(btn_test_part -> isChecked())
	{
		edit_sample_size -> setEnabled(true);
		test_all = false;
	}
}

void Interface::set_sample_size()
{
	QString str = edit_sample_size -> text();
	sample_size = str.toDouble() * 1E6;
	if(sample_size <= 0 || sample_size > INF)
	{
		sample_size = 1E6;
		edit_sample_size -> setText("1");
	}
}


void Interface::set_substitute_ref(bool b)
{
	Q_UNUSED(b);
	if(btn_def_ref -> isChecked())
	{
		btn_detail_ref -> setDisabled(true);
		detailed_ref = false;
		k_reset_value = 0;
		k_radius = 0;
		strcpy(reset_list, "");
		strcpy(percentage_list, "");
		strcpy(sort_order_sub, "K+");
	}
	else if(btn_cus_ref -> isChecked())
	{
		btn_detail_ref -> setEnabled(true);
		detailed_ref = true;
	}
}

void Interface::set_output_name_sub()
{
	strcpy(output_name_sub, edit_output_name_sub -> text().toUtf8().data());
}

void Interface::set_output_path_sub()
{
	QFileDialog* dialog = new QFileDialog(this);
	QStringList str = {"Choose output path", "选择输出文件夹"};
	dialog -> setWindowTitle(str[language]);
	dialog -> setFileMode(QFileDialog::Directory);
	dialog -> setDirectory(root_path + output_path);
	if(dialog -> exec())
	{
		QStringList temp = dialog -> selectedFiles();
		if(temp[0].right(0) != '/')
			temp[0] += '/';
		strcpy(output_path, temp[0].toLocal8Bit().data());
	}
}

void Interface::set_output_format_sub(bool state)
{
	Q_UNUSED(state);
	int n = btn_midi_sub -> isChecked() + 2 * btn_text_sub -> isChecked();
	output_mode_sub = (OutputMode)n;
}

void Interface::run_sub()
{
	QStringList str1 = {"Warning", "警告"};
	QStringList str2 = {"Please calculate stats first.", "请先测和弦指标。"};
	if(edit_stats -> toPlainText().isEmpty())
	{
		QMessageBox::warning(this, str1[language], str2[language], QMessageBox::Close);
		return;
	}

	if(object == BothChords)
	{
		QStringList str3 = {"Generating...", "生成中…"};
		QStringList str4 = {"Abort", "中止"};
		prgdialog_sub = new QProgressDialog("", str4[language], 0, 100, analyser_window);
		prgdialog_sub -> setFont(font);
		prgdialog_sub -> setWindowTitle(str3[language]);
		prgdialog_sub -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
		prgdialog_sub -> setWindowFlag(Qt::WindowCloseButtonHint, false);
		prgdialog_sub -> setWindowFlag(Qt::WindowContextHelpButtonHint, false);
		prgdialog_sub -> setWindowModality(Qt::WindowModal);
		prgdialog_sub -> setAutoClose(false);
		prgdialog_sub -> setAutoReset(false);
		prgdialog_sub -> setMinimumDuration(0);
	}

	vector<int> temp(rm_priority);
	rm_priority.assign(7, -1);
	for(int i = 0; i < (int)temp.size(); ++i)
		rm_priority[temp[i]] = i;
	// Here is the 'rm_priority' we actually use in the program.
	// The value of 'rm_priority[i]', if not equal to -1, is the order of interval i,
	// otherwise the interval is not allowed.

	try{ substitute(); }
	catch(const char* msg)
	{
		if(object == BothChords)
			prgdialog_sub -> close();
		QMessageBox::warning(this, str1[language], msg, QMessageBox::Close);
		if(fout.is_open())  fout.close();
		if(m_fout.is_open())  m_fout.close();
		return;
	}
	catch(...)
	{
		if(object == BothChords)
			prgdialog_sub -> close();
		QStringList str5 = {"Critical error", "严重错误"};
		QStringList str6 = {"Unknown error", "未知错误"};
		QMessageBox::critical(this, str5[language], str6[language], QMessageBox::Close);
		if(fout.is_open())  fout.close();
		if(m_fout.is_open())  m_fout.close();
		return;
	}

	QStringList str7  = {"Message", "消息"};
	QStringList str8  = {"Generation completed. Open generated file(s)?", "生成完毕。打开生成的文件？"};
	QStringList str9  = {"Yes", "是"};
	QStringList str10 = {"No",  "否"};
	QMessageBox* completed = new QMessageBox(QMessageBox::Information, str7[language], str8[language]);
	QPushButton* open = (completed -> addButton(str9[language], QMessageBox::AcceptRole));
	completed -> addButton(str10[language], QMessageBox::RejectRole);
	completed -> exec();

	if(completed -> clickedButton() == open)
	{
		if(output_mode_sub != TextOnly)
			QDesktopServices::openUrl(QUrl( ((QString)"file:%1%2.mid").arg(output_path).arg(output_name_sub) ));
		if(output_mode_sub != MidiOnly)
			QDesktopServices::openUrl(QUrl( ((QString)"file:%1%2.txt").arg(output_path).arg(output_name_sub) ));
	}
	rm_priority.assign(temp.begin(), temp.end());
}

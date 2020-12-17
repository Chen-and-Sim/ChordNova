// SmartChordGen v3.0 [Build: 2020.11.27]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// alignmentgui.cpp

#include "Interface.h"

void Interface::alignmentGui()
{
	alignment_window = new QWidget(this);
	alignment_window -> setWindowModality(Qt::WindowModal);
	alignment_window -> setWindowFlag(Qt::Window, true);
	alignment_window -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	alignment_window -> setWindowFlag(Qt::WindowCloseButtonHint, false);
	if(language == Chinese)
		alignment_window -> setFixedSize(500 * scale, 350 * _scale);
	else  alignment_window -> setFixedSize(640 * scale, 350 * _scale);
	QStringList str0 = {"Set alignment", "设置排列方式"};
	alignment_window -> setWindowTitle(str0[language]);
	alignment_window -> setFont(font);

	QGridLayout* grid = new QGridLayout(alignment_window);
	grid -> setContentsMargins(20 * _scale, 25 * scale, 20 * _scale, 25 * scale);
	grid -> setVerticalSpacing(15);

	QStringList str1 = {"Custom alignment: ", "自定义排列："};
	QStringList str15 = {"No constriants (not recommended)", "排列方式不限（不推荐）"};
	btn_interval  = new QRadioButton(alignment_window);
	btn_alignment = new QRadioButton(str1[language], alignment_window);
	btn_unlimited = new QRadioButton(str15[language], alignment_window);
	btn_interval  -> setChecked(true);
	grid -> addWidget(btn_interval , 0, 0);
	grid -> addWidget(btn_alignment, 3, 0, 1, 2);
	grid -> addWidget(btn_unlimited, 7, 0, 1, 2);

	QButtonGroup* btns = new QButtonGroup(alignment_window);
	btns -> addButton(btn_interval);
	btns -> addButton(btn_alignment);
	btns -> addButton(btn_unlimited);
	connect(btn_interval , SIGNAL(toggled(bool)), this, SLOT(set_alignment_mode(bool)));
	connect(btn_alignment, SIGNAL(toggled(bool)), this, SLOT(set_alignment_mode(bool)));
	connect(btn_unlimited, SIGNAL(toggled(bool)), this, SLOT(set_alignment_mode(bool)));

	QHBoxLayout* hbox1 = new QHBoxLayout();
	QStringList str2 = {"The lowermost interval is not smaller than", "最下方音程不小于"};
	label_orig[0] = new QLabel(str2[language], alignment_window);
	hbox1 -> addWidget(label_orig[0]);

	edit_i_low = new QLineEdit(alignment_window);
	edit_i_low -> setFixedWidth(60 * scale);
	hbox1 -> addWidget(edit_i_low, 0, Qt::AlignLeft);
	connect(edit_i_low, &QLineEdit::editingFinished, this, &Interface::set_i_low);

	QStringList str3 = {"semitones;", "个半音，"};
	label_orig[1] = new QLabel(str3[language], alignment_window);
	hbox1 -> addWidget(label_orig[1], 1);
	hbox1 -> setSpacing(10);
	grid -> addLayout(hbox1, 0, 1, Qt::AlignLeft);

	QHBoxLayout* hbox2 = new QHBoxLayout();
	QStringList str4 = {"the uppermost interval is not greater than", "最上方音程不大于"};
	label_orig[2] = new QLabel(str4[language], alignment_window);
	hbox2 -> addWidget(label_orig[2]);

	edit_i_high = new QLineEdit(alignment_window);
	edit_i_high -> setFixedWidth(60 * scale);
	hbox2 -> addWidget(edit_i_high, 0, Qt::AlignLeft);
	connect(edit_i_high, &QLineEdit::editingFinished, this, &Interface::set_i_high);

	QStringList str5 = {"semitones;", "个半音，"};
	label_orig[3] = new QLabel(str5[language], alignment_window);
	hbox2 -> addWidget(label_orig[3], 1);
	hbox2 -> setSpacing(10);
	grid -> addLayout(hbox2, 1, 1, Qt::AlignLeft);

	QHBoxLayout* hbox3 = new QHBoxLayout();
	QStringList str6 = {"the range of other intervals is", "其余音程大小范围为"};
	label_orig[4] = new QLabel(str6[language], alignment_window);
	hbox3 -> addWidget(label_orig[4]);

	edit_i_min = new QLineEdit(alignment_window);
	edit_i_min -> setFixedWidth(60 * scale);
	hbox3 -> addWidget(edit_i_min);
	connect(edit_i_min, &QLineEdit::editingFinished, this, &Interface::set_i_min);

	QStringList str7 = {"to", "~"};
	label_orig[5] = new QLabel(str7[language], alignment_window);
	hbox3 -> addWidget(label_orig[5]);

	edit_i_max = new QLineEdit(alignment_window);
	edit_i_max -> setFixedWidth(60 * scale);
	hbox3 -> addWidget(edit_i_max, 0, Qt::AlignLeft);
	connect(edit_i_max, &QLineEdit::editingFinished, this, &Interface::set_i_max);

	QStringList str8 = {"semitones.", "个半音。"};
	label_orig[6] = new QLabel(str8[language], alignment_window);
	hbox3 -> addWidget(label_orig[6], 1);
	hbox3 -> setSpacing(10);
	grid -> addLayout(hbox3, 2, 1, Qt::AlignLeft);

	QHBoxLayout* hbox4 = new QHBoxLayout();
	QStringList str9 = {"Alignment database: ", "排列类型库："};
	label_custom[0] = new QLabel(str9[language], alignment_window);
	hbox4 -> addWidget(label_custom[0]);

	QStringList str10 = {"Import alignment database file (.db)...", "载入排列类型库文件(.db)…"};
	btn_align_db = new QPushButton(str10[language], alignment_window);
	btn_align_db -> setMinimumHeight(25 * _scale);
	hbox4 -> addWidget(btn_align_db, Qt::AlignLeft);
	hbox4 -> setSpacing(10);
	grid -> addLayout(hbox4, 4, 1, Qt::AlignLeft);
	connect(btn_align_db, &QPushButton::clicked, this, &Interface::import_alignment);

	QHBoxLayout* hbox5 = new QHBoxLayout();
	QStringList str11 = {"Current database: ", "当前类型库："};
	label_custom[1] = new QLabel(str11[language], alignment_window);
	hbox5 -> addWidget(label_custom[1]);

	label_custom[2] = new QLabel(alignment_window);
	hbox5 -> addWidget(label_custom[2]);
	hbox5 -> setSpacing(10);
	grid -> addLayout(hbox5, 5, 1, Qt::AlignLeft);

	QHBoxLayout* hbox6 = new QHBoxLayout();
	QStringList str12 = {"Name of current database: ", "当前类型库标题："};
	label_custom[3] = new QLabel(str12[language], alignment_window);
	hbox6 -> addWidget(label_custom[3]);

	label_custom[4] = new QLabel(alignment_window);
	label_custom[4] -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	label_custom[4] -> setStyleSheet("color: #00A2E8;");
	hbox6 -> addWidget(label_custom[4]);

	QStringList str13 = {", including", "，含"};
	label_custom[5] = new QLabel(str13[language], alignment_window);
	hbox6 -> addWidget(label_custom[5]);

	label_custom[6] = new QLabel(alignment_window);
	label_custom[6] -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
	label_custom[6] -> setStyleSheet("color: #00A2E8;");
	hbox6 -> addWidget(label_custom[6]);

	QStringList str14 = {"type(s) of alignment.", "种排列类型。"};
	label_custom[7] = new QLabel(str14[language], alignment_window);
	hbox6 -> addWidget(label_custom[7]);
	hbox6 -> setSpacing(10);
	grid -> addLayout(hbox6, 6, 1, Qt::AlignLeft);

	QStringList str16 = {"OK", "确定"};
	QPushButton* btn = new QPushButton(str16[language], alignment_window);
	grid -> addWidget(btn, 8, 1, Qt::AlignRight | Qt::AlignBottom);
	connect(btn, &QPushButton::clicked, this, &Interface::closeAlignment);

	initAlignment();
	alignment_window -> show();
}

void Interface::initAlignment()
{
	QString str;
	edit_i_low  -> setText(str.setNum(i_low));
	edit_i_high -> setText(str.setNum(i_high));
	edit_i_min  -> setText(str.setNum(i_min));
	edit_i_max  -> setText(str.setNum(i_max));
	label_custom[2] -> setText(align_db_filename);
	label_custom[4] -> setText(align_db[language]);
	label_custom[6] -> setText(str.setNum(align_db_size));
	switch(align_mode)
	{
		case Interval:  btn_interval  -> setChecked(true);  break;
		case List:      btn_alignment -> setChecked(true);  break;
		case Unlimited: btn_unlimited -> setChecked(true);  break;
	}
	set_alignment_mode(0);
}

void Interface::set_alignment_mode(bool state)
{
	Q_UNUSED(state);
	if(btn_interval -> isChecked())
	{
		align_mode = Interval;
		for(int i = 0; i < 8; ++i)
			label_custom[i] -> setEnabled(false);
		btn_align_db -> setEnabled(false);
		for(int i = 0; i < 7; ++i)
			label_orig[i] -> setDisabled(false);
		edit_i_low  -> setDisabled(false);
		edit_i_high -> setDisabled(false);
		edit_i_min  -> setDisabled(false);
		edit_i_max  -> setDisabled(false);
	}
	else if(btn_alignment -> isChecked())
	{
		align_mode = List;
		for(int i = 0; i < 8; ++i)
			label_custom[i] -> setEnabled(true);
		btn_align_db -> setEnabled(true);
		for(int i = 0; i < 7; ++i)
			label_orig[i] -> setDisabled(true);
		edit_i_low  -> setDisabled(true);
		edit_i_high -> setDisabled(true);
		edit_i_min  -> setDisabled(true);
		edit_i_max  -> setDisabled(true);
	}
	else
	{
		align_mode = Unlimited;
		for(int i = 0; i < 8; ++i)
			label_custom[i] -> setDisabled(true);
		btn_align_db -> setDisabled(true);
		for(int i = 0; i < 7; ++i)
			label_orig[i] -> setDisabled(true);
		edit_i_low  -> setDisabled(true);
		edit_i_high -> setDisabled(true);
		edit_i_min  -> setDisabled(true);
		edit_i_max  -> setDisabled(true);
	}
}

void Interface::set_i_low()
{
	QString text = edit_i_low -> text();
	i_low = text.toInt();
	if(i_low <= 1)
	{
		i_low = 1;
		edit_i_low -> setText("1");
	}
}

void Interface::set_i_high()
{
	QString text = edit_i_high -> text();
	i_high = text.toInt();
	if(i_high <= 1)
	{
		i_high = 1;
		edit_i_high -> setText("1");
	}
}

void Interface::set_i_min()
{
	QString text = edit_i_min -> text();
	i_min = text.toInt();
	if(i_min > i_max)
	{
		i_min = i_max;
		edit_i_min -> setText(edit_i_max -> text());
	}
	else if(i_min <= 1)
	{
		i_min = 1;
		edit_i_min -> setText("1");
	}
}

void Interface::set_i_max()
{
	QString text = edit_i_max -> text();
	i_max = text.toInt();
	if(i_max < i_min)
	{
		i_max = i_min;
		edit_i_max -> setText(edit_i_min -> text());
	}
	else if(i_max > 127)
	{
		i_max = 127;
		edit_i_max -> setText("127");
	}
}

void Interface::import_alignment()
{
	QFileDialog* dialog = new QFileDialog(this);
	QStringList str1 = {"Choose alignment database file", "选择排列类型库文件"};
	dialog -> setWindowTitle(str1[language]);
	dialog -> setFileMode(QFileDialog::ExistingFile);
	QStringList str2 = {"Database File (*.db)", "库文件 (*.db)"};
	dialog -> setNameFilter(str2[language]);
	dialog -> setDirectory(root_path + "/db/align");
	if(dialog -> exec())
	{
		alignment_list.clear();
		QStringList temp = dialog -> selectedFiles();
		QFileInfo fileinfo(temp[0]);
		strcpy(align_db_filename, fileinfo.fileName().toLocal8Bit().data());
		ifstream fin(fileinfo.absoluteFilePath().toLocal8Bit().data());
		char str[100];
		fin.getline(str, 100, '\n');
		fin.getline(str, 100, '\n');
		fin.getline(str, 100, '\n');
		fin >> str >> str;
		fin.get();
		fin.getline(str, 100, '\n');
		strcpy(align_db[English], str);
		fin >> str >> str;
		fin.get();
		fin.getline(str, 100, '\n');
		strcpy(align_db[Chinese], str);
		align_db_size = 0;
		while(!fin.eof())
		{
			fin.getline(str, 100, '\n');
			if(strlen(str) > 1)  ++align_db_size;
		}
		fin.close();
		QString str3;
		label_custom[2] -> setText(align_db_filename);
		label_custom[4] -> setText(align_db[language]);
		label_custom[6] -> setText(str3.setNum(align_db_size));
	}
}

void Interface::closeAlignment()
{
	have_set_alignment = true;
	alignment_window -> close();
	QStringList str = {"(Set)", "（已设置）"};
	label_alignment -> setText(str[language]);
	label_alignment -> setStyleSheet("color: #0084FF;");
	label_alignment -> setFont(font);
}

// SmartChordGen v3.0 [Build: 2020.11.27]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// maingui.cpp

#include "Interface.h"

Interface::Interface(QWidget* parent): QWidget(parent)
{
#if __WIN32
	HWND wnd = ::GetDesktopWindow();
	HDC dc = ::GetDC(wnd);
	double PIXX = GetDeviceCaps(dc, LOGPIXELSX);
	scale = PIXX / 96;
	if(scale != 1)
		_scale = scale * 0.93;
	else _scale = 1;
#else
	scale = 1.0;
	_scale = 1.0;
#endif

	root_path = QDir::currentPath();
	root_path = root_path.mid(0, root_path.lastIndexOf('/'));
	font = QFont("Microsoft YaHei", 9, QFont::Normal);
	setFont(font);
	vbox = new QVBoxLayout(this);
	cur_preset[Chinese] = "默认设置";
	cur_preset[English] = "default settings";
	cur_preset_filename = "default.preset";
	cur_preset_path = "../presets/default.preset";
	read_preset(cur_preset_path.toLatin1().data());
}

void Interface::mainGui()
{
	setFixedWidth(640 * scale);
	if(language == Chinese)
		setFixedHeight(655 * _scale);
	else  setFixedHeight(680 * _scale);

	const int PARTS = 8;
	QGridLayout** grid = new QGridLayout*[PARTS];
	for(int i = 0; i < PARTS; ++i)
		grid[i] = new QGridLayout();


	{
		grid[0] -> setHorizontalSpacing(10 * scale);
		grid[0] -> setVerticalSpacing(scale);
		grid[0] -> setContentsMargins(10 * scale, 0, 5 * scale, 0);

		QString str1 = "程序：清华大学 沈智云\nProgram by Ji-woon Sim    \n(Tsinghua University)";
		QLabel* label1 = new QLabel(str1, this);
		grid[0] -> addWidget(label1, 0, 0, 3, 1);

		QString str2 = "构想：星海音乐学院 陈文戈\nConcept by Wenge Chen\n(Xinghai Conservatory of Music)  ";
		QLabel* label2 = new QLabel(str2, this);
		grid[0] -> addWidget(label2, 0, 1, 3, 1);

		QPixmap pic1 = QPixmap("icons/icon.png");
		pic1 = pic1.scaled(57 * scale, 57 * _scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QLabel* label3 = new QLabel(this);
		label3 -> setPixmap(pic1);
		grid[0] -> addWidget(label3, 0, 2, 3, 1, Qt::AlignRight);

		QLabel* label4 = new QLabel("© 2020  ver.: 3.0 ", this);
		label4 -> setAlignment(Qt::AlignRight);
		grid[0] -> addWidget(label4, 0, 3, 2, 1, Qt::AlignRight | Qt::AlignVCenter);

		QPixmap pic2 = QPixmap("icons/name.png");
		QLabel* label5 = new QLabel(this);
		label5 -> setPixmap(pic2);
		grid[0] -> addWidget(label5, 2, 3, Qt::AlignRight | Qt::AlignTop);
	}

	{
		grid[1] -> setVerticalSpacing(0);
		grid[1] -> setHorizontalSpacing(5 * scale);
		grid[1] -> setColumnStretch(0, 0);
		grid[1] -> setColumnStretch(1, 1);
		grid[1] -> setColumnMinimumWidth(3, 87 * scale);
		grid[1] -> setContentsMargins(20 * scale, 0, 20 * scale, 0);

		QStringList str1 = {"Current preset file: ", "当前预设文件："};
		label_preset_filename = new QLabel(str1[language], this);
		grid[1] -> addWidget(label_preset_filename, 0, 0, 1, 2, Qt::AlignLeft);

		QStringList str2 = {"Title: ", "标题："};
		QLabel* label = new QLabel(str2[language], this);
		grid[1] -> addWidget(label, 1, 0, Qt::AlignLeft);

		label_preset = new QLabel(this);
		label_preset -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
		label_preset -> setStyleSheet("color: #00A2E8;");
		grid[1] -> addWidget(label_preset, 1, 1, Qt::AlignLeft);

		QStringList str3 = {"Import Preset (.preset) file...", "导入预设(.preset)文件…"};
		QStringList str4 = {"Save / Save as...", "保存/另存为…"};
		QPushButton* btn1 = new QPushButton(str3[language], this);
		QPushButton* btn2 = new QPushButton(str4[language], this);
		btn1 -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
		if(language == English)
		{
			btn1 -> setFixedWidth(190 * scale);
			btn2 -> setFixedWidth(190 * scale);
		}
		else
		{
			btn1 -> setFixedWidth(150 * scale);
			btn2 -> setFixedWidth(150 * scale);
		}
		grid[1] -> addWidget(btn1, 0, 2, Qt::AlignRight);
		grid[1] -> addWidget(btn2, 1, 2, Qt::AlignRight);
		connect(btn1, &QPushButton::clicked, this, &Interface::import_preset);
		connect(btn2, &QPushButton::clicked, this, &Interface::saveSettingsGui);

		QPushButton* btn3 = new QPushButton("English");
		QPushButton* btn4 = new QPushButton("User Guide");
		QPushButton* btn5 = new QPushButton("中文");
		QPushButton* btn6 = new QPushButton("用户手册");
        btn3 -> setFixedWidth(75 * scale);
        btn4 -> setFixedWidth(75 * scale);
        btn5 -> setFixedWidth(75 * scale);
        btn6 -> setFixedWidth(75 * scale);
		grid[1] -> addWidget(btn3, 0, 3, Qt::AlignRight);
		grid[1] -> addWidget(btn4, 1, 3, Qt::AlignRight);
		grid[1] -> addWidget(btn5, 0, 4);
		grid[1] -> addWidget(btn6, 1, 4);

		connect(btn3, SIGNAL(clicked()), this, SLOT(set_to_English()));
		connect(btn5, SIGNAL(clicked()), this, SLOT(set_to_Chinese()));
		connect(btn4, SIGNAL(clicked()), this, SLOT(open_manual_English()));
		connect(btn6, SIGNAL(clicked()), this, SLOT(open_manual_Chinese()));
	}

	{
		grid[2] -> setVerticalSpacing(5 * scale);
		QStringList str1 = {"Output filename:\n(omit extension)", "输出文件名：\n（省略扩展名）"};
		QLabel* label1 = new QLabel(str1[language], this);
		label1 -> setAlignment(Qt::AlignRight);
		grid[2] -> addWidget(label1, 0, 0, Qt::AlignRight);

		edit_output_name = new QLineEdit(this);
		grid[2] -> addWidget(edit_output_name, 0, 1, 1, 3);
		connect(edit_output_name, &QLineEdit::textChanged, this, &Interface::set_output_name);

		QStringList str2 = {"Generating mode:", "生成模式："};
		QLabel* label2 = new QLabel(str2[language], this);
		grid[2] -> addWidget(label2, 1, 0, Qt::AlignRight);

		QStringList str3 = {"Continual", "连续进行"};
		QStringList str4 = {"Alternatives for a single progression", "单个进行的不同可能性"};
		btn_continual = new QRadioButton(str3[language], this);
		btn_single    = new QRadioButton(str4[language], this);
		connect(btn_continual, SIGNAL(toggled(bool)), this, SLOT(set_continual(bool)));
		grid[2] -> addWidget(btn_continual, 1, 1);
		grid[2] -> addWidget(btn_single   , 1, 2, 1, 2);

		QButtonGroup* btns1 = new QButtonGroup(this);
		btns1 -> addButton(btn_continual);
		btns1 -> addButton(btn_single);

		QStringList str5 = {"Output Format:", "输出格式："};
		QLabel* label3 = new QLabel(str5[language], this);
		grid[2] -> addWidget(label3, 2, 0, Qt::AlignRight);

		QStringList str6 = {"Default", "默认"};
		QStringList str7 = {"MIDI only", "仅MIDI"};
		QStringList str8 = {"Report only (.txt)", "仅分析报告(.txt)"};
		btn_def  = new QRadioButton(str6[language], this);
		btn_midi = new QRadioButton(str7[language], this);
		btn_text = new QRadioButton(str8[language], this);
		grid[2] -> addWidget(btn_def , 2, 1);
		grid[2] -> addWidget(btn_midi, 2, 2);
		grid[2] -> addWidget(btn_text, 2, 3);
		QButtonGroup* btns2 = new QButtonGroup(this);
		btns2 -> addButton(btn_def);
		btns2 -> addButton(btn_midi);
		btns2 -> addButton(btn_text);
		connect(btn_def, SIGNAL(toggled(bool)), this, SLOT(set_output_format(bool)));
		connect(btn_midi, SIGNAL(toggled(bool)), this, SLOT(set_output_format(bool)));
		connect(btn_text, SIGNAL(toggled(bool)), this, SLOT(set_output_format(bool)));

		QPixmap pic = QPixmap("icons/go.png");
		QPushButton* btn = new QPushButton(this);
		btn -> setIcon(pic);
		btn -> setFixedSize(QSize(86 * scale, 56 * scale));
		btn -> setIconSize (QSize(80 * scale, 50 * scale));
		grid[2] -> addWidget(btn, 0, 4, 3, 1, Qt::AlignCenter);
		connect(btn, &QPushButton::clicked, this, &Interface::run);
	}

	{
		grid[3] -> setContentsMargins(20 * scale, 5 * scale, 20 * scale, 0);
		if(language == Chinese)
			grid[3] -> setHorizontalSpacing(10 * scale);
		else  grid[3] -> setHorizontalSpacing(5 * scale);
		grid[3] -> setVerticalSpacing(5 * scale);

		QStringList str1 = {"Number of progressions: ", "和弦进行数量："};
		label_loop_count = new QLabel(str1[language], this);
		if(language == Chinese)
			grid[3] -> addWidget(label_loop_count, 0, 0, Qt::AlignRight);
		else  grid[3] -> addWidget(label_loop_count, 0, 0, 1, 3, Qt::AlignLeft);

		edit_loop_count = new QLineEdit(this);
		if(language == Chinese)
			edit_loop_count -> setFixedWidth(60 * scale);
		else  edit_loop_count -> setFixedWidth(40 * scale);
		if(language == Chinese)
			grid[3] -> addWidget(edit_loop_count, 0, 1, 1, 3, Qt::AlignLeft);
		else  grid[3] -> addWidget(edit_loop_count, 0, 3, Qt::AlignLeft);
		connect(edit_loop_count, &QLineEdit::editingFinished, this, &Interface::set_loop_count);

		QStringList str2 = {"Range of notes: ", "音域范围："};
		QLabel* label1 = new QLabel(str2[language], this);
		if(language == Chinese)
			grid[3] -> addWidget(label1, 1, 0, Qt::AlignRight);
		else  grid[3] -> addWidget(label1, 1, 0, Qt::AlignLeft);

		edit_note_min = new QLineEdit(this);
		if(language == Chinese)
			edit_note_min -> setFixedWidth(60 * scale);
		else  edit_note_min -> setFixedWidth(40 * scale);
		grid[3] -> addWidget(edit_note_min, 1, 1, Qt::AlignLeft);
		connect(edit_note_min, &QLineEdit::editingFinished, this, &Interface::set_note_min);

		QStringList str3 = {"to", "~"};
		QLabel* label2 = new QLabel(str3[language], this);
		grid[3] -> addWidget(label2, 1, 2);

		edit_note_max = new QLineEdit(this);
		if(language == Chinese)
			edit_note_max -> setFixedWidth(60 * scale);
		else  edit_note_max -> setFixedWidth(40 * scale);
		grid[3] -> addWidget(edit_note_max, 1, 3, Qt::AlignLeft);
		grid[3] -> setColumnStretch(3, 1);
		connect(edit_note_max, &QLineEdit::editingFinished, this, &Interface::set_note_max);

		QStringList str4 = {"(M) Number of parts: ", "(M) 和弦声部数量："};
		QStringList str5 = {"(N) Number of notes: ", "(N) 和弦音集音数："};
		QLabel* label3 = new QLabel(str4[language], this);
		QLabel* label4 = new QLabel(str5[language], this);
		grid[3] -> addWidget(label3, 0, 4, Qt::AlignRight);
		grid[3] -> addWidget(label4, 1, 4, Qt::AlignRight);
		grid[3] -> setColumnStretch(4, 0);

		edit_m_min = new QLineEdit(this);
		edit_n_min = new QLineEdit(this);
		edit_m_min -> setFixedWidth(40 * scale);
		edit_n_min -> setFixedWidth(40 * scale);
		connect(edit_m_min, &QLineEdit::textEdited, this, &Interface::sync_minmax);
		connect(edit_m_min, &QLineEdit::textEdited, this, &Interface::sync_nm);
		connect(edit_m_min, &QLineEdit::editingFinished, this, &Interface::set_m_min);
		connect(edit_n_min, &QLineEdit::editingFinished, this, &Interface::set_n_min);
		grid[3] -> addWidget(edit_m_min, 0, 5);
		grid[3] -> addWidget(edit_n_min, 1, 5);

		QLabel* label5 = new QLabel(str3[language], this);
		QLabel* label6 = new QLabel(str3[language], this);
		grid[3] -> addWidget(label5, 0, 6);
		grid[3] -> addWidget(label6, 1, 6);

		edit_m_max = new QLineEdit(this);
		edit_n_max = new QLineEdit(this);
		edit_m_max -> setFixedWidth(40 * scale);
		edit_n_max -> setFixedWidth(40 * scale);
		connect(edit_m_max, &QLineEdit::textEdited, this, &Interface::sync_nm);
		connect(edit_m_max, &QLineEdit::editingFinished, this, &Interface::set_m_max);
		connect(edit_n_max, &QLineEdit::editingFinished, this, &Interface::set_n_max);
		grid[3] -> addWidget(edit_m_max, 0, 7);
		grid[3] -> addWidget(edit_n_max, 1, 7);

		QStringList str6 = {"Unchanged", "不变"};
		QStringList str7 = {"Ditto", "同上"};
		cb_m_unchanged = new QCheckBox(str6[language], this);
		cb_nm_same = new QCheckBox(str7[language], this);
		grid[3] -> addWidget(cb_m_unchanged, 0, 8);
		grid[3] -> addWidget(cb_nm_same, 1, 8);
		connect(cb_m_unchanged, &QCheckBox::stateChanged, this, &Interface::set_m_unchanged);
		connect(cb_nm_same, &QCheckBox::stateChanged, this, &Interface::set_nm_same);
	}

	{
		QHBoxLayout* hbox1 = new QHBoxLayout();
		QHBoxLayout* hbox2 = new QHBoxLayout();
		hbox2 -> setSpacing(5 * scale);

		QStringList str1 = {"Chord database: ", "和弦类型库："};
		QLabel* label1 = new QLabel(str1[language], this);
		hbox1 -> addWidget(label1);

		QStringList str2 = {"Load chord database file(.db) ...", "载入和弦类型库文件(.db)…"};
		QPushButton* btn = new QPushButton(str2[language], this);
		hbox1 -> addWidget(btn, Qt::AlignLeft);
		connect(btn, &QPushButton::clicked, this, &Interface::import_database);

		QLabel* _label1 = new QLabel(" ", this);
		hbox1 -> addWidget(_label1, 1);

		QStringList str3 = {"Current database: ", "当前类型库："};
		label_database_filename = new QLabel(str3[language], this);
		hbox1 -> addWidget(label_database_filename);

		QStringList str4 = {"Title of current database: ", "当前类型库标题："};
		QLabel* label2 = new QLabel(str4[language], this);
		hbox2 -> addWidget(label2);

		label_database = new QLabel(this);
		label_database -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
		label_database -> setStyleSheet("color: #00A2E8;");
		hbox2 -> addWidget(label_database);

		QStringList str5 = {", including", "，含"};
		QLabel* label3 = new QLabel(str5[language], this);
		hbox2 -> addWidget(label3);

		label_database_size = new QLabel(this);
		label_database_size -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
		label_database_size -> setStyleSheet("color: #00A2E8;");
		hbox2 -> addWidget(label_database_size);

		QStringList str6 = {"type(s) of chords.", "种和弦类型。"};
		QLabel* label4 = new QLabel(str6[language], this);
		hbox2 -> addWidget(label4);

		QLabel* _label2 = new QLabel(" ", this);
		hbox2 -> addWidget(_label2, 1);

		grid[4] -> setContentsMargins(20 * scale, 5 * scale, 20 * scale, 0);
		grid[4] -> setVerticalSpacing(5 * scale);
		grid[4] -> addLayout(hbox1, 0, 0);
		grid[4] -> addLayout(hbox2, 1, 0);
	}

	{
		grid[5] -> setContentsMargins(20 * scale, 5 * scale, 20 * scale, 0);
		grid[5] -> setVerticalSpacing(0);
		  grid[5] -> setRowMinimumHeight(1, 25 * scale);

		QPushButton* btn1 = new QPushButton(this);
		btn1 -> setIcon(QIcon("icons/overall-scale.png"));
		btn1 -> setFixedSize(QSize(66 * scale, 66 * scale));
		btn1 -> setIconSize(QSize(60 * scale, 60 * scale));
		grid[5] -> addWidget(btn1, 0, 0, Qt::AlignCenter);
		connect(btn1, &QPushButton::clicked, this, &Interface::overallScaleGui);

		QStringList str1 = {"Overall scale", "整体音阶"};
		QLabel* label1 = new QLabel(str1[language], this);
		grid[5] -> addWidget(label1, 1, 0, Qt::AlignHCenter | Qt::AlignBottom);
		label_overall_scale = new QLabel(this);
		grid[5] -> addWidget(label_overall_scale, 2, 0, Qt::AlignCenter);

		QPushButton* btn2 = new QPushButton(this);
		btn2 -> setIcon(QIcon("icons/omission.png"));
		btn2 -> setFixedSize(QSize(66 * scale, 66 * scale));
		btn2 -> setIconSize(QSize(60 * scale, 60 * scale));
		grid[5] -> addWidget(btn2, 0, 1, Qt::AlignCenter);
		connect(btn2, &QPushButton::clicked, this, &Interface::omissionGui);

		QStringList str2 = {"Omission", "省略音"};
		QLabel* label2 = new QLabel(str2[language], this);
		grid[5] -> addWidget(label2, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);
		label_omission = new QLabel(this);
		grid[5] -> addWidget(label_omission, 2, 1, Qt::AlignCenter);

		QPushButton* btn3 = new QPushButton(this);
		btn3 -> setIcon(QIcon("icons/inversion.png"));
		btn3 -> setFixedSize(QSize(66 * scale, 66 * scale));
		btn3 -> setIconSize(QSize(60 * scale, 60 * scale));
		grid[5] -> addWidget(btn3, 0, 2, Qt::AlignCenter);
		connect(btn3, &QPushButton::clicked, this, &Interface::inversionGui);

		QStringList str3 = {"Inversion", "和弦转位"};
		QLabel* label3 = new QLabel(str3[language], this);
		grid[5] -> addWidget(label3, 1, 2, Qt::AlignHCenter | Qt::AlignBottom);
		label_inversion = new QLabel(this);
		grid[5] -> addWidget(label_inversion, 2, 2, Qt::AlignCenter);

		QPushButton* btn4 = new QPushButton(this);
		btn4 -> setIcon(QIcon("icons/alignment.png"));
		btn4 -> setFixedSize(QSize(66 * scale, 66 * scale));
		btn4 -> setIconSize(QSize(60 * scale, 60 * scale));
		grid[5] -> addWidget(btn4, 0, 3, Qt::AlignCenter);
		connect(btn4, &QPushButton::clicked, this, &Interface::alignmentGui);

		QStringList str4 = {"Alignment", "排列方式"};
		QLabel* label4 = new QLabel(str4[language], this);
		grid[5] -> addWidget(label4, 1, 3, Qt::AlignHCenter | Qt::AlignBottom);
		label_alignment = new QLabel(this);
		grid[5] -> addWidget(label_alignment, 2, 3, Qt::AlignCenter);

		QPushButton* btn5 = new QPushButton(this);
		btn5 -> setIcon(QIcon("icons/more-param.png"));
		btn5 -> setFixedSize(QSize(66 * scale, 66 * scale));
		btn5 -> setIconSize (QSize(60 * scale, 60 * scale));
		grid[5] -> addWidget(btn5, 0, 4, Qt::AlignCenter);
		connect(btn5, &QPushButton::clicked, this, &Interface::moreParamGui);

		QStringList str5 = {"Set parameters", "指标筛选"};
		QLabel* label5 = new QLabel(str5[language], this);
		grid[5] -> addWidget(label5, 1, 4, Qt::AlignHCenter | Qt::AlignBottom);
		label_more_param = new QLabel(this);
		grid[5] -> addWidget(label_more_param, 2, 4, Qt::AlignCenter);

		btn_pedal = new QPushButton(this);
		btn_pedal -> setIcon(QIcon("icons/pedal-notes.png"));
		btn_pedal -> setFixedSize(QSize(66 * scale, 66 * scale));
		btn_pedal -> setIconSize (QSize(60 * scale, 60 * scale));
		grid[5] -> addWidget(btn_pedal, 0, 5, Qt::AlignCenter);
		connect(btn_pedal, &QPushButton::clicked, this, &Interface::pedalNotesGui);

		QStringList str6 = {"Pedal notes", "持续音"};
		QLabel* label6 = new QLabel(str6[language], this);
		grid[5] -> addWidget(label6, 1, 5, Qt::AlignHCenter | Qt::AlignBottom);
		label_pedal_notes = new QLabel(this);
		grid[5] -> addWidget(label_pedal_notes, 2, 5, Qt::AlignCenter);

		QPushButton* btn7 = new QPushButton(this);
		btn7 -> setIcon(QIcon("icons/more-rules.png"));
		btn7 -> setFixedSize(QSize(66 * scale, 66 * scale));
		btn7 -> setIconSize (QSize(60 * scale, 60 * scale));
		grid[5] -> addWidget(btn7, 0, 6, Qt::AlignCenter);
		connect(btn7, &QPushButton::clicked, this, &Interface::moreRulesGui);

		QStringList str7 = {"More rules", "更多规则"};
		QLabel* label7 = new QLabel(str7[language], this);
		grid[5] -> addWidget(label7, 1, 6, Qt::AlignHCenter | Qt::AlignBottom);
		label_more_rules = new QLabel(this);
		grid[5] -> addWidget(label_more_rules, 2, 6, Qt::AlignCenter);
	}

	{
		grid[6] = new QGridLayout();
		grid[6] -> setVerticalSpacing(5);
		if(language == Chinese)
			grid[6] -> setContentsMargins(20, 0, 20 * scale, 0);
		else  grid[6] -> setContentsMargins(0, 0, 15, 0);
		grid[6] -> setColumnStretch(0, 0);
		grid[6] -> setColumnStretch(3, 1);

		QStringList str1 = {"Initial chord: ", "起始和弦："};
		QLabel* label1 = new QLabel(str1[language], this);
		grid[6] -> addWidget(label1, 0, 0, Qt::AlignRight);

		QStringList str2 = {"Output handling: ", "输出处理："};
		QLabel* label2 = new QLabel(str2[language], this);
		grid[6] -> addWidget(label2, 1, 0, Qt::AlignRight);

		QStringList str3 = {"Remove: ", "去除："};
		QLabel* label3 = new QLabel(str3[language], this);
		grid[6] -> addWidget(label3, 2, 0, Qt::AlignRight);

		QStringList str4 = {"Automatic", "自动"};
		QStringList str5 = {"Input manually: ", "手动输入："};
		btn_automatic = new QRadioButton(str4[language], this);
		btn_manual    = new QRadioButton(str5[language], this);
		grid[6] -> addWidget(btn_automatic, 0, 1);
		grid[6] -> addWidget(btn_manual   , 0, 2);

		QButtonGroup* btns = new QButtonGroup(this);
		btns -> addButton(btn_automatic);
		btns -> addButton(btn_manual);
		connect(btn_manual, SIGNAL(toggled(bool)), this, SLOT(set_manual(bool)));

		edit_initial = new QLineEdit(this);
		edit_initial -> setMinimumWidth(200);
		edit_initial -> setDisabled(true);
		grid[6] -> addWidget(edit_initial, 0, 3, Qt::AlignLeft);
		connect(edit_initial, &QLineEdit::editingFinished, this, &Interface::input_initial);

		QStringList str6 = {"Connect pedal notes", "连结持续音时值"};
		cb_connect_pedal = new QCheckBox(str6[language], this);
		cb_connect_pedal -> setDisabled(true);
		grid[6] -> addWidget(cb_connect_pedal, 1, 1);
		connect(cb_connect_pedal, &QCheckBox::stateChanged, this, &Interface::set_connect_pedal);

		QStringList str7 = {"Interlace with initial chord in MIDI file in single mode", "单个模式MIDI文件中交联初始和弦"};
		cb_interlace = new QCheckBox(str7[language], this);
		cb_interlace -> setDisabled(true);
		grid[6] -> addWidget(cb_interlace, 1, 2, 1, 2);
		connect(cb_interlace, &QCheckBox::stateChanged, this, &Interface::set_interlace);

		QStringList str8 = {"Duplicate chords", "重复的和弦"};
		cb_remove_dup = new QCheckBox(str8[language], this);
		grid[6] -> addWidget(cb_remove_dup, 2, 1);
		connect(cb_remove_dup, &QCheckBox::stateChanged, this, &Interface::set_remove_dup);

		QStringList str9 = {"Duplicate types of chords", "重复类型的和弦  （程序将自动补足进行数量。）"};
		cb_remove_dup_type = new QCheckBox(str9[language], this);
		grid[6] -> addWidget(cb_remove_dup_type, 2, 2, 1, 2);
		connect(cb_remove_dup_type, &QCheckBox::stateChanged, this, &Interface::set_remove_dup_type);

		if(language == English)
		{
			QLabel* label4 = new QLabel("(The program will replenish progressions automatically.)", this);
			grid[6] -> addWidget(label4, 3, 2, 1, 2, Qt::AlignLeft);
		}
	}

	{
		grid[7] = new QGridLayout();
		grid[7] -> setContentsMargins(20, 0, 20, 0);

		QStringList str1 = {"Open utilities folder...", "打开辅助工具(utilities)文件夹…"};
		QPushButton* btn = new QPushButton(str1[language], this);
		grid[7] -> addWidget(btn, 0, 0);
		connect(btn, &QPushButton::clicked, this, &Interface::open_utilities);

		QStringList str2 = {"Feedback email: rcswex@163.com\nFeedback QQ: 925792714",
								  "问题反馈邮箱：rcswex@163.com\n问题反馈QQ：925792714"};
		QLabel* label = new QLabel(str2[language], this);
		label -> setAlignment(Qt::AlignRight);
		label -> setFont(QFont("Microsoft YaHei", 11, QFont::Bold));
		label -> setStyleSheet("color: #3F48CC;");
		grid[7] -> addWidget(label, 0, 1, Qt::AlignRight);
	}

	vbox -> setSpacing(15 * scale);
	for(int i = 0; i < PARTS; ++i)
		vbox -> addLayout(grid[i]);
	initMain();
}

void Interface::initMain()
{
	label_preset_filename -> setText(label_preset_filename -> text() + cur_preset_filename);
	label_preset -> setText(cur_preset[language]);

	edit_output_name -> setText(output_name);
	if(continual)
	{
		btn_continual -> setChecked(true);
		// set_continual(true) is called
	}
	else
	{
		set_continual(false);
		btn_single -> setChecked(true);
	}
	switch(output_mode)
	{
		case Both:     btn_def  -> setChecked(true);  break;
		case MidiOnly: btn_midi -> setChecked(true);  break;
		case TextOnly: btn_text -> setChecked(true);  break;
	}

	QString str;
	if(continual)
		edit_loop_count -> setText(str.setNum(loop_count));
	else  edit_loop_count -> setText("/");
	edit_note_min -> setText(str.setNum(lowest));
	edit_note_max -> setText(str.setNum(highest));
	edit_m_min -> setText(str.setNum(m_min));
	edit_m_max -> setText(str.setNum(m_max));
	edit_n_min -> setText(str.setNum(n_min));
	edit_n_max -> setText(str.setNum(n_max));
	cb_m_unchanged -> setChecked(m_unchanged);
	cb_nm_same -> setChecked(nm_same);

	label_database_filename -> setText(label_database_filename -> text() + database_filename);
	label_database -> setText(database[language]);
	label_database_size -> setText(str.setNum(database_size));

	QStringList str1 = {"(Set)", "（已设置）"};
	QStringList str2 = {"(Not set)", "（未设置）"};
	QStringList str3 = {"(Default)", "（默认）"};
	if(have_set_overall_scale)
	{
		label_overall_scale -> setText(str1[language]);
		label_overall_scale -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_overall_scale -> setText(str3[language]);
		label_overall_scale -> setStyleSheet("color: #606060;");
	}
	if(have_set_omission)
	{
		label_omission -> setText(str1[language]);
		label_omission -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_omission -> setText(str3[language]);
		label_omission -> setStyleSheet("color: #606060;");
	}
	if(have_set_inversion)
	{
		label_inversion -> setText(str1[language]);
		label_inversion -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_inversion -> setText(str3[language]);
		label_inversion -> setStyleSheet("color: #606060;");
	}
	if(have_set_alignment)
	{
		label_alignment -> setText(str1[language]);
		label_alignment -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_alignment -> setText(str3[language]);
		label_alignment -> setStyleSheet("color: #606060;");
	}
	if(sort_order[0] != '\0')
	{
		label_more_param -> setText(str1[language]);
		label_more_param -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_more_param -> setText(str2[language]);
		label_more_param -> setStyleSheet("color: #606060;");
	}
	if(enable_pedal)
	{
		label_pedal_notes -> setText(str1[language]);
		label_pedal_notes -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_pedal_notes -> setText(str2[language]);
		label_pedal_notes -> setStyleSheet("color: #606060;");
	}
	have_set_more_rules = (vl_setting != Default || custom_vl_range ||
								  enable_ex || enable_rm || enable_sim);
	if(have_set_more_rules)
	{
		label_more_rules -> setText(str1[language]);
		label_more_rules -> setStyleSheet("color: #0084FF;");
	}
	else
	{
		label_more_rules -> setText(str2[language]);
		label_more_rules -> setStyleSheet("color: #606060;");
	}

	if(automatic)
	{
		btn_automatic -> setChecked(true);
		set_manual(false);
	}
	else  btn_manual -> setChecked(true);
	edit_initial -> setText(str_notes);
	set_initial();
	cb_connect_pedal -> setChecked(connect_pedal);
	cb_interlace -> setChecked(interlace);
	switch(unique_mode)
	{
		case RemoveDupType: cb_remove_dup_type -> setChecked(true);  Q_FALLTHROUGH();
		case RemoveDup:	  cb_remove_dup -> setChecked(true);
		case Disabled: ;
	}
}

void Interface::doPainting()
{
	QPainter painter(this);

	painter.setPen(QColor("#99D9EA"));
	painter.setBrush(QBrush("#99D9EA"));
	painter.drawRect(0, 0, 640 * scale, 75 * _scale);

	painter.setPen(QColor("#EFE4B0"));
	painter.setBrush(QBrush("#EFE4B0"));
	painter.drawRect(0, 75 * _scale, 640 * scale, 65 * _scale);

	painter.setPen(QColor("#00A2E8"));
	painter.drawLine(445 * scale, 85 * _scale, 445 * scale, 135 * _scale);

	painter.drawLine(20 * scale, 245 * _scale, 620 * scale, 245 * _scale);
	painter.drawLine(20 * scale, 315 * _scale, 620 * scale, 315 * _scale);
	painter.drawLine(20 * scale, 380 * _scale, 620 * scale, 380 * _scale);
    painter.drawLine(20 * scale, 505 * _scale, 620 * scale, 505 * _scale);
	if(language == Chinese)
		painter.drawLine(20 * scale, 600 * _scale, 620 * scale, 600 * _scale);
	else  painter.drawLine(20 * scale, 630 * _scale, 620 * scale, 630 * _scale);
}

void Interface::clear(QLayout* layout)
{
	QLayoutItem* child;
	while(layout -> count() != 0)
	{
		child = layout -> takeAt(0);
		if(child -> layout() != 0)
			clear(child -> layout());
		else if(child -> widget() != 0)
			delete child -> widget();
		delete child;
	}
}

void Interface::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	doPainting();
}

void Interface::closeEvent(QCloseEvent *e)
{
	QStringList str1 = {"Exit", "退出"};
	QStringList str2 = {"Are you sure you would like to exit?", "你确定要退出吗？"};
	QStringList str3 = {"Yes", "是"};
	QStringList str4 = {"No",  "否"};
	QMessageBox* exit = new QMessageBox(QMessageBox::Question, str1[language], str2[language]);
	QPushButton* open = (exit -> addButton(str3[language], QMessageBox::AcceptRole));
	exit -> addButton(str4[language], QMessageBox::RejectRole);
	exit -> exec();
	if(exit -> clickedButton() == open)
		e -> accept();
	else  e -> ignore();
}

void Interface::import_preset()
{
	QFileDialog* dialog = new QFileDialog(this);
	QStringList str1 = {"Choose preset file", "选择预设文件"};
	dialog -> setWindowTitle(str1[language]);
	dialog -> setFileMode(QFileDialog::ExistingFile);
	QStringList str2 = {"Preset File (*.preset)", "预设文件 (*.preset)"};
	dialog -> setNameFilter(str2[language]);
	dialog -> setDirectory(root_path + "/presets");
	if(dialog -> exec())
	{
		QStringList temp = dialog -> selectedFiles();
		cur_preset_path = temp[0];
		QFileInfo fileinfo(cur_preset_path);
		cur_preset_filename = fileinfo.fileName();
		read_preset(cur_preset_path.toLocal8Bit().data());
	}
}

int Interface::read_data(ifstream& fin, char* data)
// Reads the closest string beginning from '=' (not included) to ';' (not included) and saves it to 'data'.
// The data will also be converted to bool or int and the function returns the value.
{
	fin.getline(data, 100, '=');
	fin.get();
	fin.getline(data, 200, ';');
	if(strcmp(data, "true") == 0)
		return 1;
	else if(strcmp(data, "false") == 0)
		return 0;
	return atoi(data);
}

void Interface::read_vec(ifstream& fin, char* str, vector<int>& v)
{
	read_data(fin, str);
	int pos1 = 1, pos2 = 0, len = strlen(str) - 1, num;
	char temp[5];
	v.clear();
	while(pos1 < len)
	{
		pos2 = 0;
		while(pos1 < len && str[pos1] == ' ')
			++pos1;
		if(pos1 == len)  break;
		while(pos1 < len && str[pos1] != ' ')
		{
			temp[pos2] = str[pos1];
			++pos1;  ++pos2;
		}
		temp[pos2] = '\0';
		num = atoi(temp);
		v.push_back(num);
	}
}

void Interface::read_preset(char* filename)
{
	ifstream fin(filename);
	char str[200];

	read_data(fin, str);
	cur_preset[English] = str;
	read_data(fin, str);
	cur_preset[Chinese] = str;
	read_data(fin, str);
	if(strcmp(str, "Chinese") == 0)
		language = Chinese;
	else  language = English;

	read_data(fin, output_name);
	read_data(fin, str);
	continual = (strcmp(str, "continual") == 0);
	read_data(fin, str);
	if(strcmp(str, "Both") == 0)  output_mode = Both;
	else if(strcmp(str, "TextOnly") == 0)  output_mode = TextOnly;
	else if(strcmp(str, "MidiOnly") == 0)  output_mode = MidiOnly;

	if(continual)
		loop_count = read_data(fin, str);
	lowest = read_data(fin, str);
	highest = read_data(fin, str);
	m_min = read_data(fin, str);
	m_max = read_data(fin, str);
	n_min = read_data(fin, str);
	n_max = read_data(fin, str);
	m_unchanged = read_data(fin, str);
	nm_same = read_data(fin, str);

	read_data(fin, database_filename);
	read_data(fin, database[English]);
	read_data(fin, database[Chinese]);
	database_size = read_data(fin, str);

	read_data(fin, str);
	automatic = (strcmp(str, "automatic") == 0);
	if(!automatic)
	{
		read_data(fin, str_notes);
		set_initial();
	}
	connect_pedal = read_data(fin, str);
	interlace = read_data(fin, str);
	read_data(fin, str);
	if(strcmp(str, "RemoveDupType") == 0)   unique_mode = RemoveDupType;
	else if(strcmp(str, "RemoveDup") == 0)  unique_mode = RemoveDup;
	else if(strcmp(str, "Disabled") == 0)   unique_mode = Disabled;

	have_set_overall_scale = read_data(fin, str);
	read_vec(fin, str, overall_scale);

	for(int i = 3; i <= 7; ++i)
		read_vec(fin, str, omission[i]);
	read_vec(fin, str, bass_avail);

	read_data(fin, str);
	if(strcmp(str, "Interval") == 0)   align_mode = Interval;
	else if(strcmp(str, "List") == 0)  align_mode = List;
	else if(strcmp(str, "Unlimited") == 0)   align_mode = Unlimited;
	switch(align_mode)
	{
		case Interval:
		{
			i_low = read_data(fin, str);
			i_high = read_data(fin, str);
			i_min = read_data(fin, str);
			i_max = read_data(fin, str);
			strcpy(align_db[Chinese], "未选择");
			strcpy(align_db[English], "Not selected");
			strcpy(align_db_filename, "N/A");
			align_db_size = 0;
			break;
		}
		case List:
		{
			read_data(fin, align_db_filename);
			read_data(fin, align_db[English]);
			read_data(fin, align_db[Chinese]);
			align_db_size = read_data(fin, str);
			i_low = 3;    i_high = 11;
			i_min = 1;    i_max = 8;
			break;
		}
		case Unlimited:  break;
	}

	enable_pedal = read_data(fin, str);
	in_bass = read_data(fin, str);
	if(in_bass)
	{
		read_vec(fin, str, pedal_notes);
		pedal_notes_set.clear();
	}
	else
	{
		read_vec(fin, str, pedal_notes_set);
		pedal_notes.clear();
	}
	realign = read_data(fin, str);
	period = read_data(fin, str);

	read_data(fin, str);
	if(strcmp(str, "Percentage") == 0)    vl_setting = Percentage;
	else if(strcmp(str, "Number") == 0)   vl_setting = Number;
	else if(strcmp(str, "Default") == 0)  vl_setting = Default;
	enable_steady = read_data(fin, str);
	enable_ascending = read_data(fin, str);
	enable_descending = read_data(fin, str);
	steady_min = read_data(fin, str);
	steady_max = read_data(fin, str);
	ascending_min = read_data(fin, str);
	ascending_max = read_data(fin, str);
	descending_min = read_data(fin, str);
	descending_max = read_data(fin, str);
	custom_vl_range = read_data(fin, str);
	vl_min = read_data(fin, str);
	vl_max = read_data(fin, str);
	enable_rm = read_data(fin, str);
	if(enable_rm)  read_vec(fin, str, rm_priority);
	enable_ex = read_data(fin, str);
	if(enable_ex)
	{
		read_data(fin, exclusion);
		set_ex();
	}
	enable_sim = read_data(fin, str);
	if(enable_sim)
	{
		read_data(fin, str_sim);
		set_sim();
	}

	k_min = read_data(fin, str);  k_max = read_data(fin, str);
	t_min = read_data(fin, str);  t_max = read_data(fin, str);
	c_min = read_data(fin, str);  c_max = read_data(fin, str);
  sv_min = read_data(fin, str); sv_max = read_data(fin, str);
	r_min = read_data(fin, str);  r_max = read_data(fin, str);
	s_min = read_data(fin, str);  s_max = read_data(fin, str);
  ss_min = read_data(fin, str); ss_max = read_data(fin, str);
	h_min = read_data(fin, str);  h_max = read_data(fin, str);
	g_min = read_data(fin, str);  g_max = read_data(fin, str);
	x_min = read_data(fin, str);  x_max = read_data(fin, str);
	q_min = read_data(fin, str);  q_max = read_data(fin, str);
	read_data(fin, sort_order);

	have_set_omission  = false;
	have_set_alignment = false;
	have_set_inversion = false;
	fin.close();
	if(language == Chinese)
		set_to_Chinese();
	else  set_to_English();
}

void Interface::set_to_Chinese()
{
	language = Chinese;
	clear(vbox);
	mainGui();
}

void Interface::set_to_English()
{
	language = English;
	clear(vbox);
	mainGui();
}

void Interface::open_manual_Chinese()
{
	 QDesktopServices::openUrl(QUrl( ((QString)"file:%1/bin/guide/智弦用户手册.pdf").arg(root_path)) );
}

void Interface::open_manual_English()
{
	 QDesktopServices::openUrl(QUrl( ((QString)"file:%1/bin/guide/SmartChordGen_User_Guide.pdf").arg(root_path)) );
}

void Interface::set_output_name()
{
	strcpy(output_name, edit_output_name -> text().toUtf8().data());
}

void Interface::set_continual(bool state)
{
	continual = state;
	btn_pedal -> setEnabled(state);
	cb_connect_pedal -> setEnabled(state);
	cb_remove_dup -> setEnabled(state);

	if(state == true)
	// continual mode is selected
	{
		edit_initial -> clear();
		if(edit_loop_count -> text() == "/")
		{
			edit_loop_count -> setText("10");
			loop_count = 10;
		}
		notes.clear();
		edit_loop_count -> setEnabled(true);
		label_loop_count -> setEnabled(true);
		cb_interlace -> setChecked(false);
		cb_interlace -> setDisabled(true);
	}
	else
	{
		enable_pedal = false;
		QStringList str = {"(Not set)", "（未设置）"};
		label_pedal_notes -> setText(str[language]);
		label_pedal_notes -> setStyleSheet("color: #606060;");
		label_pedal_notes -> setFont(font);
		enable_sim = false;
		edit_loop_count -> setText("/");
		edit_loop_count -> setDisabled(true);
		label_loop_count -> setDisabled(true);
		if(output_mode == TextOnly)
			cb_interlace -> setDisabled(true);
		else
		{
			cb_interlace -> setEnabled(true);
			cb_interlace -> setChecked(true);
			interlace = true;
		}
		cb_remove_dup -> setChecked(true);
		unique_mode = RemoveDup;
	}
}

void Interface::set_output_format(bool state)
{
	Q_UNUSED(state);
	int n = btn_midi -> isChecked() + 2 * btn_text -> isChecked();
	output_mode = (OutputMode)n;
	if(continual || output_mode == TextOnly)
	{
		cb_interlace -> setChecked(false);
		cb_interlace -> setDisabled(true);
	}
	else
		cb_interlace -> setEnabled(true);
}

void Interface::set_loop_count()
{
	loop_count = (edit_loop_count -> text()).toInt();
	if(loop_count <= 0)
	{
		edit_loop_count -> setText("10");
		loop_count = 10;
	}
	else if(loop_count > 1000)
	{
		edit_loop_count -> setText("1000");
		loop_count = 1000;
	}
}

void Interface::set_note_min()
{
	QString text = edit_note_min -> text();
	if(text[0] >= '0' && text[0] <= '9')
		lowest = text.toInt();
	else  lowest = nametonum(text.toLatin1().data());
	if(lowest > highest)
	{
		lowest = highest;
		edit_note_min -> setText(edit_note_max -> text());
	}
	else if(lowest < 0)
	{
		lowest = 0;
		edit_note_min -> setText("0");
	}
}

void Interface::set_note_max()
{
	QString text = edit_note_max -> text();
	if(text[0] >= '0' && text[0] <= '9')
		highest = text.toInt();
	else  highest = nametonum(text.toLatin1().data());
	if(highest < lowest)
	{
		highest = lowest;
		edit_note_max -> setText(edit_note_min -> text());
	}
	else if(highest > 127)
	{
		highest = 127;
		edit_note_max -> setText("127");
	}
}

void Interface::set_m_min()
{
	QString str, text = edit_m_min -> text();
	if(m_unchanged)
	{
		m_min = text.toInt();
		if(m_min > 15)  m_min = 15;
		else if(m_min < 1)  m_min = 1;
		m_max = m_min;
		edit_m_min -> setText(str.setNum(m_min));
		edit_m_max -> setText(str.setNum(m_max));
	}
	else
	{
		m_min = text.toInt();
		if(m_min > m_max)
		{
			m_min = m_max;
			edit_m_min -> setText(str.setNum(m_min));
		}
		else if(m_min < 1)
		{
			m_min = 1;
			edit_m_min -> setText("1");
		}
	}
	if(nm_same)
	{
		if(m_min <= 12)  n_min = m_min;
		else  n_min = 12;
		edit_n_min -> setText(str.setNum(n_min));
		if(m_unchanged)
		{
			n_max = n_min;
			edit_n_max -> setText(str.setNum(n_max));
		}
	}
}

void Interface::set_m_max()
{
	QString str, text = edit_m_max -> text();
	m_max = text.toInt();
	if(m_max < m_min)
	{
		m_max = m_min;
		edit_m_max -> setText(str.setNum(m_max));
	}
	else if(m_max > 15)
	{
		m_max = 15;
		edit_m_max -> setText("15");
	}
	if(nm_same)
	{
		if(m_max <= 12)  n_max = m_max;
		else  n_max = 12;
		edit_n_max -> setText(str.setNum(n_max));
	}
}

void Interface::set_n_min()
{
	QString str, text = edit_n_min -> text();
	n_min = text.toInt();
	if(n_min > n_max)
	{
		n_min = n_max;
		edit_n_min -> setText(str.setNum(n_min));
	}
	else if(n_min < 1)
	{
		n_min = 1;
		edit_n_min -> setText("1");
	}
}

void Interface::set_n_max()
{
	QString str, text = edit_n_max -> text();
	n_max = text.toInt();
	if(n_max < n_min)
	{
		n_max = n_min;
		edit_n_max -> setText(str.setNum(n_max));
	}
	else if(n_max > 12)
	{
		n_max = 12;
		edit_n_max -> setText("12");
	}
}

void Interface::set_m_unchanged(int state)
{
	if(state == Qt::Checked)
	{
		m_unchanged = true;
		edit_m_max -> setText(edit_m_min -> text());
		m_max = m_min;
		edit_m_max -> setDisabled(true);
		if(nm_same)
		{
			QString str;
			if(m_max <= 12)  n_max = m_max;
			else  n_max = 12;
			edit_n_max -> setText(str.setNum(n_max));
		}
	}
	else
	{
		m_unchanged = false;
		edit_m_max -> setEnabled(true);
	}
}

void Interface::set_nm_same(int state)
{
	QString str;
	if(state == Qt::Checked)
	{
		nm_same = true;
		if(m_min <= 12)  n_min = m_min;
		else  n_min = 12;
		if(m_max <= 12)  n_max = m_max;
		else  n_max = 12;
		edit_n_min -> setText(str.setNum(n_min));
		edit_n_max -> setText(str.setNum(n_max));
		edit_n_min -> setDisabled(true);
		edit_n_max -> setDisabled(true);
	}
	else
	{
		nm_same = false;
		edit_n_min -> setEnabled(true);
		edit_n_max -> setEnabled(true);
	}
}

void Interface::sync_minmax()
{
	if(m_unchanged)
	{
		edit_m_max -> setText(edit_m_min -> text());
		if(nm_same)
			edit_n_max -> setText(edit_m_max -> text());
	}
}

void Interface::sync_nm()
{
	if(nm_same)
	{
		edit_n_min -> setText(edit_m_min -> text());
		edit_n_max -> setText(edit_m_max -> text());
	}
}

void Interface::import_database()
{
	QFileDialog* dialog = new QFileDialog(this);
	QStringList str1 = {"Choose chord database file", "选择和弦类型库文件"};
	dialog -> setWindowTitle(str1[language]);
	dialog -> setFileMode(QFileDialog::ExistingFile);
	QStringList str2 = {"Database File (*.db)", "库文件 (*.db)"};
	dialog -> setNameFilter(str2[language]);
	dialog -> setDirectory(root_path + "/db/chord");
	if(dialog -> exec())
	{
		QStringList temp = dialog -> selectedFiles();
		QFileInfo fileinfo(temp[0]);
		strcpy(database_filename, fileinfo.fileName().toLocal8Bit().data());
		ifstream fin(fileinfo.absoluteFilePath().toLocal8Bit().data());
		char str[100];
		fin.getline(str, 100, '\n');
		fin.getline(str, 100, '\n');
		fin.getline(str, 100, '\n');
		fin >> str >> str;
		fin.get();
		fin.getline(str, 100, '\n');
		strcpy(database[English], str);
		fin >> str >> str;
		fin.get();
		fin.getline(str, 100, '\n');
		strcpy(database[Chinese], str);
		database_size = 0;
		while(!fin.eof())
		{
			fin.getline(str, 100, '\n');
			if(strlen(str) > 1)  ++database_size;
		}
		fin.close();
		QStringList str3 = {"Current database: ", "当前类型库："};
		label_database_filename -> setText(str3[language] + database_filename);
		label_database -> setText(database[language]);
		QString str4;
		label_database_size -> setText(str4.setNum(database_size));
	}
}

void Interface::set_manual(bool state)
{
	if(state == true)
	// manual input is selected
	{
		automatic = false;
		edit_initial -> setEnabled(true);
	}
	else
	{
		automatic = true;
		edit_initial -> clear();
		edit_initial -> setDisabled(true);
		notes.clear();
	}
}

void Interface::input_initial()
{
	strcpy(str_notes, edit_initial -> text().toLatin1().data());
	set_initial();
}

void Interface::set_initial()
{
	notes.clear();
	char _note[50];
	int note;
	QString text = str_notes;
	int pos1 = 0, pos2 = 0, len = text.size();
	if(len >= 45)
	{
		edit_initial -> clear();
		notes.clear();
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
		if(note < 0)
		{
			edit_initial -> clear();
			notes.clear();
			return;
		}
		notes.push_back(note);
	}
	bubble_sort(notes);
	remove_duplicate(notes);
}

void Interface::set_connect_pedal(int state)
{
	connect_pedal = (bool)state;
}

void Interface::set_interlace(int state)
{
	interlace = (bool)state;
}

void Interface::set_remove_dup(int state)
{
	if(state == Qt::Checked)
		unique_mode = RemoveDup;
	else
	{
		cb_remove_dup_type -> setChecked(false);
		unique_mode = Disabled;
	}
}

void Interface::set_remove_dup_type(int state)
{
	if(state == Qt::Checked)
	{
		cb_remove_dup -> setChecked(true);
		unique_mode = RemoveDupType;
	}
	else
	{
		if(cb_remove_dup -> isChecked())
			unique_mode = RemoveDup;
		else  unique_mode = Disabled;
	}
}

void Interface::open_utilities()
{
	 QDesktopServices::openUrl(QUrl( ((QString)"file:%1/utilities").arg(root_path) ));
}

void Interface::run()
{
	QStringList str1 = {"Warning", "警告"};
	char path1[200], path2[200];
	strcpy(path1, "../db/chord/");
	strcat(path1, database_filename);
	dbentry(path1);

	if(align_mode == List)
	{
		if(strcmp(align_db_filename, "N/A") == 0)
		{
			QStringList str2 = {"NOTICE - You have selected custom chord alignment without specifying an alignment database (.db) file."
									  "Please import an alignment database (.db) file in 'Set alignment' window.",
									  "提示：检测到您已选择自定义和弦排列，但未导入排列库(.db)文件。请于设置排列方式处导入排列库(.db)文件。"};
			QMessageBox::warning(this, str1[language], str2[language], QMessageBox::Close);
			return;
		}
		strcpy(path2, "../db/align/");
		strcat(path2, align_db_filename);
		read_alignment(path2);
	}

	if(automatic)  choose_initial();
	else
	{
		try{ check_initial(); }
		catch(const char* msg)
		{
			QMessageBox::warning(this, str1[language], msg, QMessageBox::Close);
			return;
		}
	}

	vector<int> temp1(rm_priority);
	rm_priority.assign(7, -1);
	for(int i = 0; i < (int)temp1.size(); ++i)
		rm_priority[temp1[i]] = i;
	vector<int> temp2(notes);
	// Here is the 'rm_priority' we actually use in the program.
	// The value of 'rm_priority[i]', if not equal to -1, is the order of interval i,
	// otherwise the interval is not allowed.

	QStringList str3 = {"Generating...", "生成中…"};
	QStringList str4 = {"Abort", "中止"};
	prgdialog = new QProgressDialog("", str4[language], 0, 100, this);
	prgdialog -> setFont(font);
	prgdialog -> setWindowTitle(str3[language]);
	prgdialog -> setWindowFlag(Qt::WindowMinMaxButtonsHint, false);
	prgdialog -> setWindowFlag(Qt::WindowCloseButtonHint, false);
	prgdialog -> setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	prgdialog -> setWindowModality(Qt::WindowModal);
	prgdialog -> setAutoClose(false);
	prgdialog -> setAutoReset(false);

	bool b = false;
	try{ Main(); }
	catch(const char* msg)
	{
		prgdialog -> close();
		QMessageBox::warning(this, str1[language], msg, QMessageBox::Close);
		rm_priority.assign(temp1.begin(), temp1.end());
		notes.assign(temp2.begin(), temp2.end());
		if(fout.is_open())  fout.close();
		if(m_fout.is_open())  m_fout.close();
		return;
	}
	catch(int num)
	{
		prgdialog -> close();
		rm_priority.assign(temp1.begin(), temp1.end());
		notes.assign(temp2.begin(), temp2.end());
		if(fout.is_open())  fout.close();
		if(m_fout.is_open())  m_fout.close();
		b = true;
	}
	catch(...)
	{
		prgdialog -> close();
		new_chords.clear();
		QStringList str12 = {"Critical error", "严重错误"};
		QStringList str13 = {"Unknown error", "未知错误"};
		QMessageBox::critical(this, str12[language], str13[language], QMessageBox::Close);
		rm_priority.assign(temp1.begin(), temp1.end());
		notes.assign(temp2.begin(), temp2.end());
		if(fout.is_open())  fout.close();
		if(m_fout.is_open())  m_fout.close();
		return;
	}

	prgdialog -> close();
	QStringList str5 = {"Message", "消息"};
	QStringList str6;
	if(b)
	{
		QString str9 = "Generation stopped at progression #", str10 = "生成停止于进行#", str11;
		str9  = str9  + str11.setNum(progr_count) + ". Open generated file(s)?";
		str10 = str10 + str11.setNum(progr_count) + "。打开生成的文件？";
		str6.insert(0, str9);
		str6.insert(1, str10);
	}
	else
	{
		str6.insert(0, "Generation completed. Open generated file(s)?");
		str6.insert(1, "生成完毕。打开生成的文件？");
	}
	QStringList str7 = {"Yes", "是"};
	QStringList str8 = {"No",  "否"};
	QMessageBox* completed = new QMessageBox(QMessageBox::Information, str5[language], str6[language]);
	QPushButton* open = (completed -> addButton(str7[language], QMessageBox::AcceptRole));
	completed -> addButton(str8[language], QMessageBox::RejectRole);
	completed -> exec();

	if(completed -> clickedButton() == open)
	{
		if(output_mode != TextOnly)
			QDesktopServices::openUrl(QUrl( ((QString)"file:%1/output/%2.mid").arg(root_path).arg(output_name) ));
		if(output_mode != MidiOnly)
			QDesktopServices::openUrl(QUrl( ((QString)"file:%1/output/%2.txt").arg(root_path).arg(output_name) ));
	}
	rm_priority.assign(temp1.begin(), temp1.end());
	notes.assign(temp2.begin(), temp2.end());
}

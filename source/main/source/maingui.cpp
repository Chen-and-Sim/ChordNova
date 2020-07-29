// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// maingui.cpp

#include "Interface.h"

Interface::Interface(QWidget* parent): QWidget(parent)
{
	root_path = QDir::currentPath();
	root_path = root_path.left(root_path.size() - 4);
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
	setFixedWidth(640);
	if(language == Chinese)
		setFixedHeight(730);
	else  setFixedHeight(755);

	const int PARTS = 8;
	QGridLayout** grid = new QGridLayout*[PARTS];
	for(int i = 0; i < PARTS; ++i)
		grid[i] = new QGridLayout();


	{
		grid[0] -> setHorizontalSpacing(10);
		grid[0] -> setVerticalSpacing(5);
		grid[0] -> setContentsMargins(10, 0, 10, 0);

		QString str1 = "程序：清华大学 沈智云\nProgram by Ji-woon Sim    \n(Tsinghua University)";
		QLabel* label1 = new QLabel(str1, this);
		grid[0] -> addWidget(label1, 0, 0, 3, 1);

		QString str2 = "构想：星海音乐学院 陈文戈\nConcept by Wenge Chen\n(Xinghai Conservatory of Music)  ";
		QLabel* label2 = new QLabel(str2, this);
		grid[0] -> addWidget(label2, 0, 1, 3, 1);

		QPixmap pic1 = QPixmap("icons/icon.png");
		QLabel* label3 = new QLabel(this);
		label3 -> setPixmap(pic1);
		grid[0] -> addWidget(label3, 0, 2, 3, 1, Qt::AlignRight);

		QLabel* label4 = new QLabel("© 2020\nver.: 2.0.0730", this);
		label4 -> setAlignment(Qt::AlignRight);
		grid[0] -> addWidget(label4, 0, 3, 2, 1, Qt::AlignRight | Qt::AlignBottom);

		QPixmap pic2 = QPixmap("icons/name.png");
		QLabel* label5 = new QLabel(this);
		label5 -> setPixmap(pic2);
		grid[0] -> addWidget(label5, 2, 3, Qt::AlignRight);
	}

	{
		grid[1] -> setVerticalSpacing(0);
		grid[1] -> setHorizontalSpacing(5);
		grid[1] -> setColumnStretch(0, 0);
		grid[1] -> setColumnStretch(1, 1);
		grid[1] -> setColumnMinimumWidth(3, 87);
		grid[1] -> setContentsMargins(20, 0, 20, 0);

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
		if(language == English)
			btn2 -> setFixedWidth(170);
		else  btn2 -> setFixedWidth(138);
		grid[1] -> addWidget(btn1, 0, 2, Qt::AlignRight);
		grid[1] -> addWidget(btn2, 1, 2, Qt::AlignRight);
		connect(btn1, &QPushButton::clicked, this, &Interface::import_preset);
		connect(btn2, &QPushButton::clicked, this, &Interface::saveSettingsGui);

		QPushButton* btn3 = new QPushButton("English");
		QPushButton* btn4 = new QPushButton("User Guide");
		QPushButton* btn5 = new QPushButton("中文");
		QPushButton* btn6 = new QPushButton("用户手册");
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
		grid[2] -> setVerticalSpacing(5);
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
		QAbstractButton* btn = new QToolButton(this);
		btn -> setIcon(pic);
		btn -> setIconSize(QSize(50, 50));
		grid[2] -> addWidget(btn, 0, 4, 3, 1, Qt::AlignCenter);
		connect(btn, &QPushButton::clicked, this, &Interface::run);
	}

	{
		if(language == English)
		{
			grid[3] -> setContentsMargins(0, 5, 0, 0);
			grid[3] -> setColumnMinimumWidth(1, 50);
		}
		else
		{
			grid[3] -> setContentsMargins(20, 5, 20, 0);
			grid[3] -> setColumnMinimumWidth(1, 100);
		}
		grid[3] -> setHorizontalSpacing(5);
		grid[3] -> setVerticalSpacing(5);

		QStringList str1 = {"Number of progressions: ", "和弦进行数量："};
		label_loop_count = new QLabel(str1[language], this);
		grid[3] -> addWidget(label_loop_count, 0, 0);

		edit_loop_count = new QLineEdit(this);
		if(language == English)
			edit_loop_count -> setFixedWidth(30);
		else  edit_loop_count -> setFixedWidth(60);
		grid[3] -> addWidget(edit_loop_count, 0, 1, Qt::AlignLeft);
		connect(edit_loop_count, &QLineEdit::textChanged, this, &Interface::set_loop_count1);
		connect(edit_loop_count, &QLineEdit::editingFinished, this, &Interface::set_loop_count2);

		QStringList str2 = {"(only used in continual mode)", "（仅用于连续生成模式）"};
		QLabel* label1 = new QLabel(str2[language], this);
		grid[3] -> addWidget(label1, 1, 0, 1, 2);

		QStringList str3 = {"(M) Number of parts: ", "(M) 和弦声部数量："};
		QStringList str4 = {"(N) Number of notes: ", "(N) 和弦音集音数："};
		QLabel* label2 = new QLabel(str3[language], this);
		QLabel* label3 = new QLabel(str4[language], this);
		grid[3] -> addWidget(label2, 0, 2);
		grid[3] -> addWidget(label3, 1, 2);

		edit_m_min = new QLineEdit(this);
		edit_n_min = new QLineEdit(this);
		connect(edit_m_min, &QLineEdit::textEdited, this, &Interface::sync_minmax);
		connect(edit_m_min, &QLineEdit::textEdited, this, &Interface::sync_nm);
		connect(edit_m_min, &QLineEdit::editingFinished, this, &Interface::set_m_min);
		connect(edit_n_min, &QLineEdit::editingFinished, this, &Interface::set_n_min);
		grid[3] -> addWidget(edit_m_min, 0, 3);
		grid[3] -> addWidget(edit_n_min, 1, 3);

		QStringList str5 = {"to", "至"};
		QLabel* label4 = new QLabel(str5[language], this);
		QLabel* label5 = new QLabel(str5[language], this);
		grid[3] -> addWidget(label4, 0, 4);
		grid[3] -> addWidget(label5, 1, 4);

		edit_m_max = new QLineEdit(this);
		edit_n_max = new QLineEdit(this);
		connect(edit_m_max, &QLineEdit::textEdited, this, &Interface::sync_nm);
		connect(edit_m_max, &QLineEdit::editingFinished, this, &Interface::set_m_max);
		connect(edit_n_max, &QLineEdit::editingFinished, this, &Interface::set_n_max);
		grid[3] -> addWidget(edit_m_max, 0, 5);
		grid[3] -> addWidget(edit_n_max, 1, 5);

		QStringList str6 = {"Unchanged", "不变"};
		QStringList str7 = {"Ditto", "同上"};
		cb_m_unchanged = new QCheckBox(str6[language], this);
		cb_nm_same = new QCheckBox(str7[language], this);
		grid[3] -> addWidget(cb_m_unchanged, 0, 6);
		grid[3] -> addWidget(cb_nm_same, 1, 6);
		connect(cb_m_unchanged, &QCheckBox::stateChanged, this, &Interface::set_m_unchanged);
		connect(cb_nm_same, &QCheckBox::stateChanged, this, &Interface::set_nm_same);
	}

	{
		QHBoxLayout* hbox1 = new QHBoxLayout();
		QHBoxLayout* hbox2 = new QHBoxLayout();
		hbox2 -> setSpacing(5);

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

		grid[4] -> setContentsMargins(20, 5, 20, 0);
		grid[4] -> setVerticalSpacing(5);
		grid[4] -> addLayout(hbox1, 0, 0);
		grid[4] -> addLayout(hbox2, 1, 0);
	}

	{
		QHBoxLayout* hbox1 = new QHBoxLayout();
		hbox1 -> setSpacing(5);

		QStringList str1 = {"Range of notes: ", "音域范围："};
		QLabel* label1 = new QLabel(str1[language], this);
		hbox1 -> addWidget(label1);

		edit_note_min = new QLineEdit(this);
		hbox1 -> addWidget(edit_note_min);
		connect(edit_note_min, &QLineEdit::editingFinished, this, &Interface::set_note_min);

		QStringList str2 = {"to", "至"};
		QLabel* label2 = new QLabel(str2[language], this);
		hbox1 -> addWidget(label2);

		edit_note_max = new QLineEdit(this);
		hbox1 -> addWidget(edit_note_max);
		connect(edit_note_max, &QLineEdit::editingFinished, this, &Interface::set_note_max);

		QStringList str3 = {"(automatically judges pitch names / MIDI note numbers)", "（自动判定音名/MIDI键位号）"};
		QLabel* label3 = new QLabel(str3[language], this);
		hbox1 -> addWidget(label3, 1, Qt::AlignLeft);

		QStringList str6 = {"Overall Scale: ", "整体音阶："};
		cb_overall_scale = new QCheckBox(str6[language], this);

		QGridLayout* _grid = new QGridLayout();
		_grid -> addWidget(cb_overall_scale, 0, 0);
		_grid -> setContentsMargins(0, 0, 0, 5);
		connect(cb_overall_scale, &QCheckBox::stateChanged, this, &Interface::set_default_overall_scale);

		QLabel* label_0_11[12];
		for(int i = 0; i < 12; ++i)
		{
			QString str;
			label_0_11[i] = new QLabel(str.setNum(i), this);
			cb_0_11[i] = new QCheckBox(this);
			_grid -> addWidget(label_0_11[i], 0, i + 1, Qt::AlignCenter);
			_grid -> addWidget(cb_0_11[i], 1, i + 1, Qt::AlignCenter);
			connect(cb_0_11[i], &QCheckBox::clicked, this, &Interface::set_overall_scale);
		}

		QHBoxLayout* hbox3 = new QHBoxLayout();
		hbox3 -> setSpacing(5);

		QStringList str7 = {"Omission: ", "省略音："};
		QLabel* label4 = new QLabel(str7[language], this);
		hbox3 -> addWidget(label4);

		QStringList str8 = {"Set omission...", "设置省略音…"};
		QPushButton* btn1 = new QPushButton(str8[language], this);
		hbox3 -> addWidget(btn1, 1, Qt::AlignLeft);
		connect(btn1, &QPushButton::clicked, this, &Interface::omissionGui);

		QStringList str9 = {"Inversion: ", "和弦转位："};
		QLabel* label5 = new QLabel(str9[language], this);
		hbox3 -> addWidget(label5);

		QStringList str10 = {"Set inversion...", "设置转位…"};
		QPushButton* btn2 = new QPushButton(str10[language], this);
		hbox3 -> addWidget(btn2, 1, Qt::AlignLeft);
		connect(btn2, &QPushButton::clicked, this, &Interface::inversionGui);

		QStringList str11 = {"Alignment: ", "和弦排列方式："};
		QLabel* label6 = new QLabel(str11[language], this);
		hbox3 -> addWidget(label6);

		QStringList str12 = {"Set alignment...", "设置排列方式…"};
		QPushButton* btn3 = new QPushButton(str12[language], this);
		hbox3 -> addWidget(btn3, 1, Qt::AlignLeft);
		connect(btn3, &QPushButton::clicked, this, &Interface::alignmentGui);

		QHBoxLayout* hbox4 = new QHBoxLayout();
		hbox4 -> setSpacing(5);

		QStringList str13 = {"Enable pedal notes: ", "应用持续音："};
		cb_pedal = new QCheckBox(str13[language], this);
		hbox4 -> addWidget(cb_pedal);
		connect(cb_pedal, &QCheckBox::stateChanged, this, &Interface::set_enable_pedal_notes);

		QStringList str14 = {"Set pedal notes...", "设置持续音…"};
		btn_pedal = new QPushButton(str14[language], this);
		btn_pedal -> setDisabled(true);
		hbox4 -> addWidget(btn_pedal, 1, Qt::AlignLeft);
		connect(btn_pedal, &QPushButton::clicked, this, &Interface::pedalNotesGui);

		QStringList str15 = {"Apply more rules: ", "应用更多规则："};
		cb_more_rules = new QCheckBox(str15[language], this);
		hbox4 -> addWidget(cb_more_rules);
		connect(cb_more_rules, &QCheckBox::stateChanged, this, &Interface::set_enable_more_rules);

		QStringList str16 = {"More rules...", "更多规则…"};
		btn_more_rules = new QPushButton(str16[language], this);
		btn_more_rules -> setDisabled(true);
		hbox4 -> addWidget(btn_more_rules, 1, Qt::AlignLeft);
		connect(btn_more_rules, &QPushButton::clicked, this, &Interface::moreRulesGui);

		QHBoxLayout* hbox5 = new QHBoxLayout();
		hbox5 -> setSpacing(20);

		QStringList str17 = {"Set more parameters\nand sort priorities...", "更多指标设置\n及排序设置…"};
		btn_more_param = new QPushButton(str17[language], this);
		btn_more_param -> setMinimumWidth(100);
		hbox5 -> addWidget(btn_more_param, 0, Qt::AlignLeft);
		connect(btn_more_param, &QPushButton::clicked, this, &Interface::moreParamGui);

		QStringList str18 = {"←Support 11 parameters including 'Cuikang Hua's Chroma \n   Difference' currently.\n"
									"Warning: the parameters will keep default values if not set!",
									"←目前支持“华萃康氏色差”等11项指标。\n提醒：不设置则将保持为预设值！"};
		QLabel* label7 = new QLabel(str18[language], this);
		label7 -> setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
		label7 -> setStyleSheet("color: #FF0000;");
		hbox5 -> addWidget(label7, 1, Qt::AlignLeft);

		grid[5] -> setContentsMargins(20, 5, 20, 0);
		grid[5] -> setVerticalSpacing(5);
		grid[5] -> setRowMinimumHeight(4, 50);
		grid[5] -> addLayout(hbox1, 0, 0, 1, 2);
		grid[5] -> addLayout(_grid, 1, 0);
		grid[5] -> addLayout(hbox3, 2, 0, 1, 2);
		grid[5] -> addLayout(hbox4, 3, 0, 1, 2);
		grid[5] -> addLayout(hbox5, 4, 0, 1, 2, Qt::AlignBottom);

		QLabel* _label1 = new QLabel(" ", this);
		grid[5] -> addWidget(_label1, 1, 1);
		grid[5] -> setColumnMinimumWidth(1, 180);
	}

	{
		grid[6] = new QGridLayout();
		grid[6] -> setVerticalSpacing(5);
		if(language == Chinese)
			grid[6] -> setContentsMargins(20, 0, 20, 0);
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

	vbox -> setSpacing(15);
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
		btn_continual -> setChecked(true);
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
	edit_loop_count -> setText(str.setNum(loop_count));
	edit_m_min -> setText(str.setNum(m_min));
	edit_m_max -> setText(str.setNum(m_max));
	edit_n_min -> setText(str.setNum(n_min));
	edit_n_max -> setText(str.setNum(n_max));
	cb_m_unchanged -> setChecked(m_unchanged);
	cb_nm_same -> setChecked(nm_same);

	label_database_filename -> setText(label_database_filename -> text() + database_filename);
	label_database -> setText(database[language]);
	label_database_size -> setText(str.setNum(database_size));

	edit_note_min -> setText(str.setNum(lowest));
	edit_note_max -> setText(str.setNum(highest));
	cb_overall_scale -> setChecked(overall_scale.size() < 12);
	for(int i = 0; i < (int)overall_scale.size(); ++i)
		cb_0_11[overall_scale[i]] -> setChecked(true);
	cb_pedal -> setChecked(enable_pedal);
	cb_more_rules -> setChecked(enable_more_rules);

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
	painter.drawRect(0, 0, 640, 80);

	painter.setPen(QColor("#EFE4B0"));
	painter.setBrush(QBrush("#EFE4B0"));
	painter.drawRect(0, 80, 640, 60);

	painter.setPen(QColor("#00A2E8"));
	painter.drawLine(445, 85, 445, 135);

	painter.drawLine(20, 245, 620, 245);
	painter.drawLine(20, 315, 620, 315);
	painter.drawLine(20, 380, 620, 380);
	painter.drawLine(20, 585, 620, 585);
	if(language == Chinese)
		painter.drawLine(20, 675, 620, 675);
	else  painter.drawLine(20, 700, 620, 700);
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
	QMessageBox::StandardButton btn =
		QMessageBox::question(this, str1[language], str2[language], QMessageBox::Yes | QMessageBox::No);
	if(btn == QMessageBox::Yes)
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

	loop_count = read_data(fin, str);
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

	lowest = read_data(fin, str);
	highest = read_data(fin, str);
	read_vec(fin, str, overall_scale);
	enable_pedal = read_data(fin, str);
	enable_more_rules = read_data(fin, str);

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
	h_min = read_data(fin, str);  h_max = read_data(fin, str);
	g_min = read_data(fin, str);  g_max = read_data(fin, str);
	x_min = read_data(fin, str);  x_max = read_data(fin, str);
	read_data(fin, sort_order);

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
	QDesktopServices::openUrl(QUrl("file:guide/智弦（SmartChordGen 2.0）用户手册.pdf"));
}

void Interface::open_manual_English()
{
	QDesktopServices::openUrl(QUrl("file:guide/User Guide for SmartChordGen 2.0.pdf"));
}

void Interface::set_output_name()
{
	strcpy(output_name, edit_output_name -> text().toUtf8().data());
}

void Interface::set_continual(bool state)
{
	if(state == true)
	// continual mode is selected
	{
		continual = true;
		edit_initial -> clear();
		notes.clear();
		edit_loop_count -> setEnabled(true);
		label_loop_count -> setEnabled(true);
		cb_pedal -> setEnabled(true);
		cb_interlace -> setChecked(false);
		cb_interlace -> setDisabled(true);
		cb_remove_dup -> setEnabled(true);
	}
	else
	{
		continual = false;
		enable_sim = false;
		edit_loop_count -> setDisabled(true);
		label_loop_count -> setDisabled(true);
		cb_pedal -> setDisabled(true);
		cb_pedal -> setChecked(false);
		set_enable_pedal_notes(false);
		if(output_mode == TextOnly)
			cb_interlace -> setDisabled(true);
		else
		{
			cb_interlace -> setEnabled(true);
			cb_interlace -> setChecked(true);
			interlace = true;
		}
		cb_remove_dup -> setChecked(true);
		cb_remove_dup -> setDisabled(true);
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

void Interface::set_loop_count1(const QString& text)
{
	if(!text.isEmpty())
		set_loop_count2();
}

void Interface::set_loop_count2()
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

void Interface::set_default_overall_scale(int state)
{
	if(state == Qt::Checked)
	{
		bool b = false;
		for(int i = 0; i < 12; ++i)
		{
			if(cb_0_11[i] -> isChecked() == Qt::Unchecked)
			{
				b = true;
				break;
			}
		}
		if(!b)
		{
			cb_0_11[0] -> setChecked(false);
			if(overall_scale[0] == 0)
				overall_scale.erase(overall_scale.begin());
		}
	}
	else
	{
		overall_scale.clear();
		for(int i = 0; i < 12; ++i)
		{
			cb_0_11[i] -> setChecked(true);
			overall_scale.push_back(i);
		}
	}
}

void Interface::set_overall_scale(int state)
{
	Q_UNUSED(state);
	overall_scale.clear();
	bool b = false;
	for(int i = 0; i < 12; ++i)
	{
		if(cb_0_11[i] -> isChecked())
			overall_scale.push_back(i);
		else  b = true;
	}
	cb_overall_scale -> setChecked(b);
}

void Interface::set_enable_pedal_notes(int state)
{
	if(state == Qt::Checked)
	{
		btn_pedal -> setEnabled(true);
		cb_connect_pedal -> setEnabled(true);
		cb_connect_pedal -> setChecked(true);
		enable_pedal = true;
		connect_pedal = true;
	}
	else
	{
		btn_pedal -> setDisabled(true);
		cb_connect_pedal -> setDisabled(true);
		cb_connect_pedal -> setChecked(false);
		enable_pedal = false;
	}
}

void Interface::set_enable_more_rules(int state)
{
	if(state == Qt::Checked)
	{
		btn_more_rules -> setEnabled(true);
		enable_more_rules = true;
	}
	else
	{
		btn_more_rules -> setDisabled(true);
		enable_more_rules = false;
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
	QDesktopServices::openUrl(QUrl("file:../utilities"));
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

	if(automatic)  choose_original();
	else
	{
		try{ check_original(); }
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
			QDesktopServices::openUrl( QUrl("file:../output/" + (QString)output_name + ".mid") );
		if(output_mode != MidiOnly)
			QDesktopServices::openUrl( QUrl("file:../output/" + (QString)output_name + ".txt") );
	}
	rm_priority.assign(temp1.begin(), temp1.end());
	notes.assign(temp2.begin(), temp2.end());
}

// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// Interface.h

#ifndef INTERFACE
#define INTERFACE

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileinfo>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QToolButton>
#include <QUrl>

#include "chord.h"
#include "functions.h"

enum SaveMode {Save, SaveAs};

class Interface: public QWidget, public Chord
{
	Q_OBJECT

private:
	QFont font;
	QVBoxLayout* vbox;
	QString  root_path;
	QString  cur_preset[2];
	QString  cur_preset_filename;
	QString  cur_preset_path;
	QString  preset[2];
	QString  preset_filename;
	SaveMode save_mode;

	QLabel* label_preset_filename;
	QLabel* label_preset;

	QLineEdit* edit_output_name;
	QRadioButton* btn_continual;
	QRadioButton* btn_single;
	QRadioButton* btn_def;
	QRadioButton* btn_midi;
	QRadioButton* btn_text;

	QLineEdit* edit_loop_count;
	QLineEdit* edit_m_min;
	QLineEdit* edit_m_max;
	QLineEdit* edit_n_min;
	QLineEdit* edit_n_max;
	QLabel* label_loop_count;
	QCheckBox* cb_m_unchanged;
	QCheckBox* cb_nm_same;
	QRadioButton* btn_choose_m;
	QRadioButton* btn_choose_n;

	QLabel* label_database_filename;
	QLabel* label_database;
	QLabel* label_database_size;

	QLineEdit* edit_note_min;
	QLineEdit* edit_note_max;
	QCheckBox* cb_overall_scale;
	QCheckBox* cb_0_11[12];
	QCheckBox* cb_pedal;
	QPushButton* btn_pedal;
	QCheckBox* cb_more_rules;
	QPushButton* btn_more_rules;
	QPushButton* btn_more_param;

	QRadioButton* btn_automatic;
	QRadioButton* btn_manual;
	QLineEdit* edit_initial;
	QCheckBox* cb_connect_pedal;
	QCheckBox* cb_interlace;
	QCheckBox* cb_remove_dup;
	QCheckBox* cb_remove_dup_type;

	QDialog* save_dialog;
	QRadioButton* btn_save;
	QRadioButton* btn_save_as;
	QLineEdit* edit_Chinese;
	QLineEdit* edit_English;
	QLineEdit* edit_filename;

	QWidget* omission_window;
	QCheckBox* cb_reserve[5][7];

	QWidget* inversion_window;
	QCheckBox* cb_note[7];

	QWidget* alignment_window;
	QRadioButton* btn_interval;
	QRadioButton* btn_alignment;
	QRadioButton* btn_unlimited;
	QLabel* label_orig[7];
	QLineEdit* edit_i_low;
	QLineEdit* edit_i_high;
	QLineEdit* edit_i_min;
	QLineEdit* edit_i_max;
	QLabel* label_custom[8];
	QPushButton* btn_align_db;

	QWidget* pedal_notes_window;
	QCheckBox* cb_in_bass;
	QCheckBox* cb_realign;
	QLineEdit* edit_pitch_mode;
	QLabel* label_pitch_mode;
	QRadioButton* btn_pitch_mode;
	QRadioButton* btn_set_mode;
	QLabel* label_set_mode[12];
	QCheckBox* cb_set_mode[12];
	QLabel* label_period;
	QLineEdit* edit_period;

	QWidget* more_rules_window;
	QCheckBox* cb_vl_number;
	QRadioButton* btn_vl_number;
	QRadioButton* btn_vl_percentage;
	QCheckBox* cb_steady;
	QCheckBox* cb_ascending;
	QCheckBox* cb_descending;
	QLineEdit* edit_steady_min;
	QLineEdit* edit_ascending_min;
	QLineEdit* edit_descending_min;
	QLineEdit* edit_steady_max;
	QLineEdit* edit_ascending_max;
	QLineEdit* edit_descending_max;
	QLabel* label_steady;
	QLabel* label_ascending;
	QLabel* label_descending;
	QCheckBox* cb_vl_range;
	QLineEdit* edit_vl_min;
	QLineEdit* edit_vl_max;
	QLabel* label_vl;
	QRadioButton* btn_default_rm;
	QRadioButton* btn_custom_rm;
	QLineEdit* edit_custom_rm;
	QRadioButton* btn_default_ex;
	QRadioButton* btn_custom_ex;
	QLineEdit* edit_custom_ex;
	QRadioButton* btn_default_sim;
	QRadioButton* btn_custom_sim;
	QLineEdit* edit_custom_sim;

	QWidget* more_param_window;
	QCheckBox* cb_param[_TOTAL];
	QLineEdit* edit_min[_TOTAL];
	QLineEdit* edit_max[_TOTAL];
	QLineEdit* edit_order[_TOTAL];
	QCheckBox* cb_ascending_order[_TOTAL];

	void mainGui();
	void saveSettingsGui();
	void omissionGui();
	void inversionGui();
	void alignmentGui();
	void pedalNotesGui();
	void moreRulesGui();
	void moreParamGui();
	void initMain();
	void initOmission();
	void initInversion();
	void initAlignment();
	void initPedalNotes();
	void initMoreRules();
	void initMoreParam();
	void doPainting();
	void clear(QLayout*);
	void paintEvent(QPaintEvent* e);
	void closeEvent(QCloseEvent* e);
	void set_default_param(int);
	void set_param_min(int, int);
	void set_param_max(int, int);
	void closeMoreParam();

public slots:
	void import_preset();
	int  read_data(ifstream&, char*);
	void read_vec(ifstream&, char*, vector<int>&);
	void read_preset(char*);
	void set_to_Chinese();
	void set_to_English();
	void open_manual_Chinese();
	void open_manual_English();
	void set_output_name();
	void set_continual(bool);
	void set_output_format(bool);
	void set_loop_count1(const QString&);
	void set_loop_count2();
	void set_m_min();
	void set_m_max();
	void set_n_min();
	void set_n_max();
	void set_m_unchanged(int);
	void set_nm_same(int);
	void sync_minmax();
	void sync_nm();
	void import_database();
	void set_note_min();
	void set_note_max();
	void set_default_overall_scale(int);
	void set_overall_scale(int);
	void set_enable_pedal_notes(int);
	void set_enable_more_rules(int);
	void set_manual(bool);
	void input_initial();
	void set_initial();
	void set_connect_pedal(int);
	void set_interlace(int);
	void set_remove_dup(int);
	void set_remove_dup_type(int);
	void open_utilities();
	void run();

	void set_save_mode(bool);
	void set_preset_Chinese();
	void set_preset_English();
	void set_preset_filename();
	void write_preset();

	void set_omission(bool);
	void set_inversion(bool);

	void set_alignment_mode(bool);
	void set_i_low();
	void set_i_high();
	void set_i_min();
	void set_i_max();
	void import_alignment();

	void set_in_bass(int);
	void select_set_pitch(bool);
	void input_pedal_notes();
	void select_pedal_notes(bool);
	void set_realign();
	void set_period();

	void enable_vl_num(bool);
	void change_vl_setting(bool);
	void enable_vl_direction(bool);
	void set_steady_min();
	void set_steady_max();
	void set_ascending_min();
	void set_ascending_max();
	void set_descending_min();
	void set_descending_max();
	void enable_vl_range(bool);
	void set_vl_min();
	void set_vl_max();
	void enable_custom_rm(bool);
	void input_rm();
	void enable_custom_ex(bool);
	void input_ex();
	void set_ex();
	void enable_custom_sim(bool);
	void input_sim();
	void set_sim();

	void enable_param(bool);
	void set_k_min();
	void set_k_max();
	void set_t_min();
	void set_t_max();
	void set_c_min();
	void set_c_max();
	void set_sv_min();
	void set_sv_max();
	void set_r_min();
	void set_r_max();
	void set_h_min();
	void set_h_max();
	void set_g_min();
	void set_g_max();
	void set_x_min();
	void set_x_max();

public:
	Interface(QWidget* parent = nullptr);
};

extern void (Interface::*set_min[_TOTAL]) ();
extern void (Interface::*set_max[_TOTAL]) ();

#endif

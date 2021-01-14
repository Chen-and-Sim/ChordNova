// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// interface.h

#ifndef INTERFACE
#define INTERFACE

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
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
#include <QTextEdit>
#include <QUrl>

#if __WIN32
#include <windows.h>  // in order to get 'hscale' and 'vscale'
#include <shlobj.h>   // for default output path 'DESKTOP'
#endif

#include "chord.h"
#include "functions.h"

enum SaveMode {Save, SaveAs};

class Interface: public QWidget, public Chord
{
	Q_OBJECT

private:
	double hscale; // the actual magnification scale of the screen
	double vscale; // It seems that the magnification scales of two directions are not equal.
	QFont  font;   // default font of the program
	QVBoxLayout* main_vbox;  // the frame of maingui
	QString  root_path;
	QString  cur_preset[2];
	QString  cur_preset_filename;
	QString  cur_preset_path;
	QString  preset[2];
	QString  preset_filename;
	// the two parameters above are used only when saving settings
	// (i.e. temporarily records the data users input)
	SaveMode save_mode;

	QLabel* label_preset_filename;
	QLabel* label_preset;

	QLineEdit* edit_output_name;
	QRadioButton* btn_continual;
	QRadioButton* btn_single;
	QRadioButton* btn_def;
	QRadioButton* btn_midi;
	QRadioButton* btn_text;

	QLabel* label_loop_count;
	QLineEdit* edit_loop_count;
	QLineEdit* edit_note_min;
	QLineEdit* edit_note_max;
	QLineEdit* edit_m_min;
	QLineEdit* edit_m_max;
	QLineEdit* edit_n_min;
	QLineEdit* edit_n_max;
	QCheckBox* cb_m_unchanged;
	QCheckBox* cb_nm_same;
	QRadioButton* btn_choose_m;
	QRadioButton* btn_choose_n;

	QLabel* label_database_filename;
	QLabel* label_database;
	QLabel* label_database_size;

	QPushButton* btn_pedal;
	QLabel* label_overall_scale;
	QLabel* label_omission;
	QLabel* label_inversion;
	QLabel* label_alignment;
	QLabel* label_more_param;
	QLabel* label_pedal_notes;
	QLabel* label_more_rules;

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

	bool have_set_overall_scale;
	QWidget* overall_scale_window;
	QRadioButton* btn_default_overall_scale;
	QRadioButton* btn_custom_overall_scale;
	QCheckBox* cb_0_11[12];

	bool have_set_omission;  // is false if and only if a preset is imported
	QWidget* omission_window;
	QCheckBox* cb_reserve[5][7];

	bool have_set_inversion; // is false if and only if a preset is imported
	QWidget* inversion_window;
	QCheckBox* cb_note[7];

	bool have_set_alignment; // is false if and only if a preset is imported
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

	// 'have_set_pedal_notes' is equivalent to 'Chord::enable_pedal'.
	QWidget* pedal_notes_window;
	QCheckBox* cb_enable_pedal;
	QCheckBox* cb_in_bass;
	QCheckBox* cb_realign;
	QLabel* label_pedal_format;
	QLabel* label_pitch_mode;
	QLineEdit* edit_pitch_mode;
	QRadioButton* btn_pitch_mode;
	QRadioButton* btn_set_mode;
	QLabel* label_set_mode[12];
	QCheckBox* cb_set_mode[12];
	QLabel* label_period;
	QLineEdit* edit_period;

	bool have_set_more_rules;
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

	// 'have_set_more_param' is determined by whether 'sort_order' is empty.
	QWidget* more_param_window;
	QCheckBox* cb_param[VAR_TOTAL];
	QLineEdit* edit_min[VAR_TOTAL];
	QLineEdit* edit_max[VAR_TOTAL];
	QLineEdit* edit_order[VAR_TOTAL];
	QCheckBox* cb_ascending_order[VAR_TOTAL];

	QWidget* analyser_window;
	QLineEdit* edit_ante;
	QLineEdit* edit_post;
	QCheckBox* cb_hide_octave;
	QTextEdit* edit_stats;
	QRadioButton* btn_post;
	QRadioButton* btn_ante;
	QRadioButton* btn_both;
	QRadioButton* btn_test_all;
	QRadioButton* btn_test_part;
	QLineEdit* edit_sample_size;
	QLabel*   label_sample_size;
	QRadioButton* btn_def_ref;
	QRadioButton* btn_cus_ref;
	QPushButton*  btn_detail_ref;
	QLineEdit* edit_output_name_sub;
	QRadioButton* btn_def_sub;
	QRadioButton* btn_midi_sub;
	QRadioButton* btn_text_sub;

	QWidget* sub_settings_window;
	QCheckBox* cb_param_sub[VAR_TOTAL];
	QCheckBox* cb_reset[VAR_TOTAL];
	QLineEdit* edit_reset_value[VAR_TOTAL];
	QLineEdit* edit_plus_minus[VAR_TOTAL];
	QCheckBox* cb_percentage[VAR_TOTAL];
	QCheckBox* cb_ascending_order_sub[VAR_TOTAL];
	QLineEdit* edit_order_sub[VAR_TOTAL];

	void mainGui();
	void saveSettingsGui();
	void overallScaleGui();
	void omissionGui();
	void inversionGui();
	void alignmentGui();
	void pedalNotesGui();
	void moreRulesGui();
	void moreParamGui();
	void analyserGui();
	void subSettingsGui();
	void initMain();
	void initOverallScale();
	void initOmission();
	void initInversion();
	void initAlignment();
	void initPedalNotes();
	void initMoreRules();
	void initMoreParam();
	void initAnalyser();
	void initSubSettings();
	void clear(QLayout*);
	bool eventFilter(QObject*, QEvent*);  // draws the blue line in analyser window or closes a child window
	void paintEvent(QPaintEvent* e);      // draws the blue lines and background colours
	void closeEvent(QCloseEvent* e);		  // closes the main window

	void set_default_param(int);
	void set_param_min(int, int);
	void set_param_max(int, int);
	// The three functions above is used in 'moreparamgui'.
	void _reset(int);
	void _reset_value(int);
	void _plus_minus(int);
	// The three functions above is used in 'subsettingsgui'.

public slots:
	void import_preset();  // selects a preset and opens it
	int  read_data(ifstream&, char*);  // can read a string, bool or int from preset
	void read_vec(ifstream&, char*, vector<int>&);  // reads a vector from preset
	void read_preset(char*);  // assigns values to variables from preset
	void set_to_Chinese();
	void set_to_English();
	void open_manual_Chinese();
	void open_manual_English();
	void set_output_name();
	void set_output_path();
	void set_continual(bool);
	void set_output_format(bool);
	void set_loop_count();
	void set_note_min();
	void set_note_max();
	void set_m_min();
	void set_m_max();
	void set_n_min();
	void set_n_max();
	void set_m_unchanged(int);
	void set_nm_same(int);
	void sync_minmax();
	void sync_nm();
	void import_database();
	// not actually reads it; just gets some data that will appear in the interface
	void set_manual(bool);
	void input_initial();
	void set_notes(vector<int>&, QLineEdit*);
	void set_connect_pedal(int);
	void set_interlace(int);
	void set_remove_dup(int);
	void set_remove_dup_type(int);
	void open_utilities();
	void run();
	// reads chord and align database, receives and displays error messages,
	// and runs the program

	void set_save_mode(bool);
	void set_preset_Chinese();
	void set_preset_English();
	void set_preset_filename();
	void write_preset();

	void set_default_overall_scale(bool);
	void set_overall_scale(int);
	void closeOverallScale();

	void set_omission(bool);
	void closeOmission();

	void set_inversion(bool);
	void closeInversion();

	void set_alignment_mode(bool);
	void set_i_low();
	void set_i_high();
	void set_i_min();
	void set_i_max();
	void import_alignment(); // similar to 'import_database'
	void closeAlignment();

	void set_enable_pedal_notes(int);
	void set_in_bass(int);
	void select_set_pitch(bool);
	void input_pedal_notes();
	void select_pedal_notes(bool);
	void set_realign();
	void set_period();
	void closePedalNotes();

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
	void closeMoreRules();

	void enable_param(bool);
	void set_k_min();
	void set_k_max();
	void set_kk_min();
	void set_kk_max();
	void set_t_min();
	void set_t_max();
	void set_c_min();
	void set_c_max();
	void set_sv_min();
	void set_sv_max();
	void set_r_min();
	void set_r_max();
	void set_s_min();
	void set_s_max();
	void set_ss_min();
	void set_ss_max();
	void set_h_min();
	void set_h_max();
	void set_g_min();
	void set_g_max();
	void set_x_min();
	void set_x_max();
	void set_q_min();
	void set_q_max();
	void closeMoreParam();  // mainly checks whether the orders are set correctly

	void random_chords();
	void set_antechord();
	void set_postchord();
	void swap_chords();
	void set_hide_octave();
	void analyse();
	void set_substitute_obj(bool);
	void set_test_all();
	void set_sample_size();
	void set_substitute_ref(bool);
	void set_output_name_sub();
	void set_output_path_sub();
	void set_output_format_sub(bool);
	void run_sub();

	void enable_param_sub(bool);
	void reset_k();
	void reset_kk();
	void reset_t();
	void reset_c();
	void reset_sv();
	void reset_n();
	void reset_r();
	void reset_s();
	void reset_ss();
	void reset_x();
	void reset_p();
	void reset_q();
	void reset_value_k();
	void reset_value_kk();
	void reset_value_t();
	void reset_value_c();
	void reset_value_sv();
	void reset_value_n();
	void reset_value_r();
	void reset_value_s();
	void reset_value_ss();
	void reset_value_x();
	void reset_value_p();
	void reset_value_q();
	void plus_minus_k();
	void plus_minus_kk();
	void plus_minus_t();
	void plus_minus_c();
	void plus_minus_sv();
	void plus_minus_n();
	void plus_minus_r();
	void plus_minus_s();
	void plus_minus_ss();
	void plus_minus_x();
	void plus_minus_p();
	void plus_minus_q();
	void closeSubSettings();

public:
	Interface(QWidget* parent = nullptr);
};

extern void (Interface::*set_min[VAR_TOTAL]) ();
extern void (Interface::*set_max[VAR_TOTAL]) ();
extern void (Interface::*reset[VAR_TOTAL]) ();
extern void (Interface::*reset_value[VAR_TOTAL]) ();
extern void (Interface::*plus_minus[VAR_TOTAL]) ();
// The variables above unifies the functions in 'moreparamgui' and 'subsettingsgui'.

#endif

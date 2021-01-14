// ChordNova v3.0 [Build: 2021.1.14]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// main.cpp

#include <ctime>
#include <QApplication>
#include <QLayout>
#include "interface.h"

int main(int argc, char *argv[])
{
	srand( (int)time(0) );
	QApplication app(argc, argv);
	Interface window;

	window.setWindowTitle("ChordNova");
	window.setWindowIcon(QIcon("icons/icon.png"));
	window.show();

	return app.exec();
}

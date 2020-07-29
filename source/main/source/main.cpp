// SmartChordGen v2.0 [Build: 2020.7.30]
// (c) 2020 Wenge Chen, Ji-woon Sim.
// main.cpp

#include <ctime>
#include <iostream>
#include <QApplication>
#include <QLayout>
#include "Interface.h"

int main(int argc, char *argv[])
{
	srand( (int)time(0) );
	QApplication app(argc, argv);
	Interface window;

	window.setWindowTitle("SmartChordGen 2.0.0730");
	window.setWindowIcon(QIcon("icons/icon.png"));
	window.show();

	return app.exec();
}

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QApplication::setOrganizationName( "Electronic Design Chemnitz" );
	QApplication::setOrganizationDomain( "ed-chemnitz.de" );
	QApplication::setApplicationName( "QModBus" );

	MainWindow w;
	w.show();

	return a.exec();
}

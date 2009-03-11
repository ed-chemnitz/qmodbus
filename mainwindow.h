#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "modbus.h"
#include "ui_about.h"


namespace Ui
{
    class MainWindowClass;
}


class QLabel;



class AboutDialog : public QDialog, public Ui::AboutDialog
{
public:
	AboutDialog( QWidget * _parent ) :
		QDialog( _parent )
	{
		setupUi( this );
		aboutTextLabel->setText( aboutTextLabel->text().arg( "0.1.0" ) );
	}
} ;


class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow( QWidget * parent = 0 );
	~MainWindow();


private slots:
	void changeSerialPort( int );
	void updateRequestPreview( void );
	void updateRegisterView( void );
	void sendModbusRequest( void );
	void resetStatus( void );
	void aboutQModBus( void );


private:
	Ui::MainWindowClass * ui;
	modbus_param_t * m_mbParam;
	QWidget * m_statusInd;
	QLabel * m_statusText;

};

#endif // MAINWINDOW_H

#include <QSettings>
#include <QMessageBox>
#include "qextserialenumerator.h"
#include "serialsettingswidget.h"
#include "ui_serialsettingswidget.h"

SerialSettingsWidget::SerialSettingsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SerialSettingsWidget)
  , m_serialModbus( NULL )
{
	ui->setupUi(this);
	enableGuiItems(false);
}

SerialSettingsWidget::~SerialSettingsWidget()
{
	delete ui;
}

int SerialSettingsWidget::setupModbusPort()
{
	QSettings s;

	int portIndex = 0;
	int i = 0;
	foreach( QextPortInfo port, QextSerialEnumerator::getPorts() )
	{
		ui->serialPort->addItem( port.friendName );
		if( port.friendName == s.value( "serialinterface" ) )
		{
			portIndex = i;
		}
		++i;
	}
	ui->serialPort->setCurrentIndex( portIndex );

	ui->baud->setCurrentIndex( ui->baud->findText( s.value( "serialbaudrate" ).toString() ) );
	ui->parity->setCurrentIndex( ui->parity->findText( s.value( "serialparity" ).toString() ) );
	ui->stopBits->setCurrentIndex( ui->stopBits->findText( s.value( "serialstopbits" ).toString() ) );
	ui->dataBits->setCurrentIndex( ui->dataBits->findText( s.value( "serialdatabits" ).toString() ) );

	connect( ui->serialPort, SIGNAL( currentIndexChanged( int ) ),
			this, SLOT( changeSerialPort( int ) ) );
	connect( ui->baud, SIGNAL( currentIndexChanged( int ) ),
			this, SLOT( changeSerialPort( int ) ) );
	connect( ui->dataBits, SIGNAL( currentIndexChanged( int ) ),
			this, SLOT( changeSerialPort( int ) ) );
	connect( ui->stopBits, SIGNAL( currentIndexChanged( int ) ),
			this, SLOT( changeSerialPort( int ) ) );
	connect( ui->parity, SIGNAL( currentIndexChanged( int ) ),
			this, SLOT( changeSerialPort( int ) ) );

	changeSerialPort( portIndex );
	return portIndex;
}

void SerialSettingsWidget::releaseSerialModbus()
{
	if( m_serialModbus )
	{
		modbus_close( m_serialModbus );
		modbus_free( m_serialModbus );
		m_serialModbus = NULL;
	}
}

void SerialSettingsWidget::changeModbusInterface(const QString& port, char parity)
{
	releaseSerialModbus();

	m_serialModbus = modbus_new_rtu( port.toAscii().constData(),
			ui->baud->currentText().toInt(),
			parity,
			ui->dataBits->currentText().toInt(),
			ui->stopBits->currentText().toInt() );

	if( modbus_connect( m_serialModbus ) == -1 )
	{
		QMessageBox::critical( this, tr( "Connection failed" ),
			tr( "Could not connect serial port!" ) );
	}
}


static inline QString embracedString( const QString & s )
{
    return s.section( '(', 1 ).section( ')', 0, 0 );
}


void SerialSettingsWidget::changeSerialPort( int )
{
	const int iface = ui->serialPort->currentIndex();

	QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
	if( !ports.isEmpty() )
	{
		QSettings settings;
		settings.setValue( "serialinterface", ports[iface].friendName );
		settings.setValue( "serialbaudrate", ui->baud->currentText() );
		settings.setValue( "serialparity", ui->parity->currentText() );
		settings.setValue( "serialdatabits", ui->dataBits->currentText() );
		settings.setValue( "serialstopbits", ui->stopBits->currentText() );
#ifdef Q_OS_WIN32
		const QString port = embracedString( ports[iface].friendName ) +
									":";
#else
		const QString port = ports[iface].physName;
#endif

		char parity;
		switch( ui->parity->currentIndex() )
		{
			case 1: parity = 'O'; break;
			case 2: parity = 'E'; break;
			default:
			case 0: parity = 'N'; break;
		}

		changeModbusInterface(port, parity);
	}
	else
	{
		QMessageBox::critical( this, tr( "No serial port found" ),
				tr( "Could not find any serial port "
						"on this computer!" ) );
	}
}


void SerialSettingsWidget::enableGuiItems(bool checked)
{
	ui->serialPort->setEnabled(checked);
	ui->baud->setEnabled(checked);
	ui->dataBits->setEnabled(checked);
	ui->stopBits->setEnabled(checked);
	ui->parity->setEnabled(checked);
}

void SerialSettingsWidget::on_checkBox_clicked(bool checked)
{
	if (checked) {
		setupModbusPort();
	}
	else {
		releaseSerialModbus();
	}
	enableGuiItems(checked);
	emit serialPortActive(checked);
}

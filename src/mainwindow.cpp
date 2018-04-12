/*
 * mainwindow.cpp - implementation of MainWindow class
 *
 * Copyright (c) 2009-2013 Tobias Junghans / Electronic Design Chemnitz
 *
 * This file is part of QModBus - http://qmodbus.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include <QSettings>
#include <QDebug>
#include <QScrollBar>

#include <errno.h>

#include "mainwindow.h"
#include "BatchProcessor.h"
#include "modbus.h"
#include "modbus-private.h"

#include "ui_mainwindow.h"


const int DataTypeColumn = 0;
const int AddrColumn = 1;
const int DataColumn = 2;

extern MainWindow * globalMainWin;


MainWindow::MainWindow( QWidget * _parent ) :
	QMainWindow( _parent ),
	ui( new Ui::MainWindowClass ),
	m_modbus( NULL ),
	m_tcpActive(false),
	m_poll(false)
{
	ui->setupUi(this);

	connect( ui->rtuSettingsWidget,   SIGNAL(serialPortActive(bool)), this, SLOT(onRtuPortActive(bool)));
	connect( ui->asciiSettingsWidget, SIGNAL(serialPortActive(bool)), this, SLOT(onAsciiPortActive(bool)));
	connect( ui->tcpSettingsWidget,   SIGNAL(tcpPortActive(bool)),    this, SLOT(onTcpPortActive(bool)));

	connect( ui->rtuSettingsWidget,   SIGNAL(connectionError(const QString&)), this, SLOT(setStatusError(const QString&)));
	connect( ui->asciiSettingsWidget, SIGNAL(connectionError(const QString&)), this, SLOT(setStatusError(const QString&)));
	connect( ui->tcpSettingsWidget,   SIGNAL(connectionError(const QString&)), this, SLOT(setStatusError(const QString&)));

	connect( ui->slaveID, SIGNAL( valueChanged( int ) ),
			this, SLOT( updateRequestPreview() ) );
	connect( ui->functionCode, SIGNAL( currentIndexChanged( int ) ),
			this, SLOT( updateRequestPreview() ) );
	connect( ui->startAddr, SIGNAL( valueChanged( int ) ),
			this, SLOT( updateRequestPreview() ) );
	connect( ui->numCoils, SIGNAL( valueChanged( int ) ),
			this, SLOT( updateRequestPreview() ) );

	connect( ui->functionCode, SIGNAL( currentIndexChanged( int ) ),
			this, SLOT( updateRegisterView() ) );
	connect( ui->numCoils, SIGNAL( valueChanged( int ) ),
			this, SLOT( updateRegisterView() ) );
	connect( ui->startAddr, SIGNAL( valueChanged( int ) ),
			this, SLOT( updateRegisterView() ) );

	connect( ui->sendBtn, SIGNAL( clicked() ),
			this, SLOT( onSendButtonPress() ) );

	connect( ui->clearBusMonTable, SIGNAL( clicked() ),
			this, SLOT( clearBusMonTable() ) );

	connect( ui->actionAbout_QModBus, SIGNAL( triggered() ),
			this, SLOT( aboutQModBus() ) );

	connect( ui->functionCode, SIGNAL( currentIndexChanged( int ) ),
		this, SLOT( enableHexView() ) );


	updateRegisterView();
	updateRequestPreview();
	enableHexView();

	ui->regTable->setColumnWidth( 0, 150 );

	m_statusInd = new QWidget;
	m_statusInd->setFixedSize( 16, 16 );
	m_statusText = new QLabel;
	ui->statusBar->addWidget( m_statusInd );
	ui->statusBar->addWidget( m_statusText, 10 );
	resetStatus();

	QTimer * t = new QTimer( this );
	connect( t, SIGNAL(timeout()), this, SLOT(pollForDataOnBus()));
	t->start( 5 );

	m_pollTimer = new QTimer( this );
	connect( m_pollTimer, SIGNAL(timeout()), this, SLOT(sendModbusRequest()));

	m_statusTimer = new QTimer( this );
	connect( m_statusTimer, SIGNAL(timeout()), this, SLOT(resetStatus()));
	m_statusTimer->setSingleShot(true);
}


MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	if( event->key() == Qt::Key_Control )
	{
		//set flag to request polling
		if( m_modbus != NULL )
			m_poll = true;

		if( ! m_pollTimer->isActive() )
			ui->sendBtn->setText( tr("Poll") );
	}
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
	if( event->key() == Qt::Key_Control )
	{
		m_poll = false;

		if( ! m_pollTimer->isActive() )
			ui->sendBtn->setText( tr("Send") );
	}
}

void MainWindow::onSendButtonPress( void )
{
	//if already polling then stop
	if( m_pollTimer->isActive() )
	{
		m_pollTimer->stop();
		ui->sendBtn->setText( tr("Send") );
	}
	else
	{
		//if polling requested then enable timer
		if( m_poll )
		{
			m_pollTimer->start( 1000 );
			ui->sendBtn->setText( tr("Stop") );
		}

		sendModbusRequest();
	}
}

void MainWindow::busMonitorAddItem( bool isRequest,
					uint8_t slave,
					uint8_t func,
					uint16_t addr,
					uint16_t nb,
					uint16_t expectedCRC,
					uint16_t actualCRC )
{
	QTableWidget * bm = ui->busMonTable;
	const int rowCount = bm->rowCount();
	bm->setRowCount( rowCount+1 );

	QTableWidgetItem * ioItem = new QTableWidgetItem( isRequest ? tr( "Req >>" ) : tr( "<< Resp" ) );
	QTableWidgetItem * slaveItem = new QTableWidgetItem( QString::number( slave ) );
	QTableWidgetItem * funcItem = new QTableWidgetItem( QString::number( func ) );
	QTableWidgetItem * addrItem = new QTableWidgetItem( QString::number( addr ) );
	QTableWidgetItem * numItem = new QTableWidgetItem( QString::number( nb ) );
	QTableWidgetItem * crcItem = new QTableWidgetItem;
	if( func > 127 )
	{
		addrItem->setText( QString() );
		numItem->setText( QString() );
		funcItem->setText( tr( "Exception (%1)" ).arg( func-128 ) );
		funcItem->setForeground( Qt::red );
	}
	else
	{
		if( expectedCRC == actualCRC )
		{
			crcItem->setText( QString().sprintf( "%.4x", actualCRC ) );
		}
		else
		{
			crcItem->setText( QString().sprintf( "%.4x (%.4x)", actualCRC, expectedCRC ) );
			crcItem->setForeground( Qt::red );
		}
	}
	ioItem->setFlags( ioItem->flags() & ~Qt::ItemIsEditable );
	slaveItem->setFlags( slaveItem->flags() & ~Qt::ItemIsEditable );
	funcItem->setFlags( funcItem->flags() & ~Qt::ItemIsEditable );
	addrItem->setFlags( addrItem->flags() & ~Qt::ItemIsEditable );
	numItem->setFlags( numItem->flags() & ~Qt::ItemIsEditable );
	crcItem->setFlags( crcItem->flags() & ~Qt::ItemIsEditable );
	bm->setItem( rowCount, 0, ioItem );
	bm->setItem( rowCount, 1, slaveItem );
	bm->setItem( rowCount, 2, funcItem );
	bm->setItem( rowCount, 3, addrItem );
	bm->setItem( rowCount, 4, numItem );
	bm->setItem( rowCount, 5, crcItem );
	bm->verticalScrollBar()->setValue( 100000 );
}


void MainWindow::busMonitorRawData( uint8_t * data, uint8_t dataLen, bool addNewline )
{
	if( dataLen > 0 )
	{
		QString dump = ui->rawData->toPlainText();
		for( int i = 0; i < dataLen; ++i )
		{
			dump += QString().sprintf( "%.2x ", data[i] );
		}
		if( addNewline )
		{
			dump += "\n";
		}
		ui->rawData->setPlainText( dump );
		ui->rawData->verticalScrollBar()->setValue( 100000 );
		ui->rawData->setLineWrapMode( QPlainTextEdit::NoWrap );
	}
}

// static
void MainWindow::stBusMonitorAddItem( modbus_t * modbus, uint8_t isRequest, uint8_t slave, uint8_t func, uint16_t addr, uint16_t nb, uint16_t expectedCRC, uint16_t actualCRC )
{
    Q_UNUSED(modbus);
    globalMainWin->busMonitorAddItem( isRequest, slave, func, addr, nb, expectedCRC, actualCRC );
}

// static
void MainWindow::stBusMonitorRawData( modbus_t * modbus, uint8_t * data, uint8_t dataLen, uint8_t addNewline )
{
    Q_UNUSED(modbus);
    globalMainWin->busMonitorRawData( data, dataLen, addNewline != 0 );
}

static QString descriptiveDataTypeName( int funcCode )
{
	switch( funcCode )
	{
		case MODBUS_FC_READ_COILS:
		case MODBUS_FC_WRITE_SINGLE_COIL:
		case MODBUS_FC_WRITE_MULTIPLE_COILS:
			return "Coil (binary)";
		case MODBUS_FC_READ_DISCRETE_INPUTS:
			return "Discrete Input (binary)";
		case MODBUS_FC_READ_HOLDING_REGISTERS:
		case MODBUS_FC_WRITE_SINGLE_REGISTER:
		case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
			return "Holding Register (16 bit)";
		case MODBUS_FC_READ_INPUT_REGISTERS:
			return "Input Register (16 bit)";
		default:
			break;
	}
	return "Unknown";
}


static inline QString embracedString( const QString & s )
{
	return s.section( '(', 1 ).section( ')', 0, 0 );
}


static inline int stringToHex( QString s )
{
	return s.replace( "0x", "" ).toInt( NULL, 16 );
}


void MainWindow::clearBusMonTable( void )
{
	ui->busMonTable->setRowCount( 0 );
}


void MainWindow::updateRequestPreview( void )
{
	const int slave = ui->slaveID->value();
	const int func = stringToHex( embracedString(
						ui->functionCode->
							currentText() ) );
	const int addr = ui->startAddr->value();
	const int num = ui->numCoils->value();
	if( func == MODBUS_FC_WRITE_SINGLE_COIL || func == MODBUS_FC_WRITE_SINGLE_REGISTER )
	{
		ui->requestPreview->setText(
			QString().sprintf( "%.2x  %.2x  %.2x %.2x ",
					slave,
					func,
					addr >> 8,
					addr & 0xff ) );
	}
	else
	{
		ui->requestPreview->setText(
			QString().sprintf( "%.2x  %.2x  %.2x %.2x  %.2x %.2x",
					slave,
					func,
					addr >> 8,
					addr & 0xff,
					num >> 8,
					num & 0xff ) );
	}
}




void MainWindow::updateRegisterView( void )
{
	const int func = stringToHex( embracedString(
					ui->functionCode->currentText() ) );
	const QString dataType = descriptiveDataTypeName( func );
	const int addr = ui->startAddr->value();

	int rowCount = 0;
	switch( func )
	{
		case MODBUS_FC_WRITE_SINGLE_REGISTER:
		case MODBUS_FC_WRITE_SINGLE_COIL:
			ui->numCoils->setEnabled( false );
			rowCount = 1;
			break;
		case MODBUS_FC_WRITE_MULTIPLE_COILS:
		case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
			rowCount = ui->numCoils->value();
		default:
			ui->numCoils->setEnabled( true );
			break;
	}

	ui->regTable->setRowCount( rowCount );
	for( int i = 0; i < rowCount; ++i )
	{
		QTableWidgetItem * dtItem = new QTableWidgetItem( dataType );
		QTableWidgetItem * addrItem =
			new QTableWidgetItem( QString::number( addr+i ) );
		QTableWidgetItem * dataItem =
			new QTableWidgetItem( QString::number( 0 ) );
		dtItem->setFlags( dtItem->flags() & ~Qt::ItemIsEditable	);
		addrItem->setFlags( addrItem->flags() & ~Qt::ItemIsEditable );
		ui->regTable->setItem( i, DataTypeColumn, dtItem );
		ui->regTable->setItem( i, AddrColumn, addrItem );
		ui->regTable->setItem( i, DataColumn, dataItem );
	}

	ui->regTable->setColumnWidth( 0, 150 );
}


void MainWindow::enableHexView( void )
{
	const int func = stringToHex( embracedString(
					ui->functionCode->currentText() ) );

	bool b_enabled =
		func == MODBUS_FC_READ_HOLDING_REGISTERS ||
		func == MODBUS_FC_READ_INPUT_REGISTERS;

	ui->checkBoxHexData->setEnabled( b_enabled );
}


void MainWindow::sendModbusRequest( void )
{
	if( m_tcpActive )
		ui->tcpSettingsWidget->tcpConnect();

	if( m_modbus == NULL )
	{
		setStatusError( tr("Not configured!") );
		return;
	}

	const int slave = ui->slaveID->value();
	const int func = stringToHex( embracedString(
					ui->functionCode->currentText() ) );
	const int addr = ui->startAddr->value();
	int num = ui->numCoils->value();
	uint8_t dest[1024];
	uint16_t * dest16 = (uint16_t *) dest;

	memset( dest, 0, 1024 );

	int ret = -1;
	bool is16Bit = false;
	bool writeAccess = false;
	const QString dataType = descriptiveDataTypeName( func );

	modbus_set_slave( m_modbus, slave );

	switch( func )
	{
		case MODBUS_FC_READ_COILS:
			ret = modbus_read_bits( m_modbus, addr, num, dest );
			break;
		case MODBUS_FC_READ_DISCRETE_INPUTS:
			ret = modbus_read_input_bits( m_modbus, addr, num, dest );
			break;
		case MODBUS_FC_READ_HOLDING_REGISTERS:
			ret = modbus_read_registers( m_modbus, addr, num, dest16 );
			is16Bit = true;
			break;
		case MODBUS_FC_READ_INPUT_REGISTERS:
			ret = modbus_read_input_registers( m_modbus, addr, num, dest16 );
			is16Bit = true;
			break;
		case MODBUS_FC_WRITE_SINGLE_COIL:
			ret = modbus_write_bit( m_modbus, addr,
					ui->regTable->item( 0, DataColumn )->
						text().toInt(0, 0) ? 1 : 0 );
			writeAccess = true;
			num = 1;
			break;
		case MODBUS_FC_WRITE_SINGLE_REGISTER:
			ret = modbus_write_register( m_modbus, addr,
					ui->regTable->item( 0, DataColumn )->
						text().toInt(0, 0) );
			writeAccess = true;
			num = 1;
			break;

		case MODBUS_FC_WRITE_MULTIPLE_COILS:
		{
			uint8_t * data = new uint8_t[num];
			for( int i = 0; i < num; ++i )
			{
				data[i] = ui->regTable->item( i, DataColumn )->
								text().toInt(0, 0);
			}
			ret = modbus_write_bits( m_modbus, addr, num, data );
			delete[] data;
			writeAccess = true;
			break;
		}
		case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
		{
			uint16_t * data = new uint16_t[num];
			for( int i = 0; i < num; ++i )
			{
				data[i] = ui->regTable->item( i, DataColumn )->
								text().toInt(0, 0);
			}
			ret = modbus_write_registers( m_modbus, addr, num, data );
			delete[] data;
			writeAccess = true;
			break;
		}

		default:
			break;
	}

	if( ret == num  )
	{
		if( writeAccess )
		{
			m_statusText->setText(
					tr( "Values successfully sent" ) );
			m_statusInd->setStyleSheet( "background: #0b0;" );
			m_statusTimer->start( 2000 );
		}
		else
		{
			bool b_hex = is16Bit && ui->checkBoxHexData->checkState() == Qt::Checked;
			QString qs_num;

			ui->regTable->setRowCount( num );
			for( int i = 0; i < num; ++i )
			{
				int data = is16Bit ? dest16[i] : dest[i];

				QTableWidgetItem * dtItem =
					new QTableWidgetItem( dataType );
				QTableWidgetItem * addrItem =
					new QTableWidgetItem(
						QString::number( addr+i ) );
				qs_num.sprintf( b_hex ? "0x%04x" : "%d", data);
				QTableWidgetItem * dataItem =
					new QTableWidgetItem( qs_num );
				dtItem->setFlags( dtItem->flags() &
							~Qt::ItemIsEditable );
				addrItem->setFlags( addrItem->flags() &
							~Qt::ItemIsEditable );
				dataItem->setFlags( dataItem->flags() &
							~Qt::ItemIsEditable );

				ui->regTable->setItem( i, DataTypeColumn,
								dtItem );
				ui->regTable->setItem( i, AddrColumn,
								addrItem );
				ui->regTable->setItem( i, DataColumn,
								dataItem );
			}
		}
	}
	else
	{
		QString err;

		if( ret < 0 )
		{
			if(
#ifdef WIN32
					errno == WSAETIMEDOUT ||
#endif
					errno == EIO
																	)
			{
				err += tr( "I/O error" );
				err += ": ";
				err += tr( "did not receive any data from slave." );
			}
			else
			{
				err += tr( "Protocol error" );
				err += ": ";
				err += tr( "Slave threw exception '" );
				err += modbus_strerror( errno );
				err += tr( "' or function not implemented." );
			}
		}
		else
		{
			err += tr( "Protocol error" );
			err += ": ";
			err += tr( "Number of registers returned does not "
					"match number of registers requested!" );
		}

		if( err.size() > 0 )
			setStatusError( err );
	}
}

void MainWindow::resetStatus( void )
{
	m_statusText->setText( tr( "Ready" ) );
	m_statusInd->setStyleSheet( "background: #aaa;" );
}

void MainWindow::pollForDataOnBus( void )
{
	if( m_modbus )
	{
		modbus_poll( m_modbus );
	}
}


void MainWindow::openBatchProcessor()
{
	BatchProcessor( this, m_modbus ).exec();
}


void MainWindow::aboutQModBus( void )
{
	AboutDialog( this ).exec();
}

void MainWindow::onRtuPortActive(bool active)
{
	if (active) {
		m_modbus = ui->rtuSettingsWidget->modbus();
		if (m_modbus) {
			modbus_register_monitor_add_item_fnc(m_modbus, MainWindow::stBusMonitorAddItem);
			modbus_register_monitor_raw_data_fnc(m_modbus, MainWindow::stBusMonitorRawData);
		}
		m_tcpActive = false;
	}
	else {
		m_modbus = NULL;
	}
}

void MainWindow::onAsciiPortActive(bool active)
{
    if (active) {
        m_modbus = ui->asciiSettingsWidget->modbus();
        if (m_modbus) {
            modbus_register_monitor_add_item_fnc(m_modbus, MainWindow::stBusMonitorAddItem);
            modbus_register_monitor_raw_data_fnc(m_modbus, MainWindow::stBusMonitorRawData);
        }
        m_tcpActive = false;
    }
    else {
        m_modbus = NULL;
    }
}

void MainWindow::onTcpPortActive(bool active)
{
	m_tcpActive = active;

	if (active) {
		m_modbus = ui->tcpSettingsWidget->modbus();
		if (m_modbus) {
			modbus_register_monitor_add_item_fnc(m_modbus, MainWindow::stBusMonitorAddItem);
			modbus_register_monitor_raw_data_fnc(m_modbus, MainWindow::stBusMonitorRawData);
		}
	}
	else {
		m_modbus = NULL;
	}
}

void MainWindow::setStatusError(const QString &msg)
{
    m_statusText->setText( msg );

    m_statusInd->setStyleSheet( "background: red;" );

    m_statusTimer->start( 2000 );
}

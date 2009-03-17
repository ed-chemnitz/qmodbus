/*
 * mainwindow.cpp - implementation of MainWindow class
 *
 * Copyright (c) 2009 Tobias Doerffel / Electronic Design Chemnitz
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

#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <QtGui/QScrollBar>

#include "mainwindow.h"
#include "modbus.h"
#include "qextserialenumerator.h"
#include "ui_mainwindow.h"


const int DataTypeColumn = 0;
const int AddrColumn = 1;
const int DataColumn = 2;

extern MainWindow * globalMainWin;


MainWindow::MainWindow( QWidget * _parent ) :
	QMainWindow( _parent ),
	ui( new Ui::MainWindowClass ),
	m_mbParam( NULL )
{
	ui->setupUi(this);

	QSettings settings;

	int portIndex = 0;
	int i = 0;
	foreach( QextPortInfo port, QextSerialEnumerator::getPorts() )
	{
		ui->serialPort->addItem( port.friendName );
		if( port.friendName == settings.value( "serialinterface" ) )
		{
			portIndex = i;
		}
		++i;
	}
	ui->serialPort->setCurrentIndex( portIndex );

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
			this, SLOT( sendModbusRequest() ) );

	connect( ui->actionAbout_QModBus, SIGNAL( triggered() ),
			this, SLOT( aboutQModBus() ) );

	changeSerialPort( portIndex );
	updateRegisterView();

	ui->regTable->setColumnWidth( 0, 150 );

	m_statusInd = new QWidget;
	m_statusInd->setFixedSize( 16, 16 );
	m_statusText = new QLabel;
	ui->statusBar->addWidget( m_statusInd );
	ui->statusBar->addWidget( m_statusText, 10 );
	resetStatus();
}




MainWindow::~MainWindow()
{
	delete ui;
}



void MainWindow::busMonitorAddItem( bool isOut,
					uint8_t slave,
					uint8_t func,
					uint16_t addr,
					uint16_t nb,
					uint16_t crc )
{
	QTableWidget * bm = ui->busMonTable;
	const int rowCount = bm->rowCount();
	bm->setRowCount( rowCount+1 );

	QTableWidgetItem * ioItem = new QTableWidgetItem( isOut ? tr( "out" ) : tr( "in" ) );
	QTableWidgetItem * slaveItem = new QTableWidgetItem( QString::number( slave ) );
	QTableWidgetItem * funcItem = new QTableWidgetItem( QString::number( func ) );
	QTableWidgetItem * addrItem = new QTableWidgetItem( QString::number( addr ) );
	QTableWidgetItem * numItem = new QTableWidgetItem( QString::number( nb ) );
	QTableWidgetItem * crcItem = new QTableWidgetItem( QString().sprintf( "%.4x", crc ) );
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
}




void MainWindow::busMonitorRawData( uint8_t * data, uint8_t dataLen )
{
	QString dump = ui->rawData->toPlainText();
	if( !dump.isEmpty() )
	{
		dump += "\n";
	}
	for( int i = 0; i < dataLen; ++i )
	{
		dump += QString().sprintf( "%.2x ", data[i] );
	}
	ui->rawData->setPlainText( dump );
	ui->rawData->verticalScrollBar()->setValue( 100000 );
	ui->rawData->setLineWrapMode( QPlainTextEdit::NoWrap );
}




static QString descriptiveDataTypeName( int funcCode )
{
	switch( funcCode )
	{
		case FC_READ_COIL_STATUS:
		case FC_FORCE_SINGLE_COIL:
		case FC_FORCE_MULTIPLE_COILS:
			return "Coil (binary)";
		case FC_READ_INPUT_STATUS:
			return "Discrete Input (binary)";
		case FC_READ_HOLDING_REGISTERS:
		case FC_PRESET_SINGLE_REGISTER:
		case FC_PRESET_MULTIPLE_REGISTERS:
			return "Holding Register (16 bit)";
		case FC_READ_INPUT_REGISTERS:
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




void MainWindow::changeSerialPort( int )
{
	const int iface = ui->serialPort->currentIndex();

	QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
	QSettings settings;
	settings.setValue( "serialinterface", ports[iface].friendName );
#ifdef Q_OS_WIN32
	const QString port = embracedString( ports[iface].friendName ) + ":";
#else
	const QString port = ports[iface].physName;
#endif
	if( m_mbParam )
	{
		modbus_close( m_mbParam );
		delete m_mbParam;
	}

	m_mbParam = new modbus_param_t;
	modbus_init_rtu( m_mbParam, port.toAscii().constData(),
				ui->baud->currentText().toInt(),
				ui->parity->currentText().toAscii().constData(),
				ui->dataBits->currentText().toInt(),
				ui->stopBits->currentText().toInt() );

	if( modbus_connect( m_mbParam ) == -1 )
	{
		QMessageBox::critical( this, tr( "Connection failed" ),
				tr( "Could not connect serial port!" ) );
	}
}




void MainWindow::updateRequestPreview( void )
{
	ui->requestPreview->setText(
			QString().sprintf( "%.2x  %.2x  %.2x %.2x  %.2x %.2x",
					ui->slaveID->value(),
					stringToHex( embracedString(
						ui->functionCode->
							currentText() ) ),
					ui->startAddr->value() >> 8,
					ui->startAddr->value() & 0xff,
					ui->numCoils->value() >> 8,
					ui->numCoils->value() & 0xff ) );
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
		case FC_PRESET_SINGLE_REGISTER:
		case FC_FORCE_SINGLE_COIL:
			ui->numCoils->setEnabled( false );
			rowCount = 1;
			break;
		case FC_FORCE_MULTIPLE_COILS:
		case FC_PRESET_MULTIPLE_REGISTERS:
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




void MainWindow::sendModbusRequest( void )
{
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

	switch( func )
	{
		case FC_READ_COIL_STATUS:
			ret = read_coil_status( m_mbParam, slave, addr, num,
									dest );
			break;
		case FC_READ_INPUT_STATUS:
			ret = read_input_status( m_mbParam, slave, addr, num,
									dest );
			break;
		case FC_READ_HOLDING_REGISTERS:
			ret = read_holding_registers( m_mbParam, slave, addr,
								num, dest16 );
			is16Bit = true;
			break;
		case FC_READ_INPUT_REGISTERS:
			ret = read_input_registers( m_mbParam, slave, addr,
								num, dest16 );
			is16Bit = true;
			break;
		case FC_FORCE_SINGLE_COIL:
			ret = force_single_coil( m_mbParam, slave, addr,
					ui->regTable->item( 0, DataColumn )->
						text().toInt() ? 1 : 0 );
			writeAccess = true;
			num = 1;
			break;
		case FC_PRESET_SINGLE_REGISTER:
			ret = preset_single_register( m_mbParam, slave, addr,
					ui->regTable->item( 0, DataColumn )->
						text().toInt() );
			writeAccess = true;
			num = 1;
			break;

		case FC_FORCE_MULTIPLE_COILS:
		{
			uint8_t * data = new uint8_t[num];
			for( int i = 0; i < num; ++i )
			{
				data[i] = ui->regTable->item( i, DataColumn )->
								text().toInt();
			}
			ret = force_multiple_coils( m_mbParam, slave, addr,
								num, data );
			delete[] data;
			writeAccess = true;
			break;
		}
		case FC_PRESET_MULTIPLE_REGISTERS:
		{
			uint16_t * data = new uint16_t[num];
			for( int i = 0; i < num; ++i )
			{
				data[i] = ui->regTable->item( i, DataColumn )->
								text().toInt();
			}
			ret = preset_multiple_registers( m_mbParam, slave, addr,
								num, data );
			delete[] data;
			writeAccess = true;
			break;
		}

		case FC_REPORT_SLAVE_ID:
		case FC_READ_EXCEPTION_STATUS:
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
			QTimer::singleShot( 2000, this, SLOT( resetStatus() ) );
		}
		else
		{
			ui->regTable->setRowCount( num );
			for( int i = 0; i < num; ++i )
			{
				int data = is16Bit ? dest16[i] : dest[i];
				QTableWidgetItem * dtItem =
					new QTableWidgetItem( dataType );
				QTableWidgetItem * addrItem =
					new QTableWidgetItem(
						QString::number( addr+i ) );
				QTableWidgetItem * dataItem =
					new QTableWidgetItem(
						QString::number( data ) );
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
		if( ret < 0 )
		{
			QMessageBox::critical( this, tr( "Protocol error" ),
				tr( "Slave threw exception %1 or "
					"function not implemented." ).
								arg( ret ) );
		}
		else
		{
			QMessageBox::critical( this, tr( "Protocol error" ),
				tr( "Number of registers returned does not "
					"match number of registers "
							"requested!" ) );
		}
	}
}




void MainWindow::resetStatus( void )
{
	m_statusText->setText( tr( "Ready" ) );
	m_statusInd->setStyleSheet( "background: #aaa;" );
}




void MainWindow::aboutQModBus( void )
{
	AboutDialog( this ).exec();
}



extern "C" {

void busMonitorAddItem( uint8_t isOut, uint8_t slave, uint8_t func, uint16_t addr, uint16_t nb, uint16_t crc )
{
	globalMainWin->busMonitorAddItem( isOut, slave, func, addr, nb, crc );
}

void busMonitorRawData( uint8_t * data, uint8_t dataLen )
{
	globalMainWin->busMonitorRawData( data, dataLen );
}

}

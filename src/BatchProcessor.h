/*
 * BatchProcessor.h - header file for BatchProcessor class
 *
 * Copyright (c) 2011-2014 Tobias Junghans / Electronic Design Chemnitz
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

#ifndef _BATCH_PROCESSOR_H
#define _BATCH_PROCESSOR_H

#include <QFile>
#include <QTimer>
#include <QDialog>

#include "modbus.h"


namespace Ui
{
	class BatchProcessor;
} ;

class BatchProcessor : public QDialog
{
	Q_OBJECT
public:
	BatchProcessor( QWidget *parent, modbus_t *modbus );
	~BatchProcessor();


private slots:
	void start();
	void stop();
	void browseOutputFile();
	void runBatch();


private:
	QString sendModbusRequest( int slaveID, int func, int addr );

	Ui::BatchProcessor *ui;
	modbus_t *m_modbus;
	QTimer m_timer;
	QFile m_outputFile;

} ;





#endif // MAINWINDOW_H

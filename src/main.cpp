/*
 * main.cpp - main file for QModBus
 *
 * Copyright (c) 2009-2014 Tobias Junghans / Electronic Design Chemnitz
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


#include <QApplication>
#include "mainwindow.h"

MainWindow * globalMainWin = NULL;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QApplication::setOrganizationName( "EDC Electronic Design Chemnitz GmbH" );
	QApplication::setOrganizationDomain( "ed-chemnitz.de" );
	QApplication::setApplicationName( "QModBus" );

	MainWindow w;
	w.show();

	globalMainWin = &w;

	return a.exec();
}

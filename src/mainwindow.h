/*
 * mainwindow.h - header file for MainWindow class
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "modbus.h"
#include "ui_about.h"


class AboutDialog : public QDialog, public Ui::AboutDialog
{
public:
    AboutDialog( QWidget * _parent ) :
        QDialog( _parent )
    {
        setupUi( this );
        aboutTextLabel->setText(
            aboutTextLabel->text().arg( "0.3.0" ) );
    }
} ;



namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow( QWidget * parent = 0 );
    ~MainWindow();

    void busMonitorAddItem( bool isRequest,
                uint8_t slave,
                uint8_t func,
                uint16_t addr,
                uint16_t nb,
                uint16_t expectedCRC,
                uint16_t actualCRC );
    void busMonitorRawData( uint8_t * data, uint8_t dataLen, bool addNewline );

    static void stBusMonitorAddItem( modbus_t * modbus,
            uint8_t isOut, uint8_t slave, uint8_t func, uint16_t addr,
            uint16_t nb, uint16_t expectedCRC, uint16_t actualCRC );
    static void stBusMonitorRawData( modbus_t * modbus, uint8_t * data,
            uint8_t dataLen, uint8_t addNewline );


private slots:
    void clearBusMonTable( void );
    void updateRequestPreview( void );
    void updateRegisterView( void );
    void enableHexView( void );
    void sendModbusRequest( void );
    void onSendButtonPress( void );
    void pollForDataOnBus( void );
    void openBatchProcessor();
    void aboutQModBus( void );
    void onRtuPortActive(bool active);
    void onAsciiPortActive(bool active);
    void onTcpPortActive(bool active);
    void resetStatus( void );
    void setStatusError(const QString &msg);

private:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    Ui::MainWindowClass * ui;
    modbus_t * m_modbus;
    QWidget * m_statusInd;
    QLabel * m_statusText;
    QTimer * m_pollTimer;
    QTimer * m_statusTimer;
    bool m_tcpActive;
    bool m_poll;
};

#endif // MAINWINDOW_H

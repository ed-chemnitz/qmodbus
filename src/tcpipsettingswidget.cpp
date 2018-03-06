#include "tcpipsettingswidget.h"
#include "ui_tcpipsettingswidget.h"
#include "modbus-tcp.h"
#include <QIntValidator>
#include <QDebug>

TcpIpSettingsWidget::TcpIpSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpIpSettingsWidget),
    m_tcpModbus(0)
{
    ui->setupUi(this);
    ui->edPort->setValidator(new QIntValidator(this));
    enableGuiItems(false);
}

TcpIpSettingsWidget::~TcpIpSettingsWidget()
{
    delete ui;
}

int TcpIpSettingsWidget::setupModbusPort()
{
    return 0;
}

void TcpIpSettingsWidget::changeModbusInterface(const QString &address, int portNbr)
{
    releaseTcpModbus();

    m_tcpModbus = modbus_new_tcp( address.toLatin1().constData(), portNbr );
    if( modbus_connect( m_tcpModbus ) == -1 )
    {
        emit connectionError( tr( "Could not connect to TCP/IP port!" ) );

    	releaseTcpModbus();
    }
}

void TcpIpSettingsWidget::releaseTcpModbus()
{
    if( m_tcpModbus )
    {
        modbus_close( m_tcpModbus );
        modbus_free( m_tcpModbus );
        m_tcpModbus = NULL;
    }
}

void TcpIpSettingsWidget::enableGuiItems(bool checked)
{
    ui->edPort->setEnabled(checked);
    ui->edNetworkAddress->setEnabled(checked);
}

void TcpIpSettingsWidget::on_cbEnabled_clicked(bool checked)
{
    enableGuiItems(checked);
    emit tcpPortActive(checked);
}

void TcpIpSettingsWidget::tcpConnect()
{
    int portNbr = ui->edPort->text().toInt();
    changeModbusInterface(ui->edNetworkAddress->text(), portNbr);
    emit tcpPortActive(ui->cbEnabled->isChecked());
}

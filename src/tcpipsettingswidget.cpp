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
    connect(ui->edNetworkAddress, SIGNAL(textChanged(QString)), this, SLOT(onEdNetworkAddressTextChanged(QString)));
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
        emit connectionError( tr( "Could not connect tcp/ip port!" ) );

        ui->btnApply->setEnabled(true);
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

void TcpIpSettingsWidget::on_btnApply_clicked()
{
    tcpConnect();
}

void TcpIpSettingsWidget::tcpConnect()
{
    int portNbr = ui->edPort->text().toInt();
    ui->btnApply->setEnabled(false);
    changeModbusInterface(ui->edNetworkAddress->text(), portNbr);
    emit tcpPortActive(ui->cbEnabled->isChecked());
}

void TcpIpSettingsWidget::onEdNetworkAddressTextChanged(const QString &)
{
    ui->btnApply->setEnabled(true);
}

void TcpIpSettingsWidget::on_edPort_textChanged(const QString &)
{
    ui->btnApply->setEnabled(true);
}

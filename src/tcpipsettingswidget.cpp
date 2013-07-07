#include "tcpipsettingswidget.h"
#include "ui_tcpipsettingswidget.h"
#include "modbus-tcp.h"
#include <QIntValidator>
#include <QMessageBox>

TcpIpSettingsWidget::TcpIpSettingsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TcpIpSettingsWidget)
,   m_tcpModbus(0)
{
	ui->setupUi(this);
	ui->edPort->setValidator(new QIntValidator(this));
	ui->edPort->setInputMask("00000");
	ui->edNetworkAddress->setInputMask("000.000.000.000");
	ui->edNetworkAddress->setValidator(&m_validator);
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

	m_tcpModbus = modbus_new_tcp( address.toAscii().constData(), portNbr );
	if( modbus_connect( m_tcpModbus ) == -1 )
	{
		QMessageBox::critical( this, tr( "Connection failed" ),
			tr( "Could not connect tcp/ip port!" ) );
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
}

void TcpIpSettingsWidget::on_btnApply_clicked()
{
	int portNbr = ui->edPort->text().toInt();
	changeModbusInterface(ui->edNetworkAddress->text(), portNbr);
	ui->btnApply->setEnabled(false);
	emit tcpPortActive(ui->cbEnabled->isChecked());
}

void TcpIpSettingsWidget::on_edNetworkAddress_textChanged(const QString &)
{
	ui->btnApply->setEnabled(true);
}

void TcpIpSettingsWidget::on_edPort_textChanged(const QString &)
{
	ui->btnApply->setEnabled(true);
}

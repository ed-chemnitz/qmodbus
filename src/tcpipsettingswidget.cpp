#include "tcpipsettingswidget.h"
#include "ui_tcpipsettingswidget.h"

TcpIpSettingsWidget::TcpIpSettingsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TcpIpSettingsWidget)
,   m_tcpModbus(0)
{
	ui->setupUi(this);
	ui->edNetworkAddress->setValidator(&m_validator);
}

TcpIpSettingsWidget::~TcpIpSettingsWidget()
{
	delete ui;
}

int TcpIpSettingsWidget::setupModbusPort()
{
	return 0;
}



void TcpIpSettingsWidget::on_cbEnabled_clicked(bool checked)
{

}

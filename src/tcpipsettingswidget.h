#ifndef TCPIPSETTINGSWIDGET_H
#define TCPIPSETTINGSWIDGET_H

#include <QWidget>
#include "imodbus.h"
#include "networkaddressvalidator.h"

namespace Ui {
class TcpIpSettingsWidget;
}

class TcpIpSettingsWidget : public QWidget, public IModbus
{
	Q_OBJECT

public:
	TcpIpSettingsWidget(QWidget *parent = 0);
	~TcpIpSettingsWidget();
	// IModbus interface
	virtual modbus_t *modbus() { return m_tcpModbus; }
	virtual int setupModbusPort();

private slots:
	void on_cbEnabled_clicked(bool checked);

private:
	Ui::TcpIpSettingsWidget *ui;
	modbus_t *               m_tcpModbus;
	NetworkAddressValidator  m_validator;

};

#endif // TCPIPSETTINGSWIDGET_H

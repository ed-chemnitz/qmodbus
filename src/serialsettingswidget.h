#ifndef SERIALSETTINGSWIDGET_H
#define SERIALSETTINGSWIDGET_H

#include <QWidget>
#include "imodbus.h"
#include "modbus.h"

namespace Ui {
class SerialSettingsWidget;
}


class SerialSettingsWidget : public QWidget, public IModbus
{
	Q_OBJECT

public:
	SerialSettingsWidget(QWidget *parent = 0);
	~SerialSettingsWidget();

	virtual modbus_t*  modbus() { return m_serialModbus; }

	virtual int setupModbusPort();

protected:
	void changeModbusInterface(const QString &port, char parity);
	void releaseSerialModbus();
	void enableGuiItems(bool checked);

signals:
	void serialPortActive(bool active);

public slots:
	void changeSerialPort(int);

private slots:
	void on_checkBox_clicked(bool checked);

private:
	Ui::SerialSettingsWidget *ui;
	modbus_t *                m_serialModbus;
};

#endif // SERIALSETTINGSWIDGET_H

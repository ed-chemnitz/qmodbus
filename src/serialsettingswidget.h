#ifndef SERIALSETTINGSWIDGET_H
#define SERIALSETTINGSWIDGET_H

#include <QWidget>
#include "modbus.h"

namespace Ui {
class SerialSettingsWidget;
}


class SerialSettingsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SerialSettingsWidget(QWidget *parent = 0);
	~SerialSettingsWidget();

	int setupSerialPort();

	void changeModbusInterface(const QString port, char parity);
public slots:
	void changeSerialPort(int);

private:
	Ui::SerialSettingsWidget *ui;
	modbus_t *                m_serialModbus;
};

#endif // SERIALSETTINGSWIDGET_H

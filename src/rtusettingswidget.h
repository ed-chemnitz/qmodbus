#ifndef RTUSETTINGSWIDGET_H
#define RTUSETTINGSWIDGET_H

#include <QWidget>
#include "imodbus.h"
#include "modbus.h"
#include "serialsettingswidget.h"

class RtuSettingsWidget : public SerialSettingsWidget
{
//	Q_OBJECT

public:
	RtuSettingsWidget(QWidget *parent = 0);
	virtual ~RtuSettingsWidget();

protected:
	virtual void changeModbusInterface(const QString &port, char parity);
};

#endif // RTUSETTINGSWIDGET_H

#ifndef ASCIISETTINGSWIDGET_H
#define ASCIISETTINGSWIDGET_H

#include <QWidget>
#include "imodbus.h"
#include "modbus.h"
#include "serialsettingswidget.h"


namespace Ui {
class SerialSettingsWidget;
}


class AsciiSettingsWidget : public SerialSettingsWidget
{
//	Q_OBJECT

public:
	AsciiSettingsWidget(QWidget *parent = 0);
	virtual ~AsciiSettingsWidget();

protected:
	virtual void changeModbusInterface(const QString &port, char parity);
};

#endif // ASCIISETTINGSWIDGET_H

#ifndef SERIALSETTINGSWIDGET_H
#define SERIALSETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SerialSettingsWidget;
}

class SerialSettingsWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit SerialSettingsWidget(QWidget *parent = 0);
	~SerialSettingsWidget();
	
private:
	Ui::SerialSettingsWidget *ui;
};

#endif // SERIALSETTINGSWIDGET_H

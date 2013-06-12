#include "serialsettingswidget.h"
#include "ui_serialsettingswidget.h"

SerialSettingsWidget::SerialSettingsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SerialSettingsWidget)
{
	ui->setupUi(this);
}

SerialSettingsWidget::~SerialSettingsWidget()
{
	delete ui;
}

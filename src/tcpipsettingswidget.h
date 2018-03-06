#ifndef TCPIPSETTINGSWIDGET_H
#define TCPIPSETTINGSWIDGET_H

#include <QWidget>
#include "imodbus.h"

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
    void tcpConnect();

protected:
    void changeModbusInterface(const QString& address, int portNbr);
    void releaseTcpModbus();
    void enableGuiItems(bool checked);

private slots:
    void on_cbEnabled_clicked(bool checked);

signals:
    void tcpPortActive(bool val);
    void connectionError(const QString &msg);

private:
    Ui::TcpIpSettingsWidget *ui;
    modbus_t *               m_tcpModbus;
};

#endif // TCPIPSETTINGSWIDGET_H

#ifndef IPADDRESSCTRL_H
#define IPADDRESSCTRL_H

#include <QWidget>
#include <QString>

namespace Ui {
class IPAddressCtrl;
}

class IPAddressCtrl : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE settext NOTIFY textChanged)

public:
    IPAddressCtrl(QWidget *parent = 0);
    virtual ~IPAddressCtrl();

    void settext(const QString& t);
    QString text() const;

signals:
    void textChanged(const QString& newText);

private slots:
    void on_ipEd0_textChanged(const QString &arg1);

    void on_ipEd1_textChanged(const QString &arg1);

    void on_ipEd2_textChanged(const QString &arg1);

    void on_ipEd3_textChanged(const QString &arg1);

private:
    Ui::IPAddressCtrl *ui;
};

#endif // IPADDRESSCTRL_H

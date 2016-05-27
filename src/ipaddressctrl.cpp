#include <QStringList>
#include "ipaddressctrl.h"
#include "ui_ipaddressctrl.h"

IPAddressCtrl::IPAddressCtrl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IPAddressCtrl)
{
    ui->setupUi(this);
}

IPAddressCtrl::~IPAddressCtrl()
{
    delete ui;
}

void IPAddressCtrl::settext(const QString &t)
{
    QStringList sl = t.split(QChar('.'));
    for (int i=0; i < sl.size(); i++) {
        switch(i) {
        case 0: ui->ipEd0->setText(sl[0]); break;
        case 1: ui->ipEd0->setText(sl[1]); break;
        case 2: ui->ipEd0->setText(sl[2]); break;
        case 3: ui->ipEd0->setText(sl[3]); break;
        default: break;
        }
    }
    emit textChanged(text());
}

QString IPAddressCtrl::text() const
{
    return ui->ipEd0->text() + QChar('.') + ui->ipEd1->text() + QChar('.')
         + ui->ipEd2->text() + QChar('.') + ui->ipEd3->text();
}

void IPAddressCtrl::on_ipEd0_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->ipEd1->setFocus(Qt::OtherFocusReason);
    }
    emit textChanged(text());
}

void IPAddressCtrl::on_ipEd1_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->ipEd2->setFocus(Qt::OtherFocusReason);
    }
    emit textChanged(text());
}

void IPAddressCtrl::on_ipEd2_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->ipEd3->setFocus(Qt::OtherFocusReason);
    }
    emit textChanged(text());
}

void IPAddressCtrl::on_ipEd3_textChanged(const QString &arg1)
{
    emit textChanged(text());
}

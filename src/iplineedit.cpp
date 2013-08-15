#include <QIntValidator>
#include "iplineedit.h"

IpLineEdit::IpLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    QIntValidator * intValidator = new QIntValidator(this);
    intValidator->setBottom(0);
    intValidator->setTop(255);
    setMaxLength(3);
    setValidator(intValidator);
    setAlignment(Qt::AlignVCenter | Qt::AlignRight);
}

#include "networkaddressvalidator.h"
#include <QHostAddress>

NetworkAddressValidator::NetworkAddressValidator(QObject *parent) :
	QValidator(parent)
{
}

QValidator::State NetworkAddressValidator::validate(QString &input, int &pos) const
{
	QHostAddress theIP;
	if ( theIP.setAddress(input) )
	{
		return QValidator::Acceptable;
	}
	else return QValidator::Intermediate;
}

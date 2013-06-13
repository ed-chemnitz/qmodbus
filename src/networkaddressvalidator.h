#ifndef NETWORKADDRESSVALIDATOR_H
#define NETWORKADDRESSVALIDATOR_H

#include <QValidator>

class NetworkAddressValidator : public QValidator
{
	Q_OBJECT
public:
	NetworkAddressValidator(QObject *parent = 0);

signals:

public slots:


	// QValidator interface
public:
	State validate(QString & input, int & pos) const;
};

#endif // NETWORKADDRESSVALIDATOR_H

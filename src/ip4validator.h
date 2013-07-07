#ifndef NETWORKADDRESSVALIDATOR_H
#define NETWORKADDRESSVALIDATOR_H


#include <QValidator>

/**
 * @brief The IP4Validator class taken from http://qt-project.org/forums/viewthread/5736
 */

class IP4Validator : public QValidator
{
	Q_OBJECT
public:
	IP4Validator(QObject *parent = 0);

	// QValidator interface
public:
	State validate(QString & input, int & pos) const;
	void fixup(QString &) const {}

};


#endif // NETWORKADDRESSVALIDATOR_H

#ifndef IMODBUS_H
#define IMODBUS_H

#include "modbus.h"

class IModbus
{
public:
	virtual modbus_t*  modbus() = 0;
	virtual int        setupModbusPort() = 0;
};

#endif // IMODBUS_H

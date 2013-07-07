TARGET = QModBus
TEMPLATE = app
VERSION = 0.1.0

#QT += network

MOC_DIR     = generated
OBJECTS_DIR = generated
UI_DIR      = generated
RCC_DIR     = generated

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/BatchProcessor.cpp \
    3rdparty/qextserialport/qextserialport.cpp	\
	3rdparty/libmodbus/src/modbus.c \
	3rdparty/libmodbus/src/modbus-data.c \
	3rdparty/libmodbus/src/modbus-rtu.c \
	3rdparty/libmodbus/src/modbus-tcp.c \
    src/serialsettingswidget.cpp \
    src/tcpipsettingswidget.cpp \
    src/ip4validator.cpp

HEADERS += src/mainwindow.h \
	src/BatchProcessor.h \
    3rdparty/qextserialport/qextserialport.h \
    3rdparty/qextserialport/qextserialenumerator.h \
    3rdparty/libmodbus/src/modbus.h \
    src/serialsettingswidget.h \
    src/imodbus.h \
    src/tcpipsettingswidget.h \
    src/ip4validator.h


INCLUDEPATH += 3rdparty/libmodbus 3rdparty/libmodbus/src 3rdparty/qextserialport src

unix:SOURCES += 3rdparty/qextserialport/posix_qextserialport.cpp	\
		3rdparty/qextserialport/qextserialenumerator_unix.cpp
unix:DEFINES += _TTY_POSIX_
win32:SOURCES += 3rdparty/qextserialport/win_qextserialport.cpp \
			3rdparty/qextserialport/qextserialenumerator_win.cpp
win32:DEFINES += _TTY_WIN_  WINVER=0x0501
win32:LIBS += -lsetupapi -lwsock32

FORMS += forms/mainwindow.ui \
    forms/about.ui	\
    forms/BatchProcessor.ui \
    forms/serialsettingswidget.ui \
    forms/tcpipsettingswidget.ui

RESOURCES += data/qmodbus.qrc

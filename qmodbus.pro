TARGET = QModBus
TEMPLATE = app
VERSION = 0.1.0
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    3rdparty/qextserialport/qextserialport.cpp	\
    3rdparty/libmodbus/modbus.c
HEADERS += src/mainwindow.h \
    3rdparty/qextserialport/qextserialport.h \
    3rdparty/qextserialport/qextserialenumerator.h \
    3rdparty/libmodbus/modbus.h

INCLUDEPATH += 3rdparty/libmodbus 3rdparty/qextserialport

unix:SOURCES += 3rdparty/qextserialport/posix_qextserialport.cpp	\
		3rdparty/qextserialport/qextserialenumerator_unix.cpp
unix:DEFINES += _TTY_POSIX_
win32:SOURCES += 3rdparty/qextserialport/win_qextserialport.cpp \
			3rdparty/qextserialport/qextserialenumerator_win.cpp
win32:DEFINES += _TTY_WIN_  WINVER=0x0501
win32:LIBS += -lsetupapi -lwsock32

FORMS += forms/mainwindow.ui \
    forms/about.ui

RESOURCES += data/qmodbus.qrc

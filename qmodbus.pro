# -------------------------------------------------
# Project created by QtCreator 2009-03-09T12:58:20
# -------------------------------------------------
TARGET = QModBus
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    qextserialport.cpp \
    qextserialenumerator.cpp \
    qextserialbase.cpp \
    modbus.c
HEADERS += mainwindow.h \
    qextserialport.h \
    qextserialenumerator.h \
    qextserialbase.h \
    modbus.h
unix:HEADERS += posix_qextserialport.h
unix:SOURCES += posix_qextserialport.cpp
unix:DEFINES += _TTY_POSIX_
win32:HEADERS += win_qextserialport.h
win32:SOURCES += win_qextserialport.cpp
win32:DEFINES += _TTY_WIN_
win32:LIBS += -lsetupapi
FORMS += mainwindow.ui \
    about.ui
RESOURCES += qmodbus.qrc

#-------------------------------------------------
#
# Project created by QtCreator 2017-01-08T22:53:17
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoteControlPc
TEMPLATE = app


SOURCES += main.cpp\
        ControlPannel.cpp \
    Network/Network.cpp \
    Network/protocol_parse.c

HEADERS  += ControlPannel.h \
    Network/Network.h \
    Network/protocol_parse.h

FORMS    +=

RESOURCES += \
    RES/res.qrc

include(./qtsingleapplication/qtsingleapplication.pri)

RC_FILE = ./RES/RES.rc

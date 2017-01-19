#-------------------------------------------------
#
# Project created by QtCreator 2017-01-08T22:53:17
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoteControlPc
TEMPLATE = app

INCLUDEPATH += ../public_define


SOURCES += main.cpp\
        ControlPannel.cpp \
    Network/Network.cpp \
    Network/CommandObject.cpp

HEADERS  += ControlPannel.h \
    Network/Network.h \
    Network/CommandObject.h \
    ../public_define/PUBLIC_DATA.h

FORMS    +=

RESOURCES += \
    RES/res.qrc

include(./qtsingleapplication/qtsingleapplication.pri)
include(../protocol_parse/protocol_parse.pri);

RC_FILE = ./RES/RES.rc

#-------------------------------------------------
#
# Project created by QtCreator 2017-01-10T03:02:19
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoteControlClient
TEMPLATE = app

INCLUDEPATH += ../public_define


SOURCES += main.cpp\
        RemoteControlClientGui.cpp

HEADERS  += RemoteControlClientGui.h \
    ../public_define/PUBLIC_DATA.h

FORMS    += RemoteControlClientGui.ui

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android-sources

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    RES/res.qrc

DISTFILES += \
    android-sources/AndroidManifest.xml

include(../protocol_parse/protocol_parse.pri);


#-------------------------------------------------
#
# Project created by QtCreator 2017-01-10T03:02:19
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoteControlClient
TEMPLATE = app


SOURCES += main.cpp\
        RemoteControlClientGui.cpp \
    protocol_parse.cpp

HEADERS  += RemoteControlClientGui.h \
    protocol_parse.h

FORMS    += RemoteControlClientGui.ui

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android-sources

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    RES/res.qrc

DISTFILES += \
    android-sources/AndroidManifest.xml


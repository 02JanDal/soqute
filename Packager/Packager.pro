#-------------------------------------------------
#
# Project created by QtCreator 2013-08-28T07:38:12
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Packager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    filegetters.cpp

HEADERS += \
    filegetters.h

include(../3rd-party/karchive.pri)

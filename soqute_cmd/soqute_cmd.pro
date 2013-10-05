#-------------------------------------------------
#
# Project created by QtCreator 2013-08-02T08:58:44
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = soqute_cmd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../soqute_core
LIBS += -llzma -L$$PWD/../../build-soqute-Desktop/soqute_core -lsoqute_core

debug {
    DEFINES += INSTALL_SCRIPT_DEBUGGER
    QT += widgets
}

include(../3rd-party/qcommandlineparser.pri)
include(../common.pri)

MOC_DIR = .moc
OBJECTS_DIR = .objects

SOURCES += main.cpp \
    basecommand.cpp \
    searchcommand.cpp \
    configcommand.cpp \
    showcommand.cpp \
    installcommand.cpp \
    util_cmd.cpp \
    textstream.cpp \
    removecommand.cpp

HEADERS += \
    basecommand.h \
    searchcommand.h \
    configcommand.h \
    showcommand.h \
    installcommand.h \
    util_cmd.h \
    textstream.h \
    removecommand.h

OTHER_FILES += ../status.txt

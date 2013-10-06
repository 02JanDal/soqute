#-------------------------------------------------
#
# Project created by QtCreator 2013-08-02T08:39:28
#
#-------------------------------------------------

QT       += network core qml

QT       -= gui

TARGET = soqute_core
VERSION = 0.0.1
TEMPLATE = lib

MOC_DIR = .moc
OBJECTS_DIR = .objects

include(../common.pri)
include(../3rd-party/karchive.pri)

DEFINES += SOQUTE_CORE_LIBRARY

SOURCES += \
    configurationhandler.cpp \
    dependencycalculator.cpp \
    package.cpp \
    packagematcher.cpp \
    abstractinstaller.cpp \
    installedpackages.cpp \
    util_core.cpp \
    downloader.cpp \
    loadmetadata.cpp \
    jsinstaller.cpp \
    remover.cpp

HEADERS +=\
        soqute_core_global.h \
    configurationhandler.h \
    dependencycalculator.h \
    package.h \
    packagematcher.h \
    abstractinstaller.h \
    installedpackages.h \
    util_core.h \
    downloader.h \
    loadmetadata.h \
    jsinstaller.h \
    remover.h

OTHER_FILES += ../status.txt

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE0C944A7
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = soqute_core.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}


#-------------------------------------------------
#
# Project created by QtCreator 2013-10-10T13:21:15
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = tester
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

OTHER_FILES += \
    test_meta.json

copy_meta.commands = cp $$PWD/test_meta.json $$shadowed($$PWD)/test_meta.json
copy_packages.commands = cp -r $$PWD/packages $$shadowed($$PWD)
QMAKE_EXTRA_TARGETS += copy_meta copy_packages
POST_TARGETDEPS += copy_meta copy_packages

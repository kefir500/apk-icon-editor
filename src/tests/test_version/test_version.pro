QT += testlib

TARGET = test_version
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += test_version.cpp \
           ../../updater.cpp

HEADERS += ../../updater.h

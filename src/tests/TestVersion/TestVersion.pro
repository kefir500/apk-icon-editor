#-------------------------------------------------
#
# Project created by QtCreator 2014-01-02T00:55:55
#
#-------------------------------------------------

QT       += widgets network testlib

TARGET = test_version
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += test_version.cpp \
           ../../updater.cpp

HEADERS += ../../updater.h

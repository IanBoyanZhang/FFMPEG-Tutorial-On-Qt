#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T10:49:39
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(./engine.pri)
LIBS += -lavformat  -lavcodec -lavutil -lz  -lswscale -lavutil -lm -llzma -lswresample

TARGET = engine
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T10:14:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tutorial01-Making-Screen-Caps
TEMPLATE = app
include(../engine/engine.pri)
LIBS += -lavformat  -lavcodec -lavutil -lz  -lswscale -lavutil -lm -llzma -lswresample


SOURCES += main.cpp\
        widget.cpp \

HEADERS  += widget.h \

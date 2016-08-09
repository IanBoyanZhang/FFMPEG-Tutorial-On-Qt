#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T10:01:37
#
#-------------------------------------------------

TEMPLATE = subdirs

SUBDIRS += \
    Tutorial01-Making-Screen-Caps \
    Tutorial02-Displaying-To-Screen \
    engine
SOURCES +=engine/ffmpegworker.cpp
HEADERS +=engine/ffmpegworker.h
LIBS += -lavformat  -lavcodec -lavutil -lz  -lswscale -lavutil -lm -llzma -lswresample



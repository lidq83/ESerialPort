#-------------------------------------------------
#
# Project created by QtCreator 2020-01-20T14:13:07
#
#-------------------------------------------------

QT       += core gui serialport

DESTDIR += release
MOC_DIR += release
OBJECTS_DIR += release
DEPENDPATH += release

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ESerialPort
TEMPLATE = app


SOURCES +=  src/main.cpp \
            src/widget.cpp

HEADERS  += src/widget.h

FORMS    += src/widget.ui

UI_DIR += ui

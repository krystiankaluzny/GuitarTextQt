#-------------------------------------------------
#
# Project created by QtCreator 2013-02-27T17:12:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GuitarText2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chord.cpp

HEADERS  += mainwindow.h \
    chord.h \
    mysimplelist.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc

CONFIG += c++11

#LIBS += -lmagic


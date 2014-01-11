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
    chord.cpp \
    treeitem.cpp \
    properties/treeitemdelegate.cpp \
    properties/propertiesdialog.cpp \
    properties/basemodel.cpp \
    search/mydirmodel.cpp \
    search/searchdialog.cpp \
    search/keybasemodel.cpp

HEADERS  += mainwindow.h \
    chord.h \
    mysimplelist.h \
    treeitem.h \
    properties/treeitemdelegate.h \
    properties/propertiesdialog.h \
    properties/basemodel.h \
    search/mydirmodel.h \
    search/searchdialog.h \
    search/keybasemodel.h

FORMS    += mainwindow.ui \
    properties/propertiesdialog.ui \
    search/searchdialog.ui

RESOURCES += \
    icons.qrc

CONFIG += c++11

#LIBS += -lmagic


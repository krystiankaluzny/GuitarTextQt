QT       += core gui
QT       -= opengl
QT       += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GuitarText
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp \
    chord.cpp \
    treeitem.cpp \
    properties/treeitemdelegate.cpp \
    properties/propertiesdialog.cpp \
    properties/basemodel.cpp \
    search/mydirmodel.cpp \
    search/searchdialog.cpp \
    search/keybasemodel.cpp \
    autohidingdialog.cpp \
    quick/quickoptions.cpp \
    mysharedata.cpp

HEADERS  += mainwindow.h \
    chord.h \
    mysimplelist.h \
    treeitem.h \
    properties/treeitemdelegate.h \
    properties/propertiesdialog.h \
    properties/basemodel.h \
    search/mydirmodel.h \
    search/searchdialog.h \
    search/keybasemodel.h \
    autohidingdialog.h \
    quick/quickoptions.h \
    mysharedata.h

FORMS    += mainwindow.ui \
    properties/propertiesdialog.ui \
    search/searchdialog.ui \
    quick/quickoptions.ui

RESOURCES += \
    icons.qrc

QMAKE_CXXFLAGS += -std=c++0x
CXXFLAGS="-std=c++0x"

CONFIG += c++0x
CONFIG += -std=c++11

#Linux config
#LIBS += -lGL

#windows config
#INCLUDEPATH += "C:/boost_1_54_0"
#LIBS += -L"C:/boost_1_54_0/bin.v2/libs"

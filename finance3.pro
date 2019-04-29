#-------------------------------------------------
#
# Project created by QtCreator 2019-03-17T19:12:45
#
#-------------------------------------------------

QT       += core gui

# where is C++17 support???
#QMAKE_CXXFLAGS = -std=c++17
CONFIG += c++17


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = finance3
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    finance.cpp \
    helpers.cpp \
    financetablewidget.cpp \
    tablecontextmenu.cpp

HEADERS += \
    finance.h \
    helpers.h \
    financetablewidget.h \
    tablecontextmenu.h

FORMS += \
    finance.ui

RESOURCES += \
    cursors.qrc

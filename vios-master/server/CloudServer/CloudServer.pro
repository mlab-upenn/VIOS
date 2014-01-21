#-------------------------------------------------
#
# Project created by QtCreator 2013-04-17T11:36:23
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CloudServer
TEMPLATE = app

INCLUDEPATH += `pkg-config opencv --cflags`

LIBS += `pkg-config opencv --libs`

SOURCES += main.cpp\
        servergui.cpp \
    thread.cpp \
    server.cpp

HEADERS  += servergui.h \
    thread.h \
    server.h

FORMS    += servergui.ui

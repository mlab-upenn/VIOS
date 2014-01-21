#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T21:11:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CloudServer
TEMPLATE = app

INCLUDEPATH += `pkg-config opencv --cflags`

LIBS += `pkg-config opencv --libs`

SOURCES += main.cpp\
        server.cpp \
    thread.cpp \
    viosserver.cpp

HEADERS  += server.h \
    thread.h \
    viosserver.h

FORMS    += server.ui

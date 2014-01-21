#-------------------------------------------------
#
# Project created by QtCreator 2013-04-24T21:05:28
#
#-------------------------------------------------

QT       += core gui phonon xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ViosPlayer
TEMPLATE = app

#INCLUDEPATH +=

LIBS += -lwiiuse

SOURCES += main.cpp\
        viosplayer.cpp \
    wiiwrapper.cpp \
    dataparser.cpp \
    cloudclient.cpp

HEADERS  += viosplayer.h \
    wiiwrapper.h \
    dataparser.h \
    cloudclient.h

FORMS    += viosplayer.ui

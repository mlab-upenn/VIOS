TEMPLATE = app
TARGET = customvideowidget

QT += QtMultimediaKit multimediawidgets widgets core gui -lfl
QMAKE_CXXFLAGS += -I/usr/include/qt4/QtMultimediaKit -I/usr/include/qt4/QtMobility -lQtMultimediaKit

HEADERS = \
    videoplayer.h \
    videowidget.h \
    videowidgetsurface.h

SOURCES = \
    main.cpp \
    videoplayer.cpp \
    videowidget.cpp \
    videowidgetsurface.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/multimediawidgets/customvideosurface/customvideowidget
INSTALLS += target

#-------------------------------------------------
#
# Project created by QtCreator 2017-03-21T14:37:35
#
#-------------------------------------------------

#QT       -= gui
QT       += widgets network sql

TARGET = UMapControl
TEMPLATE = lib
DESTDIR = ../bin
DEFINES += UMapControl_LIBRARY

SOURCES += UMapControl.cpp \
    Map.cpp \
    Network.cpp \
    ILoveChina.cpp \
    SQLExcute.cpp \
    Manager.cpp \
    Layer.cpp \
    Geometry.cpp \
    GeoPie.cpp \
    GeoRect.cpp \
    GeoCircle.cpp \
    GeoStar.cpp \
    GeoTri.cpp \
    GeoPolygon.cpp \
    MapPix.cpp

HEADERS += UMapControl.h\
        umapcontrol_global.h \
    Map.h \
    Network.h \
    ILoveChina.h \
    SQLExcute.h \
    Manager.h \
    Layer.h \
    Geometry.h \
    GeoPie.h \
    GeoRect.h \
    GeoCircle.h \
    GeoStar.h \
    GeoTri.h \
    GeoPolygon.h \
    MapPix.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=

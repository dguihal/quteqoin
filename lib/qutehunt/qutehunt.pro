#-------------------------------------------------
#
# Project created by QtCreator 2013-08-16T23:21:08
#
#-------------------------------------------------

QT       += gui

TARGET = qutehunt
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
	qqduck.cpp \
	qqhuntingview.cpp \
	qqduckpixmapitem.cpp

HEADERS += \
	qqduck.h \
	qqhuntingview.h \
	qqduckpixmapitem.h

RESOURCES += \
	rc/animations.qrc

INCLUDEPATH += \
			$$PWD/../qutetools

DEPENDPATH += \
			$$PWD/../qutetools

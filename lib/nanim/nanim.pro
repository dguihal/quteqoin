#-------------------------------------------------
#
# Project created by QtCreator 2013-06-07T22:16:05
#
#-------------------------------------------------

QT       -= core gui

TARGET = nanim
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../../ext/nanopb

SOURCES += nanim.pb.c

HEADERS  += nanim.pb.h

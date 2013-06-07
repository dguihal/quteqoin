#-------------------------------------------------
#
# Project created by QtCreator 2013-06-07T22:16:05
#
#-------------------------------------------------

QT       -= core gui

TARGET = nanopb
TEMPLATE = lib
CONFIG += staticlib

SOURCES += pb_decode.c\
	pb_encode.c

HEADERS  += pb.h \
	pb_decode.h \
	pb_encode.h

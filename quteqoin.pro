#-------------------------------------------------
#
# Project created by QtCreator 2011-02-23T21:27:41
#
#-------------------------------------------------

QT += core gui network xml

TARGET = quteqoin
TEMPLATE = app

#Pour activer stdout sur la console windows
#CONFIG += console

SOURCES += main.cpp\
	mainwindow.cpp \
	core/qqsettings.cpp \
	core/qqbouchot.cpp \
	core/qqpost.cpp \
	ui/qqpalmipede.cpp \
	ui/qqpinipede.cpp \
	ui/qqbouchotsettingsdialog.cpp \
	ui/qqsettingsdialog.cpp \
	ui/qqtextbrowser.cpp \
	xml/qqxmlparser.cpp \
	core/qqnorloge.cpp \
	ui/qqsyntaxhighlighter.cpp \
	ui/qqmessageblockuserdata.cpp \
	core/qqnorlogeref.cpp \
	ui/qqproxyauthdialog.cpp \
	core/qqtotoz.cpp \
	core/qqtotozmanager.cpp \
	core/qqnetworkaccessor.cpp \
	ui/qqpalmilineedit.cpp \
	core/qqpurgebouchothistoevent.cpp \
	core/qqbigornoitem.cpp

HEADERS  += mainwindow.h \
	core/qqsettings.h \
	core/qqbouchot.h \
	core/qqpost.h \
	ui/qqpalmipede.h \
	ui/qqpinipede.h \
	ui/qqbouchotsettingsdialog.h \
	ui/qqsettingsdialog.h \
	ui/qqtextbrowser.h \
	xml/qqxmlparser.h \
	core/qqnorloge.h \
	ui/qqsyntaxhighlighter.h \
	ui/qqmessageblockuserdata.h \
	core/qqnorlogeref.h \
	ui/qqproxyauthdialog.h \
	core/qqtotoz.h \
	core/qqtotozmanager.h \
	core/qqnetworkaccessor.h \
	ui/qqpalmilineedit.h \
	core/qqpurgebouchothistoevent.h \
	core/qqbigornoitem.h

FORMS    += mainwindow.ui \
	ui/qqpalmipede.ui \
	ui/qqbouchotsettingsdialog.ui \
	ui/qqsettingsdialog.ui \
	ui/qqproxyauthdialog.ui

OTHER_FILES +=

RESOURCES += \
	rc/tribunesDef.qrc

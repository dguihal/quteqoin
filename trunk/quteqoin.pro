#-------------------------------------------------
#
# Project created by QtCreator 2011-02-23T21:27:41
#
#-------------------------------------------------

QT += core gui network xml

TARGET = quteqoin
TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
	core/qqsettings.cpp \
	core/qqbouchot.cpp \
	core/qqpost.cpp \
	ui/qqpalmipede.cpp \
	ui/qqpinipede.cpp \
	ui/qqsettingsdialog.cpp \
	ui/qqtextbrowser.cpp \
	xml/qqxmlparser.cpp \
	core/qqnorloge.cpp \
	ui/qqsyntaxhighlighter.cpp \
	ui/qqmessageblockuserdata.cpp \
	core/qqnorlogeref.cpp \
	ui/qqproxyauthdialog.cpp \
	core/qqtotoz.cpp \
	core/qqtotozdownloader.cpp \
	core/qqnetworkaccessor.cpp \
	ui/qqpalmilineedit.cpp \
	core/qqpurgebouchothistoevent.cpp \
	core/qqbigornoitem.cpp \
	ui/qqtotozmanager.cpp \
	core/totozmanager/qqtmrequester.cpp \
	ui/qqtotozviewer.cpp \
	xml/totozmanager/qqtmxmlparser.cpp \
	ui/qqsettingsmanager.cpp \
	ui/settingsmanager/qqtotozsettings.cpp \
	ui/settingsmanager/qqgeneralsettings.cpp \
	ui/settingsmanager/qqpalmisettings.cpp \
	ui/settingsmanager/qqboardssettings.cpp \
	ui/settingsmanager/qqbouchotsettingsdialog.cpp \
	ui/settingsmanager/qqpalmisettingstablewidget.cpp

HEADERS  += mainwindow.h \
	core/qqsettings.h \
	core/qqbouchot.h \
	core/qqpost.h \
	ui/qqpalmipede.h \
	ui/qqpinipede.h \
	ui/qqsettingsdialog.h \
	ui/qqtextbrowser.h \
	xml/qqxmlparser.h \
	core/qqnorloge.h \
	ui/qqsyntaxhighlighter.h \
	ui/qqmessageblockuserdata.h \
	core/qqnorlogeref.h \
	ui/qqproxyauthdialog.h \
	core/qqtotoz.h \
	core/qqtotozdownloader.h \
	core/qqnetworkaccessor.h \
	ui/qqpalmilineedit.h \
	core/qqpurgebouchothistoevent.h \
	core/qqbigornoitem.h \
	ui/qqtotozmanager.h \
	core/totozmanager/qqtmrequester.h \
	ui/qqtotozviewer.h \
	xml/totozmanager/qqtmxmlparser.h \
	core/qqsettingsparams.h \
	ui/qqsettingsmanager.h \
	ui/settingsmanager/qqgeneralsettings.h \
	ui/settingsmanager/qqboardssettings.h \
	ui/settingsmanager/qqpalmisettings.h \
	ui/settingsmanager/qqtotozsettings.h \
	ui/settingsmanager/qqbouchotsettingsdialog.h \
	ui/settingsmanager/qqpalmisettingstablewidget.h

FORMS    += mainwindow.ui \
	ui/qqpalmipede.ui \
	ui/qqsettingsdialog.ui \
	ui/qqproxyauthdialog.ui \
	ui/qqtotozmanager.ui \
	ui/qqsettingsmanager.ui \
	ui/settingsmanager/qqboardssettings.ui \
	ui/settingsmanager/qqbouchotsettingsdialog.ui \
	ui/settingsmanager/qqgeneralsettings.ui \
	ui/settingsmanager/qqpalmisettings.ui \
	ui/settingsmanager/qqtotozsettings.ui

OTHER_FILES +=

RESOURCES += \
    rc/quteqoin_img.qrc \
    rc/quteqoin_defs.qrc

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_DEBUG_OUTPUT

unix {
	FILESTOCOPY = quteqoin
	INSTRULE.path = $$PREFIX/bin
	INSTRULE.files = $$FILESTOCOPY
	INSTALLS += INSTRULE
}

win32:debug {
	CONFIG += console
}



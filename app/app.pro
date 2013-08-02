#-------------------------------------------------
#
# Project created by QtCreator 2011-02-23T21:27:41
#
#-------------------------------------------------

QT += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

TARGET = quteqoin
TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
	core/qqsettings.cpp \
	core/qqbouchot.cpp \
	core/qqpost.cpp \
	ui/qqpalmipede.cpp \
	ui/qqpinipede.cpp \
	ui/qqtextbrowser.cpp \
	xml/qqxmlparser.cpp \
	core/qqnorloge.cpp \
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
	ui/settingsmanager/qqpalmisettingstablewidget.cpp \
	core/qqpiniurlhelper.cpp \
	core/qqmessagetransformfilter.cpp \
	ui/settingsmanager/qqfiltersettings.cpp \
	core/qqbackendupdatedevent.cpp \
	ui/qqpostparser.cpp \
	ui/qqpinisearchwidget.cpp \
	core/qqpostdisplayfilter.cpp \
	core/qqsimplepostdisplayfilter.cpp

HEADERS  += mainwindow.h \
	core/qqsettings.h \
	core/qqbouchot.h \
	core/qqpost.h \
	ui/qqpalmipede.h \
	ui/qqpinipede.h \
	ui/qqtextbrowser.h \
	xml/qqxmlparser.h \
	core/qqnorloge.h \
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
	ui/settingsmanager/qqpalmisettingstablewidget.h \
	core/qqpiniurlhelper.h \
	core/qqmessagetransformfilter.h \
	ui/settingsmanager/qqfiltersettings.h \
	core/qqtypes.h \
	core/qqbackendupdatedevent.h \
	ui/qqpostparser.h \
	ui/qqpinisearchwidget.h \
	core/qqpostdisplayfilter.h \
	core/qqsimplepostdisplayfilter.h

FORMS += mainwindow.ui \
	ui/qqpalmipede.ui \
	ui/qqproxyauthdialog.ui \
	ui/qqtotozmanager.ui \
	ui/qqsettingsmanager.ui \
	ui/settingsmanager/qqboardssettings.ui \
	ui/settingsmanager/qqbouchotsettingsdialog.ui \
	ui/settingsmanager/qqgeneralsettings.ui \
	ui/settingsmanager/qqpalmisettings.ui \
	ui/settingsmanager/qqtotozsettings.ui \
	ui/settingsmanager/qqfiltersettings.ui \
	ui/qqpinisearchwidget.ui

OTHER_FILES +=

RESOURCES += \
	rc/quteqoin_img.qrc \
	rc/quteqoin_defs.qrc

QMAKE_CXXFLAGS_RELEASE += -DQT_NO_DEBUG_OUTPUT

unix {
	isEmpty(PREFIX) {
		 PREFIX = /usr/local
	}

	target.path = $$PREFIX/bin
	target.files = quteqoin

	desktop.path = $$PREFIX/share/applications
	desktop.files += quteqoin.desktop

	icon.path = $$PREFIX/share/icons/hicolor/256x256/apps/
	icon.files += QuteQoin-Icon.png

	INSTALLS += target desktop icon
}

INCLUDEPATH += $$PWD/../ext/nanopb \
			$$PWD/../lib/nanim \
			$$PWD/../lib/qutetools

DEPENDPATH += $$PWD/../ext/nanopb \
			$$PWD/../lib/nanim \
			$$PWD/../lib/qutetools

win32 {
		RC_FILE = rc/quteqoin_win.rc
		debug {
			CONFIG += console
			LIBS += -L$$OUT_PWD/../ext/nanopb/debug/ -lnanopb\
					-L$$OUT_PWD/../lib/nanim/debug/ -lnanim \
					-L$$OUT_PWD/../lib/qutetools/debug/ -lqutetools

			PRE_TARGETDEPS += $$OUT_PWD/../ext/nanopb/debug/nanopb.lib \
							$$OUT_PWD/../lib/nanim/debug/nanim.lib \
							$$OUT_PWD/../lib/qutetools/debug/qutetools.lib

		} else {
			LIBS += ../ext/nanopb/release/libnanopb.a -lnanopb \
					../lib/nanim/release/libnanim.a -lnanim \
					-L$$OUT_PWD/../lib/qutetools/release/ -lqutetools
			PRE_TARGETDEPS += $$OUT_PWD/../ext/nanopb/release/nanopb.lib \
							$$OUT_PWD/../lib/nanim/release/nanim.lib \
							$$OUT_PWD/../lib/qutetools/release/qutetools.lib
		}
} else {
	LIBS += -L$$OUT_PWD/../ext/nanopb/libnanopb.a \
			-L$$OUT_PWD/../lib/nanim/libnanim.a \
			-L$$OUT_PWD/../lib/qutetools/ -lqutetools
	PRE_TARGETDEPS += $$OUT_PWD/../ext/nanopb/libnanopb.a \
					$$OUT_PWD/../lib/nanim/libnanim.a \
					$$OUT_PWD/../lib/qutetools/libqutetools.a
}

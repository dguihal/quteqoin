#-------------------------------------------------
#
# Project created by QtCreator 2011-02-23T21:27:41
#
#-------------------------------------------------

#version check qt
contains(QT_VERSION, ^4\\.[0-7]\\..*) {
	message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
	error("Use at least Qt 4.8.")
}

QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}


# A Tester
#linux-g++ {
#    system( g++ --version | grep -e "\<4.[8-9]" ) {
#        message( g++ version > 4.8found )
#        QMAKE_CXXFLAGS_DEBUG += -fsanitize=address -Og
#    }
#}
#QMAKE_CXXFLAGS_RELEASE += -O2

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
	ui/palmipede/qqpalmilineedit.cpp \
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
	core/qqsimplepostdisplayfilter.cpp \
	ui/qqhuntingview.cpp \
	ui/qqduckpixmapitem.cpp \
	core/qutetools.cpp \
	core/qqhuntpixmapitem.cpp \
	ui/settingsmanager/qqhuntsettings.cpp \
	ui/qqboardsinfo.cpp \
	ui/qqboardinfo.cpp \
	core/qqmussel.cpp \
	ui/qqmusselinfo.cpp \
	ui/qqcmdtoolbuttons.cpp \
	ui/qqimageviewer.cpp \
	ui/qqwebimageviewer.cpp \
	core/qqwebimagedownloader.cpp \
	core/qqbakdisplayfilter.cpp \
	ui/palmipede/qqpalmifileposter.cpp \
	core/qqboardstatechangeevent.cpp \
	ui/boardinfo/qqboardinfoprogressbar.cpp \
	ui/settingsmanager/qqboardwizard.cpp \
	ui/settingsmanager/qqboardwizardintro.cpp \
	ui/settingsmanager/qqboardwizardnative.cpp \
	ui/settingsmanager/qqboardwizardolccs.cpp

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
	ui/palmipede/qqpalmilineedit.h \
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
	core/qqsimplepostdisplayfilter.h \
	ui/qqhuntingview.h \
	ui/qqduckpixmapitem.h \
	core/qutetools.h \
	core/qqhuntpixmapitem.h \
	ui/settingsmanager/qqhuntsettings.h \
	ui/qqboardsinfo.h \
	ui/qqboardinfo.h \
	core/qqmussel.h \
	ui/qqmusselinfo.h \
	ui/qqcmdtoolbuttons.h \
	ui/qqimageviewer.h \
	ui/qqwebimageviewer.h \
	core/qqwebimagedownloader.h \
	core/qqbakdisplayfilter.h \
	ui/palmipede/qqpalmifileposter.h \
	core/qqboardstatechangeevent.h \
	ui/boardinfo/qqboardinfoprogressbar.h \
	ui/settingsmanager/qqboardwizard.h \
	ui/settingsmanager/qqboardwizardintro.h \
	ui/settingsmanager/qqboardwizardnative.h \
	ui/settingsmanager/qqboardwizardolccs.h

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
	ui/qqpinisearchwidget.ui \
	ui/settingsmanager/qqhuntsettings.ui \
	ui/qqboardsinfo.ui \
	ui/qqboardinfo.ui \
	ui/qqcmdtoolbuttons.ui

RESOURCES += \
	rc/quteqoin_img.qrc \
	rc/quteqoin_defs.qrc \
	rc/quteqoin_anims.qrc

debug {
	greaterThan(QT_MAJOR_VERSION, 4) {
		QT += quick

		SOURCES += qml/documenthandler.cpp

		HEADERS += qml/documenthandler.h

		RESOURCES += qml/quteqoin_qml.qrc

		OTHER_FILES += \
			qml/QQmlMain.qml \
			qml/QQmlPinni.qml \
			qml/QQmlPalmi.qml \
			qml/QQmlGeneralSettings.qml \
			qml/QQmlSettingsEditor.qml \
			qml/QQmlSettingsItem.qml \
			qml/QQmlSettingsItemMenuBtn.qml \
			qml/QQmlTotozSettings.qml
	}
}

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

	CONFIG += link_pkgconfig
	PKGCONFIG += libnotify
}

win32 {
	RC_FILE = rc/quteqoin_win.rc
}

# vim: ts=4 sw=4 sts=4 noexpandtab

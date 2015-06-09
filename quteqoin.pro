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

QT += core network xml
equals(QT_MAJOR_VERSION, 4) {
	QT += phonon
}
equals(QT_MAJOR_VERSION, 5) {
	QT += widgets multimediawidgets
	greaterThan(QT_MINOR_VERSION, 3) {
		QT += quickwidgets
	}
}

# A Tester
#linux-g++ {
#	system( g++ --version | grep -e "\<4.[8-9]" ) {
#		message( g++ version > 4.8found )
#		QMAKE_CXXFLAGS_DEBUG += -fsanitize=address -Og
#	}
#}
#QMAKE_CXXFLAGS_RELEASE += -O2

TARGET = quteqoin
TEMPLATE = app

SOURCES += main.cpp\
	mainwindow.cpp \
	core/qqbouchot.cpp \
	core/qqpost.cpp \
	core/qqsettings.cpp \
	core/qqboardstatechangeevent.cpp \
	core/qqnorloge.cpp \
	core/qqwebimagedownloader.cpp \
	core/qqbakdisplayfilter.cpp \
	core/qqnorlogeref.cpp \
	core/qqtotoz.cpp \
	core/qqtotozdownloader.cpp \
	core/qqnetworkaccessor.cpp \
	core/qqmussel.cpp \
	core/qutetools.cpp \
	core/qqsimplepostdisplayfilter.cpp \
	core/qqbackendupdatedevent.cpp \
	core/qqpiniurlhelper.cpp \
	core/qqmessagetransformfilter.cpp \
	core/qqpurgebouchothistoevent.cpp \
	core/qqbigornoitem.cpp \
	core/totozmanager/qqtmrequester.cpp \
	core/parsers/qqbackendparser.cpp \
	core/parsers/qqtsvparser.cpp \
	core/parsers/qqxmlparser.cpp \
	ui/qqmessageblockuserdata.cpp \
	ui/qqproxyauthdialog.cpp \
	ui/qqtotozmanager.cpp \
	ui/qqtotozviewer.cpp \
	ui/qqsettingsmanager.cpp \
	ui/qqpostparser.cpp \
	ui/qqpinisearchwidget.cpp \
	ui/qqboardsinfo.cpp \
	ui/qqboardinfo.cpp \
	ui/qqmusselinfo.cpp \
	ui/qqcmdtoolbuttons.cpp \
	ui/qqimageviewer.cpp \
	ui/boardinfo/qqboardinfoprogressbar.cpp \
	ui/qqpalmipede.cpp \
	ui/palmipede/qqpalmilineedit.cpp \
	ui/palmipede/qqpalmifileposter.cpp \
	ui/palmipede/qqdockpalmi.cpp \
	ui/qqpinipede.cpp \
	ui/pinipede/qqduckpixmapitem.cpp \
	ui/pinipede/qqhuntingview.cpp \
	ui/pinipede/qqhuntpixmapitem.cpp \
	ui/pinipede/qqpinioverlay.cpp \
	ui/pinipede/qqtextbrowser.cpp \
	ui/pinipede/qqwebimageviewer.cpp \
	ui/settingsmanager/qqboardssettings.cpp \
	ui/settingsmanager/qqbouchotsettingsdialog.cpp \
	ui/settingsmanager/qqboardwizard.cpp \
	ui/settingsmanager/qqboardwizardintro.cpp \
	ui/settingsmanager/qqboardwizardnative.cpp \
	ui/settingsmanager/qqboardwizardolccs.cpp \
	ui/settingsmanager/qqfiltersettings.cpp \
	ui/settingsmanager/qqgeneralsettings.cpp \
	ui/settingsmanager/qqhuntsettings.cpp \
	ui/settingsmanager/qqnetworksettings.cpp \
	ui/settingsmanager/qqpalmisettings.cpp \
	ui/settingsmanager/qqpalmisettingstablewidget.cpp \
	ui/settingsmanager/qqtotozsettings.cpp \
	ui/totozmanager/qqtmxmlparser.cpp

HEADERS  += mainwindow.h \
	core/qqsettings.h \
	core/qqbouchot.h \
	core/qqpost.h \
	core/qqnorloge.h \
	core/qqnorlogeref.h \
	core/qqtotoz.h \
	core/qqtotozdownloader.h \
	core/qqpurgebouchothistoevent.h \
	core/qqbigornoitem.h \
	core/qqnetworkaccessor.h \
	core/qqsettingsparams.h \
	core/qqpiniurlhelper.h \
	core/qqmessagetransformfilter.h \
	core/qqtypes.h \
	core/qqbackendupdatedevent.h \
	core/totozmanager/qqtmrequester.h \
	core/qqwebimagedownloader.h \
	core/qqbakdisplayfilter.h \
	core/qqboardstatechangeevent.h \
	core/qqmussel.h \
	core/qqpostdisplayfilter.h \
	core/qqsimplepostdisplayfilter.h \
	core/qutetools.h \
	core/parsers/qqbackendparser.h \
	core/parsers/qqtsvparser.h \
	core/parsers/qqxmlparser.h \
	ui/qqmessageblockuserdata.h \
	ui/qqproxyauthdialog.h \
	ui/qqtotozmanager.h \
	ui/qqtotozviewer.h \
	ui/qqsettingsmanager.h \
	ui/qqpostparser.h \
	ui/qqpinisearchwidget.h \
	ui/qqboardsinfo.h \
	ui/qqboardinfo.h \
	ui/qqmusselinfo.h \
	ui/qqcmdtoolbuttons.h \
	ui/qqimageviewer.h \
	ui/boardinfo/qqboardinfoprogressbar.h \
	ui/qqpalmipede.h \
	ui/palmipede/qqpalmifileposter.h \
	ui/palmipede/qqpalmilineedit.h \
	ui/palmipede/qqdockpalmi.h \
	ui/qqpinipede.h \
	ui/pinipede/qqduckpixmapitem.h \
	ui/pinipede/qqhuntingview.h \
	ui/pinipede/qqhuntpixmapitem.h \
	ui/pinipede/qqpinioverlay.h \
	ui/pinipede/qqtextbrowser.h \
	ui/pinipede/qqwebimageviewer.h \
	ui/settingsmanager/qqboardssettings.h \
	ui/settingsmanager/qqbouchotsettingsdialog.h \
	ui/settingsmanager/qqboardwizard.h \
	ui/settingsmanager/qqboardwizardintro.h \
	ui/settingsmanager/qqboardwizardnative.h \
	ui/settingsmanager/qqboardwizardolccs.h \
	ui/settingsmanager/qqfiltersettings.h \
	ui/settingsmanager/qqgeneralsettings.h \
	ui/settingsmanager/qqhuntsettings.h \
	ui/settingsmanager/qqnetworksettings.h \
	ui/settingsmanager/qqpalmisettings.h \
	ui/settingsmanager/qqpalmisettingstablewidget.h \
	ui/settingsmanager/qqtotozsettings.h \
	ui/totozmanager/qqtmxmlparser.h

FORMS += mainwindow.ui \
	ui/qqboardsinfo.ui \
	ui/qqboardinfo.ui \
	ui/qqcmdtoolbuttons.ui \
	ui/qqpalmipede.ui \
	ui/qqpinisearchwidget.ui \
	ui/qqproxyauthdialog.ui \
	ui/qqsettingsmanager.ui \
	ui/qqtotozmanager.ui \
	ui/settingsmanager/qqboardssettings.ui \
	ui/settingsmanager/qqbouchotsettingsdialog.ui \
	ui/settingsmanager/qqfiltersettings.ui \
	ui/settingsmanager/qqgeneralsettings.ui \
	ui/settingsmanager/qqhuntsettings.ui \
	ui/settingsmanager/qqnetworksettings.ui \
	ui/settingsmanager/qqpalmisettings.ui \
	ui/settingsmanager/qqtotozsettings.ui

RESOURCES += \
	rc/quteqoin_img.qrc \
	rc/quteqoin_defs.qrc \
	rc/quteqoin_anims.qrc

CONFIG(QML_PALMI) {
	equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 3) {

		QT += quick

		DEFINES += QML_PALMI

		SOURCES += qml/documenthandler.cpp

		HEADERS += qml/documenthandler.h

		RESOURCES += qml/quteqoin_qml.qrc

		OTHER_FILES += \
			qml/QQmlMain.qml \
			qml/QQmlNetworkSettings.qml \
			qml/QQmlPinni.qml \
			qml/QQmlPalmi.qml \
			qml/QQmlGeneralSettings.qml \
			qml/QQmlSettingsEditor.qml \
			qml/QQmlSettingsItem.qml \
			qml/QQmlSettingsItemMenuBtn.qml \
			qml/QQmlTotozSettings.qml
	}
	else {
		message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
		error("Use at least Qt 5.4.")
	}
}

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

		CONFIG(debug, debug|release) {
			CONFIG += console
		}
}

# vim: ts=4 sw=4 sts=4 noexpandtab

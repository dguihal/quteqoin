#-------------------------------------------------
#
# Project created by QtCreator 2011-02-23T21:27:41
#
#-------------------------------------------------

#version check qt
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 4) {
    message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.4 .")
}

QT += core network xml widgets multimediawidgets

include($$PWD/gitversion.pri)

# A Tester
linux-g++ {
    QMAKE_CXXFLAGS_DEBUG += -fsanitize=address -Og
    QMAKE_LFLAGS_DEBUG += -fsanitize=address
}

QMAKE_CXXFLAGS_RELEASE += -O2

TARGET = quteqoin
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    core/qqbouchot.cpp \
    core/qqpost.cpp \
    core/qqsettings.cpp \
    core/qqboardstatechangeevent.cpp \
    core/qqnorloge.cpp \
    core/qqwebdownloader.cpp \
    core/qqbakdisplayfilter.cpp \
    core/qqnorlogeref.cpp \
    core/qqtotoz.cpp \
    core/qqtotozdownloader.cpp \
    core/qqnetworkaccessor.cpp \
    core/qqmussel.cpp \
    core/qutetools.cpp \
    core/qqsimplepostdisplayfilter.cpp \
    core/qqbackendupdatedevent.cpp \
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
    ui/qqpinisearchwidget.cpp \
    ui/qqboardsinfo.cpp \
    ui/qqboardinfo.cpp \
    ui/qqmusselinfo.cpp \
    ui/qqcmdtoolbuttons.cpp \
    ui/qqimageviewer.cpp \
    ui/boardinfo/qqboardinfoprogressbar.cpp \
    ui/qqpalmipede.cpp \
    ui/palmipede/qqpalmifileposter.cpp \
    ui/palmipede/qqpalmilineedit.cpp \
    ui/palmipede/qqpalmilineeditint.cpp \
    ui/palmipede/qqdockpalmi.cpp \
    ui/qqpinipede.cpp \
    ui/pinipede/qqduckpixmapitem.cpp \
    ui/pinipede/qqhuntingview.cpp \
    ui/pinipede/qqhuntpixmapitem.cpp \
    ui/pinipede/qqpinioverlay.cpp \
    ui/pinipede/qqpiniurlhelper.cpp \
    ui/pinipede/qqpostparser.cpp \
    ui/pinipede/qqtextbrowser.cpp \
    ui/pinipede/qqwebimageviewer.cpp \
    ui/settingsmanager/qqboardssettings.cpp \
    ui/settingsmanager/qqbouchotsettingsdialog.cpp \
    ui/settingsmanager/qqboardwizard.cpp \
    ui/settingsmanager/qqboardwizardnative.cpp \
    ui/settingsmanager/qqfiltersettings.cpp \
    ui/settingsmanager/qqgeneralsettings.cpp \
    ui/settingsmanager/qqhuntsettings.cpp \
    ui/settingsmanager/qqnetworksettings.cpp \
    ui/settingsmanager/qqpalmisettings.cpp \
    ui/settingsmanager/qqpalmisettingstablewidget.cpp \
    ui/settingsmanager/qqtotozsettings.cpp \
    ui/totozmanager/qqtmxmlparser.cpp \
    ui/totozmanager/qqtotozmanagerheader.cpp

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
    core/qqmessagetransformfilter.h \
    core/qqtypes.h \
    core/qqbackendupdatedevent.h \
    core/totozmanager/qqtmrequester.h \
    core/qqwebdownloader.h \
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
    ui/palmipede/qqpalmilineeditint.h \
    ui/palmipede/qqdockpalmi.h \
    ui/qqpinipede.h \
    ui/pinipede/qqduckpixmapitem.h \
    ui/pinipede/qqhuntingview.h \
    ui/pinipede/qqhuntpixmapitem.h \
    ui/pinipede/qqpinioverlay.h \
    ui/pinipede/qqpiniurlhelper.h \
    ui/pinipede/qqpostparser.h \
    ui/pinipede/qqtextbrowser.h \
    ui/pinipede/qqwebimageviewer.h \
    ui/settingsmanager/qqboardssettings.h \
    ui/settingsmanager/qqbouchotsettingsdialog.h \
    ui/settingsmanager/qqboardwizard.h \
    ui/settingsmanager/qqboardwizardnative.h \
    ui/settingsmanager/qqfiltersettings.h \
    ui/settingsmanager/qqgeneralsettings.h \
    ui/settingsmanager/qqhuntsettings.h \
    ui/settingsmanager/qqnetworksettings.h \
    ui/settingsmanager/qqpalmisettings.h \
    ui/settingsmanager/qqpalmisettingstablewidget.h \
    ui/settingsmanager/qqtotozsettings.h \
    ui/totozmanager/qqtmxmlparser.h \
    ui/totozmanager/qqtotozmanagerheader.h

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
    ui/settingsmanager/qqtotozsettings.ui \
    ui/totozmanager/qqtotozmanagerheader.ui

RESOURCES += \
    rc/quteqoin_img.qrc \
    rc/quteqoin_defs.qrc \
    rc/quteqoin_anims.qrc

CONFIG(QML_PALMI) {
    QT += quick quickwidgets

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

	metainfo.path = $$PREFIX/share/metainfo/
	metainfo.files += eu.ototu.quteqoin.metainfo.xml

    INSTALLS += target desktop icon metainfo

    QT += dbus
}

win32 {
    RC_FILE = rc/quteqoin_win.rc

    CONFIG(debug, debug|release) {
        CONFIG += console
    }
}

# vim: ts=4 sw=4 sts=4 noexpandtab

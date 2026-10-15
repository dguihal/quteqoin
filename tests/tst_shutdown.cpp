#include "mainwindow.h"
#include "core/qqsettings.h"
#include "ui/pinipede/qqpinioverlay.h"
#include "ui/pinipede/qqtextbrowser.h"

#include <QApplication>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTemporaryDir>
#include <QTimer>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("QuteqoinShutdownTests");
	QCoreApplication::setApplicationName("QuteqoinShutdownTests");
	QStandardPaths::setTestModeEnabled(true);
	QTemporaryDir settingsDir;
	if(!settingsDir.isValid())
		return 1;
	QSettings::setDefaultFormat(QSettings::IniFormat);
	QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
	QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settingsDir.path());
	QTcpServer server;
	if(!server.listen(QHostAddress::LocalHost))
		return 2;
	const QString url = QString("http://127.0.0.1:%1/board").arg(server.serverPort());
	QQSettings settings;
	settings.setValue(SETTINGS_NETWORK_MODE, SETTINGS_NETWORK_MODE_DIRECT);
	settings.setValue(SETTINGS_LIST_BOUCHOTS, "test");
	settings.setValue("bouchot/test/backendUrl", url);
	settings.setValue("bouchot/test/postUrl", url);
	settings.setValue("bouchot/test/group", "test");
	settings.setValue("bouchot/test/color", "#123456");
	settings.sync();
	MainWindow window;
	window.show();
	const QString preview = app.arguments().value(1, "none");
	QTimer::singleShot(0, &window, [&window, preview, url] {
		if(preview == "none")
			return;
		auto *browser = window.findChild<QQTextBrowser *>();
		auto *overlay = window.findChild<QQPiniOverlay *>();
		if(browser == nullptr || overlay == nullptr)
		{
			QCoreApplication::exit(3);
			return;
		}
		QMetaObject::invokeMethod(browser, "onAnchorHighlighted", Qt::DirectConnection,
		                          Q_ARG(QUrl, QUrl("duck://test")));
		QString contentType = preview == "image" ? "image/png" : "video/mp4";
		overlay->showUrl(QUrl(url), contentType);
	});
	QTimer::singleShot(100, &window, &QWidget::close);
	return app.exec();
}
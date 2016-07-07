#include "mainwindow.h"
#include "core/qqsettingsparams.h"

#include <QApplication>
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#include <QDateTime>
#include <QDir>
#include <QSettings>

#ifdef QML_PALMI
#if(QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
#include "qml/documenthandler.h"
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#endif
#endif //QML_PALMI

#ifdef Q_OS_UNIX
#undef signals
#include <libnotify/notify.h>
#endif

//#define QQ_ENABLE_QML true

bool removeDirRecursive(const QString &dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName))
	{
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
		{
			if (info.isDir())
				result = removeDirRecursive(info.absoluteFilePath());
			else
				result = QFile::remove(info.absoluteFilePath());

			if (!result)
				return result;
		}
		result = dir.rmdir(dirName);
	}

	return result;
}

void purgeCache()
{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QDir dirCache(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
#else
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
#endif
	QDateTime now = QDateTime::currentDateTime();
	QFileInfoList fileInfoList = dirCache.entryInfoList();
	for (int i = 0; i < fileInfoList.size(); ++i)
	{
		if(fileInfoList.at(i).lastModified().daysTo(now) >  MAX_CACHE_AGE_DAYS)
			QFile(fileInfoList.at(i).filePath()).remove();
	}

	//Nettoyage du cache reseau au demarrage : QTBUG-34498
	removeDirRecursive(dirCache.absoluteFilePath("networkCache"));
}

#ifdef QML_PALMI
#if(QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
void dumpRecurse(QQuickItem *rootItem)
{
	foreach (QQuickItem *item, rootItem->childItems())
	{
		qDebug() << Q_FUNC_INFO << item->childItems().size() << item->objectName();
		dumpRecurse(item);
	}
}
#endif //QT_VERSION
#endif //QML_PALMI

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("Moules Corp");
	a.setApplicationName("quteqoin");
	a.setApplicationVersion("SVN Crash Edition");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	purgeCache();

#ifdef Q_OS_UNIX
	notify_init(notif_name);
#endif

	MainWindow w;
	w.show();

#ifdef ENABLE_QML
#ifdef QT_DEBUG
#if(QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
	qmlRegisterType<DocumentHandler>("org.moules.quteqoin", 1, 0, "DocumentHandler");
	QQmlApplicationEngine engine(QUrl("qrc:///qml/QQmlMain.qml"));
	QQuickWindow *qmlRoot = qobject_cast<QQuickWindow *>(engine.rootObjects().at(0));
	if(!qmlRoot)
	{
		qWarning("Error: Your root item has to be a Window.");
		return -1;
	}

	QQuickItem *qmlRootItem = qmlRoot->contentItem();
	dumpRecurse(qmlRootItem);
	/*
	if(textArea)
	{
		textArea->setProperty("text", "Lorem ipsum");
		qDebug() << Q_FUNC_INFO << "textArea found";
	}
	*/
#endif //QT_VERSION
#endif //QT_DEBUG
#endif //QQ_ENABLE_QML

	return a.exec();
}

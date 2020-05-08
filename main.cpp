#include "mainwindow.h"
#include "core/qqsettingsparams.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QSettings>

#undef QML_PALMI

#ifdef QML_PALMI
#include "qml/documenthandler.h"
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#endif //QML_PALMI

//#define QQ_ENABLE_QML true

bool removeDirRecursive(const QString &dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName))
	{
		for (const auto &fi : dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
		{
			if (fi.isDir())
				result = removeDirRecursive(fi.absoluteFilePath());
			else
				result = QFile::remove(fi.absoluteFilePath());

			if (!result)
				return result;
		}
		result = dir.rmdir(dirName);
	}

	return result;
}

void purgeCache()
{
	QDir dirCache(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
	QDateTime now = QDateTime::currentDateTime();
	QFileInfoList fileInfoList = dirCache.entryInfoList();
	for (const auto &fi : fileInfoList)
	{
		if(fi.lastModified().daysTo(now) >  MAX_CACHE_AGE_DAYS)
			QFile(fi.filePath()).remove();
	}

	//Nettoyage du cache reseau au demarrage : QTBUG-34498
	removeDirRecursive(dirCache.absoluteFilePath("networkCache"));
}

#ifdef QML_PALMI
void dumpRecurse(QQuickItem *rootItem)
{
	foreach (QQuickItem *item, rootItem->childItems())
	{
		qDebug() << Q_FUNC_INFO << item->childItems().size() << item->objectName();
		dumpRecurse(item);
	}
}
#endif //QML_PALMI

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setOrganizationName("Moules Corp");
	QApplication::setApplicationName(APP_NAME);
	QApplication::setApplicationVersion(GIT_VERSION);
	QSettings::setDefaultFormat(QSettings::IniFormat);

	purgeCache();

	MainWindow w;
	w.show();

#ifdef ENABLE_QML
#ifdef QT_DEBUG
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
#endif //QT_DEBUG
#endif //QQ_ENABLE_QML

	return QApplication::exec();
}

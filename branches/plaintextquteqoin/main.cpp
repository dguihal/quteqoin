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

#ifdef Q_OS_UNIX
#undef signals
#include <libnotify/notify.h>
#endif

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

	return a.exec();
}
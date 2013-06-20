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
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setOrganizationName("Moules Corp");
	a.setApplicationName("quteqoin");
	a.setApplicationVersion("SVN Crash Edition");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	purgeCache();

	MainWindow w;
	w.show();

	return a.exec();
}

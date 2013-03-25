#include "mainwindow.h"
#include "core/qqsettingsparams.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QSettings>

void purgeCache()
{
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
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

#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include "mainwindow.h"

#define MAX_FILE_AGE_DAYS 60 //60j

void purgeCache()
{
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
	QDateTime now = QDateTime::currentDateTime();
	QFileInfoList fileInfoList = dirCache.entryInfoList();
	for (int i = 0; i < fileInfoList.size(); ++i)
	{
		if(fileInfoList.at(i).lastModified().daysTo(now) >  MAX_FILE_AGE_DAYS)
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

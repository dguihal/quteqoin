#ifndef QQTOTOZDOWNLOADER_H
#define QQTOTOZDOWNLOADER_H

#include "qqnetworkaccessor.h"
//#include "qqtotoz.h"
//#include "qqsettings.h"

#include <QHash>
#include <QObject>
#include <QString>
#include <QUrl>

class QNetworkReply;

class QQTotozDownloader : public QQNetworkAccessor
{
	Q_OBJECT

public:
	explicit QQTotozDownloader(QObject * parent) : QQNetworkAccessor(parent) {}
	~QQTotozDownloader() {}

signals:

public slots:
	void fetchTotoz(QString & totozId);

signals:
	void fetchTotozFinished(QString & totozId, bool success, QString & errMsg);

protected slots:
	void requestFinishedSlot(QNetworkReply* reply);

private:
	QHash<QNetworkReply *, QString> m_totozIdReplyHash;
};

#endif // QQTOTOZDOWNLOADER_H

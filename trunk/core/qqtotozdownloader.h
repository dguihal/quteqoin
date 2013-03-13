#ifndef QQTOTOZDOWNLOADER_H
#define QQTOTOZDOWNLOADER_H

#include "core/qqnetworkaccessor.h"
#include "core/qqtotoz.h"
#include "core/qqsettings.h"

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
	void fetchTotozFinished(QString & totozId, bool success);

protected slots:
	void requestFinishedSlot(QNetworkReply* reply);

private:
	QHash<QNetworkReply *, QString> m_totozIdReplyHash;
};

#endif // QQTOTOZDOWNLOADER_H

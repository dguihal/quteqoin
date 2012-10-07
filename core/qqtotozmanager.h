#ifndef QQTOTOZMANAGER_H
#define QQTOTOZMANAGER_H

#include "core/qqtotoz.h"

#include <QHash>
#include <QImage>
#include <QMovie>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

class QQTotozManager : public QObject
{
	Q_OBJECT

public:
	explicit QQTotozManager(const QString & totozServerURL, QObject *parent = 0);

	QQTotoz::TypeTotoz getTypeTotoz(const QString & totozId);

	QSharedPointer<QImage> getStaticTotoz(const QString & totozId);
	QSharedPointer<QMovie> getDynTotoz(const QString & totozId);

signals:

public slots:
	void fetchTotoz(const QString & totozId);
	void serverURLchanged(const QString & newUrl);
	void finishedSlot(QNetworkReply* reply);

private:

	QPointer<QNetworkAccessManager> m_qnam;

	QNetworkAccessManager* createQNAM();

	QString m_totozServerUrl;
	QHash<QString, QSharedPointer<QImage> > m_totozHashCache;
	QHash<QString, QSharedPointer<QMovie> > m_totozDynHashCache;

	QHash<QNetworkReply *, QString> m_totozIdReplyHash;
};

#endif // QQTOTOZMANAGER_H

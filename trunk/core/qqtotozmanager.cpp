#include "qqtotozmanager.h"

#include "core/qqtotoz.h"

#include <QBuffer>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

QQTotozManager::QQTotozManager(const QString & totozServerURL, QObject *parent) :
	QObject(parent)
{
	m_totozServerUrl = totozServerURL;

	m_qnam = createQNAM();
}

void QQTotozManager::fetchTotoz(const QString & totozId)
{
	// si l'id est valide et que l'on ne l'a pas déjà téléchargé dans le cache
	// TODO : Une expiration du cache, voire un if-modified-since !!!!
	if(totozId.length() > 0 && ! QFile::exists(QQTotoz::getPath(totozId)))
	{
		QString queryUrl = m_totozServerUrl;
		queryUrl.append("/").append(totozId).append(".gif");
		QUrl url(queryUrl);

		QNetworkRequest request(url);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);

		QNetworkReply * reply = m_qnam->get(request);
		m_totozIdReplyHash.insert(reply, totozId);
	}
}

void QQTotozManager::serverURLchanged(const QString &newUrl)
{
	m_totozServerUrl = newUrl;
}

void QQTotozManager::finishedSlot(QNetworkReply * reply)
{
	// ou de l'url de destination si on a affaire a une redirection:
	QUrl redirectedURL = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	//Recuperation du totozId associe a la requete
	QString totozId = m_totozIdReplyHash.value(reply);
	m_totozIdReplyHash.remove(reply);

	if(!redirectedURL.isEmpty() &&
			redirectedURL != reply->url())
	{
		qDebug() << "QQTotozManager::replyFinished: Redirected to " << redirectedURL.toString();
		QNetworkRequest request(redirectedURL);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);

		QNetworkReply * reply = m_qnam->get(request);
		m_totozIdReplyHash.insert(reply, totozId);
	} // Une erreur HTTP est survenue
	else if (reply->error() != QNetworkReply::NoError)
	{
		// Recuperation du Statut HTTP
		QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

		qWarning() << "QQTotozManager::replyFinished, error : " << reply->errorString()
				   << "HTTP statusCode : " << statusCodeV.toString();
	} // Tout est OK on poursuit
	else
	{
		QFile file(QQTotoz::getPath(totozId));
		file.open(QIODevice::WriteOnly);
		file.write(reply->readAll());

		if(reply->hasRawHeader(QString::fromAscii("Etag").toAscii()))
		{
			QString etag(reply->rawHeader(QString::fromAscii("Etag").toAscii()));
			qDebug() << "QQTotozManager::finishedSlot, etag = " << etag;
		}
	}

	reply->deleteLater();
}

QNetworkAccessManager* QQTotozManager::createQNAM()
{
	QNetworkAccessManager* qnam = new QNetworkAccessManager(this);

	connect(qnam, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(finishedSlot(QNetworkReply*)));
	return qnam;
}

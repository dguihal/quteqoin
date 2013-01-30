#include "qqtotozdownloader.h"

#include "core/qqtotoz.h"

#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkProxyFactory>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

QQTotozDownloader::QQTotozDownloader(QQSettings * settings) :
	QQNetworkAccessor(settings)
{
	m_totozServerUrl = settings->totozServerUrl();
}

void QQTotozDownloader::fetchTotoz(QString & totozId)
{
	// si l'id est valide et que l'on ne l'a pas déjà téléchargé dans le cache
	// TODO : Une expiration du cache, voire un if-modified-since !!!!
	if(totozId.length() > 0 && ! QQTotoz::cacheExists(totozId))
	{
		QString queryUrl = m_totozServerUrl;
		queryUrl.append("/").append(totozId);
		QUrl url(queryUrl);

		QNetworkRequest request(url);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);

		QNetworkReply * reply = httpGet(request);
		m_totozIdReplyHash.insert(reply, totozId);
	}
}

void QQTotozDownloader::serverURLchanged(const QString &newUrl)
{
	m_totozServerUrl = newUrl;
}

void QQTotozDownloader::requestFinishedSlot(QNetworkReply * reply)
{
	// ou de l'url de destination si on a affaire a une redirection:
	QUrl redirectedURL = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	//Recuperation du totozId associe a la requete
	QString totozId = m_totozIdReplyHash.value(reply);
	m_totozIdReplyHash.remove(reply);

	if(!redirectedURL.isEmpty() &&
			redirectedURL != reply->url())
	{
		qDebug() << "QQTotozManager::requestFinishedSlot: Redirected to " << redirectedURL.toString();
		QNetworkRequest request(redirectedURL);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);

		QNetworkReply * reply = httpGet(request);
		m_totozIdReplyHash.insert(reply, totozId);
	} // Une erreur HTTP est survenue
	else if (reply->error() != QNetworkReply::NoError)
	{
		// Recuperation du Statut HTTP
		QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

		qWarning() << "QQTotozManager::requestFinishedSlot, error : " << reply->errorString()
				   << "HTTP statusCode : " << statusCodeV.toString();
		emit fetchTotozFinished(totozId, false);
	} // Tout est OK on poursuit
	else
	{
		QQTotoz totoz(totozId);
		totoz.setData(reply->readAll());
		totoz.save();

		if(reply->hasRawHeader(QString::fromAscii("Etag").toAscii()))
		{
			QString etag(reply->rawHeader(QString::fromAscii("Etag").toAscii()));
			qDebug() << "QQTotozManager::requestFinishedSlot, etag = " << etag;
		}
		emit fetchTotozFinished(totozId, true);
	}

	reply->deleteLater();
}

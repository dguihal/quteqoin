#include "qqtotozdownloader.h"

#include "core/qqtotoz.h"

#include <QtDebug>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkProxyFactory>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

void QQTotozDownloader::fetchTotoz(QString & totozId)
{
	if(totozId.length() == 0)
		return;

	QQTotoz totoz(totozId);
	if(totoz.isCacheExpired())
	{
		QQSettings settings;
		QString queryUrl = settings.value(SETTINGS_TOTOZ_SERVER_URL, DEFAULT_TOTOZ_SERVER_URL).toString();
		queryUrl.append("/").append(
					settings.value(SETTINGS_TOTOZ_SERVER_BASE_IMG, DEFAULT_TOTOZ_SERVER_BASE_IMG).toString());
		queryUrl.append("/").append(totozId);
		QUrl url(queryUrl);

		QNetworkRequest request(url);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);

		QNetworkReply * reply = httpGet(request);
		m_totozIdReplyHash.insert(reply, totozId);
	}
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
		QDateTime expire;
		if(reply->hasRawHeader("Expires"))
		{
			expire = parseRC822(QString::fromAscii(reply->rawHeader("Expires")));
			totoz.setCacheExpireDate(expire);
		}
		else
		{
			expire = QDateTime::currentDateTime();
			expire.addDays(MAX_CACHE_AGE_DAYS);
		}
		qDebug() << "QQTotozDownloader::requestFinishedSlot, totozId =" << totozId;
		totoz.setCacheExpireDate(expire);
		totoz.save();
		emit fetchTotozFinished(totozId, true);
	}

	reply->deleteLater();
}

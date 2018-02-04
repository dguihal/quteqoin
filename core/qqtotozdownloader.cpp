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
		QString totozIdSuffix = settings.value(SETTINGS_TOTOZ_SERVER_NAME_SUFFIX, DEFAULT_TOTOZ_SERVER_NAME_SUFFIX).toString();
		QString queryUrl = settings.value(SETTINGS_TOTOZ_SERVER_URL, DEFAULT_TOTOZ_SERVER_URL).toString();
		queryUrl.append("/")
				.append(settings.value(SETTINGS_TOTOZ_SERVER_BASE_IMG, DEFAULT_TOTOZ_SERVER_BASE_IMG).toString())
				.append("/")
				.append(totozId)
				.append(totozIdSuffix);
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
		qDebug() << Q_FUNC_INFO << "Redirected to " << redirectedURL.toString();
		QNetworkRequest request(redirectedURL);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);

		QNetworkReply * reply = httpGet(request);
		m_totozIdReplyHash.insert(reply, totozId);
	} // Une erreur HTTP est survenue
	else if (reply->error() != QNetworkReply::NoError)
	{
		// Recuperation du Statut HTTP
		QString statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
		QString errString = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

		qWarning() << Q_FUNC_INFO
				   << "error : " << errString
				   << "HTTP statusCode : " << statusCodeV;
		emit fetchTotozFinished(totozId, false, errString);
	} // Tout est OK on poursuit
	else
	{
		QQTotoz totoz(totozId);
		totoz.setData(reply->readAll());
		QDateTime expire;
		if(reply->hasRawHeader("Expires"))
		{
			expire = parseRC822(QString::fromLatin1(reply->rawHeader("Expires")));
			totoz.setCacheExpireDate(expire);
		}
		else
		{
			expire = QDateTime::currentDateTime().addDays(MAX_CACHE_AGE_DAYS);
		}
		totoz.setCacheExpireDate(expire);
		totoz.save();
		QString errStr;
		emit fetchTotozFinished(totozId, true, errStr);
	}

	reply->deleteLater();
}

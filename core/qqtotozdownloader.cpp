#include "qqtotozdownloader.h"

#include "core/qqtotoz.h"

#include <QtDebug>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkProxyFactory>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

constexpr qint64 ONE_MN_IN_S = 60;

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
		        .append(QUrl::toPercentEncoding(totozId))
		        .append(totozIdSuffix);
		QUrl url(queryUrl);

		QNetworkRequest request(url);

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
		if(redirectedURL.host().isEmpty()) // Relative redirection
			redirectedURL=reply->request().url().resolved(redirectedURL);
		qDebug() << Q_FUNC_INFO << "Redirected to " << redirectedURL.toString();
		QNetworkRequest request(redirectedURL);

		QNetworkReply * reply = httpGet(request);
		m_totozIdReplyHash.insert(reply, totozId);
	} // Une erreur HTTP est survenue
	else if (reply->error() != QNetworkReply::NoError)
	{
		// Recuperation du Statut HTTP
		QString errString = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

		QQTotoz totoz(totozId);
		totoz.setData(errString.toUtf8());
		totoz.setDataContentType("Text/Plain");
		totoz.setCacheExpireDate(QDateTime::currentDateTime().addSecs(ONE_MN_IN_S));
		totoz.save();

		emit fetchTotozFinished(totozId, false, errString);
	} // Tout est OK on poursuit
	else
	{
		QQTotoz totoz(totozId);
		totoz.setData(reply->readAll());
		totoz.setDataContentType(reply->rawHeader("Content-Type"));
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

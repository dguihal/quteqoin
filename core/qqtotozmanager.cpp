#include "qqtotozmanager.h"

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


QQTotoz::TypeTotoz QQTotozManager::getTypeTotoz(const QString & totozId)
{
	if(m_totozHashCache.contains(totozId))
		return QQTotoz::QQTOTOZ_STATIC;
	else if(m_totozDynHashCache.contains(totozId))
		return QQTotoz::QQTOTOZ_ANIMATED;

	return QQTotoz::QQTOTOZ_NOTFOUND;
}

void QQTotozManager::fetchTotoz(const QString & totozId)
{
	if(! m_totozHashCache.contains(totozId) &&
			! m_totozDynHashCache.contains(totozId))
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

QSharedPointer<QImage> QQTotozManager::getStaticTotoz(const QString & totozId)
{
	if(m_totozHashCache.contains(totozId))
		return m_totozHashCache.value(totozId);

	return QSharedPointer<QImage>();
}

QSharedPointer<QMovie> QQTotozManager::getDynTotoz(const QString & totozId)
{
	if(m_totozDynHashCache.contains(totozId))
		return m_totozDynHashCache.value(totozId);

	return QSharedPointer<QMovie>();
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
		QByteArray all = reply->readAll();
		QBuffer buff(&all);
		buff.open(QIODevice::ReadOnly);

		//on teste avec un QMovie
		QMovie * movie = new QMovie(&buff);
		if(movie->isValid())
			m_totozDynHashCache.insert(totozId, QSharedPointer<QMovie>(movie));
		else
		{
			delete movie;
			buff.seek(0);
			QImageReader imageReader(&buff);
			QImage * img = new QImage(imageReader.read());
			if(! img->isNull())
				m_totozHashCache.insert(totozId, QSharedPointer<QImage>(img));
			else
				delete img;
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

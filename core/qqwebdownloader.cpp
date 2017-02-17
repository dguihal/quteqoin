#include "qqwebdownloader.h"

#include <QDebug>
#include <QDir>
#include <QDesktopServices>
#include <QNetworkDiskCache>

#define INITIAL_URL_PROPERTY "INITIAL_URL"

//////////////////////////////////////////////////////////////
QQWebDownloader::QQWebDownloader(QObject *parent) :
	QQNetworkAccessor(parent)
{
	QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
	diskCache->setMaximumCacheSize(100 * 1024 * 1024); // 100 Mo
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QDir dirCache(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
#else
	QDir dirCache(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
#endif
	diskCache->setCacheDirectory(dirCache.absoluteFilePath("networkCache"));
	setNetCacheManager(diskCache);
}

//////////////////////////////////////////////////////////////
/// \brief QQWebDownloader::getImage
/// \param url
///
void QQWebDownloader::getURL(const QUrl &url)
{
	if(! m_listPendingUrl.contains(url))
	{
		m_listPendingUrl.append(url);

		QNetworkRequest request(url);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);
		httpGet(request);
	}
	else
		qDebug() << Q_FUNC_INFO << "Already loading" << url;
}

//////////////////////////////////////////////////////////////
/// \brief QQWebDownloader::requestFinishedSlot
/// \param reply
///
void QQWebDownloader::requestFinishedSlot(QNetworkReply *reply)
{
	m_listPendingUrl.removeOne(reply->url());

	// ou de l'url de destination si on a affaire a une redirection:
	QUrl redirectedURL = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if(!redirectedURL.isEmpty() &&
			redirectedURL != reply->url() &&
			! m_listPendingUrl.contains(redirectedURL))
	{
		QNetworkRequest rq(redirectedURL);

		if(reply->property(INITIAL_URL_PROPERTY).isValid())
			newReply->setProperty(INITIAL_URL_PROPERTY, reply->property(INITIAL_URL_PROPERTY));
		else
			newReply->setProperty(INITIAL_URL_PROPERTY, reply->request().url());

		httpGet(rq);
	} // Une erreur HTTP est survenue
	else if (reply->error() != QNetworkReply::NoError)
	{
		// Recuperation du Statut HTTP
		QString statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
		QString errString = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

		m_dataContentType.clear();
		m_data.clear();

		qWarning() << Q_FUNC_INFO << "error : " << errString << "HTTP statusCode : " << statusCodeV;
	} // Tout est OK on poursuit
	else
	{
		QUrl sourceUrl = reply->request().url();
		if(reply->property(INITIAL_URL_PROPERTY).isValid())
			sourceUrl = reply->property(INITIAL_URL_PROPERTY).toUrl();

		m_dataContentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
		m_data = reply->readAll();
		emit ready(sourceUrl);
	}

	reply->deleteLater();
}

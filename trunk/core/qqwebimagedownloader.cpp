#include "qqwebimagedownloader.h"

#include <QDebug>
#include <QDir>
#include <QDesktopServices>
#include <QNetworkDiskCache>

//////////////////////////////////////////////////////////////
QQWebImageDownloader::QQWebImageDownloader(QObject *parent) :
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
/// \brief QQWebImageDownloader::getImage
/// \param url
///
void QQWebImageDownloader::getImage(const QUrl &url)
{
	QNetworkRequest request(url);
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
						 QNetworkRequest::PreferCache);
	httpGet(request);
}

//////////////////////////////////////////////////////////////
/// \brief QQWebImageDownloader::requestFinishedSlot
/// \param reply
///
void QQWebImageDownloader::requestFinishedSlot(QNetworkReply *reply)
{
	// ou de l'url de destination si on a affaire a une redirection:
	QUrl redirectedURL = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if(!redirectedURL.isEmpty() &&
			redirectedURL != reply->url())
	{
		qDebug() << Q_FUNC_INFO << "Redirected to " << redirectedURL.toString();
		QNetworkRequest request(redirectedURL);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
							 QNetworkRequest::PreferCache);

		httpGet(request);
	} // Une erreur HTTP est survenue
	else if (reply->error() != QNetworkReply::NoError)
	{
		// Recuperation du Statut HTTP
		QString statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
		QString errString = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

		qWarning() << Q_FUNC_INFO << "error : " << errString << "HTTP statusCode : " << statusCodeV;
		emit ready();
	} // Tout est OK on poursuit
	else
	{
		m_data = reply->readAll();
		emit ready();
	}

	reply->deleteLater();
}

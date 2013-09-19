#include "qqpiniurlhelper.h"

#include "core/qqtypes.h"
#include "core/qqsettings.h"

#include <QtDebug>
#include <QStringList>

#define URL_HOST_REGEXP "(<a [^>]*href=\"https?://((?:[\\w-\\.])+\\.)*([\\w-]{3,})(\\.[\\.\\w]+)[^\"]*\"[^>]*>)\\[(?:url|https?)\\](</a>)"
					//                   <-                          URL                             ->
					//                            <-                 HOST                          ->
					//                            <-       SUB     -> <-  NAME  -><-      TLD      ->
#define FULL_HOST_REPLACE "\\1[\\2\\3\\4]\\5"
#define SHORT_HOST_REPLACE "\\1[\\3\\4]\\5"
#define SHORTER_HOST_REPLACE "\\1[\\3]\\5"

#define CONTENT_TYPE_CACHE_SIZE 500

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::QQPiniUrlHelper
/// \param parent
///
QQPiniUrlHelper::QQPiniUrlHelper(QObject *parent) :
	QQNetworkAccessor(parent), QQMessageTransformFilter()
{
	m_contentTypeReplies.clear();
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::getContentType
/// \param url
///
QHash<QUrl, QString> QQPiniUrlHelper::m_contentTypeCache;
QList<QUrl> QQPiniUrlHelper::m_contentTypeCacheUrls;
void QQPiniUrlHelper::getContentType(const QUrl &url)
{
	if(! QQPiniUrlHelper::m_contentTypeCache.contains(url))
	{
		QNetworkReply *reply = httpHead(QNetworkRequest(url));
		m_contentTypeReplies.append(reply);
	}
	else
	{
		QUrl reqUrl = url;
		emit contentTypeAvailable(reqUrl, m_contentTypeCache[url]);
	}
}

//////////////////////////////////////////////////////////////
/// \brief transformMessage
/// \param post
/// \param message
///
void QQPiniUrlHelper::transformMessage(const QString &bouchot, QString &message)
{
	Q_UNUSED(bouchot);

	QString urlPatternReplace;
	QQSettings settings;
	QuteQoin::QQSmartUrlFilerTransformType trType =
				(QuteQoin::QQSmartUrlFilerTransformType) settings.value(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE, DEFAULT_FILTER_SMART_URL_TRANSFORM_TYPE).toInt();
	switch(trType)
	{
		case QuteQoin::Full:
			urlPatternReplace = FULL_HOST_REPLACE;
			break;
		case QuteQoin::Short:
			urlPatternReplace = SHORT_HOST_REPLACE;
			break;
		default:
			urlPatternReplace = SHORTER_HOST_REPLACE;
	}

	QRegExp reg(URL_HOST_REGEXP,
				Qt::CaseInsensitive,
				QRegExp::RegExp2);

	message.replace(reg, urlPatternReplace);

}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::requestFinishedSlot
/// \param reply
///
void QQPiniUrlHelper::requestFinishedSlot(QNetworkReply *reply)
{
	if(m_contentTypeReplies.contains(reply))
	{
		m_contentTypeReplies.removeAll(reply);
		QUrl sourceUrl = reply->request().url();
		QString rep(tr("Unknown"));
		if(reply->error() == QNetworkReply::NoError)
		{
			QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
			QStringList contentTypeParts = contentType.split(";", QString::SkipEmptyParts);
			if(contentTypeParts.size() > 0)
				rep = contentTypeParts.at(0);

			QQPiniUrlHelper::m_contentTypeCache.insert(sourceUrl, rep);
			QQPiniUrlHelper::m_contentTypeCacheUrls.append(sourceUrl);
			while(QQPiniUrlHelper::m_contentTypeCacheUrls.size() > CONTENT_TYPE_CACHE_SIZE)
				QQPiniUrlHelper::m_contentTypeCache.remove(QQPiniUrlHelper::m_contentTypeCacheUrls.takeFirst());
		}
		else
			qDebug() << reply->errorString();
		emit contentTypeAvailable(sourceUrl, rep);
	}
}

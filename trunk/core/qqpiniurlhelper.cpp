#include "qqpiniurlhelper.h"

#include "core/qqtypes.h"
#include "core/qqsettings.h"

#include <QtDebug>
#include <QStringList>

#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#endif

#define URL_HOST_REGEXP "(<a [^>]*href=\"https?://((?:[\\w-\\.])+\\.)*([\\w-]{3,})(\\.[\\.\\w]+)[^\"]*\"[^>]*>)\\[(?:url|https?)\\](</a>)"
					//                   <-                          URL                             ->
					//                            <-                 HOST                          ->
					//                            <-       SUB     -> <-  NAME  -><-      TLD      ->
#define FULL_HOST_REPLACE "\\1[\\2\\3\\4]\\5"
#define SHORT_HOST_REPLACE "\\1[\\3\\4]\\5"
#define SHORTER_HOST_REPLACE "\\1[\\3]\\5"

#define CONTENT_TYPE_CACHE_SIZE 500

#define YT_KEY_ID "AIzaSyBCLhoklblj8GXwl1S8i6b2yRR9tbnvPbU"
#define INITIAL_URL_PROPERTY "INITIAL_URL"

QHash<QUrl, QQPiniUrlHelper::CacheInfo *> QQPiniUrlHelper::m_cache;
QList<QUrl> QQPiniUrlHelper::m_cacheUrls;

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
/// \brief getUrlInfo
/// \param url
///
void QQPiniUrlHelper::getUrlInfo(QUrl &url)
{
	// Sans doute a ameliorer si la liste grandit
	if(url.host().endsWith("youtube.com"))
		getYoutubeExtendedInfo(url);
	else if(url.host().endsWith("dailymotion.com"))
		getDailymotionExtendedInfo(url);
	else if(url.host() == "sauf.ca")
		getSaufCaExtendedInfo(url);
	else
		getContentType(url);
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

		// ou de l'url de destination si on a affaire a une redirection:
		QUrl redirectedURL = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

		if(!redirectedURL.isEmpty() &&
				redirectedURL != sourceUrl)
		{
			qDebug() << Q_FUNC_INFO << reply->url().toString() << "redirected to" << redirectedURL.toString();
			QNetworkReply *newReply = httpHead(QNetworkRequest(redirectedURL));

			if(reply->property(INITIAL_URL_PROPERTY).isValid())
				newReply->setProperty(INITIAL_URL_PROPERTY, reply->property(INITIAL_URL_PROPERTY));
			else
				newReply->setProperty(INITIAL_URL_PROPERTY, sourceUrl);

			m_contentTypeReplies.append(newReply);
		} // Une erreur HTTP est survenue
		else if(reply->error() != QNetworkReply::NoError)
		{
			qDebug() << reply->errorString();
			QString rep = tr("Unknown");
			emit contentTypeAvailable(sourceUrl, rep);
		}
		else
		{
			QNetworkRequest rq = reply->request();
			if(reply->property(INITIAL_URL_PROPERTY).isValid())
				sourceUrl = reply->property(INITIAL_URL_PROPERTY).toUrl();

			if(rq.attribute((QNetworkRequest::Attribute) RequestContentType, false).toBool())
				handleContentTypeResponse(reply->header(QNetworkRequest::ContentTypeHeader).toString(), sourceUrl);
			else if(rq.attribute((QNetworkRequest::Attribute) RequestYoutubeInfo, false).toBool())
				handleYoutubeExtendedInfo(reply->readAll(), sourceUrl);
			else if(rq.attribute((QNetworkRequest::Attribute) RequestDailyMotionInfo, false).toBool())
				handleDailymotionExtendedInfo(reply->readAll(), sourceUrl);
		}
	}
	reply->deleteLater();
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::getContentType
/// \param url
///
void QQPiniUrlHelper::getContentType(QUrl &url)
{
	QQPiniUrlHelper::CacheInfo *urlInfo = m_cache[url];
	if(urlInfo == NULL)
	{
		QNetworkRequest r(url);
		r.setAttribute((QNetworkRequest::Attribute) RequestContentType, true);
		QNetworkReply *reply = httpHead(r);
		m_contentTypeReplies.append(reply);
	}
	else
	{
		QUrl reqUrl = url;

		if(urlInfo->contentType.startsWith("image/"))
			emit thumbnailUrlAvailable(url, url);

		emit contentTypeAvailable(reqUrl, urlInfo->contentType);
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::handleContentTypeResponse
/// \param contentType
/// \param sourceUrl
///
void QQPiniUrlHelper::handleContentTypeResponse(const QString &contentType, QUrl &sourceUrl)
{
	QStringList contentTypeParts = contentType.split(";", QString::SkipEmptyParts);
	QString rep = (contentTypeParts.size() > 0) ? contentTypeParts.at(0) : tr("Unknown");

	QQPiniUrlHelper::CacheInfo *info = new QQPiniUrlHelper::CacheInfo;
	info->contentType = rep;
	addToCache(sourceUrl, info);

	if(contentType.startsWith("image/"))
		emit thumbnailUrlAvailable(sourceUrl, sourceUrl);

	emit contentTypeAvailable(sourceUrl, rep);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::getDailymotionExtendedInfo
/// \param url
///
void QQPiniUrlHelper::getDailymotionExtendedInfo(QUrl &url)
{
	QQPiniUrlHelper::CacheInfo *urlInfo = m_cache[url];
	if(urlInfo == NULL)
	{
		QStringList tmpList = url.path().split('/');
		if(tmpList.isEmpty())
			return;
		QString tmp = tmpList.last();

		tmpList = tmp.split('_');
		if(tmpList.isEmpty())
			return;
		QString vidId = tmpList.first();

		QUrl qUrl = QUrl("https://api.dailymotion.com");
		QString postData(QString("{\"call\": \"GET /video/%1\",\"args\": {\"fields\": [\"title\",\"thumbnail_url\"]}}").arg(vidId));

		QNetworkRequest r(qUrl);
		r.setHeader(QNetworkRequest::ContentTypeHeader, "Application/json");

		r.setAttribute((QNetworkRequest::Attribute) RequestDailyMotionInfo, true);
		QNetworkReply *reply = httpPost(r, postData.toLatin1());
		reply->setProperty(INITIAL_URL_PROPERTY, url);

		m_contentTypeReplies.append(reply);
	}
	else
	{
		emit thumbnailUrlAvailable(url, urlInfo->videoThumbnailUrl);
		emit videoTitleAvailable(url, urlInfo->videoTitle);
	}
}

//////////////////////////////////////////////////////////////
/// \brief handleDailymotionExtendedInfo
/// \param jsonInfo
/// \param sourceUrl
///
///
void QQPiniUrlHelper::handleDailymotionExtendedInfo(const QByteArray &jsonInfo, QUrl &sourceUrl)
{
	QString thumbnailUrl, title;
	QQPiniUrlHelper::CacheInfo *info = new QQPiniUrlHelper::CacheInfo;
	//JSON seulement supporté par Qt 5
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QJsonDocument d = QJsonDocument::fromJson(jsonInfo);
	QJsonObject o = d.object();
	QJsonObject o2 = o["result"].toObject();

	thumbnailUrl = o2["thumbnail_url"].toString();
	title = o2["title"].toString();
#else

	QString str = QString::fromLatin1(jsonInfo);
	QRegExp r("\"thumbnail_url\":\"([^\"]*)");
	if(r.indexIn(str) > 0)
	{
		thumbnailUrl = r.capturedTexts().at(1);

		thumbnailUrl.replace("\\/", "/");
	}

	r = QRegExp("\"title\":\"([^\"]*)");

	if(r.indexIn(str) > 0)
	{
		title = r.capturedTexts().at(1);

		int idx = -1;
		while(( idx = title.indexOf("\\u") ) != -1 )
		{
			int nHex = title.mid(idx + 2, 4).toInt(0, 16);
			title.replace(idx, 6, QChar(nHex));
		}
	}
#endif

	if(! thumbnailUrl.isEmpty())
	{
		if(! title.isEmpty())
		{
			info->videoThumbnailUrl = thumbnailUrl;
			info->videoTitle = title;
			addToCache(sourceUrl, info);
		}
		QUrl tUrl(thumbnailUrl);
		emit thumbnailUrlAvailable(sourceUrl, tUrl);
	}

	if(! title.isEmpty())
		emit videoTitleAvailable(sourceUrl, title);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::getSaufCaExtendedInfo
/// \param url
///
void QQPiniUrlHelper::getSaufCaExtendedInfo(QUrl &url)
{
	if(! url.path().endsWith("/img"))
	{
		QUrl imgUrl = url;
		imgUrl.setPath(url.path().append("/img"));

		emit thumbnailUrlAvailable(url, imgUrl);
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::getYoutubeExtendedInfo
/// \param ytId
///
void QQPiniUrlHelper::getYoutubeExtendedInfo(QUrl &url)
{
	QQPiniUrlHelper::CacheInfo *urlInfo = m_cache[url];
	if(urlInfo == NULL)
	{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		QUrlQuery urlQ(url);
		QString ytId = urlQ.queryItemValue("v");
#else
		QString ytId = url.queryItemValue("v");
#endif

		if(ytId.isEmpty())
			return;

		QUrl qUrl = QUrl(QString("https://www.googleapis.com/youtube/v3/videos?id=%1&part=snippet&key=%2")
						.arg(ytId)
						.arg(YT_KEY_ID));

		QNetworkRequest r(qUrl);
		r.setAttribute((QNetworkRequest::Attribute) RequestYoutubeInfo, true);
		QNetworkReply *reply = httpGet(r);
		reply->setProperty(INITIAL_URL_PROPERTY, url);

		m_contentTypeReplies.append(reply);
	}
	else
	{
		emit thumbnailUrlAvailable(url, urlInfo->videoThumbnailUrl);
		emit videoTitleAvailable(url, urlInfo->videoTitle);
	}

}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::handleYoutubeExtendedInfo
///
/*
{
 "kind": "youtube#videoListResponse",
 "etag": "\"PSjn-HSKiX6orvNhGZvglLI2lvk/jYMo11ttS70ElzF5RMZCCrXlWp8\"",
 "pageInfo": {
  "totalResults": 1,
  "resultsPerPage": 1
 },
 "items": [
  {
   "kind": "youtube#video",
   "etag": "\"PSjn-HSKiX6orvNhGZvglLI2lvk/sco2Z98R6fUE9GmzFdnNuWVxhFo\"",
   "id": "VPKniLU6vII",
   "snippet": {
	"publishedAt": "2013-08-15T07:59:09.000Z",
	"channelId": "UCvGag7MyHR8H9oRm9iL9Ifw",
	"title": "Glace chaude instantanée ! - [Science 2.0]",
	"description": "L'expérience de la surfusion d'Acétate de Sodium, à refaire chez soi, pour une fois ! :D\n\nFabriquer de l'acétate de Sodium:\nhttp://www.chimie.ch/nuls/index.php/saison-4/48-4x09\n\n\nTwitter:\nhttp://www.twitter.com/experimentboy\nhttp://www.twitter.com/Wagique\n\nFacebook: http://www.facebook.com/ExperimentboyTV",
	"thumbnails": {
	 "default": {
	  "url": "https://i.ytimg.com/vi/VPKniLU6vII/default.jpg",
	  "width": 120,
	  "height": 90
	 },
	 "medium": {
	  "url": "https://i.ytimg.com/vi/VPKniLU6vII/mqdefault.jpg",
	  "width": 320,
	  "height": 180
	 },
	 "high": {
	  "url": "https://i.ytimg.com/vi/VPKniLU6vII/hqdefault.jpg",
	  "width": 480,
	  "height": 360
	 },
	 "standard": {
	  "url": "https://i.ytimg.com/vi/VPKniLU6vII/sddefault.jpg",
	  "width": 640,
	  "height": 480
	 },
	 "maxres": {
	  "url": "https://i.ytimg.com/vi/VPKniLU6vII/maxresdefault.jpg",
	  "width": 1280,
	  "height": 720
	 }
	},
	"channelTitle": "Experimentboy",
	"categoryId": "28",
	"liveBroadcastContent": "none"
   }
  }
 ]
}
*/
void QQPiniUrlHelper::handleYoutubeExtendedInfo(const QByteArray &jsonInfo, QUrl &sourceUrl)
{
	QString thumbnailUrl, title;

	QQPiniUrlHelper::CacheInfo *info = new QQPiniUrlHelper::CacheInfo;
	//JSON seulement supporté par Qt 5
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QJsonParseError error;
	QJsonDocument d = QJsonDocument::fromJson(jsonInfo, &error);
	if(d.isEmpty())
	{
		qDebug() << "error" << error.errorString();
		return;
	}
	QJsonObject o = d.object();
	QJsonObject items = o["items"].toArray().at(0).toObject();
	QJsonObject snippet = items["snippet"].toObject();
	title= snippet["title"].toString();
	QJsonObject thumbnails = snippet["thumbnails"].toObject();
	QJsonObject defaultThumbnail = thumbnails["default"].toObject();
	thumbnailUrl = defaultThumbnail["url"].toString();
#else

	QString str = QString::fromUtf8(jsonInfo);
	QRegExp r("\"title\": \"([^\"]*)");

	if(r.indexIn(str) > 0)
		title = r.capturedTexts().at(1);

	int offset = str.indexOf("\"thumbnails\": {");
	offset = str.indexOf("\"default\": {", offset);
	r = QRegExp("\"url\": \"([^\"]*)");
	if(r.indexIn(str, offset) > 0)
		thumbnailUrl = r.capturedTexts().at(1);

#endif
	info->videoThumbnailUrl = thumbnailUrl;
	info->videoTitle = title;
	addToCache(sourceUrl, info);

	if(! thumbnailUrl.isEmpty())
	{
		if(! title.isEmpty())
		{
			info->videoThumbnailUrl = thumbnailUrl;
			info->videoTitle = title;
			addToCache(sourceUrl, info);
		}

		QUrl tUrl(thumbnailUrl);
		emit thumbnailUrlAvailable(sourceUrl, tUrl);
	}

	if(! title.isEmpty())
		emit videoTitleAvailable(sourceUrl, title);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniUrlHelper::addToCache
/// \param sourceUrl
/// \param info
///
void QQPiniUrlHelper::addToCache(QUrl &sourceUrl, CacheInfo *info)
{
	QQPiniUrlHelper::m_cacheUrls.append(sourceUrl);
	QQPiniUrlHelper::m_cache.insert(sourceUrl, info);
	while(QQPiniUrlHelper::m_cacheUrls.size() > CONTENT_TYPE_CACHE_SIZE)
	{
		QQPiniUrlHelper::CacheInfo *oldInfo = QQPiniUrlHelper::m_cache.take(QQPiniUrlHelper::m_cacheUrls.takeFirst());
		if(oldInfo != NULL)
			delete oldInfo;
	}
}

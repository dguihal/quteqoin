#ifndef QQPINIURLHELPER_H
#define QQPINIURLHELPER_H

#include "qqnetworkaccessor.h"

#include "core/qqmessagetransformfilter.h"

#include <QList>
#include <QHash>
#include <QPair>
#include <QUrl>
#include <QString>

class QQPiniUrlHelper : public QQNetworkAccessor, public QQMessageTransformFilter
{
	Q_OBJECT
public:
	explicit QQPiniUrlHelper(QObject *parent = 0);

	void getUrlInfo(QUrl &url);

	virtual void transformMessage(const QString& bouchot, QString &message);

signals:
	void contentTypeAvailable(QUrl &url, QString &contentType);
	void thumbnailUrlAvailable(QUrl &url, QUrl &thumbnailUrl);
	void videoTitleAvailable(QUrl &url, QString &videoTitle);

public slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

private:

	enum RequestTypeAttribute {
		RequestContentType = QNetworkRequest::User,
		RequestYoutubeInfo,
		RequestDailyMotionInfo
	};

	void getContentType(QUrl &url);
	void handleContentTypeResponse(const QString &contentType, QUrl &sourceUrl);
	void getDailymotionExtendedInfo(QUrl &url);
	void handleDailymotionExtendedInfo(const QString &jsonInfo, QUrl &sourceUrl);
	void getYoutubeExtendedInfo(QUrl &url);
	void handleYoutubeExtendedInfo(const QString &jsonInfo, QUrl &sourceUrl);

	struct CacheInfo {
		QString contentType;
		QUrl videoThumbnailUrl;
		QString videoTitle;
	};

	void addToCache(QUrl &sourceUrl, CacheInfo *info);

	QList<QNetworkReply *> m_contentTypeReplies;
	static QHash<QUrl, QQPiniUrlHelper::CacheInfo *> m_cache;
	static QList<QUrl> m_cacheUrls;
};

#endif // QQPINIURLHELPER_H

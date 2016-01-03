#ifndef QQPINIURLHELPER_H
#define QQPINIURLHELPER_H

#include "core/qqnetworkaccessor.h"

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

	void cancel();
	void getUrlInfo(QUrl &url);

	virtual void transformMessage(const QString& bouchot, QString &message);

signals:
	void contentTypeAvailable(QUrl &url, QString &contentType);
	void mmDataAvailable(QUrl &url, QString &contentType);
	void videoTitleAvailable(QUrl &url, QString &videoTitle);

public slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

private:

	enum RequestTypeAttribute {
		RequestContentType = QNetworkRequest::User,
		RequestDailyMotionInfo,
		RequestVimeoInfo,
		RequestYoutubeInfo
	};

	void getContentType(QUrl &url);
	void handleContentTypeResponse(const QString &contentType, QUrl &sourceUrl);
	void getDailymotionExtendedInfo(QUrl &url);
	void handleDailymotionExtendedInfo(const QByteArray &jsonInfo, QUrl &sourceUrl);
	void getSaufCaExtendedInfo(QUrl &url);
	void getVimeoExtendedInfo(QUrl &url);
	void handleVimeoExtendedInfo(const QByteArray &jsonInfo, QUrl &sourceUrl);
	void getYoutubeExtendedInfo(QUrl &url);
	void handleYoutubeExtendedInfo(const QByteArray &jsonInfo, QUrl &sourceUrl);

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

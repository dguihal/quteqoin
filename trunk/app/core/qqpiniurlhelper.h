#ifndef QQPINIURLHELPER_H
#define QQPINIURLHELPER_H

#include "qqnetworkaccessor.h"

#include "core/qqmessagetransformfilter.h"

#include <QList>
#include <QUrl>
#include <QString>

class QQPiniUrlHelper : public QQNetworkAccessor, public QQMessageTransformFilter
{
	Q_OBJECT
public:
	explicit QQPiniUrlHelper(QObject *parent = 0);

	void getContentType(const QUrl &url);

	virtual void transformMessage(const QString& bouchot, QString &message);

signals:
	void contentTypeAvailable(QUrl &url, QString &contentType);

public slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

private:
	QList<QNetworkReply *> m_contentTypeReplies;
};

#endif // QQPINIURLHELPER_H

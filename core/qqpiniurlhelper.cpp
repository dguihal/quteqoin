#include "qqpiniurlhelper.h"

#include <QtDebug>
#include <QStringList>

QQPiniUrlHelper::QQPiniUrlHelper(QObject *parent) :
	QQNetworkAccessor(parent)
{
	m_contentTypeReplies.clear();
}

void QQPiniUrlHelper::getContentType(const QUrl &url)
{
	QNetworkReply *reply = httpHead(QNetworkRequest(url));
	m_contentTypeReplies.append(reply);
}

void QQPiniUrlHelper::requestFinishedSlot(QNetworkReply *reply)
{
	if(m_contentTypeReplies.contains(reply))
	{
		m_contentTypeReplies.removeAll(reply);
		QString rep(tr("Unknown"));
		if(reply->error() == QNetworkReply::NoError)
		{
			QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
			QStringList contentTypeParts = contentType.split(";", QString::SkipEmptyParts);
			if(contentTypeParts.size() > 0)
				rep = contentTypeParts.at(0);
		}
		else
			qDebug() << reply->errorString();

		QUrl sourceUrl = reply->request().url();
		emit contentTypeAvailable(sourceUrl, rep);
	}
}

#include "qqpiniurlhelper.h"

#include <QtDebug>
#include <QStringList>

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
void QQPiniUrlHelper::getContentType(const QUrl &url)
{
	QNetworkReply *reply = httpHead(QNetworkRequest(url));
	m_contentTypeReplies.append(reply);
}

//////////////////////////////////////////////////////////////
/// \brief transformMessage
/// \param post
/// \param message
///
void QQPiniUrlHelper::transformMessage(const QQPost *post, QString &message)
{
	Q_UNUSED(post)

	QRegExp reg("(<a [^>]*href=\"https?://"
				"(?:(?:[\\w-\\.])+\\.)*"
				"([\\w-]{3,})\\."
				"(?:[\\.\\w-]+)"
				"[^\"]*\"[^>]*>)"
				"\\[url\\]"
				"(</a>)",
				Qt::CaseInsensitive,
				QRegExp::RegExp2);

	message.replace(reg, "\\1[\\2]\\3");

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

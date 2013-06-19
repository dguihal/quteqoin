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

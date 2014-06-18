#include "qqpalmifileposter.h"

#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QStringList>
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeType>
#include <QMimeDatabase>
#endif

//////////////////////////////////////////////////////////////
/// \brief QQPalmiFilePoster::QQPalmiFilePoster
/// \param parent
///
QQPalmiFilePoster::QQPalmiFilePoster(QObject *parent) :
	QQNetworkAccessor(parent),
	m_service(UPLOAD_3TER_ORG)
{
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiFilePoster::postFile
/// \param file
///
///
bool QQPalmiFilePoster::postFile(const QString &fileName)
{
	QFileInfo fi(fileName);

	if(! (fi.exists() && fi.isReadable()))
		return false;

	QFile *file = new QFile(fi.canonicalFilePath());
	file->open(QIODevice::ReadOnly);

	bool rep = true;

	switch (m_service)
	{
	case PASTELINK:
		postFilePasteLink(file);
		break;
	case UPLOAD_3TER_ORG:
		postFileUpload3TerOrg(file);
		break;
	default:
		rep = false;
	}

	return rep;
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiFilePoster::requestFinishedSlot
/// \param reply
///
void QQPalmiFilePoster::requestFinishedSlot(QNetworkReply *reply)
{
	if(reply->error() == QNetworkReply::NoError &&
			reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
	{
		QString s = QString(reply->readAll());
		switch (m_service)
		{
		case PASTELINK:
			parsePasteLinkResponse(s);
			break;
		case UPLOAD_3TER_ORG:
			parseUpload3TerOrg(s);
			break;
		default:
			break;
		}
	}
	else
		emit postErr(reply->errorString());

	reply->deleteLater();
}

void QQPalmiFilePoster::postFilePasteLink(QFile *file)
{
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart filePart;
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QMimeDatabase mimeDatabase;
	QMimeType mimeType = mimeDatabase.mimeTypeForFile(file->fileName(), QMimeDatabase::MatchDefault);
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType.name()));
#else
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
#endif
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString("form-data; name=\"files[]\"; filename=\"%1\"").arg(file->fileName()));
	filePart.setBodyDevice(file);
	file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

	multiPart->append(filePart);

	QUrl url("http://pastelink.me/server/php/");
	QNetworkRequest request(url);
	request.setRawHeader("User-Agent", "Mozilla/5.0 (quteqoin)"); // Le service n'accepte pas l'ua par défaut ...
	request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
	request.setRawHeader("Accept-Language", "en-US,en;q=0.5");
	//request.setRawHeader("Accept-Encoding", "gzip, deflate"); // Qt 4 ne supporte pas gunzip nativement ....
	request.setRawHeader("X-Requested-With", "XMLHttpRequest");
	request.setRawHeader("Referer", "http://pastelink.me/");

	QNetworkReply *reply = httpPost(request, multiPart);
	multiPart->setParent(reply); // delete the multiPart with the reply
}

void QQPalmiFilePoster::parsePasteLinkResponse(const QString &data)
{
	//JSON seulement supporté par Qt 5
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		QJsonDocument d = QJsonDocument::fromBinaryData(data.toLatin1());
		QJsonObject o = d.array().at(0).toObject();
		emit finished(QString("http://pastelink.me/dl/") + o["key"].toString());
#else
		int i = data.indexOf("\"key\":\"");
		if(i >= 0)
		{
			int j = data.indexOf("\"", i);
			int start = i + 7; // len /"key":"/
			QString key = data.mid(start, j - start);
			emit finished(QString("http://pastelink.me/dl/") + key);
		}
#endif
}

void QQPalmiFilePoster::postFileUpload3TerOrg(QFile *file)
{
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart filePart;
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QMimeDatabase mimeDatabase;
	QMimeType mimeType = mimeDatabase.mimeTypeForFile(file->fileName(), QMimeDatabase::MatchDefault);
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType.name()));
#else
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
#endif
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString("form-data; name=\"file\"; filename=\"%1\"").arg(file->fileName()));
	filePart.setBodyDevice(file);
	file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

	multiPart->append(filePart);

	QHttpPart timePart;
	timePart.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString("form-data; name=\"time\""));
	timePart.setBody(QString::fromLatin1("week").toLatin1());
	multiPart->append(timePart);

	QUrl url("http://upload.3ter.org/script.php");
	QNetworkRequest request(url);
	//request.setRawHeader("User-Agent", "Mozilla/5.0 (quteqoin)"); // Le service n'accepte pas l'ua par défaut ...
	//request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
	//request.setRawHeader("Accept-Language", "en-US,en;q=0.5");
	//request.setRawHeader("Accept-Encoding", "gzip, deflate"); // Qt 4 ne supporte pas gunzip nativement ....
	//request.setRawHeader("X-Requested-With", "XMLHttpRequest");
	request.setRawHeader("Referer", "http://upload.3ter.org/");

	QNetworkReply *reply = httpPost(request, multiPart);
	multiPart->setParent(reply); // delete the multiPart with the reply
}

void QQPalmiFilePoster::parseUpload3TerOrg(const QString &data)
{
	QStringList strL = data.split(QChar('\n'));
	if(strL.size() > 0)
	{
		emit finished(QString("http://upload.3ter.org/f.php?h=") + strL.first());
	}
}

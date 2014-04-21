#include "qqpalmifileposter.h"

#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
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
	QQNetworkAccessor(parent)
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
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart filePart;
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QMimeDatabase mimeDatabase;
	QMimeType mimeType = mimeDatabase.mimeTypeForFile(fi, QMimeDatabase::MatchDefault);
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType.name()));
#else
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
#endif
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
					   QString("form-data; name=\"files[]\"; filename=\"%1\"").arg(fi.fileName()));
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

	return true;
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
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		QJsonDocument d = QJsonDocument::fromJson(reply->readAll());
		QJsonObject o = d.array().at(0).toObject();
		emit finished(QString("http://pastelink.me/dl/") + o["key"].toString());
#else
		QString s = QString(reply->readAll()); //JSON mais non supporté par Qt 4 (Qt 5 ?)
		int i = s.indexOf("\"key\":\"");
		if(i >= 0)
		{
			int j = s.indexOf("\",", i);
			int start = i + 7; // len /"key":"/
			QString key = s.mid(start, j - start);
			emit finished(QString("http://pastelink.me/dl/") + key);
		}
#endif
	}
	else
		emit postErr(reply->errorString());

	reply->deleteLater();
}

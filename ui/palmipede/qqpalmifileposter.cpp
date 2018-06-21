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

	QNetworkReply * rep = NULL;

	QQSettings settings;
	QString sharingService = settings.value(SETTINGS_FILE_SHARING_SERVICE, DEFAULT_FILE_SHARING_SERVICE).toString();

	if (sharingService == FILE_SHARING_SERVICE_UP_Y_FR)
		rep = postFileJusYFr(file);
	else if (sharingService == FILE_SHARING_SERVICE_JIRAFEAU_EUROMUSSELS_EU)
		rep = postFileUpload3TerOrg(file);

	if (rep != NULL)
		connect(rep, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgressSlot(qint64,qint64)));

	return (rep != NULL);
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
		QQSettings settings;
		QString sharingService = settings.value(SETTINGS_FILE_SHARING_SERVICE, DEFAULT_FILE_SHARING_SERVICE).toString();

		QString s = QString::fromUtf8(reply->readAll());
		if (sharingService == FILE_SHARING_SERVICE_UP_Y_FR)
			parseJusYFr(s);
		else if (sharingService == FILE_SHARING_SERVICE_JIRAFEAU_EUROMUSSELS_EU)
			parseUpload3TerOrg(s);
	}
	else
		emit postErr(reply->errorString());

	reply->deleteLater();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiFilePoster::uploadProgressSlot
/// \param bytesSent
/// \param bytesTotal
///
void QQPalmiFilePoster::uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal)
{
	if (bytesTotal > 0)
	{
		quint32 progress = (100 * bytesSent) / bytesTotal;
		emit uploadProgress(progress);
	}
}

////////////
/// \brief QQPalmiFilePoster::postFileUpload3TerOrg
/// \param file
///
QNetworkReply * QQPalmiFilePoster::postFileUpload3TerOrg(QFile *file)
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

	QUrl url(QString("http://%1/script.php").arg(FILE_SHARING_SERVICE_JIRAFEAU_EUROMUSSELS_EU));
	QNetworkRequest request(url);
	//request.setRawHeader("User-Agent", "Mozilla/5.0 (quteqoin)"); // Le service n'accepte pas l'ua par défaut ...
	//request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
	//request.setRawHeader("Accept-Language", "en-US,en;q=0.5");
	//request.setRawHeader("Accept-Encoding", "gzip, deflate"); // Qt 4 ne supporte pas gunzip nativement ....
	//request.setRawHeader("X-Requested-With", "XMLHttpRequest");
	request.setRawHeader("Referer", QString("http://%1/").arg(FILE_SHARING_SERVICE_JIRAFEAU_EUROMUSSELS_EU).toLatin1());

	QNetworkReply *reply = httpPost(request, multiPart);
	multiPart->setParent(reply); // delete the multiPart with the reply

	return reply;
}

///////
/// \brief QQPalmiFilePoster::parseUpload3TerOrg
/// \param data
///
void QQPalmiFilePoster::parseUpload3TerOrg(const QString &data)
{
	QStringList strL = data.split(QChar('\n'));
	if(strL.size() > 0)
		emit finished(QString("http://%1/f.php?h=%2&p=1").arg(FILE_SHARING_SERVICE_JIRAFEAU_EUROMUSSELS_EU).arg(strL.first()));
}

///////////
/// \brief postFileJusYFr
/// \param file
///
QNetworkReply * QQPalmiFilePoster::postFileJusYFr(QFile *file)
{
	QUrl url(QString("https://%1").arg(FILE_SHARING_SERVICE_UP_Y_FR));
	QNetworkRequest request(url);

	return httpPut(request, file);
}

//////////
/// \brief parseJusYFr
/// \param s
///
void QQPalmiFilePoster::parseJusYFr(const QString &s)
{
	emit finished(QString(s));
}

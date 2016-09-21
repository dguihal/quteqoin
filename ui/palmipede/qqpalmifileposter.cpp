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

	bool rep = true;

	QQSettings settings;
	QString sharingService = settings.value(SETTINGS_FILE_SHARING_SERVICE, DEFAULT_FILE_SHARING_SERVICE).toString();

	if (sharingService == FILE_SHARING_SERVICE_UP_Y_FR)
		postFileJusYFr(file);
	else if (sharingService == FILE_SHARING_SERVICE_JIRAFEAU_3TER_ORG)
		postFileUpload3TerOrg(file);
	else
		rep = false;

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
		QQSettings settings;
		QString sharingService = settings.value(SETTINGS_FILE_SHARING_SERVICE, DEFAULT_FILE_SHARING_SERVICE).toString();

		QString s = QString(reply->readAll());
		if (sharingService == FILE_SHARING_SERVICE_UP_Y_FR)
			parseJusYFr(s);
		else if (sharingService == FILE_SHARING_SERVICE_JIRAFEAU_3TER_ORG)
			parseUpload3TerOrg(s);
	}
	else
		emit postErr(reply->errorString());

	reply->deleteLater();
}

////////////
/// \brief QQPalmiFilePoster::postFileUpload3TerOrg
/// \param file
///
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

///////
/// \brief QQPalmiFilePoster::parseUpload3TerOrg
/// \param data
///
void QQPalmiFilePoster::parseUpload3TerOrg(const QString &data)
{
	QStringList strL = data.split(QChar('\n'));
	if(strL.size() > 0)
	{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		qDebug() << Q_FUNC_INFO << QString("http://upload.3ter.org/f.php?h=%1&p=1").arg(strL.first());
		emit finished(QString("http://upload.3ter.org/f.php?h=%1&p=1").arg(strL.first()));
#else
		emit finished(QString("http://upload.3ter.org/f.php?h=") + strL.first());
#endif
	}
}

///////////
/// \brief postFileJusYFr
/// \param file
///
void QQPalmiFilePoster::postFileJusYFr(QFile *file)
{

	QUrl url("https://up.ÿ.fr");
	QNetworkRequest request(url);

	httpPut(request, file);
}

//////////
/// \brief parseJusYFr
/// \param s
///
void QQPalmiFilePoster::parseJusYFr(const QString &s)
{
	emit finished(QString(s));
}

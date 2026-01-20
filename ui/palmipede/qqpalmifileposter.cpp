#include "qqpalmifileposter.h"

#include <QFile>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QMimeType>
#include <QStringList>
#include <QUrlQuery>
#include <QtDebug>

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

	QNetworkReply * rep = nullptr;

	QQSettings settings;
	QString sharingService = settings.value(SETTINGS_FILE_SHARING_SERVICE, DEFAULT_FILE_SHARING_SERVICE).toString();

	if (sharingService == FILE_SHARING_SERVICE_FILE_IO)
		rep = postFileFileIO(file);

	if (rep != nullptr)
		connect(rep, &QNetworkReply::uploadProgress, this, &QQPalmiFilePoster::uploadProgressSlot);

	return (rep != nullptr);
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
		if (sharingService == FILE_SHARING_SERVICE_FILE_IO)
			parseFileIO(s);
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

///////////
/// \brief postFileFileIO
/// \param file
///
QNetworkReply *QQPalmiFilePoster::postFileFileIO(QFile *file)
{
	QUrl url(QString("https://%1/?expires=2d").arg(FILE_SHARING_SERVICE_FILE_IO));
	QNetworkRequest request(url);

	auto postData = QByteArray("file=");
	postData.append(file->readAll());

	return httpPost(request, postData);
}

//////////
/// \brief parseJusYFr
/// \param s
///
void QQPalmiFilePoster::parseFileIO(const QString &data)
{
	auto jsonDoc = QJsonDocument::fromJson(data.toUtf8());
	emit finished(jsonDoc.object().value("link").toString());
}

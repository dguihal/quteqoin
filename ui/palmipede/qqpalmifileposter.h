#ifndef QQPALMIFILEPOSTER_H
#define QQPALMIFILEPOSTER_H

#include "core/qqnetworkaccessor.h"
#include "core/qqsettings.h"

class QFile;

class QQPalmiFilePoster : public QQNetworkAccessor
{
	Q_OBJECT
public:
	explicit QQPalmiFilePoster(QObject *parent = 0);

	bool postFile(const QString &fileName);

signals:
	void finished(QString url);
	void uploadProgress(quint32 uploadPct);
	void postErr(const QString &errMsg);

protected slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);
	void uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal);

private:
	QNetworkReply * postFileUpload3TerOrg(QFile *file);
	void parseUpload3TerOrg(const QString &s);
	QNetworkReply * postFileJusYFr(QFile *file);
	void parseJusYFr(const QString &s);

};

#endif // QQPALMIFILEPOSTER_H

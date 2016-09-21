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
	void postErr(const QString &errMsg);

protected slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

private:
	void postFileUpload3TerOrg(QFile *file);
	void parseUpload3TerOrg(const QString &s);
	void postFileJusYFr(QFile *file);
	void parseJusYFr(const QString &s);

};

#endif // QQPALMIFILEPOSTER_H

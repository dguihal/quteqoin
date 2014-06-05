#ifndef QQPALMIFILEPOSTER_H
#define QQPALMIFILEPOSTER_H

#include "core/qqnetworkaccessor.h"

class QQPalmiFilePoster : public QQNetworkAccessor
{
	Q_OBJECT
public:
	//enum Services { PASTELINK, CJOINT };
	explicit QQPalmiFilePoster(QObject *parent = 0);

	bool postFile(const QString &fileName);

signals:
	void finished(QString url);
	void postErr(const QString &errMsg);

protected slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

};

#endif // QQPALMIFILEPOSTER_H

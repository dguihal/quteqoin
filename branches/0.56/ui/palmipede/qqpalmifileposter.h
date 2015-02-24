#ifndef QQPALMIFILEPOSTER_H
#define QQPALMIFILEPOSTER_H

#include "core/qqnetworkaccessor.h"

class QFile;

class QQPalmiFilePoster : public QQNetworkAccessor
{
	Q_OBJECT
public:
	//enum Services { PASTELINK, CJOINT };
	enum Services { PASTELINK, UPLOAD_3TER_ORG };
	explicit QQPalmiFilePoster(QObject *parent = 0);

	bool postFile(const QString &fileName);

	void setService(Services service) { m_service = service; }

signals:
	void finished(QString url);
	void postErr(const QString &errMsg);

protected slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

private:
	void postFilePasteLink(QFile *file);
	void parsePasteLinkResponse(const QString &data);
	void postFileUpload3TerOrg(QFile *file);
	void parseUpload3TerOrg(const QString &s);

	Services m_service;

};

#endif // QQPALMIFILEPOSTER_H

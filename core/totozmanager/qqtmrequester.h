#ifndef QQTMREQUESTER_H
#define QQTMREQUESTER_H

#include "../qqnetworkaccessor.h"

class QNetworkReply;
class QQSettings;
class QQTMXmlParser;

// http://totoz.sauf.ca/api


class QQTMRequester : public QQNetworkAccessor
{
	Q_OBJECT

public:
	explicit QQTMRequester(QObject * parent);
	~QQTMRequester();

	QList<QString> results() { return m_totozes; }

public slots:
	virtual void requestFinishedSlot(QNetworkReply * reply);
	void searchTotoz(const QString & key);
	void cancel();

signals:
	void requestFinished();

protected:
	void searchTotoz(const QString & key, int offset);

private:
	QQTMXmlParser * m_xmlParser;

	QNetworkReply * m_netReply;

	QList<QString> m_totozes;
	QString m_currKey;
};

#endif // QQTMREQUESTER_H

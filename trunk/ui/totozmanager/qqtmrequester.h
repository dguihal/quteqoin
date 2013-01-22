#ifndef QQTMREQUESTER_H
#define QQTMREQUESTER_H

#include "core/qqnetworkaccessor.h"

class QNetworkReply;
class QQSettings;
class QQTMXmlParser;

// http://totoz.sauf.ca/api


class QQTMRequester : public QQNetworkAccessor
{
	Q_OBJECT

public:
	explicit QQTMRequester(QQSettings * settings);
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

protected slots:
	void parsingFinished();

private:
	QQSettings * m_settings;
	QQTMXmlParser * m_xmlParser;

	QNetworkReply * m_netReply;

	QList<QString> m_totozes;
	QString m_currKey;
};

#endif // QQTMREQUESTER_H

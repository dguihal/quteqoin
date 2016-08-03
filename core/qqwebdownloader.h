#ifndef QQWEBDOWNLOADER_H
#define QQWEBDOWNLOADER_H

#include "core/qqnetworkaccessor.h"

#include <QUrl>

class QQWebDownloader : public QQNetworkAccessor
{
	Q_OBJECT
public:
	QQWebDownloader(QObject *parent);

	//////////////////////////////////////////////////////////////
	/// \brief imgData
	/// \return
	///
	QByteArray imgData() { return m_data; }
	QString dataContentType() { return m_dataContentType; }

	void getURL(const QUrl &url);

signals:
	void ready(QUrl &url);

protected slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

private:
	QString m_dataContentType;
	QByteArray m_data;
	QList<QUrl> m_listPendingUrl;
};

#endif // QQWEBDOWNLOADER_H

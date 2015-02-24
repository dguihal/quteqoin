#ifndef QQWEBIMAGEDOWNLOADER_H
#define QQWEBIMAGEDOWNLOADER_H

#include "core/qqnetworkaccessor.h"

class QQWebImageDownloader : public QQNetworkAccessor
{
	Q_OBJECT
public:
	QQWebImageDownloader(QObject *parent);

	//////////////////////////////////////////////////////////////
	/// \brief imgData
	/// \return
	///
	QByteArray imgData() { return m_data; }
	QString dataContentType() { return m_dataContentType; }

	void getImage(const QUrl &url);

signals:
	void ready();

protected slots:
	virtual void requestFinishedSlot(QNetworkReply *reply);

private:
	QString m_dataContentType;
	QByteArray m_data;
	QList<QUrl> m_listPendingUrl;
};

#endif // QQWEBIMAGEDOWNLOADER_H

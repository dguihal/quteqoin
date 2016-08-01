#ifndef QQBACKEND_H
#define QQBACKEND_H

#include "core/qqnetworkaccessor.h"

class QQBackendParser;

class QQBackend : public QQNetworkAccessor
{
	Q_OBJECT

public:

	enum QQBouchotEvent { NewPostsAvailable = 1, StateChanged = 2 };
	Q_DECLARE_FLAGS(QQBouchotEvents, QQBouchotEvent)
//	enum QQBackendEvent { NewPostsAvailable = 1, StateChanged = 2 };
//	Q_DECLARE_FLAGS(QQBackendEvents, QQBackendEvent)

	QQBackend(const QString &name, QObject *parent = 0);
	~QQBackend();

	enum BackendType{ Bouchot, Slack };

	void startRefresh();
	void stopRefresh();
	int currentRefreshInterval();

	virtual QQBackend::BackendType backendType() = 0;

protected:

	QString m_name;

	QQBackendParser *m_parser;
	struct EventReceiver {
		QQBouchotEvents acceptedEvents;
		QObject * receiver;
	};
	QList<EventReceiver> m_listEventReceivers;

};

#endif // QQBACKEND_H

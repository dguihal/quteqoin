#ifndef QQBACKENDUPDATEDEVENT_H
#define QQBACKENDUPDATEDEVENT_H

#include <QEvent>
#include <QString>

class QQBackendUpdatedEvent : public QEvent
{
public:
	static const QEvent::Type BACKEND_UPDATED;
	QQBackendUpdatedEvent(QEvent::Type type, QString group);

	QString group() { return m_group; }

private:
	QString m_group;
};

#endif // QQBACKENDUPDATEDEVENT_H

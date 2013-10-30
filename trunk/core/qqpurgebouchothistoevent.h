#ifndef QQPURGEBOUCHOTHISTOEVENT_H
#define QQPURGEBOUCHOTHISTOEVENT_H

#include <QEvent>
#include <QString>

class QQPurgeBouchotHistoEvent : public QEvent
{
public:
	static const QEvent::Type PURGE_BOUCHOT_HISTO;
	QQPurgeBouchotHistoEvent();
};

#endif // QQPURGEBOUCHOTHISTOEVENT_H

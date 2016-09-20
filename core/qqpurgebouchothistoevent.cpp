#include "qqpurgebouchothistoevent.h"

const QEvent::Type QQPurgeBouchotHistoEvent::PURGE_BOUCHOT_HISTO =
 (QEvent::Type)QEvent::registerEventType();

QQPurgeBouchotHistoEvent::QQPurgeBouchotHistoEvent() :
	QEvent(PURGE_BOUCHOT_HISTO)
{
}

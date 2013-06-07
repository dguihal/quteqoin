#include "qqpurgebouchothistoevent.h"

const QEvent::Type QQPurgeBouchotHistoEvent::PURGE_BOUCHOT_HISTO =
 (QEvent::Type)QEvent::registerEventType();

QQPurgeBouchotHistoEvent::QQPurgeBouchotHistoEvent(Type type, QString maxId) :
QEvent(type), m_maxId(maxId)
{
}

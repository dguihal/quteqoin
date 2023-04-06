#include "qqbackendupdatedevent.h"

#include <utility>

const QEvent::Type QQBackendUpdatedEvent::BACKEND_UPDATED =
 (QEvent::Type)QEvent::registerEventType();

QQBackendUpdatedEvent::QQBackendUpdatedEvent(QEvent::Type type, QString group) :
QEvent(type), m_group(std::move(group))
{
}

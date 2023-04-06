#include "qqboardstatechangeevent.h"

#include <utility>

const QEvent::Type QQBoardStateChangeEvent::BOARD_STATE_CHANGED =
 (QEvent::Type)QEvent::registerEventType();

QQBoardStateChangeEvent::QQBoardStateChangeEvent(QString boardName) :
	QEvent(BOARD_STATE_CHANGED),
	m_boardName(std::move(boardName))
{
}

QString QQBoardStateChangeEvent::boardName()
{
	return m_boardName;
}

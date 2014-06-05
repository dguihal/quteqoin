#include "qqboardstatechangeevent.h"

const QEvent::Type QQBoardStateChangeEvent::BOARD_STATE_CHANGED =
 (QEvent::Type)QEvent::registerEventType();

QQBoardStateChangeEvent::QQBoardStateChangeEvent(const QString &boardName) :
	QEvent(BOARD_STATE_CHANGED),
	m_boardName(boardName)
{
}

QString QQBoardStateChangeEvent::boardName()
{
	return m_boardName;
}

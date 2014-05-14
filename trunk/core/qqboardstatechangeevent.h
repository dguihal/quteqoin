#ifndef QQBOARDSTATECHANGEEVENT_H
#define QQBOARDSTATECHANGEEVENT_H

#include <QEvent>
#include <QString>

class QQBoardStateChangeEvent : public QEvent
{
public:
	static const QEvent::Type BOARD_STATE_CHANGED;

	QQBoardStateChangeEvent(const QString &boardName);
	QString boardName();

private:
	QString m_boardName;
};

#endif // QQBOARDSTATECHANGEEVENT_H

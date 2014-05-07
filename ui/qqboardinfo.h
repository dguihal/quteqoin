#ifndef QQBOARDINFO_H
#define QQBOARDINFO_H

#include "core/qqmussel.h"

#include <QHash>
#include <QPropertyAnimation>
#include <QWidget>

namespace Ui {
class QQBoardInfo;
}

class QQBouchot;
class QQMusselInfo;

class QQBoardInfo : public QWidget
{
	Q_OBJECT

public:
	explicit QQBoardInfo(QQBouchot *board, QWidget *parent = 0);
	~QQBoardInfo();

	virtual QSize sizeHint() const;

public slots:
	void musselSelected(QQMussel mussel);
	void rearmRefreshPB();
	void resetFromErrorState();
	void showRefreshError(QString &errMsg);
	void toggleExpandedView();
	void updateUserList();

protected:

private:
	Ui::QQBoardInfo *m_ui;

	QQBouchot *m_board;
	QPropertyAnimation m_pctPollAnimation;
	bool m_refreshFailed;

	QHash<QQMussel, QQMusselInfo *> m_musselInfoHash;
};

#endif // QQBOARDINFO_H

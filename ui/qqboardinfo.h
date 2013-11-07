#ifndef QQBOARDINFO_H
#define QQBOARDINFO_H

#include <QPropertyAnimation>
#include <QWidget>

namespace Ui {
class QQBoardInfo;
}

class QQBouchot;

class QQBoardInfo : public QWidget
{
	Q_OBJECT

public:
	explicit QQBoardInfo(QQBouchot *board, QWidget *parent = 0);
	~QQBoardInfo();

public slots:
	void rearmRefreshPB();
	void showRefreshError(QString &errMsg);
	void toggleExpandedView();
	void updateUserList();

private:
	Ui::QQBoardInfo *m_ui;

	QQBouchot *m_board;
	QPropertyAnimation m_pctPollAnimation;
	bool m_refreshFailed;
};

#endif // QQBOARDINFO_H

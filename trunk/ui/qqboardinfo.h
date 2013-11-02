#ifndef QQBOARDINFO_H
#define QQBOARDINFO_H

#include <QWidget>

namespace Ui {
class QQBoardInfo;
}

class QQBouchot;

class QQBoardInfo : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(int m_pctPoll READ pctPoll WRITE setPctPoll)

public:
	explicit QQBoardInfo(QQBouchot *board, QWidget *parent = 0);
	~QQBoardInfo();

	int pctPoll() { return m_pctPoll; }

public slots:
	void setPctPoll(int pctPoll);
	void toggleExpandedView();
	void updateUserList();

private:
	Ui::QQBoardInfo *m_ui;

	QQBouchot *m_board;

	int m_pctPoll;
};

#endif // QQBOARDINFO_H

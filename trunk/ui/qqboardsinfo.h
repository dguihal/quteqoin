#ifndef QQBOARDSINFO_H
#define QQBOARDSINFO_H

namespace Ui {
class QQBoardsInfo;
}

#include <QDockWidget>


class QQBoardsInfo : public QDockWidget
{
	Q_OBJECT
public:
	explicit QQBoardsInfo(QWidget *parent = 0);

public slots:
	void updateBoardList();

private:
	Ui::QQBoardsInfo *m_ui;

	QString m_boardName;
};

#endif // QQBOARDSINFO_H

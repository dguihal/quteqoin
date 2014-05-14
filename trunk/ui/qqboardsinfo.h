#ifndef QQBOARDSINFO_H
#define QQBOARDSINFO_H

namespace Ui {
class QQBoardsInfo;
}

class QQBoardInfo;

#include <QDockWidget>
#include <QHash>
#include <QPointer>

class QQBoardsInfo : public QDockWidget
{
	Q_OBJECT
public:
	explicit QQBoardsInfo(QWidget *parent = 0);
	~QQBoardsInfo();

	void updateBoardList();

private:
	Ui::QQBoardsInfo *m_ui;

	QHash<QString, QPointer<QQBoardInfo> > m_hashbInfo;
};

#endif // QQBOARDSINFO_H

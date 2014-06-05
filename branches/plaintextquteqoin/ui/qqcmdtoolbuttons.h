#ifndef QQCMDTOOLBUTTONS_H
#define QQCMDTOOLBUTTONS_H

#include <QWidget>

namespace Ui {
class QQCmdToolButtons;
}

class QQCmdToolButtons : public QWidget
{
	Q_OBJECT

public:
	explicit QQCmdToolButtons(QWidget *parent = 0);
	~QQCmdToolButtons();

	void addAction(QAction *action);

signals:
	void showOptions();

protected slots:
	void optionsButtonClicked();

private:
	Ui::QQCmdToolButtons *ui;
};

#endif // QQCMDTOOLBUTTONS_H

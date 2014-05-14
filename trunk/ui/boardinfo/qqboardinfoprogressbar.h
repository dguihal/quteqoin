#ifndef QQBOARDINFOPROGRESSBAR_H
#define QQBOARDINFOPROGRESSBAR_H

#include <QProgressBar>
#include <QString>

class QQBoardInfoProgressBar : public QProgressBar
{
	Q_OBJECT
public:
	explicit QQBoardInfoProgressBar(QWidget *parent = 0);

	void setBoardStatusFlags(const QString &flags);
	void setBoardName(const QString &boardName);

signals:
	void bouchotSelected(const QString &bouchotName);

public slots:

protected:
	virtual void mousePressEvent(QMouseEvent *event);

private:
	QString m_boardName;
};

#endif // QQBOARDINFOPROGRESSBAR_H

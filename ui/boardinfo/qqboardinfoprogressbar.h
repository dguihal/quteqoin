#ifndef QQBOARDINFOPROGRESSBAR_H
#define QQBOARDINFOPROGRESSBAR_H

#include <QColor>
#include <QProgressBar>
#include <QString>

class QQBoardInfoProgressBar : public QProgressBar
{
	Q_OBJECT

public:
	explicit QQBoardInfoProgressBar(QWidget *parent = nullptr);

	void setBoardStatusFlags(const QString &flags);
	void setBoardName(const QString &boardName);

	QColor boardColor() const { return m_boardColor; }
	void setBoardColor(const QColor &boardColor) { m_boardColor = boardColor; }

	bool isOnError() const { return m_onError; }
	void setOnError(const bool &onError);

signals:
	void bouchotSelected();

public slots:

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void paintEvent(QPaintEvent *event);

private:
	QString m_boardName;
	QColor m_boardColor;

	bool m_onError;
};

#endif // QQBOARDINFOPROGRESSBAR_H

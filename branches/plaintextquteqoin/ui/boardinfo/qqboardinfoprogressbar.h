#ifndef QQBOARDINFOPROGRESSBAR_H
#define QQBOARDINFOPROGRESSBAR_H

#include <QColor>
#include <QProgressBar>
#include <QString>

class QQBoardInfoProgressBar : public QProgressBar
{
	Q_OBJECT
	Q_PROPERTY(bool onError READ isOnError WRITE setOnError)
	Q_PROPERTY(QColor color READ boardColor WRITE setBoardColor)

public:
	explicit QQBoardInfoProgressBar(QWidget *parent = 0);

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

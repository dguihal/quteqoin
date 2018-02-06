#ifndef QQMLPALMIPEDE_H
#define QQMLPALMIPEDE_H

#include <QQuickWidget>
#include <QQuickItem>

class QQmlPalmipede : public QQuickWidget
{
	Q_OBJECT
public:
	QQmlPalmipede(QWidget *parent = Q_NULLPTR);

signals:
	void postMessage(const QString & bouchot, const QString & message);

public slots:
	void insertReplaceText(const QString &txt);
	void insertReplaceTextBoard(const QString &board, const QString &txt);

protected:
	virtual void focusInEvent(QFocusEvent * event);
};

#endif // QQMLPALMIPEDE_H

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
	void changeBoard(bool next);
	void insertReplaceTextBoard(const QString &board, const QString &tag);
	void insertReplaceText(const QString &tag);

};

#endif // QQMLPALMIPEDE_H

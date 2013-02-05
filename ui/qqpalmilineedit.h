#ifndef QQPALMILINEEDIT_H
#define QQPALMILINEEDIT_H

#include <QLineEdit>

class QFocusEvent;
class QKeyEvent;

class QQPalmiLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit QQPalmiLineEdit(QWidget *parent = 0);

signals:

public slots:
	void changeColor(const QColor &);
	void insertSurroundText(const QString & bTag, const QString & eTag);
	void insertReplaceText(const QString & tag);

protected:
	virtual void keyPressEvent(QKeyEvent * e);
	virtual void focusInEvent(QFocusEvent *e);

private:
	void updateTotozCompleter();
};

#endif // QQPALMILINEEDIT_H

#ifndef QQPALMILINEEDIT_H
#define QQPALMILINEEDIT_H

#include <QLineEdit>
#include <QStringList>

class QFocusEvent;
class QKeyEvent;
class QToolButton;

class QQPalmiLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit QQPalmiLineEdit(QWidget *parent = 0);

signals:

public slots:
	void changeColor(const QColor &newColor);
	void insertText(const QString &str);

	void blam();
	void bold();
	void italic();
	void paf();
	void moment();
	void strike();
	void underline();

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void focusInEvent(QFocusEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

private slots:
	void updateCloseButton(const QString &text);

private:
	void updateTotozCompleter();
	void completeTotoz();

	QStringList m_listTotoz;
	QToolButton *m_clearButton;
};

#endif // QQPALMILINEEDIT_H

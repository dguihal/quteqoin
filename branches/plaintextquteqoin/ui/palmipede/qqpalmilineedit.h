#ifndef QQPALMILINEEDIT_H
#define QQPALMILINEEDIT_H

#include "qqpalmifileposter.h"

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
	void changeBoard(bool next);

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

	void attachFile(QString fileName = QString(""));

protected:
	virtual void dropEvent(QDropEvent *event);
	virtual void focusInEvent(QFocusEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

protected slots:
	void joinFileErr(const QString &errStr);

private slots:
	void updateCloseButton(const QString &text);

private:
	void updateTotozCompleter();
	void completeTotoz();

	QQPalmiFilePoster m_fPoster;

	QStringList m_listTotoz;
	QToolButton *m_clearButton;
};

#endif // QQPALMILINEEDIT_H

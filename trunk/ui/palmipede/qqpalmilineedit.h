#ifndef QQPALMILINEEDIT_H
#define QQPALMILINEEDIT_H

#include "qqpalmifileposter.h"

#include <QLineEdit>
#include <QQueue>

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

	void pushCurrentToHistory();

protected:
	virtual void dropEvent(QDropEvent *event);
	virtual void focusInEvent(QFocusEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
#if(QT_VERSION < QT_VERSION_CHECK(5, 2, 0))
	virtual void resizeEvent(QResizeEvent *e);
#endif

protected slots:
	void joinFileErr(const QString &errStr);
//#if(QT_VERSION < QT_VERSION_CHECK(5, 2, 0)) : Marche pas
	void updateCloseButton(const QString &text);
//

private:
	void updateTotozCompleter();
	void completeTotoz();
	void rotateHistory(bool forward = true);

	QQPalmiFilePoster m_fPoster;

	QStringList m_listTotoz;
#if(QT_VERSION < QT_VERSION_CHECK(5, 2, 0))
	QToolButton *m_clearButton;
#endif

	int m_indexInPostHistory;
	QQueue<QString> m_postHistory;
};

#endif // QQPALMILINEEDIT_H

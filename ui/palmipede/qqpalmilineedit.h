#ifndef QQPALMILINEEDIT_H
#define QQPALMILINEEDIT_H

#include "qqpalmifileposter.h"

#include <QFrame>

class QQPalmiLineEditInt;

class QQPalmiLineEdit : public QFrame
{
	Q_OBJECT
public:
	explicit QQPalmiLineEdit(QWidget *parent = 0);

	void insert(const QString &newText);

	void clear();
	void setText(const QString &text);
	QString text() const;

	void pushCurrentToHistory();

	void changeColor(const QColor &newColor);
	void insertText(const QString &str);

signals:
	void changeBoard(bool next);
	void returnPressed();

public slots:
	void blam();
	void bold();
	void italic();
	void paf();
	void moment();
	void strike();
	void underline();

	void attachFile(QString fileName = QString(""));

protected:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

	virtual void paintEvent(QPaintEvent *event);

protected slots:
	void joinFileErr(const QString &errStr);
	void updateUploadProgress(quint32 pctProgress);

private:

	QColor m_currBoardcolor;
	quint32 m_pctUp;

	QQPalmiFilePoster m_fPoster;
	QQPalmiLineEditInt* m_privLineEdit;
};

#endif // QQPALMILINEEDIT_H

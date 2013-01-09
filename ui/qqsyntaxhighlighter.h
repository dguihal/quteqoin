#ifndef QQSYNTAXHIGHLIGHTER_H
#define QQSYNTAXHIGHLIGHTER_H

#include <core/qqnorlogeref.h>
#include <core/qqtotoz.h>

#include <QList>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextDocument>

class QQMessageBlockUserData;

class QQSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:

	enum blockState { NOT_HIGHLIGHTED, NORMAL, NORLOGE_HIGHLIGHTED, FULL_HIGHLIGHTED };

	QQSyntaxHighlighter(QTextDocument * parent = 0);
	~QQSyntaxHighlighter();

	bool highlightSelection(QTextCursor &selection, QQMessageBlockUserData *userData);

	void setNorlogeRefToHighlight(const QQNorlogeRef & norlogeRef) { m_nRef = norlogeRef; }
	void setNotificationWindow(QWidget * window) { m_notifWindow = window; }

signals:
	void totozRequired(const QString & totozId);

public slots:

protected:
	virtual void highlightBlock(const QString &text);

private:
	void highlightBlockForNRef();
	void highlightDuck(const QString & text);
	void formatDuck(int duckIndex, int duckStringLength);
	void highlightNorloge(const QString & text);
	void formatNRef(const QQNorlogeRef & nRef);
	void highlightTableVolante(const QString & text);
	void formatTableV(int tableVIndex, int tableVStringLength);
	void highlightTotoz(const QString & text);
	void formatTotoz(const QQTotoz & totoz);
	void highlightBigorno(const QString & text);

	QQNorlogeRef m_nRef;
	QWidget * m_notifWindow;
};

#endif // QQSYNTAXHIGHLIGHTER_H

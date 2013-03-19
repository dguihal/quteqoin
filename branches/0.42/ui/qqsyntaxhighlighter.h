#ifndef QQSYNTAXHIGHLIGHTER_H
#define QQSYNTAXHIGHLIGHTER_H

#include <core/qqnorlogeref.h>
#include <core/qqsettings.h>
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

	enum blockState { NOT_HIGHLIGHTED = 0x00, NORMAL = 0x01, NORLOGE_HIGHLIGHTED = 0x02, FULL_HIGHLIGHTED = 0x04 };

	QQSyntaxHighlighter(QQSettings * settings, QTextDocument * parent = 0);
	~QQSyntaxHighlighter();

	bool highlightSelection(QTextCursor &selection, QQMessageBlockUserData *userData);

	void setNorlogeRefToHighlight(const QQNorlogeRef & norlogeRef) { m_nRef = norlogeRef; }
	void setNotificationWindow(QWidget * window) { m_notifWindow = window; }

signals:
	void totozRequired(QString & totozId);

public slots:

protected:
	virtual void highlightBlock(const QString &text);

private:
	void highlightBlockForNRef();
	void highlightDuck(const QString & text);
	void formatDuck(int duckIndex, int duckStringLength);
	void highlightNorloge(const QString & text);
	void formatNRef(QQNorlogeRef & nRef);
	void linkNorlogeRef(QQNorlogeRef & nRef);
	void highlightTableVolante(const QString & text);
	void formatTableV(int tableVIndex, int tableVStringLength);
	void highlightTotoz(const QString & text);
	void formatTotoz(int index, const QString & totozId);
	void highlightBigorno(const QString & text);

	QQNorlogeRef m_nRef;
	QWidget * m_notifWindow;
	QQSettings * m_settings;
};

#endif // QQSYNTAXHIGHLIGHTER_H

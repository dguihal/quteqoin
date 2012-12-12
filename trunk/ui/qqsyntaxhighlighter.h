#ifndef QQSYNTAXHIGHLIGHTER_H
#define QQSYNTAXHIGHLIGHTER_H

#include <core/qqnorlogeref.h>

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

	void setNorlogeRefToHighlight(const QQNorlogeRef & norlogeRef);

signals:
	void totozRequired(const QString & totozId);

public slots:

protected:
	virtual void highlightBlock(const QString &text);

private:
	void highlightBlockForNRef(QQMessageBlockUserData * userData);
	void highlightDuck(const QString & text, QQMessageBlockUserData * userData);
	void highlightNorloge(const QString & text, QQMessageBlockUserData * userData);
	void highlightTableVolante(const QString & text, QQMessageBlockUserData * userData);
	void highlightTotoz(const QString & text, QQMessageBlockUserData * userData);

	QQNorlogeRef m_nRef;
};

#endif // QQSYNTAXHIGHLIGHTER_H

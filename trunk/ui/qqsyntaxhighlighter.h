#ifndef QQSYNTAXHIGHLIGHTER_H
#define QQSYNTAXHIGHLIGHTER_H

#include <QString>
#include <QSyntaxHighlighter>
#include <QTextDocument>

class QQMessageBlockUserData;

class QQSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	QQSyntaxHighlighter(QTextDocument *parent = 0);

signals:

public slots:

protected:
	virtual void highlightBlock(const QString &text);

private:
	void highlightDuck(const QString & text, QQMessageBlockUserData * userData);
	void highlightNorloge(const QString & text, QQMessageBlockUserData * userData);
	void highlightTableVolante(const QString & text, QQMessageBlockUserData * userData);
	void highlightTotoz(const QString & text, QQMessageBlockUserData * userData);
};

#endif // QQSYNTAXHIGHLIGHTER_H

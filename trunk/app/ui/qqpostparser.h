#ifndef QQPOSTPARSER_H
#define QQPOSTPARSER_H

#include <QList>
#include <QObject>
#include <QTextDocument>
#include <QTextDocumentFragment>

class QQMessageTransformFilter;
class QQMessageBlockUserData;
class QQPost;

class QQPostParser : public QObject
{
	Q_OBJECT
public:
	explicit QQPostParser(QObject *parent = 0);
	void setIndexShift(int shift) { m_indexShit = shift; }
	QTextDocumentFragment* formatMessage(QQPost *post, QQMessageBlockUserData *userData);

signals:
	void totozRequired(QString & totozId);
	void notifyBigorno();

public slots:

private:
	QString applyMessageTransformFilters(const QString &message, const QString &bouchot);

	void colorizeBigorno(QTextDocument &doc, QQPost *post, QQMessageBlockUserData *userData);
	void colorizeDuck(QTextDocument &doc, QQMessageBlockUserData *userData);
	void colorizeNRef(QTextDocument &doc, QQPost *post, QQMessageBlockUserData *userData);
	void colorizeTableVolante(QTextDocument &doc, QQMessageBlockUserData *userData);
	void colorizeTotoz(QTextDocument &doc, QQMessageBlockUserData *userData);

	QList<QQMessageTransformFilter *> m_listMessageTransformFilters;

	int m_indexShit;
};

#endif // QQPOSTPARSER_H

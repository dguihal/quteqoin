#ifndef QQTSVPARSER_H
#define QQTSVPARSER_H

#include <QObject>
#include <QString>

#include "core/parsers/qqbackendparser.h"

class QQTsvParser : public QObject
{
	Q_OBJECT
public:
	explicit QQTsvParser(QObject *parent = 0);

	QString errorString () const;

	bool parse(const QByteArray &data);

signals:
	void newPostReady(QQPost & newPost);
	void finished();

};

#endif // QQTSVPARSER_H

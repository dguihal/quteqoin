#ifndef QQTSVPARSER_H
#define QQTSVPARSER_H

#include <QObject>
#include <QString>

#include "qqbackendparser.h"

class QQTsvParser : public QQBackendParser
{
	Q_OBJECT
public:
	explicit QQTsvParser(QObject *parent = 0);
	virtual ~QQTsvParser() {}

	QString errorString () const;

	bool parseBackend(const QByteArray &data);
};

#endif // QQTSVPARSER_H

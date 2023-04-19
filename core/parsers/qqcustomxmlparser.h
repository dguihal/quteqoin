#ifndef QQCUSTOMXMLPARSER_H
#define QQCUSTOMXMLPARSER_H

#include <QStack>

#include "qqbackendparser.h"
#include "core/qqbouchot.h"

class QQCustomXmlParser : public QQBackendParser
{
	Q_OBJECT
public:
	QQCustomXmlParser(QObject *parent = 0);

	QString errorString () const;

	bool parseBackend(const QByteArray &data);

	bool parsePost(const QByteArray &data);

	void setTypeSlip(QQBouchot::TypeSlip typeSlip) { this->m_typeSlip = typeSlip; }

private:
	QQBouchot::TypeSlip m_typeSlip;
};

#endif // QQCUSTOMXMLPARSER_H

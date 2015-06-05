#ifndef QQBACKENDPARSER_H
#define QQBACKENDPARSER_H

#include <QObject>
#include <QString>

#include "core/qqpost.h"

class QQBackendParser : public QObject
{
	Q_OBJECT
public:
	explicit QQBackendParser(QObject *parent = 0);
	virtual ~QQBackendParser();

	void setLastId(qlonglong lastId) { this->m_lastId = lastId; }

	qlonglong maxId() { return m_maxId; }

signals:
	void newPostReady(QQPost & newPost);
	void finished();

public slots:

protected:
	qlonglong m_lastId;
	qlonglong m_maxId;

	QQPost m_currentPost;
};

#endif // QQBACKENDPARSER_H

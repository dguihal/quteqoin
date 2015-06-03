#ifndef QQTSVPARSER_H
#define QQTSVPARSER_H

#include <QObject>
#include <QString>

#include "core/qqpost.h"

class QQTsvParser : public QObject
{
	Q_OBJECT
public:
	explicit QQTsvParser(QObject *parent = 0);

	QString errorString () const;
	void setLastId(qlonglong lastId) { this->m_lastId = lastId; }

	qlonglong maxId() { return m_maxId; }
	bool parse(const QByteArray &data);

signals:
	void newPostReady(QQPost & newPost);
	void finished();

public slots:

private:
	QString			m_errorString;
	QQPost			m_currentPost;

	qlonglong m_lastId;
	qlonglong m_maxId;
};

#endif // QQTSVPARSER_H

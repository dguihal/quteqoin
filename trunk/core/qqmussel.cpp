#include "qqmussel.h"

#include <QHash>

#define SEP_CHAR '@'

QQMussel::QQMussel(QString name, QString board, bool isAuth) :
	m_name(name),
	m_board(board),
	m_isAuth(isAuth)
{
}

QString QQMussel::fullName() const
{
	QString fn;
	fn.append(m_name).append(SEP_CHAR).append(m_board);
	return m_name;
}

bool QQMussel::equals(const QQMussel mussel) const
{
	return (m_name == mussel.m_name) &&
			(m_board == mussel.m_board) &&
			(m_isAuth == mussel.m_isAuth);
}

uint QQMussel::hash() const
{
	QString s(m_name);
	s.append("#").append(board())
			.append("#").append(m_isAuth ? "true" : "false");
	return qHash(s);
}

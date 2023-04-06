#include "qqmussel.h"

#include "core/qqbouchot.h"

#include <QHash>
#include <utility>

const char SEP_CHAR = '@';

//////////////////////////////////////////////////////////////
/// \brief QQMussel::QQMussel
/// \param name
/// \param board
/// \param isAuth
///
QQMussel::QQMussel(QString name, QString board, bool isAuth) :
    m_name(std::move(name)),
    m_board(std::move(board)),
    m_isAuth(isAuth),
    m_isBaked(false),
    m_isPlopified(false)
{
}

//////////////////////////////////////////////////////////////
/// \brief QQMussel::isMe
/// \return
///
bool QQMussel::isMe() const
{
	QQBouchot *b = QQBouchot::bouchot(m_board);
	if(m_isAuth)
		return b->settings().login() == m_name;

	return (b->settings().login().size() == 0 && b->settings().ua() == m_name);
}

//////////////////////////////////////////////////////////////
/// \brief QQMussel::bak
/// \param isBaked
///
void QQMussel::bak(bool isBaked)
{
	m_isBaked = isBaked;
	QQBouchot *b = QQBouchot::bouchot(m_board);
	if(isBaked)
		b->addToBak(m_name, m_isAuth);
	else
		b->removeFromBak(m_name, m_isAuth);
}

//////////////////////////////////////////////////////////////
/// \brief QQMussel::plopify
/// \param isPlopified
///
void QQMussel::plopify(bool isPlopified)
{
	m_isPlopified = isPlopified;
	QQBouchot *b = QQBouchot::bouchot(m_board);
	if(isPlopified)
		b->addToPlopify(m_name, m_isAuth);
	else
		b->removeFromPlopify(m_name, m_isAuth);
}

//////////////////////////////////////////////////////////////
/// \brief QQMussel::fullName
/// \return
///
QString QQMussel::fullName() const
{
	QString fn;
	fn.append(m_name).append(SEP_CHAR).append(m_board);
	return m_name;
}

//////////////////////////////////////////////////////////////
/// \brief QQMussel::equals
/// \param mussel
/// \return
///
bool QQMussel::equals(const QQMussel& mussel) const
{
	return (m_name == mussel.m_name) &&
	        (m_board == mussel.m_board) &&
	        (m_isAuth == mussel.m_isAuth);
}

//////////////////////////////////////////////////////////////
/// \brief QQMussel::hash
/// \return
///
uint QQMussel::hash() const
{
	QString s(m_name);
	s.append("#").append(board())
	        .append("#").append(m_isAuth ? "true" : "false");
	return qHash(s);
}

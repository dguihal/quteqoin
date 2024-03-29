/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "core/qqbouchot.h"
#include "core/qqnorloge.h"
#include "core/qqpost.h"
#include "core/qqsettings.h"

#include <QtDebug>

//
QQPost::QQPost(QQBouchot * parent) :
    QObject(parent)
{
	reset();
}

//
QQPost::QQPost(const QQPost& post) :
    QObject(post.parent()),
    m_norloge(post.m_norloge),
    m_isNorlogeMultiple(post.m_isNorlogeMultiple),
    m_isAloneInMinute(post.m_isAloneInMinute),
    m_norlogeIndex(post.m_norlogeIndex),
    m_login(post.m_login),
    m_ua(post.m_ua),
    m_isSelfPost(post.m_isSelfPost),
    m_unread(post.m_unread),
    m_message(post.m_message),
    m_id(post.m_id)
{
}

//////////////////////////////////////////////////////////////
/// \brief QQPost::bouchot
/// \return
///
QQBouchot * QQPost::bouchot() const {
	return dynamic_cast<QQBouchot *>(parent());
}

//
QString QQPost::toText()
{
	QString tmp(m_login);
	tmp.append(QString(" "))
	        .append(norlogeFormatee())
	        .append(QString(" "))
	        .append(m_message);
	return tmp;
}

void QQPost::setSelfPost(bool selfPost)
{
	m_isSelfPost = selfPost ? True : False;
}

bool QQPost::isSelfPost()
{
	if(m_isSelfPost == Unknown)
	{
		QQBouchot::QQBouchotSettings bouchotSettings = bouchot()->settings();
		QQSettings settings;

		QString login = bouchotSettings.login();

		if(login.size() == m_login.size())
		{
			if(m_login.size() > 0)
			{
				if(login.compare(m_login, Qt::CaseSensitive) == 0)
					m_isSelfPost = True;
			}
			else
			{
				QString ua = bouchotSettings.ua();
				if(ua.size() == 0)
					ua = settings.value(SETTINGS_GENERAL_DEFAULT_UA, DEFAULT_GENERAL_DEFAULT_UA).toString();

				if(ua.compare(m_ua, Qt::CaseSensitive) == 0)
					m_isSelfPost = True;
			}
		}

		//If not true => false
		if(m_isSelfPost == Unknown)
			m_isSelfPost = False;
	}
	return m_isSelfPost == True;
}

QQNorloge QQPost::norlogeObj() const
{
	QQNorloge targetN(bouchot()->name(),
	                  norloge());
	if(isNorlogeMultiple())
		targetN.setNorlogeIndex(norlogeIndex());

	if(isAloneInMinute())
		targetN.setUniqueMinute(true);

	return targetN;
}

QString QQPost::norlogeComplete() const
{
	QString res = m_norloge;
	res.append(QString::fromLatin1("^")).append(QString::number(m_norlogeIndex));
	return res;
}

//
QString QQPost::norlogeFormatee() const
{
	QString tmp("");
	tmp.append("[")
	        .append(m_norloge.midRef(8,2)).append(":")
	        .append(m_norloge.midRef(10,2)).append(":")
	        .append(m_norloge.midRef(12,2)).append("]");
	return tmp;
}

//////////////////////////////////////////////////////////
/// \brief QQPost::date
/// \return
///
QDate QQPost::date()
{
	return QDate::fromString(m_norloge.left(8), QString::fromLatin1("yyyyMMdd"));
}

//
bool QQPost::equal(const QQPost &p) const
{
	return (p.m_id == m_id) && (bouchot()->name() == p.bouchot()->name()); // comparaison bouchots
}

//
bool QQPost::operator==(const QQPost &p) const
{
	return equal(p);
}

/**
 * @brief QQPost::operator <
 * @param p
 * @return
 */
bool QQPost::operator<(const QQPost &p) const
{
	if(m_norloge == p.m_norloge)
	{
		if(bouchot()->name() == p.bouchot()->name())
			return m_id.toLongLong() < p.m_id.toLongLong();
		return bouchot()->name() < p.bouchot()->name();
	}

	return m_norloge < p.m_norloge;
}

void QQPost::reset()
{
	m_norloge.clear();
	m_norlogeIndex = 1;
	m_isNorlogeMultiple = false;
	m_isAloneInMinute = true;

	m_login.clear();
	m_ua.clear();
	m_isSelfPost = Unknown;

	m_message.clear();
	m_id.clear();
	m_unread = true;

}

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
	m_norlogeIndex(post.m_norlogeIndex),
	m_login(post.m_login),
	m_ua(post.m_ua),
	m_isSelfPost(post.m_isSelfPost),
	m_message(post.m_message),
	m_id(post.m_id),
	m_unread(post.m_unread)
{
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
			.append(m_norloge.mid(8,2)).append(":")
			.append(m_norloge.mid(10,2)).append(":")
			.append(m_norloge.mid(12,2)).append("]");
	return tmp;
}

//////////////////////////////////////////////////////////
/// \brief QQPost::date
/// \return
///
QDate QQPost::date()
{
	int Y = m_norloge.left(4).toInt();
	int M = m_norloge.mid(4,2).toInt();
	int D = m_norloge.mid(6,2).toInt();
	return QDate(Y,M,D);
}

//
bool QQPost::equal(QQPost &b)
{
	return (b.m_id == m_id); // comparaison bouchots
}

//
bool QQPost::operator== (QQPost &b)
{
	return equal(b);
}

//
bool QQPost::operator< (QQPost &b)
{
	return (bouchot()->name() == b.bouchot()->name()) ?
				m_id.toLongLong() < b.m_id.toLongLong() :
				m_norloge < b.m_norloge;
}

void QQPost::reset()
{
	m_norloge.clear();
	m_norlogeIndex = 1;
	m_isNorlogeMultiple = false;

	m_login.clear();
	m_ua.clear();
	m_isSelfPost = Unknown;

	m_message.clear();
	m_id.clear();
	m_unread = true;
}

//////////////////////////////////////////////////////////////////////////
////////////        Fonctions private        /////////////////////////////
//////////////////////////////////////////////////////////////////////////


bool postComp(QQPost *left, QQPost *right)
{
	return (* left) < (* right);
}

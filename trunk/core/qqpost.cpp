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
#include "core/qqpost.h"
#include "core/qqsettings.h"

#include <QtDebug>

//
QQPost::QQPost(QQBouchot * parent)
	: QObject(parent)
{
	reset();
}

//
QQPost::QQPost( const QQPost& post )
	: QObject( )
{
	m_login = post.m_login;
	m_norloge = post.m_norloge;
	m_id = post.m_id;
	m_ua = post.m_ua;
	m_message = post.m_message;
	m_norlogeIndex = post.m_norlogeIndex;
	m_isNorlogeMultiple = post.m_isNorlogeMultiple;
	m_isSelfPost = post.m_isSelfPost;
}

//
QQPost::~QQPost()
{
}

//
QString QQPost::toText()
{
	QString tmp(m_login);
	tmp.append(QString(" "));
	tmp.append(norlogeFormatee());
	tmp.append(QString(" "));
	tmp.append(m_message);
	return tmp;
}

void QQPost::setSelfPost(bool selfPost)
{
	if(selfPost)
		m_isSelfPost = True;
	else
		m_isSelfPost = False;
}

bool QQPost::isSelfPost()
{
	if(m_isSelfPost == Unknown)
	{
		QQBouchot::QQBouchotSettings bouchotSettings = bouchot()->settings();
		QQSettings settings;

		QString login = bouchotSettings.login();
		if(login.size() == 0)
			login = settings.value(SETTINGS_GENERAL_DEFAULT_LOGIN, DEFAULT_GENERAL_DEFAULT_LOGIN).toString();

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

bool QQPost::isReponse()
{
	return false;
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
	if(bouchot()->name() == b.bouchot()->name())
		return m_id.toInt() < b.m_id.toInt();
	else
		return m_norloge < b.m_norloge;
}

void QQPost::reset()
{
	m_login.clear();
	m_norloge.clear();
	m_ua.clear();
	m_message.clear();
	m_id.clear();
	m_norlogeIndex = 1;
	m_isNorlogeMultiple = false;
	m_isSelfPost = Unknown;
}

//////////////////////////////////////////////////////////////////////////
////////////        Fonctions private        /////////////////////////////
//////////////////////////////////////////////////////////////////////////


bool postComp(QQPost *left, QQPost *right)
{
	return (* left) < (* right);
}

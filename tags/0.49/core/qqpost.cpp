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
#include "qqpost.h"
#include "qqbouchot.h"

#include <QtDebug>
//
QQPost::QQPost(QQBouchot * parent)
	: QObject(parent)
{
	this->m_login = QString();
	this->m_norloge = QString();
	this->m_id = QString();
	this->m_ua = QString();
	this->m_message = QString();
	this->m_norlogeIndex = 1;
	this->m_isNorlogeMultiple = false;
}
//
QQPost::QQPost( const QQPost& post )
	: QObject( )
{
	this->m_login = post.m_login;
	this->m_norloge = post.m_norloge;
	this->m_id = post.m_id;
	this->m_ua = post.m_ua;
	this->m_message = post.m_message;
	this->m_norlogeIndex = post.m_norlogeIndex;
	this->m_isNorlogeMultiple = post.m_isNorlogeMultiple;;
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

bool QQPost::isSelfPost()
{
	QQBouchot::QQBouchotSettings bouchotSettings = bouchot()->settings();

	if(bouchotSettings.login().size() != 0 && QString::compare(bouchotSettings.login(), m_login, Qt::CaseSensitive) == 0)
		return true;

	if(bouchotSettings.ua().size() != 0 && QString::compare(bouchotSettings.ua(), m_ua, Qt::CaseSensitive) == 0)
		return true;

	return false;
}

bool QQPost::isReponse()
{
	return false;
}

QString QQPost::norlogeComplete()
{
	QString res = m_norloge;
	res.append(QString::fromAscii("^")).append(QString::number(m_norlogeIndex));
	return res;
}

//
QString QQPost::norlogeFormatee()
{
	QString tmp("");
	tmp.append("[")
			.append(norloge().mid(8,2)).append(":")
			.append(norloge().mid(10,2)).append(":")
			.append(norloge().mid(12,2)).append("]");
	return tmp;
}

//
bool QQPost::equal(QQPost &b)
{
	return (b.m_id == this->m_id); // comparaison bouchots
}

//
bool QQPost::operator== (QQPost &b)
{
	return this->equal(b);
}

//
bool QQPost::operator< (QQPost &b)
{
	if(this->bouchot()->name() == b.bouchot()->name())
		return this->m_id.toInt() < b.m_id.toInt();
	else
		return this->m_norloge < b.m_norloge;
}

void QQPost::reset()
{
	m_login.clear();
	m_norloge.clear();
	m_ua.clear();
	m_message.clear();
	m_id.clear();
}

bool postComp(QQPost *left, QQPost *right)
{
	return (* left) < (* right);
}

//////////////////////////////////////////////////////////////////////////
////////////        Fonctions private        /////////////////////////////
//////////////////////////////////////////////////////////////////////////


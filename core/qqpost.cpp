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

#include <QObject>
#include <QString>
#include <QtDebug>
//
QQPost::QQPost(QQBouchot * parent)
    : QObject(parent)
{
    this->setNorloge("");
    this->setLogin("");
    this->setUA("");
    this->setMessage("");
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

    if(bouchotSettings.login().size() != 0 && bouchotSettings.login().compare(m_login) == 0)
        return true;

    if(bouchotSettings.ua().size() != 0 && bouchotSettings.ua().compare(m_ua) == 0)
        return true;

    return false;
}

//////////////////////////////////////////////////////////////////////////
////////////             Accesseurs          /////////////////////////////
//////////////////////////////////////////////////////////////////////////
QQBouchot * QQPost::bouchot ()
{
    return dynamic_cast<QQBouchot *>( parent() );
}

QString QQPost::login()
{
    return this->m_login;
}
//
void QQPost::setLogin(const QString &login)
{
    this->m_login = login;
}
//
QString QQPost::norloge()
{
    return this->m_norloge;
}
//
void QQPost::setNorloge(const QString &norloge)
{
    this->m_norloge = norloge;
}
//
QString QQPost::id()
{
    return this->m_id;
}
//
void QQPost::setId( const QString &id )
{
    this->m_id = id;
}
//
QString QQPost::UA()
{
    return this->m_ua;
}
//
void QQPost::setUA(const QString &ua )
{
    this->m_ua = ua;
}
//
QString QQPost::message()
{
    return this->m_message;
}
//
void QQPost::setMessage(const QString& message)
{
    this->m_message = message;
}

//////////////////////////////////////////////////////////////////////////
////////////       Fonctions protected       /////////////////////////////
//////////////////////////////////////////////////////////////////////////

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
    if( (b.m_id == this->m_id) &&
            true ) // comparaison bouchots
        return true;
    return false;
}
//
bool QQPost::operator== (QQPost &b)
{
    return this->equal(b);
}

void QQPost::reset()
{
    m_login.clear();
    m_norloge.clear();
    m_ua.clear();
    m_message.clear();
    m_id.clear();
}

//////////////////////////////////////////////////////////////////////////
////////////        Fonctions private        /////////////////////////////
//////////////////////////////////////////////////////////////////////////


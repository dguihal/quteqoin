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
    this->m_Login = post.m_Login;
    this->m_Norloge = post.m_Norloge;
    this->m_Id = post.m_Id;
    this->m_UA = post.m_UA;
    this->m_Message = post.m_Message;
}
//
QQPost::~QQPost()
{
}
//
QString QQPost::toText()
{
    QString tmp(m_Login);
    tmp.append(QString(" "));
    tmp.append(norlogeFormatee());
    tmp.append(QString(" "));
    tmp.append(m_Message);
    return tmp;
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
    return this->m_Login;
}
//
void QQPost::setLogin(const QString &login)
{
    this->m_Login = login;
}
//
QString QQPost::norloge()
{
    return this->m_Norloge;
}
//
void QQPost::setNorloge(const QString &norloge)
{
    this->m_Norloge = norloge;
}
//
QString QQPost::id()
{
    return this->m_Id;
}
//
void QQPost::setId( const QString &id )
{
    this->m_Id = id;
}
//
QString QQPost::UA()
{
    return this->m_UA;
}
//
void QQPost::setUA(const QString &ua )
{
    this->m_UA = ua;
}
//
QString QQPost::message()
{
    return this->m_Message;
}
//
void QQPost::setMessage(const QString& message)
{
    this->m_Message = message;
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
    if( (b.m_Id == this->m_Id) &&
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
    m_Login.clear();
    m_Norloge.clear();
    m_UA.clear();
    m_Message.clear();
    m_Id.clear();
}

//////////////////////////////////////////////////////////////////////////
////////////        Fonctions private        /////////////////////////////
//////////////////////////////////////////////////////////////////////////


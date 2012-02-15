/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or(at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#ifndef QQPOST_H
#define QQPOST_H
//
#include <QObject>

class QQBouchot;

class QString;
//
class QQPost : public QObject
{
Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////
    ////////////            Constructeurs        /////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    QQPost( QQBouchot * parent = 0 );
    QQPost( const QQPost& post );

    //////////////////////////////////////////////////////////////////////////
    ////////////             Destructeurs        /////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    virtual ~QQPost();

    //////////////////////////////////////////////////////////////////////////
    ////////////             Accesseurs          /////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    QQBouchot * bouchot();
    QString login();
    void setLogin( const QString& login );
    QString norloge();
    QString norlogeFormatee();
    void setNorloge( const QString& norloge );
    QString id();
    void setId( const QString& id );
    QString UA();
    void setUA( const QString& ua );
    QString message();
    void setMessage( const QString& message );

    //////////////////////////////////////////////////////////////////////////
    ////////////                                 /////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    QString toText();

    bool isSelfPost();

    bool equal( QQPost &b );
    bool operator==( QQPost& );

    void reset();

private:
    QString m_norloge;
    QString m_login;
    QString m_ua;
    QString m_message;
    QString m_id;
};
#endif

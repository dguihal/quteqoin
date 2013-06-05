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


#include <QObject>
#include <QString>

class QQBouchot;
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
	QQBouchot * bouchot() { return (QQBouchot *)( this->parent() ); }

	QString login() { return this->m_login; }
	void setLogin( const QString& login ) { this->m_login = login; }

	QString norloge() { return this->m_norloge; }
	QString norlogeComplete();
	QString norlogeFormatee();
	void setNorloge( const QString& norloge ) { this->m_norloge = norloge; }

	/* Gestion des index de norloges en cas de post a horodatage identique */
	int norlogeIndex() { return this->m_norlogeIndex; }
	void setNorlogeIndex(const int index) { this->m_norlogeIndex = index; }
	void incrIndex() { this->m_norlogeIndex ++; }
	bool isNorlogeMultiple() { return this->m_isNorlogeMultiple || this->m_norlogeIndex > 1; }
	void setNorlogeMultiple(const bool isNorlogeMultiple) { this->m_isNorlogeMultiple = isNorlogeMultiple; }

	QString id() { return this->m_id; }
	void setId( const QString& id ) { this->m_id = id; }

	QString UA() { return this->m_ua; }
	void setUA( const QString& ua ) { this->m_ua = ua; }

	QString message() { return this->m_message; }
	void setMessage( const QString& message ) { this->m_message = message; }

	//////////////////////////////////////////////////////////////////////////
	////////////                                 /////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	QString toText();

	void setSelfPost(bool selfPost = true);
	bool isSelfPost();
	bool isReponse();

	bool equal( QQPost &b );
	bool operator==( QQPost& );
	bool operator<( QQPost& );

	void reset();

private:
	enum IsSelfPost { False = 0, True = 1, Unknown = 2 };

	QString m_norloge;
	bool m_isNorlogeMultiple;
	IsSelfPost m_isSelfPost;
	int m_norlogeIndex;
	QString m_login;
	QString m_ua;
	QString m_message;
	QString m_id;
};

bool postComp(QQPost * left, QQPost * right);

#endif

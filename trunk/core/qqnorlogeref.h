#ifndef QQNORLOGEREF_H
#define QQNORLOGEREF_H

#include "core/qqnorloge.h"

#include <QDebug>
#include <QString>

class QQNorlogeRef : public QQNorloge
{
public:
	QQNorlogeRef();
	QQNorlogeRef(const QString & bouchot, const QString & dateh, const QString & norlogeRef, int posInMessage);
	QQNorlogeRef(const QQNorlogeRef & norlogeRef);

	//Les zones capturées sont :
	// 0 - Norloge complète
	// 1 - Date + Time + Subtime
	// 2 - Date
	// 3 - Time + Subtime
	// 4 - Tribune
	static QRegExp norlogeRegexp()
	{
		return norlogeRegexp(QString::fromAscii("[A-Za-z0-9_]+"));
	}

	//Les zones capturées sont :
	// 0 - Norloge complète
	// 1 - Date + Time + Subtime
	// 2 - Date
	// 3 - Time + Subtime
	// 4 - Tribune
	static QRegExp norlogeRegexp(const QString & bouchot)
	{
		return QRegExp(QString::fromUtf8("(((?:[0-9]+/)?(?:1[0-2]|0[1-9])/(?:3[0-1]|[1-2][0-9]|0[1-9])#)?" //date
										  "((?:2[0-3]|[0-1][0-9]):(?:[0-5][0-9])(?::[0-5][0-9])?" //time
										  "(?:[¹²³]|[:\\^][1-9]|[:\\^][1-9][0-9])?))" //subtime
										  "(@") + bouchot + QString::fromAscii(")?"), //tribune
					   Qt::CaseSensitive,
					   QRegExp::RegExp);
	}

	int getNorlogeRefIndex() const { return m_refDateIndexPart; }
	QString getOrigNRef() const { return m_origNRef; }
	int getPosInMessage() const { return m_posInMessage; }

	QString dstBouchot() const;
	QString dstNorloge() const;

	QString nRefId() const;

	bool isValid() { return m_valid; }


	bool operator == ( const QQNorlogeRef & other ) const
	{
		bool res = ( QString::compare(this->nRefId(), other.nRefId()) == 0 );
		//qDebug() << "this->nRefId = " << this->nRefId() << ",other->nRefId = " << other.nRefId() << ", res=" <<  res;

		return res;
	}
	bool operator != ( const QQNorlogeRef & other ) const
		{ return !( (* this) == other ); }


private:
	QString m_refDateYearPart;
	QString m_refDateMonthPart;
	QString m_refDateDayPart;
	QString m_refDateHourPart;
	QString m_refDateMinutePart;
	QString m_refDateSecondPart;
	int m_refDateIndexPart;

	QString m_dstBouchot;

	QString m_origNRef;
	int m_posInMessage;

	bool m_valid;
};

#endif // QQNORLOGEREF_H

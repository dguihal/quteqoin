#ifndef QQNORLOGEREF_H
#define QQNORLOGEREF_H

#include "core/qqnorloge.h"
#include "core/qqpost.h"

#include <QtDebug>
#include <QList>
#include <QPointer>
#include <QString>

class QQNorlogeRef : public QQNorloge
{
public:
	explicit QQNorlogeRef();
	QQNorlogeRef(const QString & bouchot, const QString & dateh, const QString & norlogeRef, int posInMessage = 0);
	QQNorlogeRef(const QQNorlogeRef & norlogeRef);

	//Les zones capturées sont :
	// 0 - Norloge complète
	// 1 - Date + Time + Subtime
	// 2 - Date
	// 3 - Time + Subtime
	// 4 - Tribune
	static QRegExp norlogeRegexp()
	{
		return norlogeRegexp(QString::fromLatin1("[A-Za-z0-9_]+"));
	}

	//Les zones capturées sont :
	// 0 - Norloge complète
	// 1 - Date + Time + Subtime
	// 2 - Date
	// 3 - Time + Subtime
	// 4 - Tribune
	static QRegExp norlogeRegexp(const QString & bouchot)
	{
		return QRegExp(QString::fromLatin1("(((?:[0-9]+/)?(?:1[0-2]|0[1-9])/(?:3[0-1]|[1-2][0-9]|0[1-9])#)?" //date
										 "((?:2[0-3]|[0-1][0-9]):(?:[0-5][0-9])(?::[0-5][0-9])?" //time
										 "(?:[¹²³]|[:\\^][1-9]|[:\\^][1-9][0-9])?))" //subtime
										 "(@") + bouchot + QString::fromLatin1(")?"), //tribune
					   Qt::CaseSensitive,
					   QRegExp::RegExp);
	}

	QString getOrigNRef() const { return m_origNRef; }
	int getPosInMessage() const { return m_posInMessage; }

	QString dstBouchot() const;

	bool matchesPost(QQPost* post);
	bool matchesNRef(QQNorlogeRef &other);
	bool isReponse();

	bool isValid() const { return m_valid; }

	QString nRefId();

private:

	QString m_dstBouchot;

	QString m_origNRef;
	int m_posInMessage;

	QList< QPointer<QQPost> > m_listPostTarget;

	bool m_valid;

	bool m_hasDate;
	bool m_hasSec;

	bool m_isReponseDefined;
	bool m_isResponse;

	QString m_refId;
};

#endif // QQNORLOGEREF_H

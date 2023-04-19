#ifndef QQNORLOGEREF_H
#define QQNORLOGEREF_H

#include "core/qqnorloge.h"
#include "core/qqpost.h"

#include <QtDebug>
#include <QList>
#include <QPointer>
#include <QRegularExpression>
#include <QString>
#include <QStringBuilder>

class QQNorlogeRef : public QQNorloge
{
public:
	explicit QQNorlogeRef();
	QQNorlogeRef(const QQPost& post, const QString& norlogeRef = QString());
	QQNorlogeRef(const QQNorlogeRef& norlogeRef);

	//Les zones capturées sont :
	// 0 - Norloge complète
	// 1 - Date + Time + Subtime
	// 2 - Date
	// 3 - Time + Subtime
	// 4 - Tribune
	static QRegularExpression norlogeRegexp()
	{
		return norlogeRegexp(QString::fromLatin1("[A-Za-z0-9_]+"));
	}

	//Les zones capturées sont :
	// 0 - Norloge complète
	// 1 - Date + Time + Subtime
	// 2 - Date
	// 3 - Time + Subtime
	// 4 - Tribune
	static QRegularExpression norlogeRegexp(const QString & bouchot)
	{
		return QRegularExpression(QString::fromLatin1("("
		                                               "("
		                                                "(?:[0-9]+[\\/-])?" /*Y*/ "(?:1[0-2]|0[1-9])[\\/-]" /*M*/ "(?:3[0-1]|[1-2][0-9]|0[1-9])" /*D*/ "[#T]"
		                                               ")?" // End date capture group #2
		                                               "("
		                                                 "(?:2[0-3]|[0-1][0-9]):" /*h*/ "(?:[0-5][0-9])" /*m*/ "(?::[0-5][0-9])?" /*s*/ "(?:[¹²³]|[:\\^][1-9]|[:\\^][1-9][0-9])?" //subtime
		                                               ")" // End time capture group #3
		                                              ")" // End datetime capture group #1
		                                              "(@") % bouchot % QString::fromLatin1(")?"), //tribune  #4
		               QRegularExpression::NoPatternOption);
	}

	QString getOrigNRef() const { return m_origNRef; }

	QString dstBouchot() const;

	bool matchesPost(QQPost* post);
	bool matchesNRef(QQNorlogeRef &other);
	bool isReponse();

	bool isValid() const { return m_valid; }

	QQNorlogeRef& operator=(const QQNorlogeRef& other)
	{
		m_dstBouchot = other.m_dstBouchot;
		m_origNRef = other.m_origNRef;
		m_posInMessage = other.m_posInMessage;
		m_listPostTarget = other.m_listPostTarget;
		m_valid = other.m_valid;
		m_hasDate = other.m_hasDate;
		m_hasSec = other.m_hasSec;
		m_isReponseDefined = other.m_isReponseDefined;
		m_refId = other.m_refId;

		return *this;
	}

	QString nRefId();

private:

	QString m_dstBouchot;

	QString m_origNRef;
	int m_posInMessage{};

	QList< QPointer<QQPost> > m_listPostTarget;

	bool m_valid;

	bool m_hasDate;
	bool m_hasSec;

	bool m_isReponseDefined;
	bool m_isResponse;

	QString m_refId;
};

#endif // QQNORLOGEREF_H

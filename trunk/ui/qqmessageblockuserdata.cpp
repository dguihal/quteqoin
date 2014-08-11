#include "qqmessageblockuserdata.h"

#include <QtDebug>

QQMessageBlockUserData::QQMessageBlockUserData() :
	QTextBlockUserData(),
	m_wasParsed(false),
	m_post(QPointer<QQPost>())
{
}

QQMessageBlockUserData::QQMessageBlockUserData(const QQMessageBlockUserData & userData) :
	QTextBlockUserData((QTextBlockUserData) userData),
	m_listNRef(userData.m_listNRef),
	m_listBigorno(userData.m_listBigorno),
	m_mapDuck(userData.m_mapDuck),
	m_mapTableV(userData.m_mapTableV),
	m_wasParsed(userData.m_wasParsed),
	m_ranges(userData.m_ranges),
	m_post(userData.m_post)
{
}

QQNorlogeRef QQMessageBlockUserData::norlogeRefForIndex(const int index) const
{
	if(index >= 0 && index < m_listNRef.size())
		return m_listNRef.at(index);
	else
		return QQNorlogeRef();
}

bool QQMessageBlockUserData::hasNRefToSelfPost() const
{
	for(int i = 0; i < m_listNRef.size(); i++)
	{
		QQNorlogeRef nRef = m_listNRef.at(i);
		if(nRef.isReponse())
			return true;
	}
	return false;
}

QPair<int, QString> QQMessageBlockUserData::stringForIndex(int index, const QMap<int, QString> & map) const
{
	QList<int> startIndexes = map.uniqueKeys();
	int i = 0;
	while(i < startIndexes.size())
	{
		int startIndex = startIndexes[i];
		if(isIndexInString(index, startIndex, map[startIndex]))
			return qMakePair(startIndex, map[startIndex]);

		i++;
	}
	return qMakePair(-1, QString::fromLatin1(""));
}

bool QQMessageBlockUserData::isIndexInString(int index, int stringIndexStart, const QString & string) const
{
	return (index >= stringIndexStart && index < stringIndexStart + string.length());
}

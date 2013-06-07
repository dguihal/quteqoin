#include "qqmessageblockuserdata.h"

#include <QtDebug>

QQMessageBlockUserData::QQMessageBlockUserData()
{
	m_wasParsed = false;
	m_isHighlighted = false;
	m_isNew = true;
	m_post = NULL;
}

QQMessageBlockUserData::QQMessageBlockUserData(const QQMessageBlockUserData & userData)
{
	m_wasParsed = userData.m_wasParsed;
	m_isHighlighted = userData.m_isHighlighted;
	m_isNew = userData.m_isNew;
	m_post = userData.m_post;
	m_listNRef = userData.m_listNRef;
	m_mapTotozId = userData.m_mapTotozId;
	m_mapDuck = userData.m_mapDuck;
	m_mapTableV = userData.m_mapTableV;
	m_listBigorno = userData.m_listBigorno;
	m_ranges = userData.m_ranges;
}

QQMessageBlockUserData::~QQMessageBlockUserData()
{
	m_listNRef.clear();
	m_mapTotozId.clear();
	m_mapDuck.clear();
	m_mapTableV.clear();
	m_ranges.clear();
	m_listBigorno.clear();
	m_post = NULL;
}

bool QQMessageBlockUserData::isIndexInZRange( const int index, const zoneRangeID zrId )
{
	ZoneRange range = zRangeForID(zrId);
	return range.isInRange(index);
}

QQNorlogeRef QQMessageBlockUserData::norlogeRefForIndex(int index)
{
	int i = 0;
	while(i < m_listNRef.size())
	{
		QQNorlogeRef nRef = m_listNRef.at(i);
		if(isIndexInString(index, nRef.getPosInMessage(), nRef.getOrigNRef()))
			return nRef;
		i++;
	}
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

QPair<int, QString> QQMessageBlockUserData::stringForIndex(int index, const QMap<int, QString> & map)
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
	return qMakePair(-1, QString::fromAscii(""));
}

bool QQMessageBlockUserData::isIndexInString(int index, int stringIndexStart, const QString & string)
{
	return (index >= stringIndexStart && index < stringIndexStart + string.length());
}

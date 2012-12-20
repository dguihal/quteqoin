#include "qqmessageblockuserdata.h"

#include <QDebug>

QQMessageBlockUserData::QQMessageBlockUserData()
{
	m_wasParsed = false;
	m_isHighlighted = false;
	m_post = NULL;
}

QQMessageBlockUserData::~QQMessageBlockUserData()
{
	m_listNRef.clear();
	m_listTotoz.clear();
	m_mapDuck.clear();
	m_mapTableV.clear();
	m_ranges.clear();
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

QQTotoz QQMessageBlockUserData::totozForIndex(int index)
{
	int i = 0;
	while(i < m_listTotoz.size())
	{
		QQTotoz totoz = m_listTotoz.at(i);
		if(isIndexInString(index, totoz.getPosInMessage(), totoz.getOrigString()))
			return totoz;
		i++;
	}
	return QQTotoz();
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

#include "qqmessageblockuserdata.h"

#include <QDebug>

QQMessageBlockUserData::QQMessageBlockUserData()
{
	m_wasParsed = false;
	m_isHighlighted = false;
	m_blkZone = UNDEF_ZONE;
}

QQMessageBlockUserData::~QQMessageBlockUserData()
{
	m_listNRef.clear();
	m_mapTotoz.clear();
	m_mapDuck.clear();
	m_mapTableV.clear();
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

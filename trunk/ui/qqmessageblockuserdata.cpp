#include "qqmessageblockuserdata.h"

#include <QDebug>

QQMessageBlockUserData::QQMessageBlockUserData()
{
	m_wasParsed = false;
}

QQMessageBlockUserData::~QQMessageBlockUserData()
{
    m_customData.clear();

    m_mapNRef.clear();
    m_mapTotoz.clear();
    m_mapDuck.clear();
    m_mapTableV.clear();
}


bool QQMessageBlockUserData::constainsData(int key)
{
//    qDebug() << "QQMessageBlockUserData::constainsData keys=" << m_customData.keys() ;
    return m_customData.contains(key);
}

QVariant QQMessageBlockUserData::getData(int key)
{
    return m_customData.value(key);
}

void QQMessageBlockUserData::storeData(int key, const QVariant & data)
{
    m_customData.insert(key, data);
}

void QQMessageBlockUserData::removeData(int key)
{
    m_customData.remove(key);
}

QString QQMessageBlockUserData::stringForIndex(int index, const QMap<int, QString> & map)
{
    QList<int> nRefIndexes = map.uniqueKeys();
    int i = 0;
    while(i < nRefIndexes.size())
    {
        if(index >= nRefIndexes[i])
        {
            QString nRef = map.value(nRefIndexes[i]);
            if(index < nRefIndexes[i] + nRef.length())
                return nRef;
        }

        i++;
    }
    return QString::fromAscii("");
}

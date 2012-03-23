#include "qqmessageblockuserdata.h"

#include <QDebug>

QQMessageBlockUserData::QQMessageBlockUserData()
{
}


bool QQMessageBlockUserData::constainsData(int key)
{
    qDebug() << "QQMessageBlockUserData::constainsData keys=" << m_customData.keys() ;
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

#include "qqmessageblockuserdata.h"

#include <QtDebug>

QQMessageBlockUserData::QQMessageBlockUserData() :
    QTextBlockUserData(),
    m_wasParsed(false),
    m_hasDuck(false),
    m_hasTableV(false),
    m_post(QPointer<QQPost>())
{
}

QQMessageBlockUserData::QQMessageBlockUserData(const QQMessageBlockUserData & userData) :
    QTextBlockUserData((QTextBlockUserData) userData),
    m_listNRef(userData.m_listNRef),
    m_listBigorno(userData.m_listBigorno),
    m_wasParsed(userData.m_wasParsed),
    m_hasDuck(userData.m_hasDuck),
    m_hasTableV(userData.m_hasTableV),
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

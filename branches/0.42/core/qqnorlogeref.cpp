#include "qqnorlogeref.h"

#include "core/qqbouchot.h"

#include <QString>
#include <QStringList>


QQNorlogeRef::QQNorlogeRef() :
	QQNorloge()
{
	m_posInMessage = -1;
	m_norlogeIndex = 0;
	m_valid = false;
	m_hasDate = false;
	m_hasSec = false;
	m_isReponseDefined = false;
	m_isResponse = false;
}

QQNorlogeRef::QQNorlogeRef(const QString &bouchot, const QString &dateh, const QString &norlogeRef, int posInMessage) :
	QQNorloge(bouchot, dateh)
{
	//On sauve la chaine de reference pour highlighter les semblables
	m_origNRef = norlogeRef;
	m_posInMessage = posInMessage;
	m_valid = true;
	m_hasDate = true;
	m_hasSec = true;

	QRegExp reg = norlogeRegexp();

	m_norlogeIndex = 0; //tous les correspondants par défaut

	if(reg.exactMatch(norlogeRef))
	{
		QStringList capturedTexts = reg.capturedTexts();
		QString date = capturedTexts[2];
		m_hasDate = false;
		if(date.size() > 0)
		{
			QStringList dateSplit = date.split(QChar::fromAscii('/'));
			if(dateSplit.size() > 2)
				m_dateYearPart = dateSplit.takeFirst();

			m_dateMonthPart = dateSplit.takeFirst();
			m_dateDayPart = dateSplit.takeFirst();
			m_dateDayPart.remove(QChar::fromAscii('#'));
		}

		QString time = capturedTexts[3];
		QStringList timeSplit = time.split(QChar::fromAscii(':'));
		m_dateHourPart = timeSplit.takeFirst();
		m_dateMinutePart = timeSplit.takeFirst();

		if(timeSplit.size() > 0)
		{
			m_hasSec = true;
			QString sec = timeSplit.takeFirst();
			m_dateSecondPart = sec.left(2);
			sec.remove(0, 2);
			if(sec.length() > 0)
			{
				if(sec.startsWith(QString::fromUtf8("¹")))
					m_norlogeIndex = 1;
				else if(sec.startsWith(QString::fromUtf8("²")))
					m_norlogeIndex = 2;
				else if(sec.startsWith(QString::fromUtf8("³")))
					m_norlogeIndex = 3;
				else
				{
					sec.remove(0, 1);
					m_norlogeIndex = sec.toInt();
				}
			}
		}
		//gestion du :1
		if(timeSplit.size() > 0)
				m_norlogeIndex = timeSplit.takeFirst().toInt();

		m_dstBouchot = capturedTexts[4];
		//supression du @ initial
		m_dstBouchot.remove(QChar::fromAscii('@'));
	}
	m_isReponseDefined = false;
	m_isResponse = false;
}


QQNorlogeRef::QQNorlogeRef(const QQNorlogeRef & norlogeRef) :
	QQNorloge(norlogeRef)
{
	m_listPostTarget.append(norlogeRef.m_listPostTarget);

	m_dstBouchot = norlogeRef.m_dstBouchot;

	m_origNRef = norlogeRef.m_origNRef;
	m_posInMessage = norlogeRef.m_posInMessage;

	m_valid = norlogeRef.m_valid;
	m_hasDate = norlogeRef.m_hasDate;
	m_hasSec = norlogeRef.m_hasSec;

	m_isReponseDefined = norlogeRef.m_isReponseDefined;
	m_isResponse = norlogeRef.m_isResponse;
}

QString QQNorlogeRef::dstBouchot() const
{
	return (m_dstBouchot.size() > 0) ? m_dstBouchot : m_srcBouchot;
}

QString QQNorlogeRef::dstNorloge() const
{
	QString dstNorloge;

	if(m_hasDate)
	{
		dstNorloge.append(m_dateYearPart);
		dstNorloge.append(m_dateMonthPart);
		dstNorloge.append(m_dateDayPart);
	}
	dstNorloge.append(m_dateHourPart);
	dstNorloge.append(m_dateMinutePart);
	if(m_hasSec)
		dstNorloge.append(m_dateSecondPart);

	return dstNorloge;
}

bool QQNorlogeRef::matchesPost(QQPost * post)
{
	if(! m_valid)
		return false;

	bool found = false;
	for(int i = 0; i < m_listPostTarget.size(); i++ )
	{
		QPointer<QQPost> ptrPost = m_listPostTarget.at(i);
		if(ptrPost.isNull())
			m_listPostTarget.removeOne(ptrPost);
		else if(ptrPost.data() == post)
			found = true;
	}

	if(! found)
	{
		QString dstB = dstBouchot();
		if(dstB == post->bouchot()->name() ||
		   post->bouchot()->settings().containsAlias(dstB))
		{
			QString postN = post->norloge();
			QString dstN = dstNorloge();
			if(postN.endsWith(dstN))
			{
				//qDebug() << "QNorlogeRef::matchesPost : " << m_norlogeIndex << ", " << post->norlogeIndex();
				if(m_norlogeIndex == 0 ||
				   (m_norlogeIndex == post->norlogeIndex()))
				{
					m_listPostTarget.append(QPointer<QQPost>(post));
					found = true;
				}
			}
		}
	}

	return found;
}

bool QQNorlogeRef::matchesNRef(const QQNorlogeRef & other)
{
	if(! m_valid || ! other.isValid())
		return false;

	if(QString::compare(this->dstBouchot(), other.dstBouchot()) != 0)
		return false;

	QString selfNorloge = dstNorloge();
	QString otherNorloge = other.dstNorloge();

	if(! m_hasDate && ! otherNorloge.endsWith(selfNorloge))
		return false;
	else if(! selfNorloge.endsWith(otherNorloge))
		return false;

	else if(m_norlogeIndex != 0 &&
			other.m_norlogeIndex != 0 &&
			m_norlogeIndex != other.m_norlogeIndex)
		return false;

	return true;
}

bool QQNorlogeRef::isReponse()
{
	if(m_isReponseDefined)
		return m_isResponse;

	// else
	if (m_listPostTarget.size() == 0)
		return false;

	//else
	m_isReponseDefined = true;
	m_isResponse = false;
	for(int i = 0; i < m_listPostTarget.size(); i++ )
	{
		QPointer<QQPost> ptrPost = m_listPostTarget.at(i);

		if(ptrPost.isNull())
		{
			m_listPostTarget.removeOne(ptrPost);
			continue;
		}

		if(ptrPost->isSelfPost())
		{
			m_isResponse = true;
			break;
		}
	}
	return m_isResponse;
}

#include "qqnorlogeref.h"

#include "core/qqbouchot.h"

#include <QString>
#include <QStringList>


QQNorlogeRef::QQNorlogeRef() :
	QQNorloge()
{
	m_refDateIndexPart = 0;
	m_posInMessage = -1;
	m_valid = false;
	m_hasDate = false;
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

	QRegExp reg = norlogeRegexp();

	m_refDateIndexPart = 0; //tous les correspondants par défaut

	if(reg.exactMatch(norlogeRef))
	{
		QStringList capturedTexts = reg.capturedTexts();
		QString date = capturedTexts[2];
		if(date.size() > 0)
		{
			QStringList dateSplit = date.split(QChar::fromAscii('/'));
			if(dateSplit.size() > 2)
				m_refDateYearPart = dateSplit.takeFirst();

			m_refDateMonthPart = dateSplit.takeFirst();
			m_refDateDayPart = dateSplit.takeFirst();
			m_refDateDayPart.remove(QChar::fromAscii('#'));
		}
		else
			m_hasDate = false;

		QString time = capturedTexts[3];
		QStringList timeSplit = time.split(QChar::fromAscii(':'));
		m_refDateHourPart = timeSplit.takeFirst();
		m_refDateMinutePart = timeSplit.takeFirst();
		if(timeSplit.size() > 0)
		{
			QString sec = timeSplit.takeFirst();
			m_refDateSecondPart = sec.left(2);
			sec.remove(0, 2);
			if(sec.length() > 0)
			{
				if(sec.startsWith(QString::fromUtf8("¹")))
					m_refDateIndexPart = 1;
				else if(sec.startsWith(QString::fromUtf8("²")))
					m_refDateIndexPart = 2;
				else if(sec.startsWith(QString::fromUtf8("³")))
					m_refDateIndexPart = 3;
				else
				{
					sec.remove(0, 1);
					m_refDateIndexPart = sec.toInt();
				}
			}
		}
		if(timeSplit.size() > 0)
			m_refDateIndexPart = timeSplit.takeFirst().toInt();

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
	m_refDateYearPart = norlogeRef.m_refDateYearPart;
	m_refDateMonthPart = norlogeRef.m_refDateMonthPart;
	m_refDateDayPart = norlogeRef.m_refDateDayPart;
	m_refDateHourPart = norlogeRef.m_refDateHourPart;
	m_refDateMinutePart = norlogeRef.m_refDateMinutePart;
	m_refDateSecondPart = norlogeRef.m_refDateSecondPart;
	m_refDateIndexPart = norlogeRef.m_refDateIndexPart;
	m_listPostTarget.append(norlogeRef.m_listPostTarget);

	m_dstBouchot = norlogeRef.m_dstBouchot;

	m_origNRef = norlogeRef.m_origNRef;
	m_posInMessage = norlogeRef.m_posInMessage;

	m_valid = norlogeRef.m_valid;
	m_hasDate = norlogeRef.m_hasDate;;

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
		dstNorloge.append((m_refDateYearPart.size() > 0) ? m_refDateYearPart : m_dateYearPart );
		dstNorloge.append((m_refDateMonthPart.size() > 0) ? m_refDateMonthPart : m_dateMonthPart );
		dstNorloge.append((m_refDateDayPart.size() > 0) ? m_refDateDayPart : m_dateDayPart );
	}
	dstNorloge.append((m_refDateHourPart.size() > 0) ? m_refDateHourPart : m_dateHourPart );
	dstNorloge.append((m_refDateMinutePart.size() > 0) ? m_refDateMinutePart : m_dateMinutePart );
	dstNorloge.append((m_refDateSecondPart.size() > 0) ? m_refDateSecondPart : m_dateSecondPart );
	//dstNorloge.append((m_refDateSecondPart.size() > 0) ? m_refDateSecondPart : QString::fromAscii("") );
	//dstNorloge.append((m_refDateIndexPart > 0) ? QString::fromAscii("^").append(QString::number(m_refDateIndexPart)) : QString::fromAscii("") );

	return dstNorloge;
}

bool QQNorlogeRef::matchesPost(QQPost * post)
{
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
			if(m_hasDate && (postN.startsWith(dstN)))
			{
				m_listPostTarget.append(QPointer<QQPost>(post));
				found = true;
			}
			else if(postN.endsWith(dstN))
			{
				if(m_refDateIndexPart == 0 ||
				   (m_refDateIndexPart == post->norlogeIndex()))
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
	if(!this->isValid() || ! other.isValid())
		return false;

	if(QString::compare(this->dstBouchot(), other.dstBouchot()) != 0)
		return false;

	QString selfNorloge = dstNorloge();
	QString otherNorloge = other.dstNorloge();

	if(! m_hasDate && ! otherNorloge.endsWith(selfNorloge))
		return false;
	else if(! selfNorloge.endsWith(otherNorloge))
		return false;

	if(m_refDateIndexPart != 0 && other.m_refDateIndexPart != 0 && m_refDateIndexPart != other.m_refDateIndexPart)
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

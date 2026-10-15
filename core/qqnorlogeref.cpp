#include "qqnorlogeref.h"

#include "qqbouchot.h"

#include <QRegularExpression>
#include <QString>
#include <QStringList>

//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::QQNorlogeRef
///
QQNorlogeRef::QQNorlogeRef() :
	QQNorloge(),
	m_dstBouchot(QString()),
	m_origNRef(QString()),
	m_listPostTarget(QList< QPointer<QQPost> >()),
	m_valid(false),
	m_hasDate(false),
	m_hasSec(false),
	m_isReponseDefined(false),
	m_isResponse(false),
	m_refId(QString())
{
}

//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::QQNorlogeRef
/// \param post
/// \param norlogeRef
///
static QRegularExpression nrefDateSplitter(QString::fromLatin1("[/-]")); // clazy:exclude=non-pod-global-static
QQNorlogeRef::QQNorlogeRef(const QQPost& post, const QString& norlogeRef) :
	QQNorloge(post.bouchot()->name(), post.norlogeComplete()),
	m_dstBouchot(QString()),
	m_origNRef(norlogeRef),
	m_listPostTarget(QList< QPointer<QQPost> >()),
	m_valid(true),
	m_hasDate(false),
	m_hasSec(true),
	m_isReponseDefined(false),
	m_isResponse(false),
	m_refId(QString())
{
	m_norlogeIndex = 0; //tous les correspondants par défaut

	auto re = norlogeRegexp();
	auto match = re.match(norlogeRef);

	if(match.hasMatch() && match.capturedStart(0) == 0 && match.capturedLength(0) == norlogeRef.length()) // Exact Match
	{
		auto date = match.captured(2);
		if(date.size() > 0)
		{
			QStringList dateSplit = date.split(nrefDateSplitter);
			if(dateSplit.size() > 2)
				m_dateYearPart = dateSplit.takeFirst();

			m_dateMonthPart = dateSplit.takeFirst();
			m_dateDayPart = dateSplit.takeFirst().first(2);
			m_hasDate = true;
		}

		auto time = match.captured(3);
		QStringList timeSplit = time.split(QChar::fromLatin1(':'));
		m_dateHourPart = timeSplit.takeFirst();
		m_dateMinutePart = timeSplit.takeFirst();

		if(timeSplit.size() > 0)
		{
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
		else
			m_hasSec = false;
		//gestion du :1
		if(timeSplit.size() > 0)
			m_norlogeIndex = timeSplit.takeFirst().toInt();

		m_dstBouchot = match.captured(4);;
		//supression du @ initial
		m_dstBouchot.remove(QChar::fromLatin1('@'));
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::QQNorlogeRef
/// \param norlogeRef
///
QQNorlogeRef::QQNorlogeRef(const QQNorlogeRef& norlogeRef) :
	QQNorloge(norlogeRef),
	m_dstBouchot(norlogeRef.m_dstBouchot),
	m_origNRef(norlogeRef.m_origNRef),
	m_listPostTarget(norlogeRef.m_listPostTarget),
	m_valid(norlogeRef.m_valid),
	m_hasDate(norlogeRef.m_hasDate),
	m_hasSec(norlogeRef.m_hasSec),
	m_isReponseDefined(norlogeRef.m_isReponseDefined),
	m_isResponse(norlogeRef.m_isResponse),
	m_refId(norlogeRef.m_refId)
{
}


//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::dstBouchot
/// \return
///
QString QQNorlogeRef::dstBouchot() const
{
	return (m_dstBouchot.size() > 0) ? m_dstBouchot : m_srcBouchot;
}

//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::matchesPost
/// \param post
/// \return
///
bool QQNorlogeRef::matchesPost(QQPost* post)
{
	if(! m_valid)
		return false;

	bool found = false;
	QMutableListIterator< QPointer<QQPost> > i(m_listPostTarget);
	while(i.hasNext())
	{
		QPointer<QQPost> ptr = i.next();
		if(ptr.isNull())
		{
			i.remove();
			continue;
		}

		if(ptr.data() == post)
		{
			found = true;
			break;
		}
	}


	if(! found)
	{
		QString dstB = dstBouchot();
		if(dstB == post->bouchot()->name() ||
				post->bouchot()->settings().containsAlias(dstB))
		{
			if(post->norlogeObj().matchingNRefsId().contains(nRefId()))
			{
				m_listPostTarget.append(QPointer<QQPost>(post));
				found = true;
			}
		}
	}

	return found;
}


//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::matchesNRef
/// \param other
/// \return
///
bool QQNorlogeRef::matchesNRef(QQNorlogeRef& other)
{
	if(! m_valid || ! other.isValid())
		return false;

	if(QString::compare(this->dstBouchot(), other.dstBouchot()) != 0)
		return false;

	QString selfNorlogeId = nRefId();
	QString otherNorlogeId = other.nRefId();

	return otherNorlogeId.contains(selfNorlogeId) || selfNorlogeId.contains(otherNorlogeId);
}


//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::isReponse
/// \return
///
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

//////////////////////////////////////////////////////////////
/// \brief QQNorlogeRef::nRefId
/// \return
///
QString QQNorlogeRef::nRefId()
{
	QString res = m_refId;
	if(res.size() == 0 && isValid())
	{
		if(m_hasDate)
		{
			if(m_dateYearPart.size() > 0)
				res.append(m_dateYearPart).append("Y");
			res.append(m_dateMonthPart).append("M");
			res.append(m_dateDayPart).append("D");
		}
		res.append(m_dateHourPart).append("h");
		res.append(m_dateMinutePart).append("m");
		if(m_hasSec)
		{
			res.append(m_dateSecondPart).append("s");
			if(m_norlogeIndex > 0)
				res.append(QString::number(m_norlogeIndex)).append("i");
		}
		m_refId = res;
	}
	return res;
}

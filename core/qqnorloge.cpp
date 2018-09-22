#include "qqnorloge.h"

#include <QtDebug>
#include <QDateTime>
#include <QStringList>

#include "qqsettings.h"

QQNorloge::QQNorloge() :
    m_norlogeIndex(0),
    m_uniqueMinute(false)
{
}

//m_date = QDateTime::fromString(dateh.left(14), QString::fromUtf8("yyyyMMddHHmmss(^i)?"));
QQNorloge::QQNorloge(QString bouchot, QString dateh) :
    m_srcBouchot(bouchot),
    m_dateYearPart(dateh.left(4)),
    m_dateMonthPart(dateh.mid(4, 2)),
    m_dateDayPart(dateh.mid(6, 2)),
    m_dateHourPart(dateh.mid(8, 2)),
    m_dateMinutePart(dateh.mid(10, 2)),
    m_dateSecondPart(dateh.mid(12, 2)),
    m_norlogeIndex(0),
    m_uniqueMinute(false)
{
	QStringList indexSpit = dateh.split("^", QString::SkipEmptyParts);
	if(indexSpit.size() > 1)
		m_norlogeIndex = indexSpit[1].toInt();
}

QQNorloge::QQNorloge(const QQNorloge& norloge) :
    m_srcBouchot(norloge.m_srcBouchot),
    m_dateYearPart(norloge.m_dateYearPart),
    m_dateMonthPart(norloge.m_dateMonthPart),
    m_dateDayPart(norloge.m_dateDayPart),
    m_dateHourPart(norloge.m_dateHourPart),
    m_dateMinutePart(norloge.m_dateMinutePart),
    m_dateSecondPart(norloge.m_dateSecondPart),
    m_norlogeIndex(norloge.m_norlogeIndex),
    m_uniqueMinute(norloge.m_uniqueMinute)
{
}

QStringList QQNorloge::matchingNRefsId() const
{
	QStringList rep;
	QString baseHmStr = QString(m_dateHourPart).append("h").append(m_dateMinutePart).append("m");
	rep.append(baseHmStr);

	QString baseHmsStr = QString(baseHmStr).append(m_dateSecondPart).append("s");
	rep.append(baseHmsStr);

	QString baseMDhmsStr = QString(m_dateMonthPart).append("M")
	                       .append(m_dateDayPart).append("D")
	                       .append(baseHmsStr);
	rep.append(baseMDhmsStr);

	QString baseYMDhmsStr = QString(m_dateYearPart).append("Y")
	                        .append(baseMDhmsStr);
	rep.append(baseYMDhmsStr);

	if(m_norlogeIndex > 0)
	{
		QString norlogeIndexStr = QString::number(m_norlogeIndex);
		rep.append(baseHmsStr + QString(norlogeIndexStr).append("i"));
		rep.append(baseMDhmsStr + QString(norlogeIndexStr).append("i"));
		rep.append(baseYMDhmsStr + QString(norlogeIndexStr).append("i"));
	}
	else
	{
		rep.append(baseHmsStr + QString("1i"));
		rep.append(baseMDhmsStr + QString("1i"));
		rep.append(baseYMDhmsStr + QString("1i"));
	}
	return rep;
}

QString QQNorloge::toStringPalmi()
{
	QDateTime currDateT = QDateTime::currentDateTime();
	QString rep;

	bool startPrint = false;
	bool hasDate = false;
	if(m_dateYearPart.toInt() != currDateT.date().year())
	{
		rep.append(m_dateYearPart)
		        .append(QString::fromUtf8("/"));
		startPrint = true;
		hasDate = true;
	}
	if(startPrint || m_dateMonthPart.toInt() != currDateT.date().month() || m_dateDayPart.toInt() != currDateT.date().day())
	{
		rep.append(m_dateMonthPart)
		        .append(QString::fromUtf8("/"))
		        .append(m_dateDayPart)
		        .append(QString::fromUtf8("#"));
		startPrint = true;
		hasDate = true;
	}
	//On a TOUJOURS l'heure et les minutes
	rep.append(m_dateHourPart)
	        .append(QString::fromUtf8(":"))
	        .append(m_dateMinutePart);

	//Mais pas toujours les secondes
	QQSettings settings;
	if (m_uniqueMinute == false || hasDate ||
	    settings.value(SETTINGS_PALMI_SHORT_NORLOGE_ENABLED, DEFAULT_PALMI_SHORT_NORLOGE_ENABLED).toBool() == false)
	{
		rep.append(QString::fromUtf8(":"))
		        .append(m_dateSecondPart);
	}
	switch (m_norlogeIndex)
	{
		case 0:
			break;
		case 1:
			rep.append(QString::fromUtf8("¹"));
			break;
		case 2:
			rep.append(QString::fromUtf8("²"));
			break;
		case 3:
			rep.append(QString::fromUtf8("³"));
			break;
		default:
			rep.append(QString("^%1").arg(m_norlogeIndex));
	}

	rep.append(QString::fromUtf8("@")).append(m_srcBouchot);

	return rep;
}

#include "qqnorloge.h"

#include <QDebug>
#include <QDateTime>
#include <QStringList>


QQNorloge::QQNorloge()
{
	m_srcBouchot = QString();

	m_dateYearPart = QString();
	m_dateMonthPart = QString();
	m_dateDayPart = QString();
	m_dateHourPart = QString();
	m_dateMinutePart = QString();
	m_dateSecondPart = QString();

	m_norlogeIndex = 0;
}

//m_date = QDateTime::fromString(dateh.left(14), QString::fromUtf8("yyyyMMddHHmmss"));
QQNorloge::QQNorloge(QString bouchot, QString dateh)
{
	m_srcBouchot = bouchot;

	m_dateYearPart = dateh.left(4);
	m_dateMonthPart = dateh.mid(4, 2);
	m_dateDayPart = dateh.mid(6, 2);
	m_dateHourPart = dateh.mid(8, 2);
	m_dateMinutePart = dateh.mid(10, 2);
	m_dateSecondPart = dateh.mid(12, 2);

	m_norlogeIndex = 0;
}

QQNorloge::QQNorloge(const QQNorloge& norloge)
{
	m_srcBouchot = norloge.m_srcBouchot;

	m_dateYearPart = norloge.m_dateYearPart;
	m_dateMonthPart = norloge.m_dateMonthPart;
	m_dateDayPart = norloge.m_dateDayPart;
	m_dateHourPart = norloge.m_dateHourPart;
	m_dateMinutePart = norloge.m_dateMinutePart;
	m_dateSecondPart = norloge.m_dateSecondPart;

	m_norlogeIndex = norloge.m_norlogeIndex;
}


QString QQNorloge::toStringPalmi()
{
	QDateTime currDateT = QDateTime::currentDateTime();
	QString rep;

	bool startPrint = false;
	qDebug() << "QQNorloge::toStringPalmi currDateT.date().year()=" << currDateT.date().year();
	if(m_dateYearPart.toInt() != currDateT.date().year())
	{
		rep.append(m_dateYearPart)
				.append(QString::fromUtf8("/"));
		startPrint = true;
	}
	if(startPrint || m_dateMonthPart.toInt() != currDateT.date().month() || m_dateDayPart.toInt() != currDateT.date().day())
	{
		rep.append(m_dateMonthPart)
				.append(QString::fromUtf8("/"))
				.append(m_dateDayPart)
				.append(QString::fromUtf8("#"));
		startPrint = true;
	}
	//On a TOUJOURS l'heure
	rep.append(m_dateHourPart)
			.append(QString::fromUtf8(":"))
			.append(m_dateMinutePart)
			.append(QString::fromUtf8(":"))
			.append(m_dateSecondPart);

	//TODO index à placer ici
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
		rep.append(QString("^%d").arg(m_norlogeIndex));
	}

	rep.append(QString::fromUtf8("@")).append(m_srcBouchot);

	return rep;
}

QString QQNorloge::toStringPini()
{
	QString rep = QString::fromUtf8("[");

	rep.append(m_dateHourPart)
			.append(QString::fromUtf8(":"))
			.append(m_dateDayPart)
			.append(QString::fromUtf8(":"))
			.append(m_dateSecondPart)
			.append(QString::fromUtf8("]"));
	return rep;
}

#include "qqnorloge.h"

#include <QDebug>
#include <QDateTime>
#include <QStringList>

//m_date = QDateTime::fromString(dateh.left(14), QString::fromLatin1("yyyyMMddHHmmss"));
QQNorloge::QQNorloge(QString bouchot, QString dateh)
{
    m_srcBouchot = bouchot;

    m_dateYearPart = dateh.left(4);
    m_dateMonthPart = dateh.mid(4, 2);
    m_dateDayPart = dateh.mid(6, 2);
    m_dateHourPart = dateh.mid(8, 2);
    m_dateMinutePart = dateh.mid(10, 2);
    m_dateSecondPart = dateh.mid(12, 2);
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
                .append(QString::fromLatin1("/"));
        startPrint = true;
    }
    if(startPrint || m_dateMonthPart.toInt() != currDateT.date().month() || m_dateDayPart.toInt() != currDateT.date().day())
    {
        rep.append(m_dateMonthPart)
                .append(QString::fromLatin1("/"))
                .append(m_dateDayPart)
                .append(QString::fromLatin1("#"));
        startPrint = true;
    }
    //On a TOUJOURS l'heure
    rep.append(m_dateHourPart)
            .append(QString::fromLatin1(":"))
            .append(m_dateDayPart)
            .append(QString::fromLatin1(":"))
            .append(m_dateSecondPart);

    //TODO index à placer ici
    switch (m_norlogeIndex)
    {
    case 0:
        break;
    case 1:
        rep.append(QString::fromLatin1("¹"));
        break;
    case 2:
        rep.append(QString::fromLatin1("²"));
        break;
    case 3:
        rep.append(QString::fromLatin1("³"));
        break;
    default:
        rep.append(QString("^%d").arg(m_norlogeIndex));
    }

    rep.append(QString::fromLatin1("@")).append(m_srcBouchot);

    return rep;
}

QString QQNorloge::toStringPini()
{
    QString rep = QString::fromLatin1("[");

    rep.append(m_dateHourPart)
            .append(QString::fromLatin1(":"))
            .append(m_dateDayPart)
            .append(QString::fromLatin1(":"))
            .append(m_dateSecondPart)
            .append(QString::fromLatin1("]"));
    return rep;
}

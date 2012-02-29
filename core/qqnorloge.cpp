#include "qqnorloge.h"

QQNorloge::QQNorloge(QString bouchot, QString dateh)
{
    m_bouchot = bouchot;
    m_date = QDateTime::fromString(dateh.left(14), QString::fromLatin1("yyyyMMddHHmmss"));
}


QString QQNorloge::toStringPalmi()
{
    QDateTime currDateT = QDateTime::currentDateTime();
    QString format;

    bool startPrint = false;
    if(m_date.date().year() != currDateT.date().year())
    {
        format.append(QString::fromLatin1("yyyy/"));
        startPrint = true;
    }
    if(startPrint || m_date.date().month() != currDateT.date().month() || m_date.date().day() != currDateT.date().day())
    {
        format.append(QString::fromLatin1("MM/dd#"));
        startPrint = true;
    }
    //On a TOUJOURS l'heure
    format.append(QString::fromLatin1("HH:mm:ss"));

    QString rep = m_date.toString(format);
    //TODO index à placer ici
    rep.append(QString::fromLatin1("@")).append(m_bouchot);

    return rep;
}

QString QQNorloge::toStringPini()
{
    QTime t = m_date.time();

    return t.toString(QString::fromLatin1("[HH:mm:ss]"));
}

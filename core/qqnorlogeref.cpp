#include "qqnorlogeref.h"

#include <QString>
#include <QStringList>

QQNorlogeRef::QQNorlogeRef(QString bouchot, QString dateh, QString norlogeRef) :
    QQNorloge(bouchot, dateh)
{
    QRegExp reg = norlogeRegexp();
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

        QString time = capturedTexts[3];
        QStringList timeSplit = time.split(QChar::fromAscii(':'));
        m_refDateHourPart = timeSplit.takeFirst();
        m_refDateMinutePart = timeSplit.takeFirst();
        m_refDateIndexPart = 0; //tous les correspondants par défaut
        if(timeSplit.size() > 0)
        {
            QString sec = timeSplit.takeFirst();
            m_refDateSecondPart = sec.left(2);
            sec.remove(0, 2);
            if(sec.length() > 0)
            {
                if(sec.startsWith(QString::fromLatin1("¹")))
                    m_refDateIndexPart = 1;
                else if(sec.startsWith(QString::fromLatin1("²")))
                    m_refDateIndexPart = 2;
                else if(sec.startsWith(QString::fromLatin1("³")))
                    m_refDateIndexPart = 3;
                else
                {
                    sec.remove(0, 1);
                    m_refDateIndexPart = timeSplit.takeFirst().toInt();
                }
            }
        }
        if(timeSplit.size() > 0)
            m_refDateIndexPart = timeSplit.takeFirst().toInt();

        m_dstBouchot = capturedTexts[4];
        //supression du @ initial
        m_refDateDayPart.remove(QChar::fromAscii('@'));
    }
}

QString QQNorlogeRef::dstBouchot()
{
    return (m_dstBouchot.size() > 0) ? m_dstBouchot : m_srcBouchot;
}

QString QQNorlogeRef::dstNorloge()
{
    QString dstNorloge;

    dstNorloge.append((m_refDateYearPart.size() > 0) ? m_refDateYearPart : m_dateYearPart );
    dstNorloge.append((m_refDateMonthPart.size() > 0) ? m_refDateMonthPart : m_dateMonthPart );
    dstNorloge.append((m_refDateDayPart.size() > 0) ? m_refDateDayPart : m_dateDayPart );
    dstNorloge.append((m_refDateHourPart.size() > 0) ? m_refDateHourPart : m_dateHourPart );
    dstNorloge.append((m_refDateMinutePart.size() > 0) ? m_refDateMinutePart : m_dateMinutePart );
    dstNorloge.append((m_refDateSecondPart.size() > 0) ? m_refDateSecondPart : QString::fromAscii("") );

    return dstNorloge;
}

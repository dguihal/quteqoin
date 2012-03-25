#ifndef QQNORLOGE_H
#define QQNORLOGE_H

#include <QRegExp>
#include <QString>

class QQNorloge
{
public:
    QQNorloge(QString bouchot, QString dateh);

    QString toStringPalmi();
    QString toStringPini();
    QString srcBouchot() { return m_srcBouchot; }

    void setNorlogeIndex(int index) { m_norlogeIndex = index;}
    int norlogeIndex() { return m_norlogeIndex; }

private:
    QString m_srcBouchot;

    QString m_dateYearPart;
    QString m_dateMonthPart;
    QString m_dateDayPart;
    QString m_dateHourPart;
    QString m_dateMinutePart;
    QString m_dateSecondPart;

    int m_norlogeIndex;
};

#endif // QQNORLOGE_H

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

    //Les zones capturées sont :
    // 0 - Norloge complète
    // 1 - Date + Time + Subtime
    // 2 - Date
    // 3 - Time + Subtime
    // 4 - Tribune
    static QRegExp norlogeRegexp()
    {
        return norlogeRegexp(QString::fromAscii("[A-Za-z0-9_]+"));
    }

    //Les zones capturées sont :
    // 0 - Norloge complète
    // 1 - Date + Time + Subtime
    // 2 - Date
    // 3 - Time + Subtime
    // 4 - Tribune
    static QRegExp norlogeRegexp(const QString & bouchot)
    {
        return QRegExp(QString::fromUtf8("(((?:[0-9]+/)?(?:1[0-2]|0[1-9])/(?:3[0-1]|[1-2][0-9]|0[1-9])#)?" //date
                                          "((?:2[0-3]|[0-1][0-9]):(?:[0-5][0-9])(?::[0-5][0-9])?" //time
                                          "(?:[¹²³]|[:\\^][1-9]|[:\\^][1-9][0-9])?))" //subtime
                                          "(@") + bouchot + QString::fromAscii(")?"), //tribune
                       Qt::CaseSensitive,
                       QRegExp::RegExp);
    }
private:
    QString m_srcBouchot;
    QString m_dstBouchot;

    QString m_dateYearPart;
    QString m_dateMonthPart;
    QString m_dateDayPart;
    QString m_dateHourPart;
    QString m_dateMinutePart;
    QString m_dateSecondPart;
    int m_dateIndexPart;
};

#endif // QQNORLOGE_H

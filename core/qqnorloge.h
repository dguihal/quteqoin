#ifndef QQNORLOGE_H
#define QQNORLOGE_H

#include <QDateTime>
#include <QRegExp>
#include <QString>

class QQNorloge
{
public:
    QQNorloge(QString bouchot, QString dateh);

    QString toStringPalmi();
    QString toStringPini();

    //Les zones capturées sont :
    // 1 - Date + Time + Subtime
    // 2 - Date
    // 3 - Time + Subtime
    // 4 - Tribune
    static QRegExp norlogeRegexp()
    {
        return norlogeRegexp(QString::fromAscii("[A-Za-z0-9_]+"));
    }

    //Les zones capturées sont :
    // 1 - Date + Time + Subtime
    // 2 - Date
    // 3 - Time + Subtime
    // 4 - Tribune
    static QRegExp norlogeRegexp(const QString & bouchot)
    {
        return QRegExp(QString::fromAscii("(((?:1[0-2]|0[1-9])/(?:3[0-1]|[1-2][0-9]|0[1-9])#)?" //date
                                          "((?:2[0-3]|[0-1][0-9]):(?:[0-5][0-9])(?::[0-5][0-9])?" //time
                                          "(?:[¹²³]|[:\\^][1-9]|[:\\^][1-9][0-9])?))" //subtime
                                          "(@") + bouchot + QString::fromAscii(")?"), //tribune
                       Qt::CaseSensitive,
                       QRegExp::RegExp);
    }
private:
    QString m_bouchot;
    QDateTime m_date;
};

#endif // QQNORLOGE_H

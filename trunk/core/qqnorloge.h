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

    static QRegExp norlogeRegexp() {return QRegExp(QString::fromAscii("((?:(?:1[0-2]|0[1-9])/(?:3[0-1]|[1-2][0-9]|0[1-9])#)?" //date
                                                                            "(?:2[0-3]|[0-1][0-9]):(?:[0-5][0-9])(?::[0-5][0-9])?" //time
                                                                            "(?:[¹²³]|[:\\^][1-9]|[:\\^][1-9][0-9])?" //subtime
                                                                            "(?:@[A-Za-z0-9_]+)?)"), //tribune
                                                         Qt::CaseSensitive,
                                                         QRegExp::RegExp); }

private:
    QString m_bouchot;
    QDateTime m_date;
};

#endif // QQNORLOGE_H

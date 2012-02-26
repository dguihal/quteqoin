#ifndef QQNORLOGE_H
#define QQNORLOGE_H

#include <QDateTime>
#include <QString>

class QQNorloge
{
public:
    QQNorloge(QString bouchot, QString dateh);

    QString toStringPalmi();
    QString toStringPini();

private:
    QString m_bouchot;
    QDateTime m_date;
};

#endif // QQNORLOGE_H

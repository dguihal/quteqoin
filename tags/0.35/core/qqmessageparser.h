#ifndef QQMessagePARSER_H
#define QQMessagePARSER_H

#include <QObject>
#include <QList>
#include <QRegExp>
#include <QString>
#include <QVector>

class QQMessageElt
{
public:
    enum Type
    {
        Txt = 0,
        Duck,
        Totoz,
        Norloge,
        Bigorno
    };

    explicit QQMessageElt();
    QQMessageElt(Type);

    void setType(Type type) { m_type = type; }
    Type type() {return m_type;}

    void setTxt(QString txt) { m_txt = txt; }
    QString txt() { return m_txt; }
private:
    Type m_type;
    QString m_txt;
};

class QQMessageParser : public QObject
{
    Q_OBJECT
public:
    explicit QQMessageParser(QObject *parent = 0);

    void parseMessage(QString);
    QVector<QQMessageElt> messageElts() { return lstMessageElt; }


private:
    QList<QQMessageElt> splitList(QList<QQMessageElt>, QRegExp, QQMessageElt::Type);
    QList<QQMessageElt> split(QString, QRegExp, QQMessageElt::Type);

    QVector<QQMessageElt> lstMessageElt;

    QRegExp m_norlogeReg;
    QRegExp m_duck1Reg;
    QRegExp m_duck2Reg;
    QRegExp m_duck3Reg;
    QRegExp m_tableReg;
    QRegExp m_totozReg;

signals:

public slots:

};

#endif // QQMessagePARSER_H

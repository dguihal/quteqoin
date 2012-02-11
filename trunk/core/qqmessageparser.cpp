#include "qqmessageparser.h"


QQMessageElt::QQMessageElt()
{
    m_type = Txt;
}

QQMessageElt::QQMessageElt(Type t)
{
    m_type = t;
}


////////////////////////////////////////////////////////////


QQMessageParser::QQMessageParser(QObject *parent) :
    QObject(parent)
{

    m_norlogeReg = QRegExp(QString::fromAscii("((?:(?:1[0-2]|0[1-9])/(?:3[0-1]|[1-2][0-9]|0[1-9])#)?" //date
                                                      "(?:2[0-3]|[0-1][0-9]):(?:[0-5][0-9])(?::[0-5][0-9])?" //time
                                                      "(?:[¹²³]|[:\\^][1-9]|[:\\^][1-9][0-9])?" //subtime
                                                      "(?:@[A-Za-z0-9_]+)?)"), //tribune
                                   Qt::CaseSensitive,
                                   QRegExp::RegExp);
    QString tete = QString::fromAscii("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");
    m_duck1Reg = QRegExp(QString::fromAscii("(\\\\_").append(tete).append(QString::fromAscii("&lt;)")),
                       Qt::CaseSensitive,
                       QRegExp::RegExp);
    m_duck2Reg = QRegExp(QString::fromAscii("(&gt;").append(tete).append(QString::fromAscii("_\\/)")),
                       Qt::CaseSensitive,
                       QRegExp::RegExp);
    m_duck3Reg = QRegExp(QString::fromAscii("(coin ?! ?coin ?!)"),
                       Qt::CaseSensitive,
                       QRegExp::RegExp);
    m_tableReg = QRegExp(QString::fromAscii("((?:flap ?flap)|(?:table[ _]volante))"),
                       Qt::CaseSensitive,
                       QRegExp::RegExp);
    m_totozReg = QRegExp(QString::fromAscii("(\\[\\:[^\\t\\)\\]]+\\])"), //[:[^\t\)\]]
                       Qt::CaseSensitive,
                       QRegExp::RegExp);
}


void QQMessageParser::parseMessage(QString message)
{
    QList<QQMessageElt> tmpLst = split(message, m_norlogeReg, QQMessageElt::Norloge);

    tmpLst = splitList(tmpLst, m_totozReg, QQMessageElt::Totoz);

    tmpLst = splitList(tmpLst, m_duck1Reg, QQMessageElt::Duck);
    tmpLst = splitList(tmpLst, m_duck2Reg, QQMessageElt::Duck);
    tmpLst = splitList(tmpLst, m_duck3Reg, QQMessageElt::Duck);
    tmpLst = splitList(tmpLst, m_tableReg, QQMessageElt::Duck);

    lstMessageElt = QVector<QQMessageElt>::fromList(tmpLst);
}

QList<QQMessageElt> QQMessageParser::splitList(QList<QQMessageElt> tmpLst, QRegExp regexp, QQMessageElt::Type t)
{
    QList<QQMessageElt> tmpLst2;

    for (int i = 0; i < tmpLst.size(); ++i)
    {
        if(tmpLst[i].type() != QQMessageElt::Txt)
            tmpLst2 << tmpLst[i];
        else
            tmpLst2 << split(tmpLst[i].txt(), regexp, t);
    }

    return tmpLst2;
}

QList<QQMessageElt> QQMessageParser::split(QString string, QRegExp regexp, QQMessageElt::Type t)
{
    QList<QQMessageElt> currLst;

    int firstIndex;
    while((firstIndex = regexp.indexIn(string)) != -1)
    {
        if(firstIndex > 0)
        {
            QQMessageElt txtElt(QQMessageElt::Txt);
            txtElt.setTxt(string.left(firstIndex));
            currLst.append(txtElt);

            string.remove(0, firstIndex);
        }

        int matchedLength = regexp.matchedLength();

        QQMessageElt regElt(t);
        regElt.setTxt(string.left(matchedLength));
        currLst.append(regElt);

        string.remove(0, matchedLength);
    }

    if(string.length() > 0)
    {
        QQMessageElt txtElt(QQMessageElt::Txt);
        txtElt.setTxt(string);
        currLst.append(txtElt);
    }

    return currLst;
}

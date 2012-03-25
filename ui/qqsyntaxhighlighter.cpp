#include "qqsyntaxhighlighter.h"

#include "core/qqnorlogeref.h"
#include "core/qqtextcharformat.h"
#include "ui/qqmessageblockuserdata.h"

#include <QDebug>
#include <QRegExp>
#include <QTextCharFormat>

QQSyntaxHighlighter::QQSyntaxHighlighter(QQTextBrowser *parent) :
    QSyntaxHighlighter(parent)
{

}

void QQSyntaxHighlighter::highlightBlock(const QString &text)
{
    QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(currentBlockUserData());

    bool userDataWasNull = false;
    if(userData == NULL)
    {
        userDataWasNull = true;
        userData = new QQMessageBlockUserData();
    }

    qDebug() << "QQSyntaxHighlighter::highlightBlock text=" << text;
    if(userData != NULL &&
            userData->constainsData(QQMessageBlockUserData::IS_MESSAGE_ZONE) &&
            userData->getData(QQMessageBlockUserData::IS_MESSAGE_ZONE) == true)
    {
        highlightNorloge(text, userData);
        highlightDuck(text, userData);
        highlightTableVolante(text, userData);
        highlightTotoz(text, userData);
    }
    else
    {
        qDebug() << "QQSyntaxHighlighter::highlightBlock, userData is NULL";
    }

    if(userDataWasNull)
        setCurrentBlockUserData(userData);
}


void QQSyntaxHighlighter::highlightNorloge(const QString & text, QQMessageBlockUserData * userData)
{
    QColor color = QColor("#0000DD");

    QRegExp m_norlogeReg = QQNorlogeRef::norlogeRegexp();

    int index = text.indexOf(m_norlogeReg);
    while (index >= 0)
    {
        int length = m_norlogeReg.matchedLength();
        userData->addNorlogeRefZone(index, text.mid(index, length));
        setFormat(index, length, color);

        index = text.indexOf(m_norlogeReg, index + length);
    }
}

void QQSyntaxHighlighter::highlightDuck(const QString & text, QQMessageBlockUserData * userData)
{
    QColor color = QColor("#9933CC");

    QString tete = QString::fromAscii("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");

    //QRegExp m_duckReg = QRegExp(QString::fromAscii("(\\\\_").append(tete).append(QString::fromAscii("&lt;)")),
    QRegExp m_duckReg = QRegExp(QString::fromAscii("\\\\_").append(tete).append(QString::fromAscii("<")),
                                Qt::CaseSensitive,
                                QRegExp::RegExp);

    int index = text.indexOf(m_duckReg);
    while (index >= 0) {
        int length = m_duckReg.matchedLength();
        userData->addDuckZone(index, text.mid(index, length));
        setFormat(index, length, color);

        index = text.indexOf(m_duckReg, index + length);
    }

    //m_duckReg = QRegExp(QString::fromAscii("(&gt;").append(tete).append(QString::fromAscii("_\\/)")),
    m_duckReg = QRegExp(QString::fromAscii(">").append(tete).append(QString::fromAscii("_\\/")),
                        Qt::CaseSensitive,
                        QRegExp::RegExp);
    index = text.indexOf(m_duckReg);
    while (index >= 0) {
        int length = m_duckReg.matchedLength();
        userData->addDuckZone(index, text.mid(index, length));
        setFormat(index, length, color);

        index = text.indexOf(m_duckReg, index + length);
    }

    m_duckReg = QRegExp(QString::fromAscii("coin ?! ?coin ?!"),
                        Qt::CaseSensitive,
                        QRegExp::RegExp); index = text.indexOf(m_duckReg);
    while (index >= 0) {
        int length = m_duckReg.matchedLength();
        userData->addDuckZone(index, text.mid(index, length));
        setFormat(index, length, color);

        index = text.indexOf(m_duckReg, index + length);
    }
}

void QQSyntaxHighlighter::highlightTableVolante(const QString & text, QQMessageBlockUserData * userData)
{
    QColor color = QColor("#9933CC");

    QRegExp m_tvReg = QRegExp(QString::fromAscii("(?:flap ?flap)|(?:table[ _]volante)"),
                              Qt::CaseSensitive,
                              QRegExp::RegExp);
    int index = text.indexOf(m_tvReg);
    while (index >= 0) {
        int length = m_tvReg.matchedLength();
        userData->addTableVZone(index, text.mid(index, length));
        setFormat(index, length, color);

        index = text.indexOf(m_tvReg, index + length);
    }
}

void QQSyntaxHighlighter::highlightTotoz(const QString & text, QQMessageBlockUserData * userData)
{
    QTextCharFormat totozMessageFormat;
    totozMessageFormat.setForeground(QColor("#00AA11"));
    totozMessageFormat.setFontWeight(QFont::Bold);

    QRegExp m_totozReg = QRegExp(QString::fromAscii("(\\[\\:[^\\t\\)\\]]+\\])"), //[:[^\t\)\]]
                                 Qt::CaseSensitive,
                                 QRegExp::RegExp);

    int index = text.indexOf(m_totozReg);
    while (index >= 0) {
        int length = m_totozReg.matchedLength();
        userData->addTotozZone(index, text.mid(index, length));
        setFormat(index, length, totozMessageFormat);

        index = text.indexOf(m_totozReg, index + length);
    }
}

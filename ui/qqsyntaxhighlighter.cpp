#include "qqsyntaxhighlighter.h"

#include "core/qqnorloge.h"
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

    if(userData != NULL &&
            userData->constainsData(QQMessageBlockUserData::IS_MESSAGE_ZONE) &&
            userData->getData(QQMessageBlockUserData::IS_MESSAGE_ZONE) == true)
    {
        highlightNorloge(text);
        highlightDuck(text);
        highlightTableVolante(text);
        highlightTotoz(text);
    }
    else
    {
        qDebug() << "QQSyntaxHighlighter::highlightBlock, userData is NULL";
    }
}


void QQSyntaxHighlighter::highlightNorloge(const QString &text)
{
    QTextCharFormat norlogeMessageFormat;
    norlogeMessageFormat.setForeground(QColor("#0000DD"));
    norlogeMessageFormat.setObjectType(RefNorlogeTextFormat);

    QRegExp m_norlogeReg = QQNorloge::norlogeRegexp();

    int index = text.indexOf(m_norlogeReg);
    while (index >= 0) {
        int length = m_norlogeReg.matchedLength();
        norlogeMessageFormat.setProperty(NorlogeData, text.mid(index, length));
        setFormat(index, length, norlogeMessageFormat);
        index = text.indexOf(m_norlogeReg, index + length);
    }
}

void QQSyntaxHighlighter::highlightDuck(const QString &text)
{
    QTextCharFormat duckMessageFormat;
    duckMessageFormat.setForeground(QColor("#9933CC"));
    duckMessageFormat.setObjectType(DuckTextFormat);

    QString tete = QString::fromAscii("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");
    QRegExp m_duckReg = QRegExp(QString::fromAscii("(\\\\_").append(tete).append(QString::fromAscii("&lt;)")),
                                Qt::CaseSensitive,
                                QRegExp::RegExp);

    int index = text.indexOf(m_duckReg);
    while (index >= 0) {
        int length = m_duckReg.matchedLength();
        setFormat(index, length, duckMessageFormat);
        index = text.indexOf(m_duckReg, index + length);
    }

    m_duckReg = QRegExp(QString::fromAscii("(&gt;").append(tete).append(QString::fromAscii("_\\/)")),
                        Qt::CaseSensitive,
                        QRegExp::RegExp);
    index = text.indexOf(m_duckReg);
    while (index >= 0) {
        int length = m_duckReg.matchedLength();
        setFormat(index, length, duckMessageFormat);
        index = text.indexOf(m_duckReg, index + length);
    }

    m_duckReg = QRegExp(QString::fromAscii("(coin ?! ?coin ?!)"),
                        Qt::CaseSensitive,
                        QRegExp::RegExp); index = text.indexOf(m_duckReg);
    while (index >= 0) {
        int length = m_duckReg.matchedLength();
        setFormat(index, length, duckMessageFormat);
        index = text.indexOf(m_duckReg, index + length);
    }
}

void QQSyntaxHighlighter::highlightTableVolante(const QString &text)
{
    QTextCharFormat tableVolanteMessageFormat;
    tableVolanteMessageFormat.setForeground(QColor("#9933CC"));
    tableVolanteMessageFormat.setObjectType(TableVolanteTextFormat);

    QRegExp m_tvReg = QRegExp(QString::fromAscii("((?:flap ?flap)|(?:table[ _]volante))"),
                              Qt::CaseSensitive,
                              QRegExp::RegExp);
    int index = text.indexOf(m_tvReg);
    while (index >= 0) {
        int length = m_tvReg.matchedLength();
        setFormat(index, length, tableVolanteMessageFormat);
        index = text.indexOf(m_tvReg, index + length);
    }
}

void QQSyntaxHighlighter::highlightTotoz(const QString &text)
{

    QTextCharFormat totozMessageFormat;
    totozMessageFormat.setForeground(QColor("#00AA11"));
    totozMessageFormat.setFontWeight(QFont::Bold);
    totozMessageFormat.setObjectType(TotozTextFormat);

    QRegExp m_totozReg = QRegExp(QString::fromAscii("(\\[\\:[^\\t\\)\\]]+\\])"), //[:[^\t\)\]]
                                 Qt::CaseSensitive,
                                 QRegExp::RegExp);

    int index = text.indexOf(m_totozReg);
    while (index >= 0) {
        int length = m_totozReg.matchedLength();
        totozMessageFormat.setProperty(TotozData, text.mid(index, length));
        setFormat(index, length, totozMessageFormat);
        index = text.indexOf(m_totozReg, index + length);
    }
}

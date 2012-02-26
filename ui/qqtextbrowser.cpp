#include "qqtextbrowser.h"

#include "core/qqpost.h"
#include "core/qqtextcharformat.h"

#include <QDebug>
#include <QMouseEvent>
#include <QTextBlock>
#include <QTextTable>
#include <QTextTableCell>

QQTextBrowser::QQTextBrowser(QString groupName, QQPinipede *parent) :
    QTextEdit(parent)
{
    setReadOnly(true);
    m_groupName = groupName;
    m_parent = parent;
    this->setMouseTracking(true);
}

QQTextBrowser::~QQTextBrowser()
{
}


QString QQTextBrowser::bouchotUnderCursor()
{
    return m_currBouchot;
}

QString QQTextBrowser::messageUnderCursor()
{
    return m_message;
}


void QQTextBrowser::mouseMoveEvent(QMouseEvent *event)
{
    QTextEdit::mouseMoveEvent(event);

    mousePressed = false;

    QTextCursor cursor = cursorForPosition(event->pos());
    QTextTable * table = cursor.currentTable();

    if(table != NULL)
    {
        QTextTableCell tableCell = table->cellAt(cursor);

        mouseColNum = tableCell.column();
        QTextCharFormat format;
        switch (mouseColNum)
        {
        case 0:
            break;
        case 1: format = cursor.charFormat();
            m_message = format.property(NorlogeData).toString();
            m_currBouchot = format.property(BouchotData).toString();
            //qDebug() << "QQTextBrowser::mouseMoveEvent 2 : "
            //         << "m_currBouchot = " << m_currBouchot
            //         << ", m_message = " << m_message;
            break;
        case 2: qDebug() << "QQTextBrowser::mouseMoveEvent 3 "
                         << m_parent->getPostForGroup(m_groupName, tableCell.row())->login();
            break;
        case 3: qDebug() << "QQTextBrowser::mouseMoveEvent 4 "
                         << m_parent->getPostForGroup(m_groupName, tableCell.row())->message();

            break;
        default: qDebug() << "QQTextBrowser::mouseMoveEvent Oups !!!!!!! : tableCell.column()=" << tableCell.column();
            break;
        }
    }
    else
    {
        mouseRowNum = -1;
        mouseColNum = -1;
        m_currBouchot.clear();
    }
}

void QQTextBrowser::mousePressEvent ( QMouseEvent * event )
{
    QTextEdit::mousePressEvent(event);

    mousePressed = true;
}

void QQTextBrowser::mouseReleaseEvent(QMouseEvent * event)
{
    QTextEdit::mouseReleaseEvent(event);

    //pour ne pas confondre clic et selection
    if(mousePressed == false)
        return;

    if(mouseColNum == 1)
    {
        QQNorloge norloge(m_currBouchot, m_message);
        emit norlogeClicked(norloge);
    }
    else if(mouseColNum == 2)
        emit loginClicked(m_groupName);
}

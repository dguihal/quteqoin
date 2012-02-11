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

void QQTextBrowser::mouseMoveEvent(QMouseEvent *event)
{
    QTextEdit::mouseMoveEvent(event);

    mousePressed = false;

    QTextCursor cursor = cursorForPosition(event->pos());
    qDebug() << "QQTextBrowser::mouseMoveEvent 1 " << cursor.block().text();
    QTextCharFormat format = cursor.charFormat();
    qDebug() << "type : " << format.type()
             << "ObjectType : " << format.objectType()
             << "position : " << cursor.position()
             << "positionInBlock : " << cursor.positionInBlock();
    QTextTable * table = cursor.currentTable();
    if(table != NULL)
    {
        QTextTableCell tableCell = table->cellAt(cursor);
        switch (tableCell.column())
        {
        case 0:
            break;
        case 1: qDebug() << "QQTextBrowser::mouseMoveEvent 2 "
                         << m_parent->getPostForGroup(m_groupName, tableCell.row())->norloge();
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

    if(mouseColNum == 0)
        emit norlogeClicked(mouseColNum);
    else if(mouseColNum == 1)
        emit loginClicked(mouseRowNum);
}

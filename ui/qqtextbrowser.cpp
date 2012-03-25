#include "qqtextbrowser.h"

#include "core/qqpost.h"
#include "core/qqtextcharformat.h"
#include "ui/qqmessageblockuserdata.h"

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

    QTextBlock block = cursor.block();
    QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(block.userData());
    if(blockData != NULL)
    {
        QString currBouchot = blockData->getData(QQMessageBlockUserData::BOUCHOT_NAME).toString();
        QString postNorloge = blockData->getData(QQMessageBlockUserData::POST_NORLOGE).toString();

        //Zone message
        if(blockData->constainsData(QQMessageBlockUserData::IS_MESSAGE_ZONE) &&
                blockData->constainsData(QQMessageBlockUserData::IS_MESSAGE_ZONE) == true)
        {
            //Est-on au dessus d'une norloge
            QString str = blockData->norlogeRefForIndex(cursor.positionInBlock());
            qDebug() << "QQTextBrowser::mouseMoveEvent, str = " << str;
            if(str.length() > 0)
            {
                QQNorlogeRef nRef = QQNorlogeRef(currBouchot, postNorloge, str);
                qDebug() << "QQTextBrowser::mouseMoveEvent norlogeRefHovered";
                emit norlogeRefHovered(nRef);
            }
        }
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
    mousePressed = false;

    QTextCursor cursor = cursorForPosition(event->pos());
    QTextBlock block = cursor.block();
    QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(block.userData());

    if(blockData != NULL)
    {

        if(blockData->constainsData(QQMessageBlockUserData::IS_NORLOGE_ZONE) &&
                blockData->getData(QQMessageBlockUserData::IS_NORLOGE_ZONE) == true)
        {
            QString bouchot = blockData->getData(QQMessageBlockUserData::BOUCHOT_NAME).toString();
            QString postNorloge = blockData->getData(QQMessageBlockUserData::POST_NORLOGE).toString();

            QQNorloge norloge(bouchot, postNorloge);
            emit norlogeClicked(norloge);
        }
        else if(blockData->constainsData(QQMessageBlockUserData::IS_LOGIN_UA_ZONE) &&
                blockData->getData(QQMessageBlockUserData::IS_LOGIN_UA_ZONE) == true)
            emit loginClicked(m_groupName);
    }
}

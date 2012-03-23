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

    QTextBlock block = cursor.block();
    QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(block.userData());
    if(blockData != NULL)
    {
        m_message = block.text();
        m_currBouchot = blockData->getData(QQMessageBlockUserData::BOUCHOT_NAME).toString();
    }
    else
    {
        m_message.clear();
        m_currBouchot.clear();
    }
}

void QQTextBrowser::messageColumnIsHovered(const QTextCharFormat & textHoveredFormat)
{
    switch(textHoveredFormat.objectType())
    {
    case RefNorlogeTextFormat:
        m_message = textHoveredFormat.property(NorlogeData).toString();
        m_currBouchot = textHoveredFormat.property(BouchotData).toString();
        norlogeRefIsHovered();
        break;

    default:
        m_message.clear();
        m_currBouchot.clear();
        break;
    }
}

void QQTextBrowser::norlogeRefIsHovered()
{
    QQNorloge norloge(m_currBouchot, m_message);
    qDebug() << "Norloge : " << m_message << " highlighted";
    emit norlogeRefHovered(norloge);
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
            QString bouchot = m_currBouchot = blockData->getData(QQMessageBlockUserData::BOUCHOT_NAME).toString();
            QString postNorloge = m_message = blockData->getData(QQMessageBlockUserData::POST_NORLOGE).toString();

            QQNorloge norloge(bouchot, postNorloge);
            emit norlogeClicked(norloge);
        }
        else if(blockData->constainsData(QQMessageBlockUserData::IS_LOGIN_UA_ZONE) &&
                blockData->getData(QQMessageBlockUserData::IS_LOGIN_UA_ZONE) == true)
            emit loginClicked(m_groupName);
    }
}

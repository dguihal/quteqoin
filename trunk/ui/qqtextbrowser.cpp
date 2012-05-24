#include "qqtextbrowser.h"

#include "core/qqpost.h"
#include "ui/qqmessageblockuserdata.h"

#include <QDebug>
#include <QMouseEvent>
#include <QTextBlock>
#include <QTextTable>
#include <QTextTableCell>

QQTextBrowser::QQTextBrowser(QString groupName, QQPinipede *parent) :
    QTextEdit(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setReadOnly(true);
    m_groupName = groupName;
    m_parent = parent;
    m_highlightAsked = false;
    this->setMouseTracking(true);
}

QQTextBrowser::~QQTextBrowser()
{
}

void QQTextBrowser::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "####################################";
    qDebug() << "QQTextBrowser::mouseMoveEvent x=" << event->x() << ",y=" << event->y();
    qDebug() << "####################################";
    QTextEdit::mouseMoveEvent(event);

    bool highlightActivated = false;

    mousePressed = false;

    QTextCursor cursor = cursorForPosition(event->pos());

    QTextBlock block = cursor.block();
    QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(block.userData());

    if(blockData != NULL)
    {
        /*
        QString currBouchot = blockData->getData(QQMessageBlockUserData::BOUCHOT_NAME).toString();
        QString postNorloge = blockData->getData(QQMessageBlockUserData::POST_NORLOGE).toString();
*/
        //Zone message
        if(blockData->constainsData(QQMessageBlockUserData::IS_MESSAGE_ZONE) &&
                blockData->constainsData(QQMessageBlockUserData::IS_MESSAGE_ZONE) == true)
        {
            //Est-on au dessus d'une norloge
            QQNorlogeRef nRef = blockData->norlogeRefForIndex(cursor.positionInBlock());
            qDebug() << "QQTextBrowser::mouseMoveEvent, str = " << nRef.getOrigNRef() << " position : " << nRef.getPosInMessage();
            if(nRef.getOrigNRef().length() > 0)
            {
                if(m_highlightedBlockUserData != blockData || m_highlightedNRef != nRef)
                {
                    m_highlightedBlockUserData = blockData;
                    m_highlightedNRef = nRef;
                    qDebug() << "QQTextBrowser::mouseMoveEvent norlogeRefHovered";
                    emit unHighlight();
                    emit norlogeRefHovered(nRef);
                }
                highlightActivated = true;
            }
        }
    }

	qDebug() << "highlightActivated = " << highlightActivated;
	if(m_highlightedBlockUserData != NULL && highlightActivated == false)
	{
		m_highlightedBlockUserData = NULL;
		m_highlightedNRef = QQNorlogeRef();
		emit unHighlight();
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

void QQTextBrowser::paintEvent( QPaintEvent * event )
{
    QTextEdit::paintEvent(event);

}

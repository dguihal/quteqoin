#include "qqtextbrowser.h"

#include "core/qqbouchot.h"
#include "core/qqpost.h"
#include "core/qqtotoz.h"
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
	//qDebug() << "####################################";
	//qDebug() << "QQTextBrowser::mouseMoveEvent x=" << event->x() << ",y=" << event->y();
	//qDebug() << "####################################";
	QTextEdit::mouseMoveEvent(event);

	mousePressed = false;

	QTextCursor cursor = cursorForPosition(event->pos());

	QTextBlock block = cursor.block();
	QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(block.userData());

	if(blockData != NULL)
	{
		//Zone message
		if(blockData->blockZone() == QQMessageBlockUserData::MESSAGE_ZONE)
		{
			//Est-on au dessus d'une norloge
			QQNorlogeRef nRef = blockData->norlogeRefForIndex(cursor.positionInBlock());
			if( nRef.isValid() )
			{
				qDebug() << "QQTextBrowser::mouseMoveEvent, Norloge detectee, str = " << nRef.getOrigNRef() << " position : " << nRef.getPosInMessage();
				highlightNorloge(nRef);
			}
			else
			{
				//qDebug() << "QQTextBrowser::mouseMoveEvent, Pas de Norloge detectee";
				// Il faut unhilighter puisqu'on ne survole pas de norloge
				unHighlightNorloge();

			}

			//Gestion des Totoz
			QQTotoz totoz = blockData->totozForIndex(cursor.positionInBlock());
			if( totoz.isValid() )
			{
				qDebug() << "QQTextBrowser::mouseMoveEvent, Totoz detecte, str = " << totoz.getId() << " position : " << totoz.getPosInMessage();
			}
			else
			{
				//il faut cacher l'affichage du Totoz puisqu'on n'en survole pas
			}

		}
		else if(blockData->blockZone() == QQMessageBlockUserData::NORLOGE_ZONE)
		{
			QPointer<QQPost> post = blockData->post();
			Q_ASSERT(!post.isNull());
			QString norlogeString = post->norloge();
			QQNorlogeRef nRef = QQNorlogeRef(post->bouchot()->name(),
											 norlogeString, norlogeString, 1);
			highlightNorloge(nRef);
		}
		else
		{
			// Il faut unhilighter puisqu'on ne survole pas de zone de norloge ni de zone de message
			unHighlightNorloge();
		}
	}
}

void QQTextBrowser::highlightNorloge(QQNorlogeRef nRef)
{
	qDebug() << "nRef.getOrigNRef() = " << nRef.getOrigNRef();
	if( nRef.isValid() == 0 )
		return ;

	if(nRef.isValid() && m_highlightedNRef != nRef)
	{
		qDebug() << "QQTextBrowser::mouseMoveEvent norlogeRefHovered";
		unHighlightNorloge();
		m_highlightedNRef = nRef;
		emit norlogeRefHovered(nRef);
	}
}

void QQTextBrowser::unHighlightNorloge()
{
	if(m_highlightedNRef.isValid())
	{
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
		if(blockData->blockZone() == QQMessageBlockUserData::NORLOGE_ZONE)
		{
			Q_ASSERT(blockData->post() != NULL);

			QQNorloge norloge(blockData->post()->bouchot()->name(),
							  blockData->post()->norloge());
			emit norlogeClicked(norloge);
		}
		else if(blockData->blockZone() == QQMessageBlockUserData::LOGIN_UA_ZONE)
			emit loginClicked(m_groupName);
	}
}

void QQTextBrowser::paintEvent( QPaintEvent * event )
{
	QTextEdit::paintEvent(event);

}

#include "qqtextbrowser.h"

#include "core/qqbouchot.h"
#include "core/qqpost.h"
#include "ui/qqmessageblockuserdata.h"

#include <QCursor>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextTable>
#include <QTextTableCell>
#include <QToolTip>

#define TAB_POS_IN_PX 70

QQTextBrowser::QQTextBrowser(QString groupName, QQPinipede *parent) :
	QTextEdit(parent)
{
	setFrameStyle(QFrame::NoFrame);
	setReadOnly(true);
	m_groupName = groupName;
	m_highlightAsked = false;
	m_mouseClick = false;

	QTextDocument * doc = document();
	doc->setUndoRedoEnabled(false);
	doc->setDocumentMargin(0);

	QTextOption opt = doc->defaultTextOption();
	QList<QTextOption::Tab> tabs;
	tabs << QTextOption::Tab(TAB_POS_IN_PX, QTextOption::CenterTab);
	opt.setTabs(tabs);
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	opt.setAlignment(Qt::AlignLeft);
	doc->setDefaultTextOption(opt);

	this->setMouseTracking(true);
	viewport()->setCursor(Qt::ArrowCursor);

	verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );
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

	if( anchorAt(event->pos()).length() > 0 )
		viewport()->setCursor(Qt::PointingHandCursor);
	else if (! m_mouseClick)
		viewport()->setCursor(Qt::ArrowCursor);

	QTextCursor cursor = cursorForPosition(event->pos());

	QTextBlock block = cursor.block();
	QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(block.userData());

	if(blockData != NULL)
	{
		//Zone message
		if(blockData->isIndexInZRange(cursor.positionInBlock(),
									  QQMessageBlockUserData::MESSAGE))
		{
			QToolTip::hideText();
			//Est-on au dessus d'une norloge
			QQNorlogeRef nRef = blockData->norlogeRefForIndex(cursor.positionInBlock());
			if( nRef.isValid() )
			{
				qDebug() << "QQTextBrowser::mouseMoveEvent, Norloge detectee, str = "
						 << nRef.getOrigNRef() << " position : " << nRef.getPosInMessage();
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
				qDebug() << "QQTextBrowser::mouseMoveEvent, Totoz detecte, str = " << totoz.getId()
						 << " position : " << totoz.getPosInMessage();
				showTotoz(totoz);
			}
			else
			{
				//il faut cacher l'affichage du Totoz puisqu'on n'en survole pas
				hideTotoz();
			}

		}
		else if(blockData->isIndexInZRange(cursor.positionInBlock(),
										   QQMessageBlockUserData::NORLOGE))
		{
			QToolTip::hideText();
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

void QQTextBrowser::showTotoz(QQTotoz & totoz)
{
	if(totoz.isValid() && totoz.getId() != m_displayedTotozId)
	{
		hideTotoz();
		m_displayedTotozId = totoz.getId();
		emit showTotozSig(totoz);
	}
}

void QQTextBrowser::hideTotoz()
{
	if(m_displayedTotozId.length() > 0)
	{
		m_displayedTotozId.clear();
		emit hideTotozSig();
	}
}

/*
 * Recuperation de d'un evenement lorsque la souris quitte la
 *surface de l'afficheur.
 */
void QQTextBrowser::leaveEvent(QEvent * event)
{
	QTextEdit::leaveEvent(event);

	// On masque les élements d'affichage dynamiques
	unHighlightNorloge();
	QToolTip::hideText();
	hideTotoz();
}

void QQTextBrowser::mousePressEvent(QMouseEvent * event)
{
	QTextEdit::mousePressEvent(event);

	// Stockage de la postion
	m_lastPoint = event->pos();
	// positionnement du flag de detection de debut du clic
	m_mouseClick = true;

	if(event->button() == Qt::LeftButton)
		viewport()->setCursor(Qt::IBeamCursor);
}

void QQTextBrowser::mouseReleaseEvent(QMouseEvent * event)
{
	QTextEdit::mouseReleaseEvent(event);

	viewport()->setCursor(Qt::ArrowCursor);
	// Verification que l'on est pas en pleine selection
	if((m_mouseClick == false) || (event->pos() != m_lastPoint))
		return;

	m_mouseClick = false;

	// Reset de l'icone de l'application
	QIcon icon = QIcon(QString::fromAscii(":/img/rubber_duck_yellow.svg"));
	window()->setWindowIcon(icon);

	QString httpAnchor = anchorAt(event->pos());
	if( httpAnchor.length() > 0 )
	{
		QDesktopServices::openUrl(httpAnchor);
		return;
	}

	QTextCursor cursor = cursorForPosition(event->pos());
	QTextBlock block = cursor.block();
	QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(block.userData());

	if(blockData != NULL)
	{
		if(blockData->isIndexInZRange(cursor.positionInBlock(),
									  QQMessageBlockUserData::NORLOGE))
		{
			Q_ASSERT(blockData->post() != NULL);

			QQNorloge norloge(blockData->post()->bouchot()->name(),
							  blockData->post()->norloge());
			emit norlogeClicked(norloge);
		}
		else if(blockData->isIndexInZRange(cursor.positionInBlock(),
										   QQMessageBlockUserData::LOGINUA))
			emit loginClicked(m_groupName);
	}
}

void QQTextBrowser::paintEvent(QPaintEvent * event)
{
	QTextEdit::paintEvent(event);

	QPainter pd(viewport());
	QColor colorVeryLight(80, 0, 0, 80);
	QColor colorLight(90, 0, 0, 90);
	QColor colorDark(150, 0, 0);
	pd.setPen(QPen(colorDark));

	// Recuperation du premier bloc affiche
	QTextBlock bloc = cursorForPosition(
						  event->rect().topLeft()
						  ).block();

	// Recuperation du dernier bloc affiche
	QTextBlock blocFin = cursorForPosition(
							 event->rect().bottomRight()
							 ).block();
	while(bloc.isValid() && bloc.blockNumber() <= blocFin.blockNumber())
	{
		QQMessageBlockUserData * uData = (QQMessageBlockUserData * ) bloc.userData();
		if(uData != NULL)
		{

			QList<QQBigornoItem> bigItems = uData->bigornoItems();
			if(! bigItems.isEmpty())
			{
				QListIterator<QQBigornoItem> i(bigItems);
				while(i.hasNext())
				{
					QQBigornoItem bigItem = i.next();
					QTextCursor cursor = QTextCursor(bloc);
					cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, bigItem.position());
					QRect rect = cursorRect(cursor);
					rect.setWidth(pd.fontMetrics().boundingRect(bigItem.word()).width());
					rect.adjust(-2, 0, +2, 0);
					if(event->region().boundingRect().contains(rect))
					{
						qDebug() << bigItem.word() << " : " << rect;

						QLinearGradient linearGrad(0, 0, 0, rect.height());
						linearGrad.setColorAt(0.0, colorVeryLight);
						linearGrad.setColorAt(1.0, colorLight);
						QBrush br(linearGrad);
						pd.setBrush(br);
						pd.drawRoundedRect(rect, 4.0, 2.0);
					}
				}
			}
		}
		bloc = bloc.next();
	}

}

void QQTextBrowser::resizeEvent(QResizeEvent * event)
{
	QScrollBar * vScrollBar = verticalScrollBar();
	bool isMax = (vScrollBar->sliderPosition() == vScrollBar->maximum());
	QTextEdit::resizeEvent(event);
	if(isMax)
		vScrollBar->triggerAction( QAbstractSlider::SliderToMaximum );
}

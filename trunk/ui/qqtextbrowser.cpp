#include "qqtextbrowser.h"

#include "core/qqbouchot.h"
#include "core/qqpost.h"
#include "ui/qqmessageblockuserdata.h"

#include <QtDebug>
#include <QApplication>
#include <QCursor>
#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextTable>
#include <QTextTableCell>
#include <QToolTip>

#define TAB_POS_IN_PX 70
#define NOTIF_AREA_WIDTH 20

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
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	opt.setAlignment(Qt::AlignLeft);

	QList<QTextOption::Tab> listTabs;
	m_timeUAAreaWidthPx = QFontMetrics(document()->defaultFont()).averageCharWidth() * (10 + 1 + 16); // Time + space + login/ua
	int tabPosPx = QFontMetrics(font()).averageCharWidth() * (10 + 1 + 16 + 1); // Time + space + login/ua + margin
	QTextOption::Tab tab(tabPosPx, QTextOption::DelimiterTab, '\t');
	listTabs << tab;
	
	opt.setTabs(listTabs);
	doc->setDefaultTextOption(opt);

	this->setMouseTracking(true);
	viewport()->setCursor(Qt::ArrowCursor);
	setViewportMargins(notifAreaWidth(), 0, 0, 0);

	m_notifArea = new QQNotifArea(this);
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateNotifArea(int)));

	verticalScrollBar()->setInvertedControls(true);
	verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );
}

QQTextBrowser::~QQTextBrowser()
{
}

int QQTextBrowser::notifAreaWidth()
{
	return NOTIF_AREA_WIDTH;
}

void QQTextBrowser::notifAreaPaintEvent(QPaintEvent * event)
{
	QPainter painter(m_notifArea);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// Pour les nouveaux posts
	QColor newPostsBrushColor(0, 255, 0, 100);
	QColor newPostsPenColor(0, 100, 0, 100);

	// Pour les propres posts
	QColor selfPostsBrushColor(0, 0, 255, 100);
	QColor selfPostsPenColor(0, 0, 100, 100);

	// Pour les reponses
	QColor repPostsBrushColor(127, 0, 127, 100);
	QColor repPostsPenColor(50, 0, 50, 100);

	// Pour le bigorno
	QColor bigornoBrushColor(255, 0, 0, 100);
	QColor  bigornoPenColor(100, 0, 0);

	// Recuperation du premier bloc a dessiner
	QTextBlock block = cursorForPosition(
						   event->rect().topLeft()
						   ).block();

	// Recuperation du dernier bloc a dessiner
	QTextBlock lastBlock = cursorForPosition(
							   event->rect().bottomRight()
							   ).block();
	while(block.isValid() && block.blockNumber() <= lastBlock.blockNumber())
	{
		QQMessageBlockUserData * uData = (QQMessageBlockUserData *) block.userData();
		int offset = 0;
		int width = notifAreaWidth() / 3;
		if(uData != NULL)
		{
			QTextCursor curs(block);
			qreal height = block.layout()->boundingRect().height();
			int posY = cursorRect(curs).y();
			///////////////////////////////////////////////////////////////////
			/////        LES NOUVEAUX POSTS                   /////////////////
			///////////////////////////////////////////////////////////////////
			if(uData->isNew())
			{
				painter.setBrush(newPostsBrushColor);
				painter.setPen(QPen(QBrush(newPostsPenColor), 0.6));
				QRect drawRect(offset, posY, width, height);
				painter.drawRect(drawRect);
			}
			offset += width;

			///////////////////////////////////////////////////////////////////
			/////        LE TRACKING DES POSTS                /////////////////
			///////////////////////////////////////////////////////////////////
			QQPost * post = uData->post();
			if(post->isSelfPost())
			{
				painter.setBrush(selfPostsBrushColor);
				painter.setPen(QPen(QBrush(selfPostsPenColor), 0.6));
				QRect drawRect(offset, posY, width, height);
				painter.drawRect(drawRect);
			}
			else if(uData->hasNRefToSelfPost())
			{
				painter.setBrush(repPostsBrushColor);
				painter.setPen(QPen(QBrush(repPostsPenColor), 0.6));
				QRect drawRect(offset, posY, width, height);
				painter.drawRect(drawRect);
			}
			offset += width;

			///////////////////////////////////////////////////////////////////
			/////        LE BIGONO (ENCORE)                  /////////////////
			///////////////////////////////////////////////////////////////////
			QList<QQBigornoItem> bigItems = uData->bigornoItems();
			if(! bigItems.isEmpty())
			{
				painter.setBrush(QBrush(bigornoBrushColor));
				painter.setPen(QPen(QBrush(bigornoPenColor), 0.6));
				QRect drawRect(offset, posY, width, height);
				painter.drawRect(drawRect);
			}
		}
		block = block.next();
	}
}


void QQTextBrowser::updateNotifArea(int)
{
	m_notifArea->update();
}

void QQTextBrowser::mouseMoveEvent(QMouseEvent * event)
{
	//qDebug() << "####################################";
	//qDebug() << "QQTextBrowser::mouseMoveEvent x=" << event->x() << ",y=" << event->y();
	//qDebug() << "####################################";
	QTextEdit::mouseMoveEvent(event);

	if(anchorAt(event->pos()).length() > 0)
		viewport()->setCursor(Qt::PointingHandCursor);
	else if (! m_mouseClick)
		viewport()->setCursor(Qt::ArrowCursor);

	//Est-on au dessus d'une url
	// Ouverture l'url si on est au dessus d'un lien
	QString httpAnchor = anchorAt(event->pos());
	if(httpAnchor.length() > 0)
	{
		QFontMetrics fm(QToolTip::font());
		QToolTip::showText(event->globalPos(), fm.elidedText(httpAnchor, Qt::ElideRight, 400), this);
		return;
	}

	QTextCursor cursor = cursorForPosition(event->pos());

	QTextBlock block = cursor.block();
	QQMessageBlockUserData * blockData = (QQMessageBlockUserData *) (block.userData());

	if(blockData != NULL)
	{
		//Zone message
		if(blockData->isIndexInZRange(cursor.positionInBlock(),
									  QQMessageBlockUserData::MESSAGE))
		{
			QToolTip::hideText();
			//Est-on au dessus d'une norloge
			QQNorlogeRef nRef = blockData->norlogeRefForIndex(cursor.positionInBlock());
			if(nRef.isValid())
			{
				//qDebug() << "QQTextBrowser::mouseMoveEvent, Norloge detectee, str = "
				//		 << nRef.getOrigNRef() << " position : " << nRef.getPosInMessage();
				highlightNorloge(nRef);
			}
			else
			{
				//qDebug() << "QQTextBrowser::mouseMoveEvent, Pas de Norloge detectee";
				// Il faut unhilighter puisqu'on ne survole pas de norloge
				unHighlightNorloge();

			}

			//Gestion des Totoz
			QString totozId = blockData->totozIdForIndex(cursor.positionInBlock());
			if(totozId.length() > 0)
			{
				//qDebug() << "QQTextBrowser::mouseMoveEvent, Totoz detecte, str = " << totozId;
				showTotoz(totozId);
			}
			else //il faut cacher l'affichage du Totoz puisqu'on n'en survole pas
				hideTotoz();

		}
		else if(blockData->isIndexInZRange(cursor.positionInBlock(),
										   QQMessageBlockUserData::NORLOGE))
		{
			QToolTip::hideText();
			QPointer<QQPost> post = blockData->post();
			Q_ASSERT(!post.isNull());
			QString norlogeString = post->norloge();
			QQNorlogeRef nRef = QQNorlogeRef(post->bouchot()->name(),
											 norlogeString, norlogeString);
			nRef.setNorlogeIndex(post->norlogeIndex());
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
	if(! (m_highlightedNRef == nRef))
	{
		qDebug() << "QQTextBrowser::mouseMoveEvent norlogeRefHovered : nRef.getOrigNRef() = " << nRef.getOrigNRef();
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

void QQTextBrowser::showTotoz(QString & totozId)
{
	if(totozId != m_displayedTotozId)
	{
		m_displayedTotozId = totozId;
		emit displayTotoz(totozId);
	}
}

void QQTextBrowser::hideTotoz()
{
	if(m_displayedTotozId.length() > 0)
	{
		m_displayedTotozId.clear();
		emit concealTotoz();
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
	// Stockage de la postion
	m_lastPoint = event->pos();
	// positionnement du flag de detection de debut du clic
	m_mouseClick = true;

	QTextEdit::mousePressEvent(event);

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

	// Marquage des posts comme lus
	QTextBlock block = document()->firstBlock();
	QQMessageBlockUserData * blockData = NULL;
	bool needUpdate = false;
	while(block.isValid())
	{
		blockData = (QQMessageBlockUserData *) block.userData();
		if(blockData != NULL && blockData->isNew())
		{
			needUpdate = true;
			blockData->setAcknowledged();
		}

		block = block.next();
	}

	if(needUpdate)
		m_notifArea->update();

	// Ouverture l'url si on est au dessus d'un lien
	QString httpAnchor = anchorAt(event->pos());
	if( httpAnchor.length() > 0 )
	{
		QDesktopServices::openUrl(QUrl::fromPercentEncoding(httpAnchor.toAscii()));
		return;
	}

	// Gestion du clic sur une norloge ou un login
	QTextCursor cursor = cursorForPosition(event->pos());
	block = cursor.block();
	blockData = (QQMessageBlockUserData *) block.userData();

	if(blockData != NULL)
	{
		if(blockData->isIndexInZRange(cursor.positionInBlock(),
									  QQMessageBlockUserData::NORLOGE))
		{
			Q_ASSERT(blockData->post() != NULL);

			QQPost * post = blockData->post();
			QQNorloge norloge(post->bouchot()->name(),
							  post->norloge());
			if(post->isNorlogeMultiple())
				norloge.setNorlogeIndex(post->norlogeIndex());
			emit norlogeClicked(norloge);
		}
		else if(blockData->isIndexInZRange(cursor.positionInBlock(),
										   QQMessageBlockUserData::LOGINUA))
		{
			QString login = blockData->post()->login();
			if(login.size() == 0)
				login = blockData->post()->UA();
			emit loginClicked(login);
		}
	}
}

void QQTextBrowser::paintEvent(QPaintEvent * event)
{
	QTextEdit::paintEvent(event);

	// Pour le bigorno
	QPainter bigornoPainter(viewport());
	bigornoPainter.setRenderHint(QPainter::Antialiasing, true);
	QColor bigornoBrushColor(60, 0, 0, 100);
	bigornoPainter.setBrush(QBrush(bigornoBrushColor));
	QColor bigornoPenColor(150, 0, 0);
	bigornoPainter.setPen(QPen(QBrush(bigornoPenColor), 0.6));

	// Recuperation du premier bloc a dessiner
	QTextBlock block = cursorForPosition(
						   event->rect().topLeft()
						   ).block();

	// Recuperation du dernier bloc a dessiner
	QTextBlock lastBlock = cursorForPosition(
							   event->rect().bottomRight()
							   ).block();
	while(block.isValid() && block.blockNumber() <= lastBlock.blockNumber())
	{
		QQMessageBlockUserData * uData = (QQMessageBlockUserData *) block.userData();
		if(uData != NULL)
		{
			///////////////////////////////////////////////////////////////////
			/////        LE BIGORNO                           /////////////////
			///////////////////////////////////////////////////////////////////
			QList<QQBigornoItem> bigItems = uData->bigornoItems();
			if(! bigItems.isEmpty())
			{
				QListIterator<QQBigornoItem> i(bigItems);
				//qDebug() << "QQTextBrowser::paintEvent, highlighting bloc num : " << bloc.blockNumber() << ", nb items : " << bigItems.size();
				while(i.hasNext())
				{
					QQBigornoItem bigItem = i.next();
					QTextCursor cursor = QTextCursor(block);
					cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, bigItem.position());
					QRect rect = cursorRect(cursor);
					rect.setWidth(bigornoPainter.fontMetrics().boundingRect(bigItem.word()).width());
					rect.adjust(-2, 0, +2, 0);

					//qDebug() << "QQTextBrowser::paintEvent, highlighting : " << bigItem.word() << ", rect = : " << rect;

					bigornoPainter.drawRoundedRect(rect, 3.0, 3.0);
				}
			}
		}
		block = block.next();
	}
	m_notifArea->update();
}

void QQTextBrowser::resizeEvent(QResizeEvent * event)
{
	QScrollBar * vScrollBar = verticalScrollBar();
	bool isMax = (vScrollBar->sliderPosition() == vScrollBar->maximum());
	QTextEdit::resizeEvent(event);
	if(isMax)
		vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

	QRect cr = contentsRect();
	m_notifArea->setGeometry(QRect(cr.left(), cr.top(), notifAreaWidth(), cr.height()));
}

void QQTextBrowser::wheelEvent(QWheelEvent * event)
{
	if(! (event->modifiers() && Qt::ControlModifier != 0))
		QTextEdit::wheelEvent(event);
}

void QQTextBrowser::contextMenuEvent(QContextMenuEvent * ev)
{
	if(m_displayedTotozId.size() > 0)
	{
		QPoint pos = ev->globalPos();
		emit displayTotozContextMenu(pos);
		ev->accept();
	}
	else
		QTextEdit::contextMenuEvent(ev);
}

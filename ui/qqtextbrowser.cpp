#include "qqtextbrowser.h"

#include "core/qqbouchot.h"
#include "core/qqpiniurlhelper.h"
#include "core/qqpost.h"
#include "core/qqsettings.h"
#include "ui/qqmessageblockuserdata.h"

#include <QtDebug>
#include <QApplication>
#include <QCursor>
#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>
#include <QNetworkReply>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCodec>
#include <QTextLayout>
#include <QTextTable>
#include <QTextTableCell>
#include <QToolTip>

#define TIME_UA_AREA_WIDTH_CHAR 26 // 10 + 1 + 15 Chars
#define NOTIF_AREA_WIDTH 18 //Px
#define ITEM_AREA_WIDTH 6 //Px

QQTextBrowser::QQTextBrowser(QString groupName, QQPinipede *parent) :
	QTextBrowser(parent)
{
	QQSettings settings;

	setFrameStyle(QFrame::NoFrame);
	setReadOnly(true);
	setOpenExternalLinks(true);

	m_groupName = groupName;
	m_highlightedNorlogeRef.clear();
	m_mouseClick = false;
	m_urlHelper = new QQPiniUrlHelper(this);
	connect(m_urlHelper, SIGNAL(contentTypeAvailable(QUrl&,QString&)), this, SLOT(handleContentTypeAvailable(QUrl&,QString&)));

	QTextDocument * doc = document();
	doc->setUndoRedoEnabled(false);
	doc->setDocumentMargin(0);
	QFont docFont;
	docFont.fromString(settings.value(SETTINGS_GENERAL_DEFAULT_FONT, DEFAULT_GENERAL_DEFAULT_FONT).toString());
	doc->setDefaultFont(docFont);

	QTextOption opt = doc->defaultTextOption();
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	opt.setAlignment(Qt::AlignLeft);

	QList<QTextOption::Tab> listTabs;
	qreal baseWidth = QFontMetricsF(docFont).averageCharWidth();
	m_timeUAAreaWidthPx = baseWidth * TIME_UA_AREA_WIDTH_CHAR;
	qreal tabPosPx = baseWidth * (TIME_UA_AREA_WIDTH_CHAR + 1); // + margin

#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
	QTextOption::Tab tab(tabPosPx, QTextOption::DelimiterTab, '\t');
#else
	QTextOption::Tab tab;
	tab.position  = tabPosPx;
	tab.type      = QTextOption::DelimiterTab;
	tab.delimiter =  '\t';
#endif
	listTabs << tab;

	opt.setTabs(listTabs);
	doc->setDefaultTextOption(opt);

	this->setMouseTracking(true);
	viewport()->setCursor(Qt::ArrowCursor);
	setViewportMargins(notifAreaWidth(), 0, 0, 0);

	m_notifArea = new QQNotifArea(this);
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateNotifArea(int)));

	verticalScrollBar()->setInvertedControls(true);
	verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);

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
	painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

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
		if(uData != NULL && uData->post() != NULL)
		{
			QTextCursor curs(block);
			qreal height = block.layout()->boundingRect().height();
			int posY = cursorRect(curs).y();
			QQPost * post = uData->post();

			///////////////////////////////////////////////////////////////////
			/////        BACKGROUND                           /////////////////
			///////////////////////////////////////////////////////////////////
			painter.setBrush(post->bouchot()->settings().color());
			painter.setPen(QPen(QBrush(post->bouchot()->settings().color()), 0.6));
			QRect drawRect(0, posY, notifAreaWidth(), height);
			painter.drawRect(drawRect);

			///////////////////////////////////////////////////////////////////
			/////        LES NOUVEAUX POSTS                   /////////////////
			///////////////////////////////////////////////////////////////////
			if(uData->isNew())
			{
				painter.setBrush(newPostsBrushColor);
				painter.setPen(QPen(QBrush(newPostsPenColor), 0.6));
				QRect drawRect(0, posY, ITEM_AREA_WIDTH, height);
				painter.drawRect(drawRect);
			}
			offset += ITEM_AREA_WIDTH;

			///////////////////////////////////////////////////////////////////
			/////        LE TRACKING DES POSTS                /////////////////
			///////////////////////////////////////////////////////////////////
			if(post->isSelfPost())
			{
				painter.setBrush(selfPostsBrushColor);
				painter.setPen(QPen(QBrush(selfPostsPenColor), 0.6));
				QRect drawRect(offset, posY, ITEM_AREA_WIDTH, height);
				painter.drawRect(drawRect);
			}
			else if(uData->hasNRefToSelfPost())
			{
				painter.setBrush(repPostsBrushColor);
				painter.setPen(QPen(QBrush(repPostsPenColor), 0.6));
				QRect drawRect(offset, posY, ITEM_AREA_WIDTH, height);
				painter.drawRect(drawRect);
			}
			offset += ITEM_AREA_WIDTH;

			///////////////////////////////////////////////////////////////////
			/////        LE BIGONO (ENCORE)                  /////////////////
			///////////////////////////////////////////////////////////////////
			QList<QQBigornoItem> bigItems = uData->bigornoItems();
			if(! bigItems.isEmpty())
			{
				painter.setBrush(QBrush(bigornoBrushColor));
				painter.setPen(QPen(QBrush(bigornoPenColor), 0.6));
				QRect drawRect(offset, posY, ITEM_AREA_WIDTH, height);
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

void QQTextBrowser::highlightNorloge(QQNorlogeRef nRef)
{
	if(nRef.toString() != m_highlightedNorlogeRef)
	{
		unHighlightNorloge();
		emit norlogeRefHovered(nRef);
		m_highlightedNorlogeRef = nRef.toString();
	}
}

void QQTextBrowser::unHighlightNorloge()
{
	if(! m_highlightedNorlogeRef.isEmpty())
	{
		emit unHighlight(this);
		m_highlightedNorlogeRef.clear();
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

void QQTextBrowser::handleContentTypeAvailable(QUrl &url, QString &contentType)
{
	if(QToolTip::isVisible())
	{
		QString ttText = QToolTip::text();
		QString compUrl;

		int indexElide = ttText.indexOf(QString::fromUtf8("\u2026")); //…
		if(indexElide >= 0)
			compUrl = ttText.left(indexElide);
		else
			compUrl = ttText;

		if(url.toString().startsWith(compUrl))
		{
			ttText.append(" (").append(contentType).append(")");
			QToolTip::showText(QCursor::pos(), ttText, this);
		}
	}
}

void QQTextBrowser::webSearchActionTriggered()
{
	QQSettings settings;
	QString webSearchUrl = settings.value(SETTINGS_GENERAL_WEBSEARCH_URL, DEFAULT_GENERAL_WEBSEARCH_URL).toString();
	QTextCursor cursor = textCursor();
	QString selection = cursor.selectedText();

	if(! selection.isEmpty())
	{
		webSearchUrl.replace("%s", QUrl::toPercentEncoding(selection));
		QDesktopServices::openUrl(QUrl::fromEncoded(webSearchUrl.toLatin1()));
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
	QTextBrowser::leaveEvent(event);

	// On masque les élements d'affichage dynamiques
	unHighlightNorloge();
	QToolTip::hideText();
	hideTotoz();
}

void QQTextBrowser::mouseMoveEvent(QMouseEvent * event)
{
	//qDebug() << "####################################";
	//qDebug() << "QQTextBrowser::mouseMoveEvent x=" << event->x() << ",y=" << event->y();
	//qDebug() << "####################################";
	QTextBrowser::mouseMoveEvent(event);

	QString httpAnchor = anchorAt(event->pos());
	if(! m_mouseClick)
	{
		QCursor cursor(Qt::ArrowCursor);
		if(httpAnchor.length() > 0)
			cursor.setShape(Qt::PointingHandCursor);
		viewport()->setCursor(cursor);
	}

	QTextCursor cursor = cursorForPosition(event->pos());

	QTextBlock block = cursor.block();
	QQMessageBlockUserData * blockData = (QQMessageBlockUserData *) (block.userData());

	if(blockData != NULL)
	{
		//Zone message
#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
		int posInBlock =  cursor.positionInBlock();
#else
		int posInBlock =  cursor.position() - cursor.block().position();
#endif
		if(blockData->isIndexInZRange(posInBlock, QQMessageBlockUserData::MESSAGE))
		{
			//Est-on au dessus d'une url
			// Ouverture l'url si on est au dessus d'un lien
			if(httpAnchor.length() > 0)
			{
				if(! QToolTip::isVisible())
				{
					QFontMetrics fm(QToolTip::font());
					QToolTip::showText(event->globalPos(), fm.elidedText(httpAnchor, Qt::ElideMiddle, 500), this);

					m_urlHelper->getContentType(QUrl(httpAnchor));
				}
			}
			else
				QToolTip::hideText();

			//Est-on au dessus d'une norloge
			QQNorlogeRef nRef = blockData->norlogeRefForIndex(posInBlock);
			if(nRef.isValid())
				highlightNorloge(nRef);
			else // Il faut unhilighter puisqu'on ne survole pas de norloge
				unHighlightNorloge();

			//Gestion des Totoz
			QString totozId = blockData->totozIdForIndex(posInBlock);
			if(totozId.length() > 0)
				showTotoz(totozId);
			else //il faut cacher l'affichage du Totoz puisqu'on n'en survole pas
				hideTotoz();

		}
		else if(blockData->isIndexInZRange(posInBlock, QQMessageBlockUserData::NORLOGE))
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
			hideTotoz();
		}
	}
}

void QQTextBrowser::mousePressEvent(QMouseEvent * event)
{
	// Stockage de la postion
	m_lastPoint = event->pos();
	// positionnement du flag de detection de debut du clic
	m_mouseClick = true;

	QTextBrowser::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
		viewport()->setCursor(Qt::IBeamCursor);
}

void QQTextBrowser::mouseReleaseEvent(QMouseEvent * event)
{
	QTextBrowser::mouseReleaseEvent(event);

	m_mouseClick = false;

	QString httpAnchor = anchorAt(event->pos());
	QCursor mCursor(Qt::ArrowCursor);
	if(httpAnchor.length() > 0)
		mCursor.setShape(Qt::PointingHandCursor);
	viewport()->setCursor(mCursor);

	// Verification que l'on est pas en pleine selection
	if(event->pos() != m_lastPoint)
		return;

	// Reset de l'icone de l'application
	QIcon icon = QIcon(QString::fromLatin1(":/img/rubber_duck_yellow.svg"));
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
	{
		m_notifArea->update();
		emit newPostsAcknowledged(m_groupName);
	}

	// Gestion du clic sur une norloge ou un login
	QTextCursor cursor = cursorForPosition(event->pos());
	block = cursor.block();
	blockData = (QQMessageBlockUserData *) block.userData();

	if(blockData != NULL)
	{
		QQPost * post = blockData->post();
		Q_ASSERT(post != NULL);

		// Clic sur Norloge
		//Zone message
#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
		int posInBlock =  cursor.positionInBlock();
#else
		int posInBlock =  cursor.position() - cursor.block().position();
#endif
		bool duckLaunched = false;
		if(blockData->isIndexInZRange(posInBlock, QQMessageBlockUserData::NORLOGE))
		{
			QQNorloge norloge(post->bouchot()->name(),
							  post->norloge());
			if(post->isNorlogeMultiple())
				norloge.setNorlogeIndex(post->norlogeIndex());
			emit norlogeClicked(post->bouchot()->name(), norloge);
		}
		// Clic sur Login/UA
		else if(blockData->isIndexInZRange(posInBlock, QQMessageBlockUserData::LOGINUA))
		{
			QString login = post->login();
			if(login.size() == 0)
			{
				QString ua = post->UA();
				if(ua.size() != 0 && ! ua.contains('/', Qt::CaseSensitive))
					emit loginClicked(post->bouchot()->name(), ua);
			}
			else
				emit loginClicked(post->bouchot()->name(), login);
		}
		// Clic sur Norloge dans Message (NorlogeRef)
		else if(blockData->isIndexInZRange(posInBlock, QQMessageBlockUserData::MESSAGE))
		{
			QQNorlogeRef nRef = blockData->norlogeRefForIndex(posInBlock);
			if(nRef.isValid())
				emit norlogeRefClicked(post->bouchot()->name(), nRef);
			else
			{
				QPair<int, QString> duck = blockData->duckForIndex(posInBlock);
				if(duck.first >= 0)
				{
					duckLaunched = true;
					qDebug() << blockData->post()->bouchot()->name() << blockData->post()->id();
					emit duckClicked(blockData->post()->bouchot()->name(), blockData->post()->id());
				}
			}
		}
		if(!duckLaunched)
			emit shotDuck(event->button() == Qt::MiddleButton);
	}
}

void QQTextBrowser::paintEvent(QPaintEvent * event)
{
	QTextBrowser::paintEvent(event);

	// Pour le bigorno
	QPainter bigornoPainter(viewport());
	bigornoPainter.setRenderHint(QPainter::Antialiasing, true);
	bigornoPainter.setRenderHint(QPainter::HighQualityAntialiasing, true);
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
					rect.setWidth(QFontMetrics(document()->defaultFont()).boundingRect(bigItem.word()).width());
					rect.adjust(-1, 0, +1, 0);

					//qDebug() << "QQTextBrowser::paintEvent, highlighting : " << bigItem.word() << ", rect = : " << rect;

					bigornoPainter.drawRoundedRect(rect, 5.0, 5.0);
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
	QTextBrowser::resizeEvent(event);
	if(isMax)
		vScrollBar->triggerAction(QAbstractSlider::SliderToMaximum);

	QRect cr = contentsRect();
	m_notifArea->setGeometry(QRect(cr.left(), cr.top(), notifAreaWidth(), cr.height()));
}

void QQTextBrowser::wheelEvent(QWheelEvent * event)
{
	if(! (event->modifiers() && Qt::ControlModifier != 0))
		QTextBrowser::wheelEvent(event);
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
	{
		QPoint evPos = ev->pos();
		evPos.setX(evPos.x() + horizontalScrollBar()->value());
		evPos.setY(evPos.y() + verticalScrollBar()->value());
		QMenu *menu = createStandardContextMenu(evPos);
		QTextCursor cursor = textCursor();
		if(cursor.hasSelection())
		{
			QAction *action = menu->addAction(tr("&Search on web"));
			connect(action, SIGNAL(triggered()), this, SLOT(webSearchActionTriggered()));
		}
		menu->exec(QCursor::pos());
	}
}

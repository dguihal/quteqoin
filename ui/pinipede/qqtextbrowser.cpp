#include "qqtextbrowser.h"

#include "core/qqbouchot.h"
#include "core/qqpost.h"
#include "core/qqsettings.h"
#include "ui/qqmessageblockuserdata.h"
#include "ui/pinipede/qqpiniurlhelper.h"

#include <QtDebug>
#include <QApplication>
#include <QCursor>
#include <QFontMetrics>
#include <QLabel>
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
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif

#define TIME_UA_AREA_WIDTH_CHAR 26 // 10 + 1 + 15 Chars
#define NOTIF_AREA_WIDTH 18 //Px
#define ITEM_AREA_WIDTH 6 //Px

QQTextBrowser::QQTextBrowser(QString groupName, QQPinipede *parent) :
	QTextBrowser(parent),
	m_notifArea(new QQNotifArea(this)),
	m_urlHelper(new QQPiniUrlHelper(this)),
	m_highlightedNorlogeRef(""),
	m_displayedTotozId(""),
	m_shownUrl(),
	m_groupName(groupName)
{
	setFrameStyle(QFrame::NoFrame);
	setReadOnly(true);
	setOpenLinks(false);
	setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);

	connect(m_urlHelper, SIGNAL(contentTypeAvailable(QUrl&, QString&)), this, SLOT(onExtendedInfoAvailable(QUrl&, QString&)));
	connect(m_urlHelper, SIGNAL(videoTitleAvailable(QUrl&, QString&)), this, SLOT(onExtendedInfoAvailable(QUrl&, QString&)));
	connect(m_urlHelper, SIGNAL(mmDataAvailable(QUrl&,QString&)), this, SLOT(onThumbnailUrlAvailable(QUrl&, QString&)));
	connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));
	connect(this, SIGNAL(highlighted(QUrl)), this, SLOT(onAnchorHighlighted(QUrl)));

	QTextDocument *doc = document();
	doc->setUndoRedoEnabled(false);
	doc->setDocumentMargin(0);

	QTextOption opt = doc->defaultTextOption();
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	opt.setAlignment(Qt::AlignLeft);
	doc->setDefaultTextOption(opt);

	updateFont();

	this->setMouseTracking(true);
	setViewportMargins(notifAreaWidth(), 0, 0, 0);

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

void QQTextBrowser::updateFont()
{
	QQSettings settings;
	QFont docFont;
	docFont.fromString(settings.value(SETTINGS_GENERAL_DEFAULT_FONT, DEFAULT_GENERAL_DEFAULT_FONT).toString());

	QTextDocument *doc = document();
	doc->setDefaultFont(docFont);
	QTextOption opt = doc->defaultTextOption();

	QList<QTextOption::Tab> listTabs;
	qreal baseWidth = QFontMetricsF(docFont).averageCharWidth();
	m_timeUAAreaWidthPx = baseWidth * TIME_UA_AREA_WIDTH_CHAR;
	qreal tabPosPx = baseWidth * (TIME_UA_AREA_WIDTH_CHAR + 1); // + margin

	QTextOption::Tab tab(tabPosPx, QTextOption::DelimiterTab, '\t');
	listTabs << tab;

	opt.setTabs(listTabs);
	doc->setDefaultTextOption(opt);

	//Il faut mettre a jour les blocks existant (il y en a toujours un par defaut)
	QTextCursor c(doc); // Positionne au 1° bloc
	c.beginEditBlock();
	do {
		QTextBlockFormat bf = c.blockFormat();
		bf.setTabPositions(listTabs);
		c.setBlockFormat(bf);
	} while (c.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, 1));
	c.endEditBlock();
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
	QColor bigornoPenColor(100, 0, 0);

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
			/////        LE TRACKING DES POSTS                /////////////////
			///////////////////////////////////////////////////////////////////
			if(post->isSelfPost())
			{
				offset += ITEM_AREA_WIDTH;
				painter.setBrush(selfPostsBrushColor);
				painter.setPen(QPen(QBrush(selfPostsPenColor), 0.6));
				QRect drawRect(offset, posY, ITEM_AREA_WIDTH, height);
				painter.drawRect(drawRect);
			}
			else
			{
				if(post->isUnread())
				{
					painter.setBrush(newPostsBrushColor);
					painter.setPen(QPen(QBrush(newPostsPenColor), 0.6));
					QRect drawRect(0, posY, ITEM_AREA_WIDTH, height);
					painter.drawRect(drawRect);
				}

				offset += ITEM_AREA_WIDTH;

				if(uData->hasNRefToSelfPost())
				{
					painter.setBrush(repPostsBrushColor);
					painter.setPen(QPen(QBrush(repPostsPenColor), 0.6));
					QRect drawRect(offset, posY, ITEM_AREA_WIDTH, height);
					painter.drawRect(drawRect);
				}
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
	QString id = nRef.nRefId().append("@").append(nRef.dstBouchot());
	if(id != m_highlightedNorlogeRef)
	{
		unHighlightNorloge();
		emit norlogeRefHovered(nRef);
		m_highlightedNorlogeRef = id;
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

void QQTextBrowser::onAddTotozToBookmarksAction()
{
	qDebug() << Q_FUNC_INFO << m_contextMenuTotozId;
	emit totozBookmarkAct(m_contextMenuTotozId, QQTotoz::ADD);
	m_contextMenuTotozId.clear();
}

void QQTextBrowser::onAnchorClicked(const QUrl &link)
{
	if(link.scheme().startsWith("http"))
		QDesktopServices::openUrl(link);
	else if(link.scheme() == "nref")
	{
		bool isInt = true;
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		QUrlQuery anchorUrlQuery(link);
		int index = anchorUrlQuery.queryItemValue("index").toInt(&isInt);
#else
		int index = link.queryItemValue("index").toInt(&isInt);
#endif
		if(isInt)
		{
			QTextCursor c = cursorForPosition(mapFromGlobal(QCursor::pos()));
			QQMessageBlockUserData *d = (QQMessageBlockUserData *) c.block().userData();
			if(index == 0) // clic sur la norloge du post
			{
				emit norlogeClicked(link.host(), d->post()->norlogeObj());
			}
			else
			{
				QQNorlogeRef nRef = d->norlogeRefForIndex(index);
				if(nRef.isValid())
					emit norlogeRefClicked(link.host(), nRef);
				else
					qWarning() << Q_FUNC_INFO << "Invalid nRef returned for" << link.toString();
			}
		}
	}
	else if(link.scheme() == "msl")
	{
		QString name;
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		QUrlQuery anchorUrlQuery(link);
		name = QUrl::fromPercentEncoding(anchorUrlQuery.queryItemValue("login").toUtf8());
		if(name.size() == 0)
			name = QUrl::fromPercentEncoding(anchorUrlQuery.queryItemValue("ua").toUtf8());
#else
		name = QUrl::fromPercentEncoding(link.queryItemValue("login").toLocal8Bit());
		if(name.size() == 0)
			name = QUrl::fromPercentEncoding(link.queryItemValue("ua").toLocal8Bit());
#endif
		QString board = link.host();
		emit loginClicked(board, name);
	}
	else if(link.scheme() == "duck")
	{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		QUrlQuery anchorUrlQuery(link);
		QString postId = anchorUrlQuery.queryItemValue("postId");
		QString self = anchorUrlQuery.queryItemValue("self");
#else
		QString postId = link.queryItemValue("postId");
		QString self = link.queryItemValue("self");
#endif
		QString board = link.host();

		emit duckClicked(board, postId, (self == "1"));
	}

	QTextCursor c = textCursor();
	c.clearSelection();
	setTextCursor(c);
}

void QQTextBrowser::onAnchorHighlighted(const QUrl &link)
{
	viewport()->unsetCursor();
	if(link.isEmpty())
	{
		// Il faut unhilighter puisqu'on ne survole pas de zone de norloge ni de zone de message
		unHighlightNorloge();
		QToolTip::hideText();
		clearViewers();
	}
	else
	{
		if(link != m_shownUrl)
		{
			unHighlightNorloge();
			hideViewers();
			m_shownUrl = link;
		}
		else
			return;

		QString linkScheme = link.scheme();
		if(linkScheme == "totoz") // Un [:totoz]
		{
			QString totozId = link.path().remove(0, 1); // Le / initial
			showTotoz(totozId);
		}
		else if ((linkScheme == "msl") || // Une moule
				 (linkScheme == "duck") || // Une moule
				 (linkScheme == "tablev")) // Une table volante
		{
		}
		else if (linkScheme == "nref") // Une norloge
		{

			QTextCursor c = cursorForPosition(mapFromGlobal(QCursor::pos()));
			QQMessageBlockUserData *d = (QQMessageBlockUserData *) c.block().userData();

			bool isInt = true;
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			QUrlQuery linkQuery(link);
			int index = linkQuery.queryItemValue("index").toInt(&isInt);
#else
			int index = link.queryItemValue("index").toInt(&isInt);
#endif
			if(isInt)
			{
				QQNorlogeRef nRef = d->norlogeRefForIndex(index);
				if(nRef.isValid())
					highlightNorloge(nRef);
				else
					qWarning() << Q_FUNC_INFO << "Invalid nRef returned for" << link.toString();
			}
			else
				qWarning() << Q_FUNC_INFO << "NRef anchor badly formatted :" << link.toString();
		}
		else // if(anchorScheme == "http" || anchorScheme == "https") Une [url]
		{
			viewport()->setCursor(Qt::PointingHandCursor);

			QFontMetrics fm(QToolTip::font());
			QToolTip::showText(QCursor::pos(), fm.elidedText(link.toString(), Qt::ElideMiddle, 500), this);

			m_urlHelper->getUrlInfo(m_shownUrl);
		}
	}
}

void QQTextBrowser::onExtendedInfoAvailable(QUrl &url, QString &contentType)
{
	QString ttText = QToolTip::text();
	if(ttText.length() > 0 && url == m_shownUrl)
	{
		ttText.append(" (").append(contentType).append(")");
		QToolTip::showText(QCursor::pos(), ttText, this);
	}
}

void QQTextBrowser::onThumbnailUrlAvailable(QUrl &url, QString &contentType)
{
	Q_UNUSED(url)
	if(url.isValid())
		emit displayMmdaData(url, contentType);

}

void QQTextBrowser::onWebSearchAction()
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

void QQTextBrowser::clearViewers()
{
	m_urlHelper->cancel();
	if(m_displayedTotozId.length() > 0)
		emit hideViewers();

	if(QToolTip::isVisible())
		emit hideViewers();

	m_shownUrl.clear();
	m_displayedTotozId.clear();
}

void QQTextBrowser::contextMenuEvent(QContextMenuEvent * ev)
{
	QPoint evPos = ev->pos();
	evPos.setX(evPos.x() + horizontalScrollBar()->value());
	evPos.setY(evPos.y() + verticalScrollBar()->value());
	QMenu *menu = createStandardContextMenu(evPos);
	QTextCursor cursor = textCursor();

	if(cursor.hasSelection())
	{
		QAction *action = menu->addAction(tr("&Search on web"));
		connect(action, SIGNAL(triggered()), this, SLOT(onWebSearchAction()));
	}

	QString anchor = anchorAt(ev->pos());
	if(anchor.size() > 0)
	{
		QUrl anchorUrl(anchor);
		QString anchorUrlScheme = anchorUrl.scheme();
		if(anchorUrlScheme == "totoz") // Un [:totoz]
		{
			m_contextMenuTotozId = anchorUrl.path().remove(0, 1); // Le / initial

			QAction *action = menu->addAction(tr("Add to &bookmarks"));
			connect(action, SIGNAL(triggered()), this, SLOT(onAddTotozToBookmarksAction()));

			//Suppression du Copy Link Location
			menu->actions().at(1)->setEnabled(false);
		}
		else if ((anchorUrlScheme == "msl")    || // Une moule
				 (anchorUrlScheme == "duck")   || // Un canard
				 (anchorUrlScheme == "tablev") || // Une table volante
				 (anchorUrlScheme == "nref"))     // Une norloge
		{
			//Suppression du Copy Link Location
			menu->actions().at(1)->setEnabled(false);
		}
	}

	menu->exec(QCursor::pos());
}


//////////////////////////////////////////////////////////////
/// \brief enterEvent
/// \param event
///
void QQTextBrowser::enterEvent(QEvent *event)
{
	QTextBrowser::enterEvent(event);

	//Contournement : Si le cursor sort de la fenêtre puis revient, il
	// ne renvoie pas le signal du AnchorHovered si on repasse sur le
	// meme lien
	onAnchorHighlighted(anchorAt(mapFromGlobal(QCursor::pos())));
}


//////////////////////////////////////////////////////////////
/// \brief QQTextBrowser::leaveEvent
/// \param event
///
void QQTextBrowser::leaveEvent(QEvent *event)
{
	QTextBrowser::leaveEvent(event);

	viewport()->unsetCursor();
	// On masque les élements d'affichage dynamiques
	unHighlightNorloge();
	QToolTip::hideText();
	clearViewers();
}

void QQTextBrowser::mousePressEvent(QMouseEvent *event)
{
	QTextBrowser::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
		viewport()->setCursor(Qt::IBeamCursor);
}

void QQTextBrowser::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
		viewport()->unsetCursor();

	emit shotDuck(event->button() == Qt::MiddleButton);

	QTextBrowser::mouseReleaseEvent(event);

	// Verification que l'on est pas en pleine selection
	if(textCursor().hasSelection())
		return;

	// Reset de l'icone de l'application
	QIcon icon = QIcon(QString::fromLatin1(":/img/rubber_duck_yellow.svg"));
	window()->setWindowIcon(icon);

	// Marquage des posts comme lus
	bool needUpdate = false;
	foreach(QQBouchot *b, QQBouchot::listBouchotsGroup(m_groupName))
	{
		QListIterator<QQPostPtr> pI(b->postsHistory());
		pI.toBack();
		while(pI.hasPrevious())
		{
			QQPost *p = pI.previous();
			if(p->isUnread())
			{
				p->setRead();
				needUpdate = true;
			}
			else
				break;
		}
	}
	if(needUpdate)
	{
		m_notifArea->update();
		emit newPostsAcknowledged(m_groupName);
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
				while(i.hasNext())
				{
					QQBigornoItem bigItem = i.next();
					QTextCursor cursor = QTextCursor(block);
					cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, bigItem.position());
					QRect rect = cursorRect(cursor);
					rect.setWidth(QFontMetrics(document()->defaultFont()).boundingRect(bigItem.word()).width());
					rect.adjust(-1, 0, +1, 0);

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

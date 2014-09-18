#include "qqpinipede.h"

#include "mainwindow.h"
#include "core/qqbackendupdatedevent.h"
#include "core/qqbakdisplayfilter.h"
#include "core/qqbouchot.h"
#include "core/qqpostdisplayfilter.h"
#include "core/qqpurgebouchothistoevent.h"
#include "core/qqtotozdownloader.h"
#include "ui/qqpostparser.h"
#include "ui/qqmessageblockuserdata.h"
#include "ui/qqpalmipede.h"
#include "ui/qqtextbrowser.h"
#include "ui/qqtotozmanager.h"
#include "ui/qqtotozviewer.h"
#include "ui/qqwebimageviewer.h"

#ifdef Q_OS_UNIX
#undef signals
#include <libnotify/notify.h>
#endif

#include <QtAlgorithms>
#include <QApplication>
#include <QContextMenuEvent>
#include <QCursor>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMovie>
#include <QScrollBar>
#include <QTabBar>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextFrame>
#include <QTime>
#include <QToolButton>
#include <QVBoxLayout>

#define LOGIN_COLOR "#553333"
#define UA_COLOR "#883333"
#define UNKNOWN_POSTER_COLOR "#BB3333"

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::QQPinipede
/// \param parent
///
QQPinipede::QQPinipede(QWidget * parent) :
	QTabWidget(parent)
{
	m_totozManager = NULL;
	m_huntingView = NULL;
	m_duckAutolaunchEnabled = false;

	m_totozDownloader = new QQTotozDownloader(this);

	m_postparser = new QQPostParser(this);
	connect(m_postparser, SIGNAL(totozRequired(QString &)),
			m_totozDownloader, SLOT(fetchTotoz(QString &)));
	connect(m_postparser, SIGNAL(bigorNotify(QString &, QString &, bool)),
			this, SLOT(bigorNotify(QString &, QString &, bool)));

	m_hiddenPostViewerLabelSSheet = QString::fromLatin1("border: 2px solid black; border-radius: 4px;");
	m_hiddenPostViewerLabel = new QLabel(this);
	m_hiddenPostViewerLabel->setStyleSheet(m_hiddenPostViewerLabelSSheet);

	m_hiddenPostViewerLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_hiddenPostViewerLabel->setTextFormat(Qt::RichText);
	m_hiddenPostViewerLabel->setWordWrap(true);
	m_hiddenPostViewerLabel->setScaledContents(true);
	m_hiddenPostViewerLabel->hide();

	m_totozViewer = new QQTotozViewer("", this);
	m_totozViewer->hide();
	m_totozViewer->setTotozDownloader(m_totozDownloader);
	m_totozViewer->enableBookmarksAdd();
	m_totozViewer->setAttribute(Qt::WA_TransparentForMouseEvents);

	m_webImageViewer = new QQWebImageViewer(this);
	m_webImageViewer->setImgMaxSize(QSize(50, 50));
	m_webImageViewer->hide();
	m_webImageViewer->setAttribute(Qt::WA_TransparentForMouseEvents);

	addTab(new QWidget(), "(void)");

	m_huntingView = new QQHuntingView(this);
	m_huntingView->resize(size());
	connect(m_huntingView, SIGNAL(duckKilled(QString,QString)), this, SLOT(duckKilled(QString,QString)));

	setMovable(true);

	m_listpostDisplayFilters.append(new QQBakDisplayFilter());
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::~QQPinipede
///
QQPinipede::~QQPinipede()
{
	delete m_totozDownloader;

	QList<QString> listTabs = m_listPostsTabMap.keys();
	QString tab;
	foreach(tab, listTabs)
	{
		for(int i = 0; i < listTabs.size(); i++)
			delete m_listPostsTabMap.take(tab);
	}

	foreach(QQPostDisplayFilter *df, m_listpostDisplayFilters)
	{
		if(df != NULL)
			delete df;
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::setToolButton
/// \param toolButton
///
void QQPinipede::setToolButton(QWidget *toolButton)
{
	setCornerWidget(toolButton, Qt::TopRightCorner);
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::addPiniTab
/// \param groupName
///
void QQPinipede::addPiniTab(const QString &groupName)
{
	if(count() == 1 && this->m_textBrowserHash.size() == 0)
	{
		currentWidget()->deleteLater();
		clear();
	}

	if(this->m_textBrowserHash.value(groupName) != NULL)
		return;

	QQTextBrowser * textBrowser = new QQTextBrowser(groupName, this);
	addTab(textBrowser, groupName);

	m_textBrowserHash.insert(groupName, textBrowser);

	connect(textBrowser, SIGNAL(duckClicked(QString,QString,bool)), m_huntingView, SLOT(launchDuck(QString,QString,bool)));
	connect(textBrowser, SIGNAL(shotDuck(bool)), m_huntingView, SLOT(killDuck(bool)));
	connect(textBrowser, SIGNAL(norlogeClicked(QString, QQNorloge)), this, SLOT(norlogeClicked(QString, QQNorloge)));
	connect(textBrowser, SIGNAL(norlogeRefClicked(QString, QQNorlogeRef)), this, SLOT(norlogeRefClicked(QString, QQNorlogeRef)));
	connect(textBrowser, SIGNAL(loginClicked(QString, QString)), this, SLOT(loginClicked(QString, QString)));
	connect(textBrowser, SIGNAL(norlogeRefHovered(QQNorlogeRef)), this, SLOT(norlogeRefHovered(QQNorlogeRef)));
	connect(textBrowser, SIGNAL(unHighlight(QQTextBrowser *)), this, SLOT(unHighlight(QQTextBrowser *)));
	connect(textBrowser, SIGNAL(displayTotoz(const QString &)), this, SLOT(showTotozViewer(const QString &)));
	connect(textBrowser, SIGNAL(displayWebImage(const QUrl &)), this, SLOT(showWebImageViewer(const QUrl &)));
	connect(textBrowser, SIGNAL(hideViewers()), this, SLOT(hideViewers()));
	connect(textBrowser, SIGNAL(newPostsAcknowledged(QString)), this, SLOT(tabEventsAcknowledged(QString)));
	connect(textBrowser, SIGNAL(displayTotozContextMenu(QPoint &)), m_totozViewer, SLOT(displayContextMenu(QPoint &)));
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::clearPiniTab
/// \param groupName
///
void  QQPinipede::clearPiniTab(const QString &groupName)
{
	QQTextBrowser *textBrowser = m_textBrowserHash.value(groupName);
	if(textBrowser != NULL)
		textBrowser->clear();
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::removePiniTab
/// \param groupName
///
void QQPinipede::removePiniTab(const QString &groupName)
{
	QQTextBrowser *textBrowser = m_textBrowserHash.value(groupName);
	if(textBrowser == NULL)
		return;

	removeTab(indexOf(textBrowser));
	m_textBrowserHash.remove(groupName);
	delete textBrowser;

	m_listPostsTabMap.remove(groupName);

	if(count() == 0)
		addTab(new QWidget(), "(void)");
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::repaintPiniTab
/// \param groupName
///
void QQPinipede::repaintPiniTab(const QString &groupName)
{
	QQTextBrowser *textBrowser = m_textBrowserHash.value(groupName);
	if(textBrowser == NULL)
		return;

	while(! newPostsAvailableMutex.tryLock(1000))
		qWarning() << Q_FUNC_INFO << groupName << "tryLock timeout";

	QApplication::setOverrideCursor(Qt::BusyCursor);
	int sliderPos = textBrowser->verticalScrollBar()->sliderPosition();

	qDebug() << Q_FUNC_INFO << textBrowser->document()->blockCount();

	clearPiniTab(groupName);

	QQListPostPtr *posts = m_listPostsTabMap.value(groupName);
	QTextCursor cursor(textBrowser->document());
	cursor.beginEditBlock();
	bool postwasPrinted = printPostAtCursor(cursor, posts->at(0));
	for(int i = 1; i < posts->size(); i++)
	{
		if(postwasPrinted)
			cursor.insertBlock();
		postwasPrinted = printPostAtCursor(cursor, posts->at(i));
	}
	cursor.endEditBlock();

	textBrowser->verticalScrollBar()->setSliderPosition(sliderPos);

	QApplication::restoreOverrideCursor();

	newPostsAvailableMutex.unlock();
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::purgePiniTab
/// \param groupName
/// \param bouchotName
///
void QQPinipede::purgePiniTab(const QString &groupName, const QString &bouchotName)
{
	purgePinitab(groupName, bouchotName, INT_MAX);
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::purgePinitab
/// \param groupName
/// \param bouchotName
/// \param max
///
void QQPinipede::purgePinitab(const QString &groupName, const QString &bouchotName, unsigned int max)
{
	QQTextBrowser *textBrowser = m_textBrowserHash.value(groupName);
	QQListPostPtr *destListPosts = m_listPostsTabMap[groupName];

	if(textBrowser == NULL)
		return;

	unsigned int count = 0;

	bool moveOK = true;
	QTextCursor cursor(textBrowser->document());
	cursor.beginEditBlock();
	int destListPostsIndex = 0;
	QQPost *post = NULL;
	do
	{
		qDebug() << Q_FUNC_INFO << "block num=" << cursor.block().blockNumber()
				 << ", doc->blockCount()=" << textBrowser->document()->blockCount();

		QQMessageBlockUserData * userData = (QQMessageBlockUserData *) (cursor.block().userData());
		qDebug() << Q_FUNC_INFO << "userData->post()->bouchot()->name()=" << userData->post()->bouchot()->name()
				 << ", bouchotName=" << bouchotName;
		post = userData->post();
		if(post->bouchot()->name() == bouchotName)
		{
			moveOK = cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
			//Ce cas represente le dernier block
			if(!moveOK)
			{
				//on se positionne a la fin du block precedent
				moveOK = cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor);
				moveOK = cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
				//et on selectionne le block
				moveOK = cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
				moveOK = cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
			}
			cursor.removeSelectedText();

			for(; destListPostsIndex < destListPosts->size(); destListPostsIndex ++)
			{
				if(destListPosts->at(destListPostsIndex) == post)
				{
					destListPosts->removeAt(destListPostsIndex);
					break;
				}
			}

			if(count >= max)
				break;
		}
		else
			moveOK = cursor.movePosition(QTextCursor::NextBlock);
	} while(moveOK);

	cursor.endEditBlock();
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::purgePinitabHistory
/// \param groupName
///
void QQPinipede::purgePinitabHistory(const QString & groupName)
{
	QQTextBrowser *textBrowser = m_textBrowserHash.value(groupName);
	QQListPostPtr *destlistPosts = m_listPostsTabMap[groupName];

	if(textBrowser == NULL || destlistPosts == NULL)
		return;

	QQSettings settings;
	int maxHistorySize = settings.value(SETTINGS_GENERAL_MAX_HISTLEN, DEFAULT_GENERAL_MAX_HISTLEN).toInt();

	// L'historique est plus petit que le max, pas besoin d'aller plus loin
	if(destlistPosts->size() <= maxHistorySize)
		return;

	// Purge de la table d'affichage
	QTextCursor cursor = textBrowser->document()->rootFrame()->firstCursorPosition();
	cursor.beginEditBlock();
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, destlistPosts->size() - maxHistorySize);
	cursor.removeSelectedText();
	cursor.endEditBlock();

	// Purge de l'historique interne
	QStringList purgedBoards;
	while(destlistPosts->size() > (int) maxHistorySize)
	{
		QQPostPtr post = destlistPosts->takeFirst();
		if(! purgedBoards.contains(post->bouchot()->name()))
			purgedBoards.append(post->bouchot()->name());

		delete post;
	}

	// Purge de l'historique interne des bouchots
	foreach(QString boardName, purgedBoards)
	{
		QQBouchot * board = QQBouchot::bouchot(boardName);
		QApplication::postEvent(board, new QQPurgeBouchotHistoEvent());
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::event
/// \param e
/// \return
///
bool QQPinipede::event(QEvent *e)
{
	bool rep = true;
	if(e->type() == QQBackendUpdatedEvent::BACKEND_UPDATED)
	{
		QString group = ((QQBackendUpdatedEvent *) e)->group();
		newPostsAvailable(group);
	}
	else
		rep = QTabWidget::event(e);

	return rep;
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::tabEventsAcknowledged
/// \param groupName
///
void QQPinipede::tabEventsAcknowledged(const QString& groupName)
{
	QQTextBrowser * textBrowser = m_textBrowserHash.value(groupName);
	setTabText(indexOf(textBrowser), groupName);

	QList<QQBouchot *> listBouchots = QQBouchot::listBouchotsGroup(groupName);
	foreach (QQBouchot *b, listBouchots)
		b->resetStatus();
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::notify
///
void QQPinipede::bigorNotify(QString &srcBoard, QString &poster, bool global)
{
#ifndef Q_OS_UNIX
	Q_UNUSED(srcBoard)
	Q_UNUSED(poster)
	Q_UNUSED(global)
#endif

	QIcon icon = QIcon(QString::fromLatin1(":/img/Point_exclamation_rouge.svg"));
	window()->setWindowIcon(icon);

#ifdef Q_OS_UNIX
	QQSettings settings;
	if(settings.value(SETTINGS_BIGORNOTIFY_ENABLED, DEFAULT_BIGORNOTIFY_ENABLED).toBool())
	{
		QString msg;
		if(global)
			msg = QString(tr("%1 called everyone on %2 board")).arg(poster).arg(srcBoard);
		else
			msg = QString(tr("%1 called you on %2 board")).arg(poster).arg(srcBoard);

		NotifyNotification *notification = notify_notification_new(notif_name, msg.toUtf8(), NULL);
		if(notification)
		{
			notify_notification_set_timeout(notification, 3000);
			if (!notify_notification_show(notification, NULL))
				qDebug() << Q_FUNC_INFO << "Failed to send notification";

			/* Clean up the memory */
			g_object_unref(notification);
		}
		else
			qDebug() << Q_FUNC_INFO << "Failed to create notification";
	}
#endif

	QQBouchot::bouchot(srcBoard)->setHasBigorno();
}

//////////////////////////////////////////////////////////////
/// \brief searchText
/// \param text
/// \param forward
///
void QQPinipede::searchText(const QString &text, bool forward)
{
	QQSettings settings;
	QColor color(settings.value(SETTINGS_GENERAL_HIGHLIGHT_COLOR, DEFAULT_GENERAL_HIGHLIGHT_COLOR).toString());

	foreach (QQTextBrowser *textBrowser, m_textBrowserHash.values())
	{
		if(! textBrowser->isVisible())
			continue;

		QTextDocument *doc = textBrowser->document();
		QTextCursor cursor = textBrowser->textCursor();

		QTextDocument::FindFlags flags = 0;
		if(! forward)
			flags |= QTextDocument::FindBackward;

		QTextCursor findCursor = doc->find(text, cursor, flags);

		//En cas d'alternance back/forward
		if(!findCursor.isNull())
		{
			if(forward && findCursor.selectionStart() == cursor.position())
			{
				cursor.movePosition(QTextCursor::NextCharacter,
									QTextCursor::MoveAnchor,
									findCursor.selectedText().size());
				findCursor = doc->find(text, cursor, flags);
			}
			else if(findCursor.selectionEnd() == cursor.position())
			{
				cursor.movePosition(QTextCursor::PreviousCharacter,
									QTextCursor::MoveAnchor,
									findCursor.selectedText().size());
				findCursor = doc->find(text, cursor, flags);
			}
		}

		if(findCursor.isNull())
		{
			if(forward)
				cursor.movePosition(QTextCursor::Start);
			else
				cursor.movePosition(QTextCursor::End);

			findCursor = doc->find(text, cursor, flags);
		}

		if(! findCursor.isNull())
		{
			QColor highlightColor;
			if(color.isValid())
				highlightColor = color;
			else
			{
				QQMessageBlockUserData * userData = (QQMessageBlockUserData *) findCursor.block().userData();
				highlightColor = getDynHighlightColor(userData->post()->bouchot()->settings().colorLight());
			}

			QTextEdit::ExtraSelection extra;
				extra.format.setBackground(highlightColor);
			extra.cursor = findCursor;

			QList<QTextEdit::ExtraSelection> extraSelections;
			extraSelections.append(extra);

			textBrowser->setExtraSelections(extraSelections);

			if(! forward)
			{
				int selLength = findCursor.selectedText().size();
				findCursor.clearSelection();
				findCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, selLength);
			}
			else
				findCursor.clearSelection();
			textBrowser->setTextCursor(findCursor);
			textBrowser->ensureCursorVisible();
		}

		break; //Pas besoin d'aller plus loin seul un seul est visible a la fois
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::bouchotVisibilityChanged
/// \param board
///
void QQPinipede::bouchotVisibilityChanged(QString board)
{
	QString groupName = QQBouchot::bouchot(board)->settings().group();

	QTextDocument *doc = m_textBrowserHash.value(groupName)->document();
	QTextBlock b = doc->firstBlock();
	while(b.isValid())
	{
		QQPost *post = ((QQMessageBlockUserData *) b.userData())->post();
		b.setVisible(applyPostDisplayFilters(post));
		doc->markContentsDirty(b.position(), b.length()); //TODO a améliorer pour optimiser par zone réellement modifiée

		b = b.next();
	}

}

//////////////////////////////////////////////////////////////
/// \brief duckKilled
/// \param board
/// \param postId
///
void QQPinipede::duckKilled(QString board, QString postId)
{
	QQBouchot *dstBoard = QQBouchot::bouchot(board);
	QString message;
	foreach(QQPost *post, dstBoard->postsHistory())
	{
		if(post->id() == postId)
		{
			QQNorloge norloge(board, post->norloge());
			if(post->isNorlogeMultiple())
				norloge.setNorlogeIndex(post->norlogeIndex());
			message = norloge.toStringPalmi();
			break;
		}
	}

	int arobasePos = message.indexOf('@');
	if(arobasePos >= 0)
		message = message.left(arobasePos);
	dstBoard->postMessage(message + QString::fromLatin1(" pan ! pan !"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::norlogeClicked
/// \param srcBouchot
/// \param norloge
///
void QQPinipede::norlogeClicked(QString srcBoard, QQNorloge norloge)
{
	emit insertTextPalmi(srcBoard, norloge.toStringPalmi() + QString::fromLatin1(" "));
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::norlogeRefClicked
/// \param srcBouchot
/// \param nRef
///
void QQPinipede::norlogeRefClicked(QString srcBoard, QQNorlogeRef nRef)
{
	QQBouchot *srcQQBouchot = QQBouchot::bouchot(srcBoard);
	QQBouchot *dstQQBouchot = QQBouchot::bouchot(nRef.dstBouchot());

	if(dstQQBouchot == NULL)
		return;

	QQTextBrowser *textBrowser = m_textBrowserHash.value(srcQQBouchot->settings().group());
	unHighlight(textBrowser);

	textBrowser = m_textBrowserHash.value(dstQQBouchot->settings().group());

	QTextCursor cursor(textBrowser->document()->lastBlock());
	bool found = false;
	do
	{
		QQMessageBlockUserData * userData = (QQMessageBlockUserData *) (cursor.block().userData());

		if(userData == NULL)
			continue;
		else if(nRef.matchesPost(userData->post()))
			found = true;
	} while(cursor.movePosition(QTextCursor::PreviousBlock) && !found);

	// Si non trouve, aucune raison de changer de tab
	if(found)
	{
		if(dstQQBouchot->settings().group() != srcQQBouchot->settings().group())
			setCurrentIndex(indexOf(textBrowser));

		cursor.movePosition(QTextCursor::NextBlock); //Puisqu'on est allé un bloc trop haut
		textBrowser->setTextCursor(cursor);
		textBrowser->ensureCursorVisible();

		norlogeRefHovered(nRef);
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::loginClicked
/// \param bouchot
/// \param login
///
void QQPinipede::loginClicked(QString board, QString login)
{
	emit insertTextPalmi(board, login + QString::fromLatin1("< "));
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::norlogeRefHovered
/// \param norlogeRef
///
void QQPinipede::norlogeRefHovered(QQNorlogeRef norlogeRef)
{
	//qDebug() << Q_FUNC_INFO << "value =" << norlogeRef.nRefId() << norlogeRef.dstBouchot();

	QStringList groups;

	// Src
	QQBouchot * bouchot = QQBouchot::bouchot(norlogeRef.srcBouchot());
	Q_ASSERT(bouchot != NULL);
	groups.append(bouchot->settings().group());

	// Dest
	bouchot = QQBouchot::bouchot(norlogeRef.dstBouchot());
	if(bouchot != NULL && !groups.contains(bouchot->settings().group()))
		groups.append(bouchot->settings().group());

	bool highlightSuccess = false;
	QQTextBrowser *textBrowser = NULL;
	QQSettings settings;
	QColor color(settings.value(SETTINGS_GENERAL_HIGHLIGHT_COLOR, DEFAULT_GENERAL_HIGHLIGHT_COLOR).toString());
	foreach (QString group, groups)
	{
		textBrowser = m_textBrowserHash.value(group);

		if(textBrowser->isVisible())
		{
			// Get the cursor position near the top left corner of the current viewport.
			QTextCursor cursor = textBrowser->cursorForPosition(QPoint(0, 0));
			//last visible block number
			QPoint bRP(textBrowser->viewport()->width(), textBrowser->viewport()->height());
			int lastBlkNum = textBrowser->cursorForPosition(bRP).block().blockNumber();

			QList<QTextEdit::ExtraSelection> extraSelections;
			extraSelections.clear();
			while(cursor.movePosition(QTextCursor::NextBlock))
			{
				QTextBlock currBlock = cursor.block();
				if(currBlock.blockNumber() > lastBlkNum)
					break;

				QQMessageBlockUserData* userData = (QQMessageBlockUserData *) currBlock.userData();
				if(userData == NULL)
					continue;

				if(norlogeRef.matchesPost(userData->post()))
				{
					QColor highlightColor;
					if(color.isValid())
						highlightColor = color;
					else
						highlightColor = getDynHighlightColor(userData->post()->bouchot()->settings().colorLight());

					QTextBlockFormat format = cursor.blockFormat();
					format.setBackground(highlightColor);
					cursor.mergeBlockFormat(format);

					QTextEdit::ExtraSelection extra;
					extra.cursor = cursor;
					extraSelections.append(extra);

					highlightSuccess = true;
				}
				else
				{
					foreach (QQNorlogeRef nRef, userData->norlogeRefs())
					{
						if(norlogeRef.matchesNRef(nRef))
						{
							QColor highlightColor;
							if(color.isValid())
								highlightColor = color;
							else
								highlightColor = getDynHighlightColor(userData->post()->bouchot()->settings().colorLight());

							QTextCursor selCursor = cursor;
							selCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, nRef.getPosInMessage());
							selCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, nRef.getOrigNRef().length());

							QTextEdit::ExtraSelection extra;
							extra.format.setBackground(highlightColor);
							extra.cursor = selCursor;
							extraSelections.append(extra);
						}
					}
				}
			}

			textBrowser->setExtraSelections(extraSelections);
		}
	}

	if(! highlightSuccess)
	{
		QTextCursor cursor(textBrowser->document());

		QTextDocument destDocument;
		QTextCursor destCursor(& destDocument);
		destCursor.atStart();

		destCursor.beginEditBlock();
		do
		{
			QQMessageBlockUserData * userData = (QQMessageBlockUserData *) (cursor.block().userData());
			if(userData == NULL)
				continue;

			if(norlogeRef.matchesPost(userData->post()))
			{
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				QTextDocumentFragment fragment = cursor.selection();
				if(! destCursor.atBlockStart())
					destCursor.insertBlock();
				destCursor.insertFragment(fragment);
			}

		} while(cursor.movePosition(QTextCursor::NextBlock));

		destCursor.endEditBlock();

		if(destDocument.toPlainText().length() > 0)
		{
			m_hiddenPostViewerLabel->setFixedWidth(this->currentWidget()->width());
			QString styleSheet = m_hiddenPostViewerLabelSSheet;
			styleSheet.append("background-color: ")
					.append(bouchot->settings().colorLight().name())
					.append(";");
			m_hiddenPostViewerLabel->setStyleSheet(styleSheet);
			m_hiddenPostViewerLabel->setText(destDocument.toHtml());
			m_hiddenPostViewerLabel->move(mapFromGlobal(textBrowser->mapToGlobal(QPoint(0, 0))));
			m_hiddenPostViewerLabel->show();
		}
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::unHighlight
/// \param tBrowser
///
void QQPinipede::unHighlight(QQTextBrowser *tBrowser)
{
	m_hiddenPostViewerLabel->hide();
	foreach (QTextEdit::ExtraSelection extra, tBrowser->extraSelections())
	{
		if(! extra.cursor.hasSelection()) //fulll block
		{
			QTextBlockFormat format = extra.cursor.blockFormat();
			QQMessageBlockUserData * userData = (QQMessageBlockUserData *) extra.cursor.block().userData();
			format.setBackground(userData->post()->bouchot()->settings().colorLight());
			extra.cursor.mergeBlockFormat(format);
		}
	}
	tBrowser->setExtraSelections(QList<QTextEdit::ExtraSelection>());
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::showWebImageViewer
/// \param totozId
///
void QQPinipede::showWebImageViewer(const QUrl &url)
{
	hideViewers();

	QQSettings settings;
	if(! settings.value(SETTINGS_WEB_IMAGE_VIEWER_ENABLED, DEFAULT_WEB_IMAGE_VIEWER_ENABLED).toBool())
		return;

	int maxSize = settings.value(SETTINGS_WEB_IMAGE_PREVIEW_SIZE, DEFAULT_WEB_IMAGE_PREVIEW_SIZE).toInt();
	m_webImageViewer->setImgMaxSize(QSize(maxSize, maxSize));
	m_webImageViewer->setParent(currentWidget());
	m_webImageViewer->showImg(url);
	m_webImageViewer->show();
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::showTotozViewer
/// \param totozId
///
void QQPinipede::showTotozViewer(const QString &totozId)
{
	hideViewers();

	QQSettings settings;
	if(settings.value(SETTINGS_TOTOZ_VISUAL_MODE, DEFAULT_TOTOZ_VISUAL_MODE).toString() ==
			TOTOZ_VISUAL_MODE_DISABLED)
		return;

	m_totozViewer->setParent(currentWidget());
	m_totozViewer->setTotozId(totozId);
	m_totozViewer->show();
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::hideViewers
///
void QQPinipede::hideViewers()
{
	m_totozViewer->hide();
	m_totozViewer->setParent(this);
	m_webImageViewer->hide();
	m_webImageViewer->setParent(this);
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::getPostForGroup
/// \param groupName
/// \param numPost
/// \return
///
QQPost * QQPinipede::getPostForGroup(QString &groupName, int numPost)
{
	if(m_listPostsTabMap.contains(groupName))
	{
		QQListPostPtr *listPosts = m_listPostsTabMap[groupName];
		if(listPosts->size() > numPost)
			return listPosts->at(numPost);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::setTotozManager
/// \param ttManager
///
void QQPinipede::setTotozManager(QQTotozManager * ttManager)
{
	if(m_totozManager != NULL)
		disconnect(m_totozManager);

	m_totozManager = ttManager;

	if(m_totozManager != NULL)
		connect(m_totozViewer, SIGNAL(totozBookmarkAct(QString,QQTotoz::TotozBookmarkAction)),
				m_totozManager, SLOT(totozBookmarkDo(QString,QQTotoz::TotozBookmarkAction)));
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::contextMenuEvent
/// \param ev
///
void QQPinipede::contextMenuEvent(QContextMenuEvent * ev)
{
	if(m_totozViewer->isVisible())
	{
		QApplication::sendEvent(m_totozViewer, ev);
		ev->accept();
	}
	else
		QTabWidget::contextMenuEvent(ev);
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::resizeEvent
/// \param event
///
void QQPinipede::resizeEvent(QResizeEvent *event)
{
	if(m_huntingView)
		m_huntingView->resize(event->size());
	QTabWidget::resizeEvent(event);
}


/*************************************************************
 * Private
 *************************************************************/

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::applyPostDisplayFilters
/// \param post
/// \return
///
bool QQPinipede::applyPostDisplayFilters(QQPost *post)
{
	if(! post->bouchot()->isVisible())
		return false;

	foreach(QQPostDisplayFilter *filter, m_listpostDisplayFilters)
	{
		if(filter->filterMatch(post))
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::insertPostToList
/// \param listPosts
/// \param post
/// \param indexStart
/// \return
///
unsigned int QQPinipede::insertPostToList(QQListPostPtr *listPosts, QQPost *post, unsigned int indexStart)
{
	for(int i = indexStart; i < listPosts->size(); i++)
	{
		if(listPosts->at(i)->norloge().toLongLong() > post->norloge().toLongLong())
		{
			listPosts->insert(i, post);
			return i;
		}
		// Gestion de l'index de norloge multiple
		else if(listPosts->at(i)->norloge().toLongLong() == post->norloge().toLongLong() &&
				listPosts->at(i)->bouchot()->name().compare(post->bouchot()->name()) == 0)
		{
			listPosts->at(i)->setNorlogeMultiple(true);
			post->setNorlogeIndex(listPosts->at(i)->norlogeIndex() + 1);
		}
	}
	listPosts->append(post);
	return listPosts->size() - 1;
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::newPostsAvailable
/// \param groupName
///
void QQPinipede::newPostsAvailable(QString groupName)
{
	qDebug() << Q_FUNC_INFO << "from : " << groupName;

	//On est obligé de locker pour éviter la pagaille dans le pini.
	// un locking plus fin pourrait être obtenu en implémentant un lock par groupe
	while(! newPostsAvailableMutex.tryLock(1000))
		qWarning() << Q_FUNC_INFO << groupName << "tryLock timeout";

	QQTextBrowser * textBrowser = m_textBrowserHash.value(groupName);
	bool wasAtEnd = (textBrowser->verticalScrollBar()->sliderPosition() == textBrowser->verticalScrollBar()->maximum());

	QQListPostPtr newPosts;
	foreach(QQBouchot *b, QQBouchot::listBouchotsGroup(groupName))
	{
		QQListPostPtr newPostsBouchot = b->takeNewPosts();
		if(newPostsBouchot.size() > 0)
		{
			//qDebug() << Q_FUNC_INFO << "newPosts from :" << b->name();
			newPosts.append(newPostsBouchot);
		}
	}

	// Au cas ou on serait deja passe avant (cas du signal multiple)
	if(newPosts.size() == 0)
	{
		newPostsAvailableMutex.unlock();
		return;
	}

	QQSettings settings;
	//On ne peut pas vérifier la valeur dans le "printPostAtCursor", trop couteux, du coup on la met a jour ici
	m_duckAutolaunchEnabled =
			(((QuteQoin::QQHuntMode) settings.value(SETTINGS_HUNT_MODE, DEFAULT_HUNT_MODE).toInt()) == QuteQoin::HuntMode_Auto);

	int maxHistorySize = settings.value(SETTINGS_GENERAL_MAX_HISTLEN, DEFAULT_GENERAL_MAX_HISTLEN).toInt();
	//Il ne sert a rien d'insérer plus que de posts que le max de l'historique
	while(newPosts.size() > maxHistorySize)
		newPosts.removeFirst();

	// Tri necessaire puisqu'on a potentiellement melange les posts de plusieurs tribunes
	qSort(newPosts.begin(), newPosts.end(), postComp);

	//On signale via la forme de la souris qu'un traitement est en cours
	QApplication::setOverrideCursor(Qt::BusyCursor);
	QTextDocument * doc = textBrowser->document();

	QTextCursor cursor(doc);
	cursor.beginEditBlock();

	bool postWasPrinted = true;
	// Recuperation de l'historique des posts (ou creation si absent)
	QQListPostPtr *destlistPosts = NULL;
	if(! m_listPostsTabMap.contains(groupName))
	{
		// Cas du pini vide, il contient déjà un bloc vide, on
		//  a juste a afficher le premier post;
		QQPost *firstPost = newPosts.takeFirst();
		destlistPosts = new QQListPostPtr();
		destlistPosts->append(firstPost);
		m_listPostsTabMap.insert(groupName, destlistPosts);
		postWasPrinted = printPostAtCursor(cursor, firstPost);

		// Si le pini était vide on scrolle forcément vers le dernier post
		wasAtEnd = true;
	}
	else
		destlistPosts = m_listPostsTabMap[groupName];

	int newPostsIndex = 0, baseInsertIndex = 0, insertIndex = 0;
	// Tant qu'il reste des posts a afficher
	while(newPostsIndex < newPosts.size())
	{
		QQPost *newPost = newPosts.at(newPostsIndex);

		insertIndex = insertPostToList(destlistPosts, newPost, baseInsertIndex);

		if(newPost == destlistPosts->last()) //insertion a la fin
			break;

		//Deplacement vers la nouvelle ligne
		if(insertIndex == 0)
		{
			//Necessite de le copier car il sera supprime par le nouveau userData de la premiere ligne
			QQMessageBlockUserData * uData = new QQMessageBlockUserData(* ((QQMessageBlockUserData *) cursor.block().userData()));
			if(postWasPrinted)
				cursor.insertBlock();
			cursor.block().setUserData(uData);
			cursor.movePosition(QTextCursor::PreviousBlock);
			postWasPrinted = printPostAtCursor(cursor, newPost);
		}
		else
		{
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, (insertIndex - baseInsertIndex) - 1);
			cursor.movePosition(QTextCursor::EndOfBlock);
			if(postWasPrinted)
				cursor.insertBlock();
			postWasPrinted = printPostAtCursor(cursor, newPost);
		}

		newPostsIndex++;
		baseInsertIndex = insertIndex;
	}

	//Insertion a la fin
	if(newPostsIndex < newPosts.size())
	{
		//Le premier item a deja ete insere dans la liste destlistPosts dans la boucle while au dessus
		//on a juste a l'afficher
		cursor.movePosition(QTextCursor::End);
		if(postWasPrinted)
			cursor.insertBlock();
		postWasPrinted = printPostAtCursor(cursor, newPosts.at(newPostsIndex++));

		while(newPostsIndex < newPosts.size())
		{
			// Gestion de l'index de norloge multiple
			if(newPosts.at(newPostsIndex)->norloge().toLongLong() == destlistPosts->last()->norloge().toLongLong() &&
			   newPosts.at(newPostsIndex)->bouchot()->name().compare(destlistPosts->last()->bouchot()->name()) == 0)
			{
				destlistPosts->last()->setNorlogeMultiple(true);
				newPosts.at(newPostsIndex)->setNorlogeIndex(destlistPosts->last()->norlogeIndex() + 1);
			}

			destlistPosts->append(newPosts.at(newPostsIndex));
			if(postWasPrinted)
				cursor.insertBlock();
			postWasPrinted = printPostAtCursor(cursor, newPosts.at(newPostsIndex++));
		}
	}

	if(!postWasPrinted) //remove last inserted bloc if not printed
	{
		cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
	}

	cursor.endEditBlock();

	// Purge des anciens messages
	purgePinitabHistory(groupName);

	if(wasAtEnd)
		textBrowser->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);

	//Remise en place de l'ancienne forme du pointeur
	QApplication::restoreOverrideCursor();

	//Signalement de nouveaux posts dans le nom du Tab
	foreach (QQPost *p, newPosts)
	{
		if(!p->isSelfPost())
		{
			QString tabName = groupName;
			tabName.append(" (*)");
			setTabText(indexOf(textBrowser), tabName);
			break;
		}
	}

	newPostsAvailableMutex.unlock();
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::printPostAtCursor
/// \param cursor
/// \param post
/// \return
///
bool QQPinipede::printPostAtCursor(QTextCursor &cursor, QQPost *post)
{
	QTextBlock block = cursor.block();

	QTextBlockFormat bFormat = cursor.blockFormat();
	bFormat.setTabPositions(cursor.document()->defaultTextOption().tabs());
	bFormat.setBackground(post->bouchot()->settings().colorLight());
	cursor.setBlockFormat(bFormat);

	QQMessageBlockUserData * data = new QQMessageBlockUserData();
	data->setPost(post);

	block.setVisible(applyPostDisplayFilters(post));

	QQTextBrowser * browser = m_textBrowserHash.value(post->bouchot()->settings().group());

	qreal textLen = 0;
	QFont currFont = cursor.document()->defaultFont();
	qreal loginUAAreaWidth = browser->timeUAAreaWidth();

	QTextCharFormat defaultFormat;
	defaultFormat.setFont(currFont);

	//norloge

	QTextCharFormat norlogeFormat;
	norlogeFormat.setFont(currFont);
	norlogeFormat.setToolTip(post->id());
	norlogeFormat.setFontWeight(QFont::Bold);

	QString txt = post->norlogeFormatee();

	QQMessageBlockUserData::ZoneRange rangeNorloge;
	rangeNorloge.begin = cursor.positionInBlock();
	cursor.insertText(txt, norlogeFormat);
	rangeNorloge.end = cursor.positionInBlock();
	data->setZRange(QQMessageBlockUserData::NORLOGE, rangeNorloge);

	QFontMetricsF fm = QFontMetricsF(norlogeFormat.font());
	textLen += fm.size(Qt::TextSingleLine | Qt::TextExpandTabs, txt).width();

	cursor.insertText(" ", defaultFormat);

	fm = QFontMetricsF(defaultFormat.font());
	textLen += fm.size(Qt::TextSingleLine | Qt::TextExpandTabs," ").width();

	//login ou ua

	QTextCharFormat loginUaFormat;
	loginUaFormat.setFont(currFont);
	loginUaFormat.setToolTip(post->UA());

	QQMessageBlockUserData::ZoneRange rangeLoginUA;
	rangeLoginUA.begin = cursor.positionInBlock();
	if(post->login().size() != 0)
	{
		loginUaFormat.setForeground(QColor(LOGIN_COLOR));

		txt = post->login();
	}
	else if(post->UA().size() != 0)
	{
		loginUaFormat.setFontItalic(true);
		loginUaFormat.setForeground(QColor(UA_COLOR));

		txt = post->UA();
	}
	else
	{
		loginUaFormat.setFontFamily("Monospace");
		loginUaFormat.setForeground(QColor(UNKNOWN_POSTER_COLOR));

		txt = QString::fromLatin1("$NO UA$");
	}

	fm = QFontMetricsF(loginUaFormat.font());
	if(textLen + fm.size(Qt::TextSingleLine | Qt::TextExpandTabs, txt).width() > loginUAAreaWidth)
		txt = fm.elidedText(txt, Qt::ElideMiddle, loginUAAreaWidth - textLen);
	txt.append("\t");

	cursor.insertText(txt, loginUaFormat);

	rangeLoginUA.end = cursor.positionInBlock();

	data->setZRange(QQMessageBlockUserData::LOGINUA, rangeLoginUA);

	//message

	QQMessageBlockUserData::ZoneRange rangeMsg;
	rangeMsg.begin = cursor.positionInBlock();
	cursor.setCharFormat(defaultFormat);

	// Application des filtres de transformation du message
	//QString message = applyMessageTransformFilters(post);
	m_postparser->setIndexShift(rangeMsg.begin);
	QTextDocumentFragment *message = m_postparser->formatMessage(post, data);
	cursor.insertFragment(* message);
	delete message;

	rangeMsg.end = cursor.positionInBlock();
	data->setZRange(QQMessageBlockUserData::MESSAGE, rangeMsg);
	block.setUserData(data);

	//Duck autolaunch
	if(m_duckAutolaunchEnabled && !post->isSelfPost() && data->duckIndexes().size() > 0)
		m_huntingView->launchDuck(post->bouchot()->name(), post->id(), post->isSelfPost());

	//Alerte en cas de réponse
	if(!post->isSelfPost() && data->hasNRefToSelfPost())
	{
		post->bouchot()->setHasNewResponse();
		QApplication::alert(this->parentWidget(), 0);
	}


	return true;
}

//////////////////////////////////////////////////////////////
/// \brief QQPinipede::getDynHighlightColor
/// \param bgColor
/// \return
///
#define HIGHLIGHT_COLOR_S 200
#define HIGHLIGHT_COLOR_V 200
QColor QQPinipede::getDynHighlightColor(const QColor &bgColor)
{
	int h, nh, s, ns, v, nv;
	bgColor.getHsv(&h, &s, &v);
	nh = (h + 180) % 360;
	// zone a exclure car elle correspond au bleu fonce qui passe mal avec la font noire
	if(nh > 210 && nh < 270)
		nh = (nh <= 240) ? 210 : 270;
	ns = qMax(s, HIGHLIGHT_COLOR_S);
	nv = qMax(v, HIGHLIGHT_COLOR_V);
	return QColor::fromHsv(nh, ns, nv);
}
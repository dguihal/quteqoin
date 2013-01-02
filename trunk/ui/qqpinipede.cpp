#include "qqpinipede.h"

#include "mainwindow.h"
#include "core/qqbouchot.h"
#include "core/qqpurgebouchothistoevent.h"
#include "core/qqtotozmanager.h"
#include "ui/qqmessageblockuserdata.h"
#include "ui/qqpalmipede.h"
#include "ui/qqsyntaxhighlighter.h"
#include "ui/qqtextbrowser.h"

#include <QtAlgorithms>
#include <QApplication>
#include <QCursor>
#include <QImage>
#include <QLabel>
#include <QMovie>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextFrame>
#include <QTime>
#include <QTabBar>
#include <QVBoxLayout>

QQPinipede::QQPinipede(QQSettings *settings, QWidget *parent) :
	QTabWidget(parent)
{
	this->tabBar()->hide();

	m_settings = settings;
	m_totozManager = new QQTotozManager(m_settings);
	m_tBrowserHighlighted = NULL;

	m_hiddenPostViewerLabelSSheet = QString::fromAscii("border: 2px solid black; border-radius: 4px;");
	m_hiddenPostViewerLabel = new QLabel(this);
	m_hiddenPostViewerLabel->setStyleSheet(m_hiddenPostViewerLabelSSheet);

	m_hiddenPostViewerLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_hiddenPostViewerLabel->setTextFormat(Qt::RichText);
	m_hiddenPostViewerLabel->setWordWrap(true);
	m_hiddenPostViewerLabel->setScaledContents(true);
	m_hiddenPostViewerLabel->hide();

	m_totozLabel = new QLabel(this);
	m_totozLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_totozLabel->setScaledContents(false);
	m_totozLabel->hide();
	m_totozMovie = NULL;
	connect(m_settings, SIGNAL(totozServerUrlChanged(QString)),
			m_totozManager, SLOT(serverURLchanged(QString)));
}

QQPinipede::~QQPinipede()
{
	delete m_totozManager;
	delete m_totozLabel;
	if( m_totozMovie != NULL )
		delete m_totozMovie;
}

void QQPinipede::addPiniTab(const QString& groupName)
{
	qDebug() << "QQPinipede::addPiniTab" ;

	if( this->m_textBrowserHash.value(groupName) != NULL )
		return;

	QWidget * widget = new QWidget;
	QVBoxLayout * layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);

	QQTextBrowser * textBrowser = new QQTextBrowser(groupName, this);
	layout->addWidget(textBrowser);
	this->addTab(widget, groupName);

	m_textBrowserHash.insert(groupName, textBrowser);
	//textBrowser->document() devient le proprietaire du highlighter
	QQSyntaxHighlighter * highlighter = new QQSyntaxHighlighter(textBrowser->document());
	highlighter->setNotificationWindow(window());
	connect(highlighter, SIGNAL(totozRequired(const QString &)),
			m_totozManager, SLOT(fetchTotoz(const QString &)));

	connect(textBrowser, SIGNAL(norlogeClicked(QQNorloge)), this, SLOT(norlogeClicked(QQNorloge)));
	connect(textBrowser, SIGNAL(norlogeRefHovered(QQNorlogeRef)), this, SLOT(norlogeRefHovered(QQNorlogeRef)));
	connect(textBrowser, SIGNAL(unHighlight()), this, SLOT(unHighlight()));
	connect(textBrowser, SIGNAL(showTotozSig(QQTotoz &)), this, SLOT(showTotozSlot(QQTotoz &)));
	connect(textBrowser, SIGNAL(hideTotozSig()), this, SLOT(hideTotozSlot()));

	if (this->count() > 1)
		this->tabBar()->show();
}

void QQPinipede::createPiniTabs(const QList<QString> &groups)
{
	for (int i = 0; i < groups.size(); i++)
		this->addPiniTab(groups[i]);
}

void QQPinipede::removePiniTab(const QString& name)
{
	for (int i = 0; i < this->count(); i++)
		if (this->tabText(i) == name)
		{
			delete this->widget(i);
			this->removeTab(i);
			break;
		}

	if (this->count() < 2)
		this->tabBar()->hide();
}

void QQPinipede::purgePiniTab(const QString &groupName, const QString &bouchotName)
{
	purgePinitab(groupName, bouchotName, INT_MAX);
}

void QQPinipede::purgePinitab(const QString &groupName, const QString &bouchotName, unsigned int max)
{
	QQTextBrowser *textBrowser = m_textBrowserHash.value(groupName);

	if (textBrowser == NULL)
		return;

	unsigned int count = 0;

	bool moveOK = true;
	QTextCursor cursor(textBrowser->document());
	cursor.beginEditBlock();
	do
	{
		qDebug() << "QQPinipede::purgePiniTab block num=" << cursor.block().blockNumber()
				 << ", doc->blockCount()=" << textBrowser->document()->blockCount();

		QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData());
		qDebug() << "QQPinipede::purgePiniTab userData->post()->bouchot()->name()=" << userData->post()->bouchot()->name()
				 << ", bouchotName=" << bouchotName;
		if ( userData->post()->bouchot()->name() == bouchotName )
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
			if (count >= max)
				break;
		}
		else
			moveOK = cursor.movePosition(QTextCursor::NextBlock);
	} while(moveOK);

	cursor.endEditBlock();
}

void QQPinipede::purgePinitabHistory(const QString & groupName)
{
	QQTextBrowser *textBrowser = m_textBrowserHash.value(groupName);
	QList<QQPost *> *destlistPosts = m_listPostsTabMap[groupName];

	if (textBrowser == NULL || destlistPosts == NULL)
		return;

	unsigned int maxHistorySize = m_settings->maxHistoryLength();

	// L'historique est plus petit que le max, pas besoin d'aller plus loin
	if(destlistPosts->size() <= (int) maxHistorySize)
		return;

	// Purge de la table d'affichage
	QTextCursor cursor = textBrowser->document()->rootFrame()->firstCursorPosition();
	cursor.beginEditBlock();
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, destlistPosts->size() - maxHistorySize);
	cursor.removeSelectedText();
	cursor.endEditBlock();

	// Purge de l'historique interne
	QHash<QString, QString> maxIdRemoved;
	while(destlistPosts->size() > (int) maxHistorySize)
	{
		//On ne supprime pas, ils sont aussi stockés au niveau du bouchot
		QQPost * post = destlistPosts->takeFirst();
		maxIdRemoved.insert(post->bouchot()->name(), post->id());
	}

	// Purge de l'historique interne des bouchots
	QHashIterator<QString, QString> i(maxIdRemoved);
	while (i.hasNext())
	{
		i.next();
		QQBouchot * bouchot = m_settings->bouchot(i.key());
		QApplication::postEvent(
					bouchot,
					new QQPurgeBouchotHistoEvent(
						QQPurgeBouchotHistoEvent::PURGE_BOUCHOT_HISTO,
						i.value()
						)
					);
	}

}

void QQPinipede::printPostAtCursor( QTextCursor & cursor, QQPost * post )
{
	QTextBlock block = cursor.block();

	QTextCharFormat defaultFormat;
	QQMessageBlockUserData * data = new QQMessageBlockUserData();
	data->setPost(post);

	//norloge

	QTextCharFormat norlogeFormat;
	norlogeFormat.setFontWeight(QFont::Bold);

	QQMessageBlockUserData::ZoneRange rangeNorloge;
	rangeNorloge.begin = cursor.positionInBlock();
	cursor.insertText(post->norlogeFormatee(), norlogeFormat);
	rangeNorloge.end = cursor.positionInBlock();
	data->setZRange(QQMessageBlockUserData::NORLOGE, rangeNorloge);
	cursor.insertText(QString::fromUtf8(" "), defaultFormat);

	//login ou ua

	QTextCharFormat loginUaFormat;
	loginUaFormat.setToolTip(post->UA());
	QString txt;
	QQMessageBlockUserData::ZoneRange rangeLoginUA;
	rangeLoginUA.begin = cursor.positionInBlock();
	if( post->login().size() != 0 )
	{
		loginUaFormat.setForeground(QColor("#553333"));

		txt = post->login().left(12);
	}
	else if( post->UA().size() != 0 )
	{
		loginUaFormat.setFontItalic(true);
		loginUaFormat.setForeground(QColor("#883333"));

		txt = post->UA().left(12);
	}
	else
	{
		loginUaFormat.setFontFamily("Monospace");
		loginUaFormat.setForeground(QColor("#BB3333"));

		txt = QString::fromAscii("$NO UA$");
	}

	//[:magic]
	if(txt.length() < 4)
		txt.append(QString::fromUtf8("   "));

	cursor.insertText(txt, loginUaFormat);
	rangeLoginUA.end = cursor.positionInBlock();

	data->setZRange(QQMessageBlockUserData::LOGINUA, rangeLoginUA);

	// marque la tabulation pour affichage
	cursor.insertText(QString::fromUtf8("\t"), defaultFormat);

	//message
	QQMessageBlockUserData::ZoneRange rangeMsg;
	rangeMsg.begin = cursor.positionInBlock();
	cursor.insertHtml(post->message());
	rangeMsg.end = cursor.positionInBlock();
	data->setZRange(QQMessageBlockUserData::MESSAGE, rangeMsg);
	block.setUserData(data);

	if(block.userData() == NULL)
		qCritical() << "uxam";
}

void QQPinipede::newPostsAvailable(QString groupName)
{
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQPinipede::newPostsAvailable from : " << groupName;

	//On est obligé de locker pour éviter la pagaille dans le pini.
	// un locking plus fin pourrait être obtenu en implémentant un lock par groupe
	while(! newPostsAvailableMutex.tryLock(1000))
		qWarning() << "newPostsAvailable " << groupName << "tryLock timeout";

	QList<QQPost *> newPosts;

	QListIterator<QQBouchot *> i(m_settings->listBouchots(groupName));
	while(i.hasNext())
		newPosts.append(i.next()->takeNewPosts());

	// Au cas ou on serait deja passe avant (cas du signal multiple)
	if(newPosts.size() == 0)
		return;

	//Il ne sert a rien d'insérer plus que de posts que le max de l'historique
	while(newPosts.size() > (int) m_settings->maxHistoryLength())
		newPosts.removeFirst();

	// Tri necessaire puisqu'on a potentiellement melange les posts de plusieurs tribunes
	qSort(newPosts.begin(), newPosts.end(), postComp);

	QQTextBrowser * textBrowser = m_textBrowserHash.value(groupName);
	//On signale via la forme de la souris qu'un traitement est en cours
	textBrowser->viewport()->setCursor(Qt::BusyCursor);
	QTextDocument * doc = textBrowser->document();

	QTextCursor cursor(doc);
	cursor.beginEditBlock();

	bool wasAtEnd = ( textBrowser->verticalScrollBar()->sliderPosition() == textBrowser->verticalScrollBar()->maximum() );
	// Recuperation de l'historique des posts (ou creation si absent)
	QList<QQPost *> * destlistPosts = NULL;
	if(! m_listPostsTabMap.contains(groupName))
	{
		// Cas du pini vide, il contient déjà un bloc vide, on
		//  a juste a afficher le premier post;
		QQPost * firstPost = newPosts.takeFirst();
		destlistPosts = new QList<QQPost *>();
		destlistPosts->append(firstPost);
		m_listPostsTabMap.insert(groupName, destlistPosts);
		printPostAtCursor(cursor, firstPost);

		// Si le pini était vide on scrolle forcément vers le dernier post
		wasAtEnd = true;
	}
	else
		destlistPosts = m_listPostsTabMap[groupName];

	int newPostsIndex = 0, baseInsertIndex = 0, insertIndex = 0;
	// Tant qu'il reste des posts a afficher
	while(newPostsIndex < newPosts.size())
	{
		QQPost * newPost = newPosts.at(newPostsIndex);

		insertIndex = insertPostToList( destlistPosts, newPost, baseInsertIndex );

		if(newPost == destlistPosts->last()) //insertion a la fin
			break;

		//Deplacement vers la nouvelle ligne
		if(insertIndex == 0)
		{
			QQMessageBlockUserData * uData = (QQMessageBlockUserData *) cursor.block().userData();
			cursor.insertBlock();
			cursor.block().setUserData(uData);
			cursor.movePosition(QTextCursor::PreviousBlock);
			printPostAtCursor(cursor, newPost);
		}
		else
		{
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, ( insertIndex - baseInsertIndex ) - 1);
			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.insertBlock();
			printPostAtCursor(cursor, newPost);
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
		cursor.insertBlock();
		printPostAtCursor(cursor, newPosts.at(newPostsIndex++));

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
			cursor.insertBlock();
			printPostAtCursor(cursor, newPosts.at(newPostsIndex++));
		}
	}

	cursor.endEditBlock();

	// Purge des anciens messages
	purgePinitabHistory(groupName);

	if(wasAtEnd)
		textBrowser->verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );

	//Remise en place de l'ancienne forme du pointeur
	textBrowser->viewport()->setCursor(Qt::ArrowCursor);

	newPostsAvailableMutex.unlock();

}


unsigned int QQPinipede::insertPostToList(QList<QQPost *> *listPosts, QQPost *post, unsigned int indexStart)
{
	// qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
	//		 << "QQPinipede::insertPostToList, indexStart=" << indexStart;
	for( int i = indexStart; i < listPosts->size(); i++ )
	{
		// qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
		//		 << "QQPinipede::insertPostToList, listPosts->at(i)->norloge()=" << listPosts->at(i)->norloge()
		//		 << ", post->norloge()=" << post->norloge();
		if(listPosts->at(i)->norloge().toLongLong() > post->norloge().toLongLong() )
		{
			// qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			//		 << "QQPinipede::insertPostToList, listPosts->insert i=" << i;
			listPosts->insert(i, post);
			return i;
		}
		// Gestion de l'index de norloge multiple
		else if(listPosts->at(i)->norloge().toLongLong() == post->norloge().toLongLong() &&
				listPosts->at(i)->bouchot()->name().compare(post->bouchot()->name()) == 0)
		{
			listPosts->at(i)->setNorlogeMultiple(true);
			post->incrIndex();
		}
	}
	listPosts->append(post);
	return listPosts->size() - 1;
}


void QQPinipede::norlogeClicked(QQNorloge norloge)
{
	emit insertTextPalmi(norloge.toStringPalmi() + QString::fromAscii(" "));
}

void QQPinipede::norlogeRefHovered(QQNorlogeRef norlogeRef)
{
	QString dstBouchot = norlogeRef.dstBouchot();
	QString dstNorloge = norlogeRef.dstNorloge();

	qDebug() << "QQPinipede::norlogeRefHovered, datetimepart=" << dstNorloge << ", destbouchot=" << dstBouchot;

	QQBouchot * bouchot = m_settings->bouchot(dstBouchot);

	if(bouchot == NULL)
	{
		qCritical() << "QQPinipede::norlogeRefHovered m_settings->bouchot(dstBouchot) a retourne NULL";
		return;
	}

	QQTextBrowser* textBrowser = m_textBrowserHash.value(bouchot->settings().group());

	bool highlightSuccess = false;

	if(m_tBrowserHighlighted != NULL)
		unHighlight();

	if(textBrowser->isVisible())
	{
		m_tBrowserHighlighted = textBrowser;
		QQSyntaxHighlighter * highlighter = textBrowser->document()->findChildren<QQSyntaxHighlighter *>().at(0);
		highlighter->setNorlogeRefToHighlight(norlogeRef);

		// Get the cursor position near the top left corner of the current viewport.
		QTextCursor cursor = textBrowser->cursorForPosition(QPoint(0, 0));
		// Get the cursor position near the bottom left corner of the current viewport.
		int endBlockPos = (textBrowser->cursorForPosition(QPoint(textBrowser->viewport()->width(), textBrowser->viewport()->height()))).blockNumber();

		do
		{
			highlighter->rehighlightBlock(cursor.block());
			if(cursor.block().userState() == QQSyntaxHighlighter::FULL_HIGHLIGHTED)
				highlightSuccess = true;
		} while ( cursor.movePosition(QTextCursor::NextBlock) &&
				  cursor.blockNumber() <= endBlockPos );
	}

	if( ! highlightSuccess )
	{
		QTextCursor cursor(textBrowser->document());

		QTextDocument destDocument;
		QTextCursor destCursor(& destDocument);
		destCursor.atStart();

		destCursor.beginEditBlock();
		do
		{
			// qDebug() << "QQPinipede::norlogeRefHovered cursor.blockNumber()=" << cursor.blockNumber()
			//		 << ", mainTable->rows()=" << mainTable->rows()
			//		 << ", mainTable->columns()=" << mainTable->columns();
			if(cursor.block().userState() == QQSyntaxHighlighter::NOT_HIGHLIGHTED)
				continue;
			QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData());

			QString currNorloge = userData->post()->norloge();
			QQBouchot * currBouchot = userData->post()->bouchot();

			if( ( dstBouchot == currBouchot->name() || currBouchot->settings().containsAlias(dstBouchot) ) &&
					currNorloge.startsWith(dstNorloge) )
			{
				qDebug() << "QQPinipede::norlogeRefHovered cursor.blockNumber()=" << cursor.blockNumber();
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				QTextDocumentFragment fragment = cursor.selection();
				destCursor.insertFragment(fragment);
			}

		} while ( cursor.movePosition(QTextCursor::NextBlock) );

		destCursor.endEditBlock();

		if( destDocument.toPlainText().length() > 0)
		{
			m_hiddenPostViewerLabel->setFixedWidth(this->currentWidget()->width());
			QString styleSheet = m_hiddenPostViewerLabelSSheet;
			styleSheet.append("background-color: ")
					.append(bouchot->settings().colorLight().name())
					.append(";");
			m_hiddenPostViewerLabel->setStyleSheet(styleSheet);
			m_hiddenPostViewerLabel->setText(destDocument.toHtml());
			m_hiddenPostViewerLabel->show();
		}
	}
}

void QQPinipede::unHighlight()
{
	//qDebug() << "QQPinipede::unHighlight";

	if(m_tBrowserHighlighted == NULL)
		return;

	qDebug() << "QQPinipede::unHighlight, m_tBrowserHighlighted not NULL";

	m_hiddenPostViewerLabel->hide();

	QTextCursor cursor( m_tBrowserHighlighted->document() );
	QQSyntaxHighlighter * highlighter = m_tBrowserHighlighted->document()->findChildren<QQSyntaxHighlighter *>().at(0);
	highlighter->setNorlogeRefToHighlight(QQNorlogeRef());

	do
	{
		if(cursor.block().userState() == QQSyntaxHighlighter::NORLOGE_HIGHLIGHTED ||
				cursor.block().userState() == QQSyntaxHighlighter::FULL_HIGHLIGHTED)
			highlighter->rehighlightBlock(cursor.block());

	} while ( cursor.movePosition( QTextCursor::NextBlock ) );

	m_tBrowserHighlighted = NULL;
}

void QQPinipede::showTotozSlot(QQTotoz & totoz)
{
	m_totozMovie = new QMovie(totoz.getPath());
	if( m_totozMovie->isValid() )
	{
		m_totozMovie->setCacheMode(QMovie::CacheNone);
		m_totozMovie->jumpToFrame(0);
		m_totozLabel->setMovie(m_totozMovie);
		m_totozLabel->resize(m_totozMovie->frameRect().size());
		m_totozLabel->move(mapFromGlobal(QCursor::pos()));
		m_totozMovie->start();
		m_totozLabel->show();
	}
	else
	{
		delete m_totozMovie;
		m_totozMovie = NULL;
		QImage image;
		if(image.load(totoz.getPath()))
		{
			QPixmap pixmap = QPixmap::fromImage(image);
			m_totozLabel->setPixmap(pixmap);
			m_totozLabel->setMinimumSize(pixmap.width(), pixmap.height());
			m_totozLabel->adjustSize();
			m_totozLabel->move(mapFromGlobal(QCursor::pos()));
			m_totozLabel->show();
		}
	}
}

void QQPinipede::hideTotozSlot()
{
	if(m_totozMovie != NULL)
	{
		m_totozLabel->hide();
		delete m_totozMovie;
		m_totozMovie = NULL;
	}
}

void QQPinipede::loginClicked(QString tabGroupName)
{
	// Pour éviter le warning
	(void) tabGroupName;
	//QQTextBrowser *qqTBSender = (QQTextBrowser *)sender();
}

QQPost * QQPinipede::getPostForGroup(QString &groupName, int numPost)
{
	if(m_listPostsTabMap.contains(groupName))
	{
		QList<QQPost *> *listPosts = m_listPostsTabMap[groupName];
		if(listPosts->size() > numPost)
			return listPosts->at(numPost);
	}

	return NULL;
}

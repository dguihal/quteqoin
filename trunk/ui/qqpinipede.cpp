#include "qqpinipede.h"

#include "mainwindow.h"
#include "core/qqbouchot.h"
#include "ui/qqpalmipede.h"
#include "ui/qqsyntaxhighlighter.h"
#include "ui/qqtextbrowser.h"
#include "ui/qqmessageblockuserdata.h"

#include <QBrush>
#include <QDebug>
#include <QScrollBar>
#include <QTabBar>
#include <QCursor>
#include <QTextBrowser>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextFormat>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTime>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWidget>

QQPinipede::QQPinipede(QQSettings *settings, QWidget *parent) :
	QTabWidget(parent)
{
	this->tabBar()->hide();

	m_settings = settings;
}

QQPinipede::~QQPinipede()
{
}

void QQPinipede::addPiniTab(const QString& groupName)
{
	qDebug() << "QQPinipede::addPiniTab" ;

	if (this->m_textBrowserHash.value(groupName) != NULL)
		return;

	QWidget *widget = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(widget);

	QQTextBrowser *textBrowser = new QQTextBrowser(groupName, this);
	textBrowser->document()->setUndoRedoEnabled(false);
	textBrowser->document()->setDocumentMargin(0);
	QScrollBar *vScrollBar = textBrowser->verticalScrollBar();
	vScrollBar->setSliderPosition( vScrollBar->maximum() );

	layout->addWidget(textBrowser);

	this->addTab(widget, groupName);

	m_textBrowserHash.insert(groupName, textBrowser);
	//textBrowser->document() devient le proprietaire du highlighter
	QQSyntaxHighlighter * highlighter = new QQSyntaxHighlighter(textBrowser->document());
	// Pour éviter le warning
	(void) highlighter;

	qDebug() << "QQPinipede::addPiniTab this->m_textBrowserHash.size()=" << this->m_textBrowserHash.size();

	connect(textBrowser, SIGNAL(norlogeClicked(QQNorloge)), this, SLOT(norlogeClicked(QQNorloge)));
	connect(textBrowser, SIGNAL(norlogeRefHovered(QQNorlogeRef)), this, SLOT(norlogeRefHovered(QQNorlogeRef)));
	connect(textBrowser, SIGNAL(unHighlight()), this, SLOT(unHighlight()));

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

	int count = 0;

	QTextFrame* root = textBrowser->document()->rootFrame();
	QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
	QTextCursor cursor = textBrowser->textCursor();
	QTextTableCell rowStart, rowEnd;
	cursor.beginEditBlock();

	for(int row = 0; row < mainTable->rows(); )
	{
		qDebug() << "QQPinipede::purgePiniTab row=" << row << ", mainTable->rows()=" << mainTable->rows();
		rowStart = mainTable->cellAt(row, 0);
		rowEnd = mainTable->cellAt(row, mainTable->columns() - 1);

		cursor.setPosition(rowStart.firstCursorPosition().position());
		cursor.setPosition(rowEnd.lastCursorPosition().position(), QTextCursor::KeepAnchor);
		QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData());
		qDebug() << "QQPinipede::purgePiniTab userData->post()->bouchot()->name()=" << userData->post()->bouchot()->name() << ", bouchotName=" << bouchotName;
		if ( userData->post()->bouchot()->name().compare(bouchotName) == 0 )
		{
			cursor.removeSelectedText();
			mainTable->removeRows(row, 1);
			if (count >= max)
				break;
		}
		else
			row++;
	}

	cursor.endEditBlock();
}

void QQPinipede::createQTextTable( QQTextBrowser* textBrowser, int numRow )
{
	QTextFrame* root = textBrowser->document()->rootFrame();

	QTextCursor cursor = root->firstCursorPosition();
	QTextTableFormat tableFormat;
	tableFormat.setBorder(0);
	tableFormat.setMargin(0);
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(0);
	tableFormat.setLeftMargin(0);
	tableFormat.setRightMargin(0);
	tableFormat.setTopMargin(0);
	tableFormat.setBottomMargin(0);
	tableFormat.setPosition(QTextFrameFormat::InFlow);
	QVector<QTextLength> columnWidthConstraints = tableFormat.columnWidthConstraints();
	columnWidthConstraints.clear();
	columnWidthConstraints << QTextLength(QTextLength::FixedLength, 20.0)
						   << QTextLength(QTextLength::FixedLength, 70.0)
						   << QTextLength(QTextLength::FixedLength, 70.0)
						   << QTextLength(QTextLength::VariableLength, 0.0);
	tableFormat.setColumnWidthConstraints(columnWidthConstraints);
	cursor.insertTable( numRow, 4, tableFormat)->columns();

	textBrowser->verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );
}

void QQPinipede::printPostAtCursor( QTextCursor & cursor, QQPost * post )
{
	QTextCharFormat cellMarkColorFormat;
	cellMarkColorFormat.setBackground(post->bouchot()->settings().color());

	QTextCharFormat defaultFormat;

	//Post / Reply
	QTextTableCell cell = cursor.currentTable()->cellAt(cursor);

	//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
	//		 << "Cell 1 : row=" << cell.row() << ", column=" << cell.column();

	cell.setFormat(cellMarkColorFormat);

	//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
	//		 << "post->login() = " << post->login()
	//		 << ", post->UA() = " << post->UA();

	if( post->isSelfPost())
	{
		QFont defautFont = cellMarkColorFormat.font();
		QFontMetrics fontMetrics = QFontMetrics(defautFont);
		cursor.insertImage(QImage(QString::fromAscii(":/img/Fleche_verte.svg")).scaledToHeight(fontMetrics.height() - 2));
	}
	else
	{
		cursor.insertText(QString::fromUtf8(" "));
	}

	cursor.movePosition(QTextCursor::NextCell);

	QColor baseBgColor = post->bouchot()->settings().colorLight();
	cellMarkColorFormat.setBackground(baseBgColor);

	//norloge
	cell = cursor.currentTable()->cellAt(cursor);

	//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
	//		 << "Cell 2 : row=" << cell.row() << ", column=" << cell.column();

	cell.setFormat(cellMarkColorFormat);

	QTextCharFormat norlogeFormat;
	norlogeFormat.setFontWeight(QFont::Bold);

	QTextBlock block = cursor.block();
	QQMessageBlockUserData * data = new QQMessageBlockUserData();
	data->setPost(post);
	data->setBlockZone(QQMessageBlockUserData::NORLOGE_ZONE);
	block.setUserData(data);

	cursor.insertText(post->norlogeFormatee(), norlogeFormat);
	cursor.insertText(QString::fromUtf8(" "), defaultFormat);

	cursor.movePosition(QTextCursor::NextCell);

	//login ou ua
	cell = cursor.currentTable()->cellAt(cursor);

	//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
	//		 << "Cell 3 : row=" << cell.row() << ", column=" << cell.column();

	cell.setFormat(cellMarkColorFormat);

	QTextCharFormat loginUaFormat;
	QString txt;
	if( post->login().size() != 0 )
	{
		loginUaFormat.setForeground(QColor("#553333"));

		txt = post->login();
	}
	else
	{
		loginUaFormat.setFontItalic(true);
		loginUaFormat.setForeground(QColor("#883333"));

		txt = post->UA().left(12);
	}
	block = cursor.block();
	data = new QQMessageBlockUserData();
	data->setPost(post);
	data->setBlockZone(QQMessageBlockUserData::LOGIN_UA_ZONE);
	block.setUserData(data);

	cursor.insertText(txt, loginUaFormat);

	cursor.insertText(QString::fromUtf8(" "), defaultFormat);

	cursor.movePosition(QTextCursor::NextCell);

	//message
	cell = cursor.currentTable()->cellAt(cursor);

	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "Cell 4 : row=" << cell.row() << ", column=" << cell.column()
			 << ", Message=" << post->message();

	cell.setFormat(cellMarkColorFormat);

	block = cursor.block();
	data = new QQMessageBlockUserData();
	data->setPost(post);
	data->setBlockZone(QQMessageBlockUserData::MESSAGE_ZONE);
	block.setUserData(data);
	cursor.insertHtml(post->message());
	cursor.insertText(QString::fromUtf8(" "), defaultFormat);

}

void QQPinipede::newPostsAvailable(QQBouchot *sender)
{
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQPinipede::newPostsAvailable";

	if(sender == NULL)
		return;

	//On est obligé de locker pour éviter la pagaille dans le pini.
	// un locking plus fin pourrait être obtenu en implémentant un lock par groupe
	while(! newPostsAvailableMutex.tryLock(1000))
		qWarning() << "newPostsAvailable " << sender->name() << "tryLock timeout";

	QList<QQPost *> newPosts = sender->getNewPosts();
	QList<QQPost *> *destlistPosts = m_listPostsTabMap[sender->settings().group()];

	QTime time = QTime::currentTime();
	time.start();

	QQTextBrowser* textBrowser = m_textBrowserHash.value(sender->settings().group());
	QTextFrame* root = textBrowser->document()->rootFrame();

	if(root->childFrames().size() == 0)
	{
		qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
				 << "QQPinipede, inserting " << newPosts.size() << " posts";
		destlistPosts = new QList<QQPost *>(newPosts);
		m_listPostsTabMap.insert(sender->settings().group(), destlistPosts);

		createQTextTable(textBrowser, newPosts.size());

		QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
		QTextCursor cursor = mainTable->cellAt(0, 0).firstCursorPosition();

		cursor.beginEditBlock();

		int i = 0;
		while(i < newPosts.size())
		{
			// Gestion de l'index de norloge multiple
			if(i > 0 &&
					newPosts.at(i)->norloge().toLongLong() == newPosts.at(i - 1)->norloge().toLongLong() &&
					newPosts.at(i)->bouchot()->name().compare(newPosts.at(i - 1)->bouchot()->name()) == 0)
			{
				newPosts.at(i - 1)->setNorlogeMultiple(true);
				newPosts.at(i)->setNorlogeIndex(newPosts.at(i - 1)->norlogeIndex() + 1);
			}

			qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
					 << "QQPinipede, appending post " << i << "/" << newPosts.length();
			printPostAtCursor(cursor, newPosts.at(i++));
			cursor.movePosition(QTextCursor::NextRow);
		}
		cursor.endEditBlock();

		textBrowser->verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );
	}
	else
	{
		qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
				 << "QQPinipede inserting posts : newPosts.size=" << newPosts.size()
				 << ", destlistPosts->size=" << destlistPosts->size();

		bool wasAtEnd = false;
		if( textBrowser->verticalScrollBar()->sliderPosition() == textBrowser->verticalScrollBar()->maximum() )
			wasAtEnd = true;

		QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
		QTextCursor cursor = mainTable->cellAt(0, 0).firstCursorPosition();

		cursor.beginEditBlock();

		int newPostsIndex = 0;

		if(newPosts.at(newPostsIndex)->norloge().toLongLong() < destlistPosts->at(0)->norloge().toLongLong())
		{
			QQPost * newPost = newPosts.at(newPostsIndex);

			mainTable->insertRows(0, 1);
			//ligne précedente + 3 colonnes
			cursor.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 4);

			printPostAtCursor(cursor, newPost);
			destlistPosts->insert(0, newPost);

			newPostsIndex++;
		}

		int baseInsertIndex = 0, insertIndex = 0;

		while(newPostsIndex < newPosts.size())
		{
			QQPost * newPost = newPosts.at(newPostsIndex);

			insertIndex = insertPostToList( destlistPosts, newPost, baseInsertIndex );
			qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
					 << "QQPinipede::newPostsAvailable insertIndex=" << insertIndex
					 << ", destlistPosts->size()=" << destlistPosts->size();

			if(newPost == destlistPosts->last()) //insertion a la fin
				break;

			//Deplacement vers l'element suivant la ligne qu'on va inserer
			cursor.movePosition(QTextCursor::NextRow, QTextCursor::MoveAnchor, insertIndex - baseInsertIndex );
			QTextTableCell cell = cursor.currentTable()->cellAt(cursor);
			qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
					 << "Cell X1 : row=" << cell.row() << ", column=" << cell.column();

			//Creation de la ligne
			mainTable->insertRows(insertIndex, 1);
			cell = cursor.currentTable()->cellAt(cursor);
			qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
					 << "Cell X2 : row=" << cell.row() << ", column=" << cell.column();

			//Deplacement vers le début de la nouvelle ligne
			cursor.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 4);
			printPostAtCursor(cursor,newPost);

			newPostsIndex++;
			baseInsertIndex = insertIndex;
		}

		if(newPostsIndex < newPosts.size())
		{
			mainTable->appendRows(newPosts.size() - newPostsIndex);
			cursor.movePosition(QTextCursor::NextRow, QTextCursor::MoveAnchor, insertIndex - baseInsertIndex );
			//Le premier item a déjà  été inséré dans la liste destlistPosts dans la boucle while au dessus
			//on a juste a l'afficher
			printPostAtCursor(cursor, newPosts.at(newPostsIndex++));
			cursor.movePosition(QTextCursor::NextRow);

			while(newPostsIndex < newPosts.size())
			{
				// Gestion de l'index de norloge multiple
				if(newPosts.at(newPostsIndex)->norloge().toLongLong() == destlistPosts->last()->norloge().toLongLong() &&
						newPosts.at(newPostsIndex)->bouchot()->name().compare(destlistPosts->last()->bouchot()->name()) == 0)
				{
					destlistPosts->last()->setNorlogeMultiple(true);
					newPosts.at(newPostsIndex)->setNorlogeIndex(destlistPosts->last()->norlogeIndex() + 1);
				}

				qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
						 << "QQPinipede, appending post " << newPostsIndex << "/" << newPosts.length();

				destlistPosts->append(newPosts.at(newPostsIndex));
				printPostAtCursor(cursor, newPosts.at(newPostsIndex++));
				cursor.movePosition(QTextCursor::NextRow);
			}
		}
		cursor.endEditBlock();

		if(wasAtEnd)
			textBrowser->verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );
	}

	//TODO : insérer ici la purge des anciens messages
	//Fin TODO

	newPostsAvailableMutex.unlock();
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "Time taken is: " << time.elapsed() << " ms";
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

	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "norlogeRefHovered, datetimepart=" << dstNorloge << ", destbouchot=" << dstBouchot;

	QQBouchot * bouchot = m_settings->bouchot(dstBouchot);
	QQTextBrowser* textBrowser = m_textBrowserHash.value(bouchot->settings().group());

	bool highlightSuccess = false;

	if( m_tBrowserHighlighted != NULL )
		unHighlight();

	if(textBrowser->isVisible())
	{
		m_tBrowserHighlighted = textBrowser;
		QQSyntaxHighlighter * highlighter = textBrowser->document()->findChildren<QQSyntaxHighlighter *>().at(0);
		highlighter->setNorlogeRefToHighlight(norlogeRef);

		QTextCursor cursor = textBrowser->cursorForPosition(QPoint(0, 0)); //get the cursor position near the top left corner of the current viewport.
		QTextCursor endPosition = textBrowser->cursorForPosition(QPoint(textBrowser->viewport()->width(), textBrowser->viewport()->height())); //get the cursor position near the bottom left corner of the current viewport.
		// qDebug() << "QQPinipede::norlogeRefHovered from position: " << cursor.blockNumber()
		//				<< " to " << endPosition.blockNumber();

		cursor.beginEditBlock();

		bool moveSuccess = cursor.movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor, 1);
		do
		{
			QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData());
			moveSuccess &= cursor.movePosition(QTextCursor::NextCell, QTextCursor::KeepAnchor, 2);

			if( ! highlighter->highlightLine(cursor, userData) )
			{
				cursor.clearSelection();
				highlighter->rehighlightMessageBlockAtCursor(cursor, dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData()));
			}
			else
			{
				highlightSuccess = true;
			}
			moveSuccess &= cursor.movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor, 2);
		} while ( moveSuccess && cursor.blockNumber() <= endPosition.blockNumber() );

		cursor.endEditBlock();
	}

	if( ! highlightSuccess )
	{

		QTextFrame* root = textBrowser->document()->rootFrame();
		QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
		QTextCursor cursor = mainTable->cellAt(0, 0).firstCursorPosition();

		QTextDocument destDocument;
		QTextCursor destCursor(& destDocument);
		destCursor.atStart();

		destCursor.beginEditBlock();

		bool moveSuccess = cursor.movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor, 1);
		do
		{
			// qDebug() << "QQPinipede::norlogeRefHovered cursor.blockNumber()=" << cursor.blockNumber()
			//		 << ", mainTable->rows()=" << mainTable->rows()
			//		 << ", mainTable->columns()=" << mainTable->columns();
			QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData());

			QString currNorloge = userData->post()->norloge();
			QString currBouchot = userData->post()->bouchot()->name();

			if( currBouchot.compare(dstBouchot, Qt::CaseInsensitive) == 0 &&
					currNorloge.startsWith(dstNorloge) )
			{
				moveSuccess &= cursor.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 1);
				moveSuccess &= cursor.movePosition(QTextCursor::NextCell, QTextCursor::KeepAnchor, 3);
				qDebug() << "QQPinipede::norlogeRefHovered cursor.blockNumber()=" << cursor.blockNumber();
				QTextDocumentFragment fragment = cursor.selection();
				destCursor.insertFragment(fragment);
			}
			moveSuccess &= cursor.movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor, 4);

		} while ( moveSuccess && cursor.blockNumber() <= mainTable->rows() *mainTable->columns() );

		destCursor.endEditBlock();

		QPoint cursorPos = QCursor::pos();
		if( destDocument.toPlainText().length() > 0)
			QToolTip::showText(cursorPos, destDocument.toHtml(), this);
	}
}

void QQPinipede::unHighlight()
{
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQPinipede::unHighlight";

	if(m_tBrowserHighlighted == NULL)
		return;

	QToolTip::hideText();

	QTextFrame* root = m_tBrowserHighlighted->document()->rootFrame();
	QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
	QTextCursor cursor = mainTable->cellAt(0, 1).firstCursorPosition();

	cursor.beginEditBlock();

	bool moveSuccess = true;
	while(moveSuccess == true)
	{
		QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData());
		moveSuccess &= cursor.movePosition(QTextCursor::NextCell, QTextCursor::KeepAnchor, 2);
		if( userData->isHighlighted() )
		{
			QTextCharFormat format;
			Q_ASSERT(userData->post() != NULL);
			format.setBackground(userData->post()->bouchot()->settings().colorLight());
			cursor.mergeBlockCharFormat(format);
			userData->resetHighlighted();
		}
		else
		{
			cursor.clearSelection();
			userData = dynamic_cast<QQMessageBlockUserData *>(cursor.block().userData());

			if( userData->isHighlighted() )
			{
				QTextCharFormat format;
				format.setBackground(QBrush(Qt::NoBrush));
				cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
				cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
				cursor.mergeCharFormat(format);
			}
		}

		moveSuccess &= cursor.movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor, 2);
	}

	cursor.endEditBlock();

	m_tBrowserHighlighted = NULL;
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

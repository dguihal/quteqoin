#include "qqpinipede.h"

#include "mainwindow.h"
#include "core/qqbouchot.h"
#include "core/qqtextcharformat.h"
#include "core/qqmessageparser.h"
#include "ui/qqpalmipede.h"
#include "ui/qqtextbrowser.h"

#include <QBrush>
#include <QDebug>
#include <QScrollBar>
#include <QTabBar>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextFormat>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTime>
#include <QVBoxLayout>
#include <QWidget>

QQPinipede::QQPinipede(QWidget* parent) :
    QTabWidget(parent)
{
    this->tabBar()->hide();
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
    QScrollBar *vScrollBar = textBrowser->verticalScrollBar();
    vScrollBar->setSliderPosition( vScrollBar->maximum() );

    layout->addWidget(textBrowser);

    this->addTab(widget, groupName);

    m_textBrowserHash.insert(groupName, textBrowser);
    qDebug() << "QQPinipede::addPiniTab this->m_textBrowserHash.size()=" << this->m_textBrowserHash.size();

    connect(textBrowser, SIGNAL(norlogeClicked(QQNorloge)), this, SLOT(norlogeClicked(QQNorloge)));

    if (this->count() > 1)
        this->tabBar()->show();
}

void QQPinipede::createPiniTabs(const QList<QString> &tabs)
{
    for (int i = 0; i < tabs.size(); i++)
        this->addPiniTab(tabs[i]);
}

void QQPinipede::removePiniTab(const QString& name)
{
    for (int i = 0; i < this->count(); i++)
        if (this->tabText(i) == name)
        {
            this->removeTab(i);
            break;
        }

    if (this->count() < 2)
        this->tabBar()->hide();
}

void QQPinipede::createQTextTableRows( QQTextBrowser* textBrowser, int pos, int num )
{
    QTextFrame* root = textBrowser->document()->rootFrame();

    if(root->childFrames().size() == 0)
    {
        QTextCursor cursor = root->lastCursorPosition();
        QTextTableFormat tableFormat;
        tableFormat.setBorder(0);
        tableFormat.setMargin(0);
        tableFormat.setCellSpacing(0);
        tableFormat.setCellPadding(0);
        QVector<QTextLength> columnWidthConstraints = tableFormat.columnWidthConstraints();
        columnWidthConstraints.clear();
        columnWidthConstraints << QTextLength(QTextLength::FixedLength, 20.0)
                               << QTextLength(QTextLength::FixedLength, 70.0)
                               << QTextLength(QTextLength::FixedLength, 70.0)
                               << QTextLength(QTextLength::VariableLength, 0.0);
        tableFormat.setColumnWidthConstraints(columnWidthConstraints);
        cursor.insertTable( num, 4, tableFormat)->columns();
    }
    else
    {
        QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
        if( pos < 0 || pos >= mainTable->rows() )
            mainTable->appendRows( num );
        else
            mainTable->insertRows( pos, num );
    }

    QScrollBar* vScrollBar = textBrowser->verticalScrollBar();
    vScrollBar->setSliderPosition( vScrollBar->maximum() );
}

void QQPinipede::printPostAtCursor( QTextCursor & cursor, QQPost * post )
{
    QTextCharFormat cellMarkColorFormat;
    cellMarkColorFormat.setBackground(post->bouchot()->settings().color());

    QTextCharFormat defaultFormat;

    //Post / Reply
    QTextTableCell cell = cursor.currentTable()->cellAt(cursor);
    cell.setFormat(cellMarkColorFormat);

    qDebug() << "post->login() = " << post->login()
             << ", post->UA() = " << post->UA();

    if( post->isSelfPost())
        cursor.insertImage(QImage(QString::fromAscii(":/img/Fleche_verte.svg")));

    cursor.movePosition(QTextCursor::NextCell);


    //qDebug() << "QQPinipede::printPostAtCursor : baseBgColor=" << post->bouchot()->color().name();
    //qDebug() << "QQPinipede::printPostAtCursor : baseBgColor=" << baseBgColor.name();
    QColor baseBgColor = post->bouchot()->settings().color().lighter(110);

    //norloge
    cell = cursor.currentTable()->cellAt(cursor);
    cell.setFormat(cellMarkColorFormat);

    QTextCharFormat cellNormalColorFormat;
    cellNormalColorFormat.setBackground(baseBgColor);

    QTextCharFormat norlogeFormat;
    norlogeFormat.setFontWeight(QFont::Bold);
    norlogeFormat.setObjectType(NorlogeTextFormat);
    norlogeFormat.setProperty(NorlogeData, post->norloge());
    norlogeFormat.setProperty(BouchotData, post->bouchot()->name());

    cursor.insertText(post->norlogeFormatee(), norlogeFormat);

    cursor.insertText(QString::fromLatin1(" "), defaultFormat);

    cursor.movePosition(QTextCursor::NextCell);

    //login ou ua
    cell = cursor.currentTable()->cellAt(cursor);
    cell.setFormat(cellMarkColorFormat);

    QTextCharFormat loginUaFormat;
    QString txt;
    if( post->login().size() != 0 )
    {
        loginUaFormat.setForeground(QColor("#553333"));
        loginUaFormat.setObjectType(LoginTextFormat);
        loginUaFormat.setProperty(LoginData, post->login());

        txt = post->login();
    }
    else
    {
        loginUaFormat.setFontItalic(true);
        loginUaFormat.setForeground(QColor("#883333"));
        loginUaFormat.setObjectType(UATextFormat);
        loginUaFormat.setProperty(UAData, post->UA());

        txt = post->UA().left(12);
    }
    cursor.insertText(txt, loginUaFormat);

    cursor.insertText(QString::fromLatin1(" "), defaultFormat);

    cursor.movePosition(QTextCursor::NextCell);

    //message
    cell = cursor.currentTable()->cellAt(cursor);
    cell.setFormat(cellMarkColorFormat);

    QQMessageParser parser;
    parser.parseMessage(post->message());
    QVector<QQMessageElt> messageElts = parser.messageElts();

    for(int i=0; i < messageElts.size(); ++i)
    {
        QQMessageElt elt = messageElts.at(i);
        switch(elt.type())
        {
        case QQMessageElt::Txt:
        {
            QString txt = elt.txt();
            if(txt[0].isSpace())
            {
                txt.remove(0, 1);
                txt.prepend(QString("&nbsp;"));
            }
            cursor.insertHtml(txt);
            break;
        }

        case QQMessageElt::Norloge:
        {
            QTextCharFormat norlogeMessageFormat;
            norlogeMessageFormat.setForeground(QColor("#0000DD"));
            norlogeMessageFormat.setObjectType(NorlogeTextFormat);
            norlogeMessageFormat.setProperty(NorlogeData, elt.txt());

            cursor.insertText(elt.txt(), norlogeMessageFormat);
            break;
        }

        case QQMessageElt::Totoz:
        {
            QTextCharFormat totozMessageFormat;
            totozMessageFormat.setForeground(QColor("#00AA11"));
            totozMessageFormat.setFontWeight(QFont::Bold);
            totozMessageFormat.setObjectType(TotozTextFormat);
            totozMessageFormat.setProperty(TotozData, elt.txt());

            cursor.insertText(elt.txt(), totozMessageFormat);
            break;
        }

        case QQMessageElt::Duck:
        {
            QTextDocument duck;
            duck.setHtml(elt.txt());

            QTextCharFormat duckMessageFormat;
            duckMessageFormat.setForeground(QColor("#9933CC"));
            duckMessageFormat.setObjectType(DuckTextFormat);
            // Inutile
            //totozMessageFormat.setProperty(DuckData, duck.toPlainText());

            cursor.insertText(duck.toPlainText(), duckMessageFormat);
            break;
        }

        default:
            qDebug() << "QQPinipede::printPostAtCursor Totoz !!!!!!!!!!! type : " << elt.type();
        }
    }

    cursor.insertText(QString::fromLatin1(" "), defaultFormat);

}

void QQPinipede::printPostsAtEnd( QList<QQPost *> & posts )
{
    if(posts.size() <= 0)
        return;

    QQTextBrowser* textBrowser = m_textBrowserHash.value(posts[0]->bouchot()->settings().group());
    QScrollBar* vScrollBar = textBrowser->verticalScrollBar();

    bool atEnd = false;
    if( vScrollBar->sliderPosition() == vScrollBar->maximum() )
        atEnd = true;

    createQTextTableRows( textBrowser, -1, posts.size() );

    QTextFrame* root = textBrowser->document()->rootFrame();

    QTextTable *mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));

    QTextCursor cursor = mainTable->cellAt(mainTable->rows() - posts.size(), 0).firstCursorPosition();
    QList<QQPost *>::iterator post;
    int i=0;
    cursor.beginEditBlock();
    for (post = posts.begin(); post != posts.end(); ++post)
    {
        qDebug() << "QQPinipede::printPostsAtEnd : post " << ++i << "/" << posts.length();
        printPostAtCursor(cursor, *post);
        cursor.movePosition(QTextCursor::NextRow);
    }
    cursor.endEditBlock();

    if( atEnd )
        vScrollBar->setSliderPosition( vScrollBar->maximum() );
}

void QQPinipede::printPostAt( QQPost * post, int pos )
{
    QQTextBrowser* textBrowser = m_textBrowserHash.value(post->bouchot()->settings().group());

    createQTextTableRows( textBrowser, pos, 1 );

    QTextFrame* root = textBrowser->document()->rootFrame();

    QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));

    QTextCursor cursor = mainTable->cellAt(pos, 0).firstCursorPosition();
    printPostAtCursor(cursor, post);
}

QTextDocument* QQPinipede::document(const QString& name)
{
    QQTextBrowser* textBrowser = m_textBrowserHash.value(name);
    return textBrowser->document();
}

void QQPinipede::newPostsAvailable(QQBouchot *sender)
{
    qDebug() << "QQDisplayBackend::newPostsAvailable";

    if(sender == NULL)
        return;

    QList<QQPost *> newPosts = sender->getNewPosts();

    QList<QQPost *> *destlistPosts = m_listPostsTabMap[sender->settings().group()];

    QTime time = QTime::currentTime();
    time.start();

    if(destlistPosts == NULL)
    {
        destlistPosts = new QList<QQPost *>(newPosts);
        m_listPostsTabMap.insert(sender->settings().group(), destlistPosts);
        printPostsAtEnd(* destlistPosts);
    }
    else
    {
        qDebug() << "QQDisplayBackend::newPostsAvailable : newPosts.size=" << newPosts.size()
                 << ", destlistPosts->size=" << destlistPosts->size();

        int lastInsert = 0;
        //on insère en commençant par le plus ancien

        while( newPosts.size() > 0)
        {
            QQPost* post = newPosts.takeFirst();
            lastInsert = insertPostToList( destlistPosts, post, lastInsert );
            printPostAt( post, lastInsert );
            if( lastInsert == destlistPosts->size() - 1 )
                break;
        }
        destlistPosts->append(newPosts);
        printPostsAtEnd(newPosts);

    }
    qDebug() << "Time taken is: " << time.elapsed() << " ms";
}


unsigned int QQPinipede::insertPostToList(QList<QQPost *> *listPosts, QQPost *post, unsigned int indexStart)
{
    qDebug() << "QQDisplayBackend::insertPostToList, indexStart=" << indexStart;
    for( int i = indexStart; i < listPosts->size(); i++ )
    {
        qDebug() << "QQDisplayBackend::insertPostToList, listPosts->at(i)->norloge()=" << listPosts->at(i)->norloge()
                 << ", post->norloge()=" << post->norloge();
        if(listPosts->at(i)->norloge().toLongLong() > post->norloge().toLongLong() )
        {
            listPosts->insert(i, post);
            return i;
        }
    }
    listPosts->append(post);
    return listPosts->size() - 1;
}


void QQPinipede::norlogeClicked(QQNorloge norloge)
{
    emit insertTextPalmi(norloge.toStringPalmi());
}

void QQPinipede::loginClicked(QString tabGroupName)
{
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

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

void QQPinipede::createQTextTable( QQTextBrowser* textBrowser, int numRow )
{
    QTextFrame* root = textBrowser->document()->rootFrame();

    QTextCursor cursor = root->firstCursorPosition();
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

    qDebug() << "Cell 1 : row=" << cell.row() << ", column=" << cell.column();

    cell.setFormat(cellMarkColorFormat);

    qDebug() << "post->login() = " << post->login()
             << ", post->UA() = " << post->UA();

    if( post->isSelfPost())
        cursor.insertImage(QImage(QString::fromAscii(":/img/Fleche_verte.svg")));

    cursor.movePosition(QTextCursor::NextCell);


    //qDebug() << "QQPinipede::printPostAtCursor : baseBgColor=" << post->bouchot()->color().name();
    //qDebug() << "QQPinipede::printPostAtCursor : baseBgColor=" << baseBgColor.name();
    QColor baseBgColor = post->bouchot()->settings().colorLight();
    cellMarkColorFormat.setBackground(baseBgColor);

    //norloge
    cell = cursor.currentTable()->cellAt(cursor);

    qDebug() << "Cell 2 : row=" << cell.row() << ", column=" << cell.column();

    cell.setFormat(cellMarkColorFormat);

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

    qDebug() << "Cell 3 : row=" << cell.row() << ", column=" << cell.column();

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

    qDebug() << "Cell 4 : row=" << cell.row() << ", column=" << cell.column();

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

void QQPinipede::newPostsAvailable(QQBouchot *sender)
{
    qDebug() << "QQDisplayBackend::newPostsAvailable";

    if(sender == NULL)
        return;

    QList<QQPost *> newPosts = sender->getNewPosts();

    QList<QQPost *> *destlistPosts = m_listPostsTabMap[sender->settings().group()];

    QTime time = QTime::currentTime();
    time.start();

    QQTextBrowser* textBrowser = m_textBrowserHash.value(sender->settings().group());
    QTextFrame* root = textBrowser->document()->rootFrame();

    if(root->childFrames().size() == 0)
    {
        destlistPosts = new QList<QQPost *>(newPosts);
        m_listPostsTabMap.insert(sender->settings().group(), destlistPosts);

        createQTextTable(textBrowser, newPosts.size());

        QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
        qDebug() << mainTable->rows();
        QTextCursor cursor = mainTable->cellAt(0, 0).firstCursorPosition();
        cursor.beginEditBlock();

        int i = 0;
        while(i < newPosts.size())
        {
            qDebug() << "QQPinipede, appending post " << i << "/" << newPosts.length();
            printPostAtCursor(cursor, newPosts.at(i++));
            cursor.movePosition(QTextCursor::NextRow);
        }
        cursor.endEditBlock();

        textBrowser->verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );
    }
    else
    {
        qDebug() << "QQPinipede inserting posts : newPosts.size=" << newPosts.size()
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
            mainTable->insertRows(0, 1);
            //ligne précedente + 3 colonnes
            cursor.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 4);

            printPostAtCursor(cursor, newPosts.at(newPostsIndex));
            destlistPosts->insert(0, newPosts.at(newPostsIndex));
            newPostsIndex++;
        }

        int baseInsertIndex = 0, insertIndex = 0;

        while(newPostsIndex < newPosts.size())
        {
            insertIndex = insertPostToList( destlistPosts, newPosts.at(newPostsIndex), baseInsertIndex );
            qDebug() << "QQPinipede::newPostsAvailable insertIndex=" << insertIndex
                     << ", destlistPosts->size()=" << destlistPosts->size();

            if(newPosts.at(newPostsIndex) == destlistPosts->last()) //insertion a la fin
                break;

            //Deplacement vers l'element suivant la ligne qu'on va inserer
            cursor.movePosition(QTextCursor::NextRow, QTextCursor::MoveAnchor, insertIndex - baseInsertIndex );
            QTextTableCell cell = cursor.currentTable()->cellAt(cursor);
            qDebug() << "Cell X1 : row=" << cell.row() << ", column=" << cell.column();

            //Creation de la ligne
            mainTable->insertRows(insertIndex, 1);
            cell = cursor.currentTable()->cellAt(cursor);
            qDebug() << "Cell X2 : row=" << cell.row() << ", column=" << cell.column();

            //Deplacement vers le début de la nouvelle ligne
            cursor.movePosition(QTextCursor::PreviousCell, QTextCursor::MoveAnchor, 4);
            printPostAtCursor(cursor, newPosts.at(newPostsIndex++));

            baseInsertIndex = insertIndex;
        }

        if(newPostsIndex < newPosts.size())
        {
            mainTable->appendRows(newPosts.size() - newPostsIndex);
            cursor.movePosition(QTextCursor::NextRow, QTextCursor::MoveAnchor, insertIndex - baseInsertIndex );

            while(newPostsIndex < newPosts.size())
            {
                qDebug() << "QQPinipede, appending post " << newPostsIndex << "/" << newPosts.length();
                destlistPosts->append(newPosts.at(newPostsIndex));
                printPostAtCursor(cursor, newPosts.at(newPostsIndex++));
                cursor.movePosition(QTextCursor::NextRow);
            }
        }
        cursor.endEditBlock();

        if(wasAtEnd)
            textBrowser->verticalScrollBar()->triggerAction( QAbstractSlider::SliderToMaximum );
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
            qDebug() << "QQDisplayBackend::insertPostToList, listPosts->insert i=" << i;
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

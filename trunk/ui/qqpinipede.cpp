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
    QScrollBar *vScrollBar = textBrowser->verticalScrollBar();
    vScrollBar->setSliderPosition( vScrollBar->maximum() );

    layout->addWidget(textBrowser);

    this->addTab(widget, groupName);

    m_textBrowserHash.insert(groupName, textBrowser);
    m_textBMsgHighlighterHash.insert(groupName, new QQSyntaxHighlighter(textBrowser));

    qDebug() << "QQPinipede::addPiniTab this->m_textBrowserHash.size()=" << this->m_textBrowserHash.size();

    connect(textBrowser, SIGNAL(norlogeClicked(QQNorloge)), this, SLOT(norlogeClicked(QQNorloge)));
    connect(textBrowser, SIGNAL(norlogeRefHovered(QQNorlogeRef)), this, SLOT(norlogeRefHovered(QQNorlogeRef)));
    connect(textBrowser, SIGNAL(unHighlight()), this, SLOT(unHighlight()));

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
    tableFormat.setPosition(QTextFrameFormat::InFlow);
    QVector<QTextLength> columnWidthConstraints = tableFormat.columnWidthConstraints();
    columnWidthConstraints.clear();
    columnWidthConstraints << QTextLength(QTextLength::VariableLength, 20.0)
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
    {
        QFont defautFont = cellMarkColorFormat.font();
        QFontMetrics fontMetrics = QFontMetrics(defautFont);
        cursor.insertImage(QImage(QString::fromAscii(":/img/Fleche_verte.svg")).scaledToHeight(fontMetrics.height() - 2));
    }
    else
    {
        cursor.insertText(QString::fromLatin1(" "));
    }

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

    QTextBlock block = cursor.block();
    QQMessageBlockUserData * data = new QQMessageBlockUserData();
    data->storeData(QQMessageBlockUserData::BOUCHOT_NAME, post->bouchot()->name());
    data->storeData(QQMessageBlockUserData::POST_NORLOGE, post->norloge());
    data->storeData(QQMessageBlockUserData::IS_NORLOGE_ZONE, true);
    block.setUserData(data);

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
    data->storeData(QQMessageBlockUserData::BOUCHOT_NAME, post->bouchot()->name());
    data->storeData(QQMessageBlockUserData::POST_NORLOGE, post->norloge());
    data->storeData(QQMessageBlockUserData::IS_LOGIN_UA_ZONE, true);
    block.setUserData(data);

    cursor.insertText(txt, loginUaFormat);

    cursor.insertText(QString::fromLatin1(" "), defaultFormat);

    cursor.movePosition(QTextCursor::NextCell);

    //message
    cell = cursor.currentTable()->cellAt(cursor);

    qDebug() << "Cell 4 : row=" << cell.row() << ", column=" << cell.column();

    cell.setFormat(cellMarkColorFormat);

    block = cursor.block();
    data = new QQMessageBlockUserData();
    data->storeData(QQMessageBlockUserData::BOUCHOT_NAME, post->bouchot()->name());
    data->storeData(QQMessageBlockUserData::POST_NORLOGE, post->norloge());
    data->storeData(QQMessageBlockUserData::IS_MESSAGE_ZONE, true);
    block.setUserData(data);
    cursor.insertHtml(post->message());
    cursor.insertText(QString::fromLatin1(" "), defaultFormat);

}

void QQPinipede::newPostsAvailable(QQBouchot *sender)
{
    qDebug() << "QQDisplayBackend::newPostsAvailable";

    if(sender == NULL)
        return;

    QList<QQPost *> newPosts = sender->getNewPosts();

    //On est obligé de locker pour éviter la pagaille dans le pini.
    // un locking plus fin pourrait être obtenu en implémentant un lock par groupe
    while(! newPostsAvailableMutex.tryLock(1000))
        qWarning() << "newPostsAvailable " << sender->name() << "tryLock timeout";

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
            qDebug() << "QQPinipede::newPostsAvailable insertIndex=" << insertIndex
                     << ", destlistPosts->size()=" << destlistPosts->size();

            if(newPost == destlistPosts->last()) //insertion a la fin
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
            printPostAtCursor(cursor,newPost);

            newPostsIndex++;
            baseInsertIndex = insertIndex;
        }

        if(newPostsIndex < newPosts.size())
        {
            mainTable->appendRows(newPosts.size() - newPostsIndex);
            cursor.movePosition(QTextCursor::NextRow, QTextCursor::MoveAnchor, insertIndex - baseInsertIndex );
            //Le premier item a déjà été inséré dans la liste destlistPosts dans la boucle while au dessus
            //on a juste a l'afficher
            printPostAtCursor(cursor, newPosts.at(newPostsIndex++));
            cursor.movePosition(QTextCursor::NextRow);

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

    //TODO : insérer ici la purge des anciens messages
    //Fin TODO

    newPostsAvailableMutex.unlock();
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
    emit insertTextPalmi(norloge.toStringPalmi() + QString::fromAscii(" "));
}

void QQPinipede::norlogeRefHovered(QQNorlogeRef norlogeRef)
{
    QString dstBouchot = norlogeRef.dstBouchot();
    QString dstNorloge = norlogeRef.dstNorloge();

    qDebug() << "norlogeRefHovered, datetimepart=" << dstNorloge << ", destbouchot=" << dstBouchot;

    QQBouchot * bouchot = m_settings->bouchot(dstBouchot);
    QQTextBrowser* textBrowser = m_textBrowserHash.value(bouchot->settings().group());
    if(textBrowser->isVisible())
    {
        QTextFrame* root = textBrowser->document()->rootFrame();
        QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));
        QList<QQPost *> *destlistPosts = m_listPostsTabMap[bouchot->settings().group()];
        QQPost * post = NULL;
        for(int row = destlistPosts->size() - 1; row >= 0; row--)
        {
            post = destlistPosts->at(row);
            if(post->bouchot()->name() == dstBouchot &&
                    post->norloge().indexOf(dstNorloge) == 0)
            {
                    qDebug() << "QQPinipede::norlogeRefHovered : Found at row " << row << " !!!!!!!";
                    m_rowHighlighted.append(row);
                    m_bouchotHighlighted = bouchot->name();
                    highlightRow(mainTable, row);
                    break;
            }
        }
    }
    else
    {
        qDebug() << "Group " << bouchot->settings().group() << " is not visible";
    }
}

void QQPinipede::unHighlight()
{
    qDebug() << "QQPinipede::unHighlight";
    if(m_bouchotHighlighted.size() == 0 || m_rowHighlighted.size() == 0)
        return;

    QQBouchot * bouchot = m_settings->bouchot(m_bouchotHighlighted);
    QQTextBrowser* textBrowser = m_textBrowserHash.value(bouchot->settings().group());
    QTextFrame* root = textBrowser->document()->rootFrame();
    QTextTable* mainTable = dynamic_cast<QTextTable *>(root->childFrames().at(0));

    for(int i = 0; i < m_rowHighlighted.size(); i++)
    {
        QTextTableCell cell = mainTable->cellAt(m_rowHighlighted[i], 0);
/*
        QQMessageBlockUserData * blockData = dynamic_cast<QQMessageBlockUserData *>(cell.firstCursorPosition().block().userData());
        blockData->getData(QQMessageBlockUserData::BOUCHOT_NAME);
*/
        QColor color = bouchot->settings().color();
        QTextCursor cursor = cell.firstCursorPosition();
        cursor.beginEditBlock();
        for(int i = 0; i < 4; i++)
        {
            QTextCharFormat format = cursor.blockCharFormat();
            format.setBackground(color);
            cursor.setBlockCharFormat(format);
            cursor.movePosition(QTextCursor::NextCell);
            if(i == 0)
                color = bouchot->settings().colorLight();
        }
        cursor.endEditBlock();
    }

    m_rowHighlighted.clear();
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

void QQPinipede::highlightRow(QTextTable * mainTable, int row)
{

    QTextTableCell cell = mainTable->cellAt(row, 0);
    QTextCursor cursor = cell.firstCursorPosition();
    cursor.beginEditBlock();
    for(int i = 0; i < 4; i++)
    {
        QTextCharFormat format = cursor.blockCharFormat();
        format.setBackground(QColor(255, 255, 0, 255));
        cursor.setBlockCharFormat(format);
        cursor.movePosition(QTextCursor::NextCell);
    }
    cursor.endEditBlock();
}

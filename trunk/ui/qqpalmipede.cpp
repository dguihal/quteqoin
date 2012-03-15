#include "qqpalmipede.h"
#include "ui_qqpalmipede.h"

#include "core/qqnorloge.h"

#include <QDebug>
#include <QRegExp>
#include <QShortcut>

QQPalmipede::QQPalmipede(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::QQPalmipede)
{
    ui->setupUi(this);

    QKeySequence keySeqBlam(Qt::ALT + Qt::Key_O);
    blamShortcut = new QShortcut(keySeqBlam, this);
    QObject::connect(blamShortcut, SIGNAL(activated()), this, SLOT(insertBlam()));
    QKeySequence keySeqPaf(Qt::ALT + Qt::Key_P);
    pafShortcut = new QShortcut(keySeqPaf, this);
    QObject::connect(pafShortcut, SIGNAL(activated()), this, SLOT(insertPaf()));

    connect(ui->boldButton, SIGNAL( clicked() ), this, SLOT( boldClicked() ) );
    connect(ui->italicButton, SIGNAL( clicked() ), this, SLOT( italicClicked() ) );
    connect(ui->underlineButton, SIGNAL( clicked() ), this, SLOT( underlineClicked() ) );
    connect(ui->strikeButton, SIGNAL( clicked() ), this, SLOT( strikeClicked() ) );
    connect(ui->momentButton, SIGNAL( clicked() ), this, SLOT( momentClicked() ) );
    connect(ui->blamPafComboBox, SIGNAL(activated(QString)), this, SLOT(blamPafActivated(QString)));
    connect(ui->boardSelectorComboBox, SIGNAL(activated(int)), this, SLOT(bouchotSelectorActivated(int)));
    connect(ui->postPushButton, SIGNAL(clicked()), this, SLOT(postPushButtonClicked()));
    connect(ui->postLineEdit, SIGNAL(returnPressed()), ui->postPushButton, SLOT(animateClick()));
}

QQPalmipede::~QQPalmipede()
{
    delete ui;
}

void QQPalmipede::insertText(const QString &text)
{
    ui->postLineEdit->insert(text);
}

void QQPalmipede::changePalmiColor(const QColor& newColor)
{
    QPalette p = ui->postLineEdit->palette();
    p.setColor( QPalette::Base, newColor );
    ui->postLineEdit->setPalette(p);
    qDebug()<<"QQPalmipede::changePalmiColor";
}

void QQPalmipede::insertSurroundText(const QString & bTag, const QString & eTag)
{
    if(ui->postLineEdit->hasSelectedText())
    {
        qDebug()<<"QQPalmipede::insertTagsLineEdit : hasSelectedText";
        QString text=ui->postLineEdit->text();
        int selectionStart=ui->postLineEdit->selectionStart();
        int selectedTextLength=ui->postLineEdit->selectedText().length();
        text.insert(selectionStart, bTag);
        text.insert(selectionStart + bTag.length() + selectedTextLength, eTag);
        ui->postLineEdit->setText(text);
        ui->postLineEdit->setCursorPosition(selectionStart + bTag.length() + selectedTextLength);
    }
    else
    {
        ui->postLineEdit->insert(bTag);
        ui->postLineEdit->insert(eTag);
        ui->postLineEdit->setFocus(Qt::OtherFocusReason);
        ui->postLineEdit->setCursorPosition(ui->postLineEdit->cursorPosition() - eTag.length());
    }
}

void QQPalmipede::insertReplaceText(const QString & tag)
{
    QString text = tag;
    //Suppression des @bouchot excédentaires lorsque l'on a déjà selectionné le dit bouchot
    QRegExp regexp = QQNorloge::norlogeRegexp(ui->boardSelectorComboBox->currentText());
    text.replace(regexp, QString::fromAscii("\\1"));

    if(ui->postLineEdit->hasSelectedText())
    {
        qDebug()<<"QQPalmipede::insertTagsLineEdit : hasSelectedText";
        QString text=ui->postLineEdit->text();
        int selectionStart=ui->postLineEdit->selectionStart();
        int selectedTextLength=ui->postLineEdit->selectedText().length();
        text.replace(selectionStart, selectedTextLength, text);
        ui->postLineEdit->setText(text);
        ui->postLineEdit->setCursorPosition(selectionStart + text.length());
    }
    else
    {
        ui->postLineEdit->insert(text);
        ui->postLineEdit->setFocus(Qt::OtherFocusReason);
        ui->postLineEdit->setCursorPosition(ui->postLineEdit->cursorPosition());
    }
}

void QQPalmipede::addBouchot(const QString &newBouchot, const QColor& newBouchotColor)
{
    ui->boardSelectorComboBox->addItem(newBouchot, newBouchotColor);
    int index = ui->boardSelectorComboBox->currentIndex();
    QColor bouchotColor = ui->boardSelectorComboBox->itemData(index).value<QColor>();
    changePalmiColor(bouchotColor);

}

void QQPalmipede::removeBouchot(const QString &oldBouchot)
{
    int index = ui->boardSelectorComboBox->findText(oldBouchot, Qt::MatchExactly | Qt::MatchCaseSensitive);
    if(index > 0 )
        ui->boardSelectorComboBox->removeItem(index);
}

void QQPalmipede::changeNorloges(const QString & bouchot)
{
    QString text = ui->postLineEdit->text();
    QRegExp norlogeReg = QQNorloge::norlogeRegexp();
    QRegExp bouchotRemoverReg = QRegExp(QString::fromAscii("@").append(bouchot),
                                        Qt::CaseSensitive,
                                        QRegExp::RegExp);
    QRegExp bouchotAdderReg = QRegExp(QString::fromAscii("@[A-Za-z0-9_]+"),
                                      Qt::CaseSensitive,
                                      QRegExp::RegExp);
    QString destText;

    int firstIndex;
    while((firstIndex = norlogeReg.indexIn(text)) != -1)
    {
        if(firstIndex > 0)
        {
            destText.append(text.left(firstIndex));
            text.remove(0, firstIndex);
        }

        QString norloge = text.left(norlogeReg.matchedLength());

        if(norloge.contains(bouchotRemoverReg))
            destText.append(norloge.left(norloge.length() - bouchotRemoverReg.matchedLength()));
        else if(! norloge.contains(bouchotAdderReg))
            destText.append(norloge).append(QString::fromAscii("@")).append(m_oldBouchot);
        else
            destText.append(norloge);

        text.remove(0, norlogeReg.matchedLength());
    }

    if(text.length() > 0)
        destText.append(text);

    ui->postLineEdit->setText(destText);
}

void QQPalmipede::boldClicked()
{
    insertSurroundText(QString::fromAscii("<b>"), QString::fromAscii("</b>"));
    qDebug()<<"QQPalmipede::boldClicked";
}

void QQPalmipede::italicClicked()
{
    insertSurroundText(QString::fromAscii("<i>"), QString::fromAscii("</i>"));
    qDebug()<<"QQPalmipede::italicClicked";
}

void QQPalmipede::underlineClicked()
{
    insertSurroundText(QString::fromAscii("<u>"), QString::fromAscii("</u>"));
    qDebug()<<"QQPalmipede::underlineClicked";
}

void QQPalmipede::strikeClicked()
{
    insertSurroundText(QString::fromAscii("<s>"), QString::fromAscii("</s>"));
    qDebug()<<"QQPalmipede::strikeClicked";
}

void QQPalmipede::momentClicked()
{
    insertSurroundText(QString::fromAscii("====> <b>Moment "), QString::fromAscii("</b> <===="));
    qDebug()<<"QQPalmipede::momentClicked";
}

void QQPalmipede::blamPafActivated(const QString & text)
{
    qDebug()<<"QQPalmipede::blamPafActivated itemText=" << text;
    if(text.contains(QString::fromAscii("paf"), Qt::CaseInsensitive))
        insertPaf();
    else if(text.contains(QString::fromAscii("BLAM"), Qt::CaseInsensitive))
        insertBlam();
    else
        qDebug()<<"QQPalmipede::momentClicked : index non reconnu : " << text;

    qDebug()<<"QQPalmipede::blamPafActivated end";
}

void QQPalmipede::bouchotSelectorActivated(int index)
{
    QString bouchot = ui->boardSelectorComboBox->itemText(index);
    changeNorloges(bouchot);
    QColor bouchotColor = ui->boardSelectorComboBox->itemData(index).value<QColor>();
    changePalmiColor(bouchotColor);
    m_oldBouchot = bouchot;
}

void QQPalmipede::postPushButtonClicked()
{
    QString message = ui->postLineEdit->text();
    QString bouchotDest = ui->boardSelectorComboBox->currentText();
    emit postMessage(bouchotDest, message);

    //envisager de garder un histo des derniers posts "Au cas ou"
    ui->postLineEdit->clear();
}

void QQPalmipede::insertBlam()
{
    insertReplaceText(QString::fromAscii("_o/* <b>BLAM</b>! "));
    qDebug()<<"QQPalmipede::insertBlam";
}

void QQPalmipede::insertPaf()
{
    insertReplaceText(QString::fromAscii("_o/* <b>paf!</b> "));
    qDebug()<<"QQPalmipede::insertPaf";
}

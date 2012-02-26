#include "qqpalmipede.h"
#include "ui_qqpalmipede.h"

#include <QDebug>
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
    connect(ui->blamPafComboBox, SIGNAL( activated(int) ), this, SLOT( blamPafActivated(int) ) );
}

QQPalmipede::~QQPalmipede()
{
    delete ui;
}

/*
QString QQPalmipede::getCurrentBouchot()
{
    QString text;
    return text;
}
*/

void QQPalmipede::insertText(const QString &text)
{
    ui->lineEdit->insert(text);
}

void QQPalmipede::changePalmiColor(const QColor& newColor)
{
    QPalette p = ui->lineEdit->palette();
    p.setColor( QPalette::Base, newColor );
    ui->lineEdit->setPalette(p);
    qDebug()<<"QQPalmipede::changePalmiColor";
}

void QQPalmipede::insertSurroundText(const QString& bTag, const QString& eTag)
{
    if(ui->lineEdit->hasSelectedText())
    {
        qDebug()<<"QQPalmipede::insertTagsLineEdit : hasSelectedText";
        QString text=ui->lineEdit->text();
        int selectionStart=ui->lineEdit->selectionStart();
        int selectedTextLength=ui->lineEdit->selectedText().length();
        text.insert(selectionStart, bTag);
        text.insert(selectionStart + bTag.length() + selectedTextLength, eTag);
        ui->lineEdit->setText(text);
        ui->lineEdit->setCursorPosition(selectionStart + bTag.length() + selectedTextLength);
    }
    else
    {
        ui->lineEdit->insert(bTag);
        ui->lineEdit->insert(eTag);
        ui->lineEdit->setFocus(Qt::OtherFocusReason);
        ui->lineEdit->setCursorPosition(ui->lineEdit->cursorPosition() - eTag.length());
    }
}

void QQPalmipede::insertReplaceText(const QString& tag)
{
    if(ui->lineEdit->hasSelectedText())
    {
        qDebug()<<"QQPalmipede::insertTagsLineEdit : hasSelectedText";
        QString text=ui->lineEdit->text();
        int selectionStart=ui->lineEdit->selectionStart();
        int selectedTextLength=ui->lineEdit->selectedText().length();
        text.replace(selectionStart, selectedTextLength, tag);
        ui->lineEdit->setText(text);
        ui->lineEdit->setCursorPosition(selectionStart + tag.length());
    }
    else
    {
        ui->lineEdit->insert(tag);
        ui->lineEdit->setFocus(Qt::OtherFocusReason);
        ui->lineEdit->setCursorPosition(ui->lineEdit->cursorPosition());
    }
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

void QQPalmipede::blamPafActivated(int index)
{
    qDebug()<<"QQPalmipede::momentClicked begin, index="<<index;
    qDebug()<<"QQPalmipede::momentClicked itemText="<<ui->blamPafComboBox->itemText(index);
    if(ui->blamPafComboBox->itemText(index).contains(QString::fromAscii("paf"), Qt::CaseInsensitive))
        insertPaf();
    else if(ui->blamPafComboBox->itemText(index).contains(QString::fromAscii("BLAM"), Qt::CaseInsensitive))
        insertBlam();
    else
        qDebug()<<"QQPalmipede::momentClicked : index non reconnu";

    qDebug()<<"QQPalmipede::momentClicked end";
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

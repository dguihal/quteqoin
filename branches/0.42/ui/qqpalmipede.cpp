#include "qqpalmipede.h"
#include "ui_qqpalmipede.h"

#include "core/qqnorlogeref.h"

#include <QtDebug>
#include <QFocusEvent>
#include <QRegExp>
#include <QShortcut>

#define PALMIPEDE_OBJECT_NAME "QQPalmipede"

QQPalmipede::QQPalmipede(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::QQPalmipede)
{
	setObjectName(PALMIPEDE_OBJECT_NAME);
	m_minimal = false;

	ui->setupUi(this);

	connect(ui->boldButton, SIGNAL(clicked()),
			this, SLOT(boldClicked()));
	connect(ui->italicButton, SIGNAL(clicked()),
			this, SLOT(italicClicked()));
	connect(ui->underlineButton, SIGNAL(clicked()),
			this, SLOT(underlineClicked()));
	connect(ui->strikeButton, SIGNAL(clicked()),
			this, SLOT(strikeClicked()));
	connect(ui->momentButton, SIGNAL(clicked()),
			this, SLOT(momentClicked()));
	connect(ui->blamPafComboBox, SIGNAL(activated(QString)),
			this, SLOT(blamPafActivated(QString)));
	connect(ui->boardSelectorComboBox, SIGNAL(activated(int)),
			this, SLOT(bouchotSelectorActivated(int)));
	connect(ui->boardSelectorComboBoxMin, SIGNAL(activated(int)),
			this, SLOT(bouchotSelectorActivated(int)));
	connect(ui->postPushButton, SIGNAL(clicked()),
			this, SLOT(postPushButtonClicked()));
	connect(ui->postLineEdit, SIGNAL(returnPressed()),
			ui->postPushButton, SLOT(animateClick()));

	ui->boardSelectorComboBoxMin->setVisible(this->m_minimal);
}

QQPalmipede::~QQPalmipede()
{
	delete ui;
}

void QQPalmipede::insertText(const QString &text)
{
	ui->postLineEdit->insert(text);
}

void QQPalmipede::addBouchot(const QString &newBouchot, const QColor& newBouchotColor)
{
	ui->boardSelectorComboBox->addItem(newBouchot, newBouchotColor);
	ui->boardSelectorComboBoxMin->addItem(newBouchot, newBouchotColor);
	int index = ui->boardSelectorComboBox->currentIndex();
	QColor bouchotColor = ui->boardSelectorComboBox->itemData(index).value<QColor>();
	ui->postLineEdit->changeColor(bouchotColor);

}

void QQPalmipede::removeBouchot(const QString &oldBouchot)
{
	int index = ui->boardSelectorComboBox->findText(oldBouchot, Qt::MatchExactly | Qt::MatchCaseSensitive);
	if(index >= 0)
	{
		ui->boardSelectorComboBox->removeItem(index);
		ui->boardSelectorComboBoxMin->removeItem(index);
	}
}

void QQPalmipede::insertReplaceText(const QString &bouchot, const QString &tag)
{
	int wasPostLineEditEmpty = ui->postLineEdit->text().isEmpty();
	insertReplaceText(tag);

	// Si le palmi est vide, il faut changer la tribune selectionnee
	if(wasPostLineEditEmpty)
	{
		int index = ui->boardSelectorComboBox->findText(bouchot);
		if(index >= 0)
		{
			ui->boardSelectorComboBox->setCurrentIndex(index);
			ui->boardSelectorComboBoxMin->setCurrentIndex(index);
			bouchotSelectorActivated(index);
		}
	}
}

void QQPalmipede::insertReplaceText(const QString &tag)
{
	QString t_tag = tag;
	ui->postLineEdit->insertReplaceText(t_tag);
	setFocus();
}

void QQPalmipede::focusInEvent(QFocusEvent * event)
{
	QDockWidget::focusInEvent(event);
	ui->postLineEdit->setFocus();
}

void QQPalmipede::changeNorloges(const QString & bouchot)
{
	QString text = ui->postLineEdit->text();
	QRegExp norlogeReg = QQNorlogeRef::norlogeRegexp();
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

void QQPalmipede::setMinimal(bool minimal)
{
	m_minimal = minimal;

	ui->cmdGrpWidget->setVisible(! m_minimal);
	ui->postPushButton->setVisible(! m_minimal);
	ui->boardSelectorComboBoxMin->setVisible(m_minimal);

	int height = ui->postLineEdit->height();
	if(! minimal)
		height += ui->cmdGrpWidget->height();

	ui->dockWidgetContents->setMaximumHeight(height);
}


void QQPalmipede::boldClicked()
{
	ui->postLineEdit->
			insertSurroundText(QString::fromAscii("<b>"), QString::fromAscii("</b>"));
}

void QQPalmipede::italicClicked()
{
	ui->postLineEdit->
			insertSurroundText(QString::fromAscii("<i>"), QString::fromAscii("</i>"));
}

void QQPalmipede::underlineClicked()
{
	ui->postLineEdit->
			insertSurroundText(QString::fromAscii("<u>"), QString::fromAscii("</u>"));
}

void QQPalmipede::strikeClicked()
{
	ui->postLineEdit->
			insertSurroundText(QString::fromAscii("<s>"), QString::fromAscii("</s>"));
}

void QQPalmipede::momentClicked()
{
	ui->postLineEdit->
			insertSurroundText(QString::fromAscii("====> <b>Moment "), QString::fromAscii("</b> <===="));
}

void QQPalmipede::blamPafActivated(const QString & text)
{
	if(text.contains(QString::fromAscii("paf"), Qt::CaseInsensitive))
		insertPaf();
	else if(text.contains(QString::fromAscii("BLAM"), Qt::CaseInsensitive))
		insertBlam();
	else
		qDebug()<<"QQPalmipede::momentClicked : index non reconnu : " << text;
}

void QQPalmipede::bouchotSelectorActivated(int index)
{
	QString bouchot;
	if(m_minimal)
	{
		bouchot = ui->boardSelectorComboBoxMin->itemText(index);
		ui->boardSelectorComboBox->setCurrentIndex(index);
	}
	else
	{
		bouchot = ui->boardSelectorComboBox->itemText(index);
		ui->boardSelectorComboBoxMin->setCurrentIndex(index);
	}
	changeNorloges(bouchot);
	QColor bouchotColor = ui->boardSelectorComboBox->itemData(index).value<QColor>();
	ui->postLineEdit->changeColor(bouchotColor);
	ui->postLineEdit->setFocus();
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
	ui->postLineEdit->insertReplaceText(QString::fromAscii("_o/* <b>BLAM</b>! "));
	qDebug()<<"QQPalmipede::insertBlam";
}

void QQPalmipede::insertPaf()
{
	ui->postLineEdit->insertReplaceText(QString::fromAscii("_o/* <b>paf!</b> "));
	qDebug()<<"QQPalmipede::insertPaf";
}

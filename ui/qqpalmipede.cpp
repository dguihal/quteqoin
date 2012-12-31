#include "qqpalmipede.h"
#include "ui_qqpalmipede.h"

#include "core/qqnorlogeref.h"

#include <QDebug>
#include <QRegExp>
#include <QShortcut>

#define PALMI_TITLE "Palmipède Command Board"

QQPalmipede::QQPalmipede(QWidget *parent) :
	QGroupBox(parent),
	ui(new Ui::QQPalmipede)
{
	this->m_minimal = false;

	ui->setupUi(this);

	QKeySequence keySeqBlam(Qt::ALT + Qt::Key_O);
	blamShortcut = new QShortcut(keySeqBlam, this);
	QObject::connect(blamShortcut, SIGNAL(activated()), this, SLOT(insertBlam()));
	QKeySequence keySeqPaf(Qt::ALT + Qt::Key_P);
	pafShortcut = new QShortcut(keySeqPaf, this);
	QObject::connect(pafShortcut, SIGNAL(activated()), this, SLOT(insertPaf()));

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

	this->ui->boardSelectorComboBoxMin->setVisible(this->m_minimal);

	setTitle(trUtf8(PALMI_TITLE));
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
	if(index > 0)
	{
		ui->boardSelectorComboBox->removeItem(index);
		ui->boardSelectorComboBoxMin->removeItem(index);
	}
}

void QQPalmipede::minimizePalmi()
{
	setMinimal(true);
}

void QQPalmipede::maximizePalmi()
{
	setMinimal(false);
}
void QQPalmipede::insertReplaceText(const QString & tag)
{
	QString t_tag = tag;

	// Si le palmi est vide, il est preferable de changer le tribune selectionnee
	if(ui->postLineEdit->text().isEmpty())
	{
		QRegExp regexp = QQNorlogeRef::norlogeRegexp();
		t_tag.indexOf(regexp);
		QString bouchot = regexp.cap(4).remove(0, 1); // pour supprimer le '@' initial
		if(bouchot.length() > 0)
		{
			int index = ui->boardSelectorComboBox->findText(bouchot);
			if(index >= 0)
			{
				ui->boardSelectorComboBox->setCurrentIndex(index);
				ui->boardSelectorComboBoxMin->setCurrentIndex(index);
				bouchotSelectorActivated(index);
				t_tag.replace(regexp, QString::fromAscii("\\1"));
			}
		}
	}
	else
	{
		QRegExp regexp = QQNorlogeRef::norlogeRegexp(ui->boardSelectorComboBox->currentText());
		//Suppression des @bouchot excedentaires lorsque l'on a deja  selectionne le dit bouchot
		t_tag.replace(regexp, QString::fromAscii("\\1"));
	}

	ui->postLineEdit->insertReplaceText(t_tag);
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

	this->ui->cmdGrpWidget->setVisible(! m_minimal);
	this->ui->postPushButton->setVisible(! m_minimal);
	this->ui->boardSelectorComboBoxMin->setVisible(m_minimal);
	m_minimal ? this->setTitle("") : setTitle(trUtf8(PALMI_TITLE));
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

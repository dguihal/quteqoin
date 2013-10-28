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
	m_ui(new Ui::QQPalmipede),
	m_minimal(false),
	m_wasVisible(false)

{
	setObjectName(PALMIPEDE_OBJECT_NAME);

	m_ui->setupUi(this);

	setFeatures(QDockWidget::DockWidgetClosable |
				QDockWidget::DockWidgetMovable |
				QDockWidget::DockWidgetFloatable);

	connect(m_ui->boldButton, SIGNAL(clicked()),
			m_ui->postLineEdit, SLOT(bold()));
	connect(m_ui->italicButton, SIGNAL(clicked()),
			m_ui->postLineEdit, SLOT(italic()));
	connect(m_ui->underlineButton, SIGNAL(clicked()),
			m_ui->postLineEdit, SLOT(underline()));
	connect(m_ui->strikeButton, SIGNAL(clicked()),
			m_ui->postLineEdit, SLOT(strike()));
	connect(m_ui->momentButton, SIGNAL(clicked()),
			m_ui->postLineEdit, SLOT(moment()));
	connect(m_ui->blamPafComboBox, SIGNAL(activated(QString)),
			this, SLOT(blamPafActivated(QString)));
	connect(m_ui->boardSelectorComboBox, SIGNAL(activated(int)),
			this, SLOT(bouchotSelectorActivated(int)));
	connect(m_ui->boardSelectorComboBoxMin, SIGNAL(activated(int)),
			this, SLOT(bouchotSelectorActivated(int)));
	connect(m_ui->postPushButton, SIGNAL(clicked()),
			this, SLOT(postPushButtonClicked()));
	connect(m_ui->postLineEdit, SIGNAL(returnPressed()),
			m_ui->postPushButton, SLOT(animateClick()));

	m_ui->boardSelectorComboBoxMin->setVisible(this->m_minimal);
}

QQPalmipede::~QQPalmipede()
{
	delete m_ui;
}

void QQPalmipede::insertText(const QString &text)
{
	m_ui->postLineEdit->insert(text);
}

void QQPalmipede::addBouchot(const QString &newBouchot, const QColor& newBouchotColor)
{
	m_ui->boardSelectorComboBox->addItem(newBouchot, newBouchotColor);
	m_ui->boardSelectorComboBoxMin->addItem(newBouchot, newBouchotColor);
	int index = m_ui->boardSelectorComboBox->currentIndex();
	QColor bouchotColor = m_ui->boardSelectorComboBox->itemData(index).value<QColor>();
	m_ui->postLineEdit->changeColor(bouchotColor);

}

void QQPalmipede::removeBouchot(const QString &oldBouchot)
{
	int index = m_ui->boardSelectorComboBox->findText(oldBouchot, Qt::MatchExactly | Qt::MatchCaseSensitive);
	if(index >= 0)
	{
		m_ui->boardSelectorComboBox->removeItem(index);
		m_ui->boardSelectorComboBoxMin->removeItem(index);
	}
}

void QQPalmipede::setVisible(bool visible)
{
	QDockWidget::setVisible(visible);
	m_wasVisible = visible;
}

void QQPalmipede::insertReplaceText(const QString &bouchot, const QString &tag)
{
	int wasPostLineEditEmpty = m_ui->postLineEdit->text().isEmpty();
	insertReplaceText(tag);

	// Si le palmi est vide, il faut changer la tribune selectionnee
	int boardIndex = m_ui->boardSelectorComboBox->currentIndex();
	if(wasPostLineEditEmpty)
	{
		boardIndex = m_ui->boardSelectorComboBox->findText(bouchot);
		if(boardIndex >= 0)
		{
			m_ui->boardSelectorComboBox->setCurrentIndex(boardIndex);
			m_ui->boardSelectorComboBoxMin->setCurrentIndex(boardIndex);
		}
	}
	bouchotSelectorActivated(boardIndex);
}

void QQPalmipede::insertReplaceText(const QString &tag)
{
	QString t_tag = tag;
	m_ui->postLineEdit->insertText(t_tag);

	// Warning : le show va appeler le setVisible(bool),
	//    il faut donc sauver l'état antérieur avant
	bool wasVisible = isVisible();
	if(! isVisible())
		show();

	m_wasVisible = wasVisible;

	setFocus();
}

void QQPalmipede::focusInEvent(QFocusEvent * event)
{
	QDockWidget::focusInEvent(event);
	m_ui->postLineEdit->setFocus();
}

void QQPalmipede::changeNorloges(const QString & bouchot)
{
	QString text = m_ui->postLineEdit->text();
	QRegExp norlogeReg = QQNorlogeRef::norlogeRegexp();
	QRegExp bouchotRemoverReg = QRegExp(QString::fromLatin1("@").append(bouchot),
										Qt::CaseSensitive,
										QRegExp::RegExp);
	QRegExp bouchotAdderReg = QRegExp(QString::fromLatin1("@[A-Za-z0-9_]+"),
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
		else if(! norloge.contains(bouchotAdderReg) && bouchot != m_oldBouchot)
			destText.append(norloge).append(QString::fromLatin1("@")).append(m_oldBouchot);
		else
			destText.append(norloge);

		text.remove(0, norlogeReg.matchedLength());
	}

	if(text.length() > 0)
		destText.append(text);

	m_ui->postLineEdit->setText(destText);
}

void QQPalmipede::setMinimal(bool minimal)
{
	m_minimal = minimal;

	m_ui->cmdGrpWidget->setVisible(! m_minimal);
	m_ui->postPushButton->setVisible(! m_minimal);
	m_ui->boardSelectorComboBoxMin->setVisible(m_minimal);

	int height = m_ui->postLineEdit->height();
	if(! minimal)
		height += m_ui->cmdGrpWidget->height();

	m_ui->dockWidgetContents->setMaximumHeight(height);
}

void QQPalmipede::blamPafActivated(const QString & text)
{
	if(text.contains(QString::fromLatin1("paf"), Qt::CaseInsensitive))
		m_ui->postLineEdit->paf();
	else if(text.contains(QString::fromLatin1("BLAM"), Qt::CaseInsensitive))
		m_ui->postLineEdit->blam();
	else
		qDebug()<<"QQPalmipede::momentClicked : index non reconnu : " << text;
}

void QQPalmipede::bouchotSelectorActivated(int index)
{
	QString bouchot;
	if(m_minimal)
	{
		bouchot = m_ui->boardSelectorComboBoxMin->itemText(index);
		m_ui->boardSelectorComboBox->setCurrentIndex(index);
	}
	else
	{
		bouchot = m_ui->boardSelectorComboBox->itemText(index);
		m_ui->boardSelectorComboBoxMin->setCurrentIndex(index);
	}
	changeNorloges(bouchot);
	QColor bouchotColor = m_ui->boardSelectorComboBox->itemData(index).value<QColor>();
	m_ui->postLineEdit->changeColor(bouchotColor);
	m_ui->postLineEdit->setFocus();
	m_oldBouchot = bouchot;
}

void QQPalmipede::postPushButtonClicked()
{
	QString message = m_ui->postLineEdit->text();
	QString bouchotDest = m_ui->boardSelectorComboBox->currentText();
	emit postMessage(bouchotDest, message.replace(
						 QRegExp("\\s", Qt::CaseInsensitive, QRegExp::RegExp2), " "));

	//envisager de garder un histo des derniers posts "Au cas ou"
	m_ui->postLineEdit->clear();

	if(! m_wasVisible)
		hide();
}

#include "qqpalmipede.h"
#include "ui_qqpalmipede.h"

#include "core/qqnorlogeref.h"

#include <QtDebug>
#include <QFocusEvent>
#include <QRegExp>
#include <QShortcut>

#define PALMIPEDE_OBJECT_NAME "QQPalmipede"

QQPalmipede::QQPalmipede(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::QQPalmipede),
	m_minimal(false),
	m_wasVisible(true)

{
	setObjectName(PALMIPEDE_OBJECT_NAME);

	m_ui->setupUi(this);

	connect(m_ui->boldButton, SIGNAL(clicked()),
			m_ui->palmiEditor, SLOT(bold()));
	connect(m_ui->italicButton, SIGNAL(clicked()),
			m_ui->palmiEditor, SLOT(italic()));
	connect(m_ui->underlineButton, SIGNAL(clicked()),
			m_ui->palmiEditor, SLOT(underline()));
	connect(m_ui->strikeButton, SIGNAL(clicked()),
			m_ui->palmiEditor, SLOT(strike()));
	connect(m_ui->momentButton, SIGNAL(clicked()),
			m_ui->palmiEditor, SLOT(moment()));
	connect(m_ui->blamPafComboBox, SIGNAL(activated(QString)),
			this, SLOT(blamPafActivated(QString)));
	connect(m_ui->boardSelectorComboBox, SIGNAL(activated(int)),
			this, SLOT(bouchotSelectorActivated(int)));
	connect(m_ui->boardSelectorComboBoxMin, SIGNAL(activated(int)),
			this, SLOT(bouchotSelectorActivated(int)));
	connect(m_ui->postPushButton, SIGNAL(clicked()),
			this, SLOT(postPushButtonClicked()));
	connect(m_ui->palmiEditor, SIGNAL(returnPressed()),
			m_ui->postPushButton, SLOT(animateClick()));
	connect(m_ui->palmiEditor, SIGNAL(changeBoard(bool)),
			this, SLOT(changeBoard(bool)));
	connect(m_ui->attachButton, SIGNAL(clicked()),
			m_ui->palmiEditor, SLOT(attachFile()));

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	m_ui->boardSelectorComboBoxMin->setVisible(this->m_minimal);
}

QQPalmipede::~QQPalmipede()
{
	delete m_ui;
}

void QQPalmipede::insertText(const QString &text)
{
	m_ui->palmiEditor->insert(text);
}

void QQPalmipede::addBouchot(const QString &newBouchot, const QColor& newBouchotColor)
{
	m_ui->boardSelectorComboBox->addItem(newBouchot, newBouchotColor);
	m_ui->boardSelectorComboBoxMin->addItem(newBouchot, newBouchotColor);
	int index = m_ui->boardSelectorComboBox->currentIndex();
	QColor bouchotColor = m_ui->boardSelectorComboBox->itemData(index).value<QColor>();
	m_ui->palmiEditor->changeColor(bouchotColor);

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

void QQPalmipede::insertReplaceText(const QString &bouchot, const QString &tag)
{
	int wasPostLineEditEmpty = m_ui->palmiEditor->text().isEmpty();
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
	m_ui->palmiEditor->insertText(t_tag);

	// Warning : le show va appeler le setVisible(bool),
	//    il faut donc sauver l'état antérieur avant
	bool wasVisible = isVisible();
	if(! isVisible())
		show();

	m_wasVisible = wasVisible;

	QApplication::postEvent(this, new QFocusEvent(QEvent::FocusIn), Qt::LowEventPriority);
}

void QQPalmipede::focusInEvent(QFocusEvent * event)
{
	QWidget::focusInEvent(event);
	m_ui->palmiEditor->setFocus();
}

void QQPalmipede::changeNorloges(const QString & bouchot)
{
	QString text = m_ui->palmiEditor->text();
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

	m_ui->palmiEditor->setText(destText);
}

void QQPalmipede::setMinimal(bool minimal)
{
	m_minimal = minimal;

	m_ui->cmdGrpW->setVisible(! m_minimal);
	m_ui->postPushButton->setVisible(! m_minimal);
	m_ui->boardSelectorComboBoxMin->setVisible(m_minimal);

	adjustSize();
}

QSize QQPalmipede::sizeHint() const
{
	QSize s;
	if(m_ui->cmdGrpW->isVisible())
		s.setHeight((m_ui->palmiEditor->height() * 3) + 2); // 2*1 spacing
	else
		s.setHeight(m_ui->palmiEditor->height());


	s.setWidth(m_ui->palmiEditor->minimumWidth());

	return s;
}

void QQPalmipede::changeBoard(bool next)
{
	int index = m_ui->boardSelectorComboBox->currentIndex();
	int num = m_ui->boardSelectorComboBox->count();

	if(next)
		index = (index + 1) % num;
	else
		index = (index == 0) ? (num - 1) :  (index - 1);

	m_ui->boardSelectorComboBox->setCurrentIndex(index);
	m_ui->boardSelectorComboBoxMin->setCurrentIndex(index);
	bouchotSelectorActivated(index);

	qDebug() << Q_FUNC_INFO << QApplication::focusWidget();
}

void QQPalmipede::blamPafActivated(const QString & text)
{
	if(text.contains(QString::fromLatin1("paf"), Qt::CaseInsensitive))
		m_ui->palmiEditor->paf();
	else if(text.contains(QString::fromLatin1("BLAM"), Qt::CaseInsensitive))
		m_ui->palmiEditor->blam();
	else
		qDebug() << Q_FUNC_INFO << "Index non reconnu : " << text;
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
	m_ui->palmiEditor->changeColor(bouchotColor);
	m_ui->palmiEditor->setFocus();
	m_oldBouchot = bouchot;
}

void QQPalmipede::postPushButtonClicked()
{
	QString message = m_ui->palmiEditor->text();
	message.replace(QRegExp("\\s", Qt::CaseInsensitive, QRegExp::RegExp2), " ");
	QString bouchotDest = m_ui->boardSelectorComboBox->currentText();

	m_ui->palmiEditor->pushCurrentToHistory();
	m_ui->palmiEditor->clear();

	emit postMessage(bouchotDest, message);

	if(! m_wasVisible)
		hide();
}

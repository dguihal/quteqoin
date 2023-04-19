#include "qqpalmipede.h"
#include "ui_qqpalmipede.h"

#include "core/qqnorlogeref.h"

#include <QtDebug>
#include <QFocusEvent>
#include <QRegularExpression>
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

	connect(m_ui->boldButton, &QPushButton::clicked,
	        m_ui->palmiEditor, &QQPalmiLineEdit::bold);
	connect(m_ui->italicButton, &QPushButton::clicked,
	        m_ui->palmiEditor, &QQPalmiLineEdit::italic);
	connect(m_ui->underlineButton, &QPushButton::clicked,
	        m_ui->palmiEditor, &QQPalmiLineEdit::underline);
	connect(m_ui->strikeButton, &QPushButton::clicked,
	        m_ui->palmiEditor, &QQPalmiLineEdit::strike);
	connect(m_ui->momentButton, &QPushButton::clicked,
	        m_ui->palmiEditor, &QQPalmiLineEdit::moment);
	connect(m_ui->blamPafComboBox, &QComboBox::activated,
	        this, &QQPalmipede::blamPafActivated);
	connect(m_ui->boardSelectorComboBox, &QComboBox::activated,
	        this, &QQPalmipede::bouchotSelectorActivated);
	connect(m_ui->boardSelectorComboBoxMin, &QComboBox::activated,
	        this, &QQPalmipede::bouchotSelectorActivated);
	connect(m_ui->postPushButton, &QPushButton::clicked,
	        this, &QQPalmipede::postPushButtonClicked);
	connect(m_ui->palmiEditor, &QQPalmiLineEdit::returnPressed,
	        m_ui->postPushButton, &QPushButton::animateClick);
	connect(m_ui->palmiEditor, &QQPalmiLineEdit::changeBoard,
	        this, &QQPalmipede::changeBoard);
	connect(m_ui->attachButton, &QPushButton::clicked,
	        m_ui->palmiEditor, &QQPalmiLineEdit::attachFile);

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
		// Possible si on a ete active par un bouchot en ro
		if(boardIndex >= 0)
		{
			m_ui->boardSelectorComboBox->setCurrentIndex(boardIndex);
			m_ui->boardSelectorComboBoxMin->setCurrentIndex(boardIndex);
		}
	}

	// Possible si on a ete active par un bouchot en ro
	if(boardIndex >= 0)
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
	auto norlogeReg = QQNorlogeRef::norlogeRegexp();
	auto bouchotRemoverReg = QRegularExpression(QString::fromLatin1("@").append(bouchot),
	                                    QRegularExpression::NoPatternOption);
	auto bouchotAdderReg = QRegularExpression(QString::fromLatin1("@[A-Za-z0-9_]+"),
	                                  QRegularExpression::NoPatternOption);
	QString destText;

	QRegularExpressionMatch firstMatch;
	while((firstMatch = norlogeReg.match(text)).hasMatch())
	{
		if(firstMatch.capturedStart() > 0)
		{
			destText.append(text.first(firstMatch.capturedStart()));
			text.remove(0, firstMatch.capturedStart());
		}

		QString norloge = text.left(firstMatch.capturedLength());

		if(norloge.indexOf(bouchotRemoverReg) >= 0)
			destText.append(norloge.remove(bouchotRemoverReg));
		else if(norloge.indexOf(bouchotAdderReg) < 0 && bouchot != m_oldBouchot)
			destText.append(norloge).append(QString::fromLatin1("@")).append(m_oldBouchot);
		else
			destText.append(norloge);

		text.remove(0, firstMatch.capturedLength());
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

void QQPalmipede::blamPafActivated(int index)
{
	auto text = m_ui->blamPafComboBox->itemText(index);
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

static QRegularExpression re_space("\\s", QRegularExpression::NoPatternOption); // clazy:exclude=use-static-qregularexpression
void QQPalmipede::postPushButtonClicked()
{
	QString message = m_ui->palmiEditor->text();
	message.replace(re_space, " ");
	QString bouchotDest = m_ui->boardSelectorComboBox->currentText();

	m_ui->palmiEditor->pushCurrentToHistory();
	m_ui->palmiEditor->clear();

	emit postMessage(bouchotDest, message);

	if(! m_wasVisible)
		hide();
}

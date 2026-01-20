#include "qqpinisearchwidget.h"
#include "ui_qqpinisearchwidget.h"

#include "core/qutetools.h"

#include <QCommonStyle>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QTransform>

#define MIN_SEARCH_LEN 3

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::QQPiniSearchWidget
/// \param parent
///
QQPiniSearchWidget::QQPiniSearchWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQPiniSearchWidget)
{
	ui->setupUi(this);

	m_oldFocusWidget = nullptr;

	setAutoFillBackground(true);

	QStyle *currentStyle = style();

	ui->revSearchPB->setIcon(currentStyle->standardIcon(QStyle::SP_ArrowLeft));
	ui->fwdSearchPB->setIcon(currentStyle->standardIcon(QStyle::SP_ArrowRight));
	ui->stopSearchPB->setIcon(currentStyle->standardIcon(QStyle::SP_DialogCancelButton));

	connect(ui->stopSearchPB, &QPushButton::clicked, this, &QQPiniSearchWidget::hide);
	connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &QQPiniSearchWidget::searchTextChanged);
	connect(ui->searchLineEdit, &QLineEdit::returnPressed, this, &QQPiniSearchWidget::searchTextNextFwd);
	connect(ui->fwdSearchPB, &QPushButton::clicked, this, &QQPiniSearchWidget::searchTextNextFwd);
	connect(ui->revSearchPB, &QPushButton::clicked, this, &QQPiniSearchWidget::searchTextNextBwd);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::~QQPiniSearchWidget
///
QQPiniSearchWidget::~QQPiniSearchWidget()
{
	delete ui;
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::setVisible
/// \param visible
///
void QQPiniSearchWidget::setVisible(bool visible)
{
	if(! visible)
	{
		ui->searchLineEdit->clear();

		if(QuteTools::checkFocusRecurse(this) &&
		   m_oldFocusWidget != nullptr &&
		   m_oldFocusWidget->isVisible())
			m_oldFocusWidget->setFocus();
	}
	else
	{
		m_oldFocusWidget = QApplication::focusWidget();
		ui->searchLineEdit->setFocus();
	}

	QWidget::setVisible(visible);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::focusInEvent
/// \param event
///
void QQPiniSearchWidget::focusInEvent(QFocusEvent *event)
{
	ui->searchLineEdit->setFocus(event->reason());
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::keyPressEvent
/// \param event
///
void QQPiniSearchWidget::keyPressEvent(QKeyEvent *event)
{
	bool processed = true;

	switch(event->key())
	{
	case Qt::Key_F3:
		if(event->modifiers() == Qt::SHIFT)
			ui->revSearchPB->animateClick();
		else
			ui->fwdSearchPB->animateClick();
		processed = true;
		break;
	default:
		processed = false;
	}

	if(!processed)
		QWidget::keyPressEvent(event);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::searchTextChanged
/// \param text
///
void QQPiniSearchWidget::searchTextChanged(const QString &text)
{
	if(text.length() >= MIN_SEARCH_LEN)
		emit search(text, true);
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::searchTextNextFwd
///
void QQPiniSearchWidget::searchTextNextFwd()
{
	QString text = ui->searchLineEdit->text();
	if(text.length() >= MIN_SEARCH_LEN)
		emit search(text, true);
}

void QQPiniSearchWidget::searchTextNextBwd()
{
	QString text = ui->searchLineEdit->text();
	if(text.length() >= MIN_SEARCH_LEN)
		emit search(text, false);
}

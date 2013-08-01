#include "qqpinisearchwidget.h"
#include "ui_qqpinisearchwidget.h"

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

	m_oldFocusWidget = NULL;

	setAutoFillBackground(true);

	QPixmap image(":/img/Fleche_verte.png");
	image = image.scaled(16, 16, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	ui->fwdSearchPB->setIcon(QIcon(image));
	QTransform mirrorTr;
	mirrorTr = mirrorTr.rotate(180, Qt::YAxis);
	image = image.transformed(mirrorTr);
	ui->revSearchPB->setIcon(QIcon(image));

	connect(ui->stopSearchPB, SIGNAL(clicked()), this, SLOT(hide()));
	connect(ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)));
	connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchTextNextFwd()));
	connect(ui->fwdSearchPB, SIGNAL(clicked()), this, SLOT(searchTextNextFwd()));
	connect(ui->revSearchPB, SIGNAL(clicked()), this, SLOT(searchTextNextBwd()));
}

//////////////////////////////////////////////////////////////
/// \brief QQPiniSearchWidget::~QQPiniSearchWidget
///
QQPiniSearchWidget::~QQPiniSearchWidget()
{
	delete ui;
}

//////////////////////////////////////////////////////////////
/// \brief checkFocusRecurse
/// \param parent
/// \return
///
bool checkFocusRecurse(QWidget *parent)
{
	bool focus = parent->hasFocus();
	if(! focus)
	{
		foreach(QObject *child, parent->children())
		{
			if(child->isWidgetType())
			{
				if((focus = checkFocusRecurse((QWidget *)child)) == true)
					break;
			}
		}
	}
	return focus;
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

		if(checkFocusRecurse(this) &&
		   m_oldFocusWidget != NULL &&
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

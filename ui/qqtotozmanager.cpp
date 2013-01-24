#include "qqtotozmanager.h"
#include "ui_qqtotozmanager.h"

#include "core/qqtotozdownloader.h"
#include "core/totozmanager/qqtmrequester.h"
#include "ui/totozmanager/qqtmlabel.h"

#include <QCursor>
#include <QtDebug>
#include <QLabel>
#include <QGraphicsGridLayout>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QVBoxLayout>

#define TAB_BOOKMARKS_INDEX 0
#define TAB_SEARCH_INDEX 1

#define TOTOZMANAGER_TITLE	"Totoz Manager"

QQTotozManager::QQTotozManager(QQSettings * settings, QWidget *parent) :
	QDockWidget(TOTOZMANAGER_TITLE, parent),
	ui(new Ui::QQTotozManager)
{
	m_positionInitialized = false;

	m_settings = settings;

	m_requester = new QQTMRequester(this, settings);
	connect(m_requester, SIGNAL(requestFinished()), this, SLOT(totozSearchFinished()));

	m_totozDownloader = new QQTotozDownloader(settings);

	ui->setupUi(this);
	if(m_settings->totozServerAllowSearch())
		ui->qqTMTabWidget->setCurrentIndex(TAB_BOOKMARKS_INDEX);
	else
		ui->qqTMTabWidget->removeTab(TAB_SEARCH_INDEX);

	ui->cancelSearchButton->hide();

	this->layout()->setContentsMargins(1, 1, 1, 1);
	ui->qqTMTabWidget->widget(TAB_BOOKMARKS_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);
	ui->qqTMTabWidget->widget(TAB_SEARCH_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);

	ui->serverScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->bookmarkScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->serverScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->bookmarkScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	connect(ui->qqTMTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchTotoz()));
}

QQTotozManager::~QQTotozManager()
{
	delete ui;
}

void QQTotozManager::closeEvent(QCloseEvent * e)
{
	QDockWidget::closeEvent(e);
	m_position = pos();
	m_positionInitialized = true;
}

void QQTotozManager::showEvent(QShowEvent * e)
{
	QDockWidget::showEvent(e);
	if(m_positionInitialized)
		move(m_position);
}

void QQTotozManager::tabChanged(int tabIndex)
{
	ui->qqTMTabWidget->currentWidget()->layout()->setContentsMargins(1, 1, 2, 1);
	if(tabIndex == TAB_SEARCH_INDEX)
	{
		ui->searchLineEdit->setFocus();
	}
}

void QQTotozManager::searchTotoz()
{
	ui->cancelSearchButton->show();

	QString searchStr = ui->searchLineEdit->text();
	if(searchStr.length() >= 3)
	{
		setCursor(QCursor(Qt::BusyCursor));
		ui->searchLineEdit->setCursor(QCursor(Qt::BusyCursor));
		searchLineEditSS = ui->searchLineEdit->styleSheet();
		ui->searchLineEdit->setStyleSheet("QLineEdit{background: black;color: white;}");

		m_requester->searchTotoz(searchStr);
	}
}

void QQTotozManager::totozSearchFinished()
{
	ui->cancelSearchButton->hide();

	setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setStyleSheet(searchLineEditSS);

	QList<QString> results = m_requester->results();
	if(results.size() > 0)
	{
		for(int i = 0; i < results.size(); i++)
		{
			QString result = results.at(i);
			m_totozDownloader->fetchTotoz(result);
		}

		QWidget * widget = new QWidget(this);
		QVBoxLayout * layout = new QVBoxLayout();
		layout->setContentsMargins(0, 0, 0, 0);

		for(int i = 0; i < results.size(); i++)
		{
			QQTMLabel * label = new QQTMLabel(results.at(i));
			label->enableBookmarksMenu();
			connect(label, SIGNAL(totozClicked(QString)), this, SLOT(totozSelected(QString)));
			layout->addWidget(label);
		}

		widget->setLayout(layout);
		QWidget * oldWidget = ui->serverScrollArea->takeWidget();
		ui->serverScrollArea->setWidget(widget);
		ui->serverScrollAreaContents = widget;

		delete oldWidget;
	}
}

void QQTotozManager::totozSearchCanceled()
{
	m_requester->cancel();

	ui->cancelSearchButton->hide();

	setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setStyleSheet(searchLineEditSS);

}

void QQTotozManager::totozSelected(QString anchor)
{
	qDebug() << "QQTotozManager::totozClicked, anchor=" << anchor;
	emit totozClicked(anchor);
}

#include "qqtotozmanager.h"
#include "ui_qqtotozmanager.h"

#include "core/qqtotozdownloader.h"
#include "core/totozmanager/qqtmrequester.h"
#include "ui/qqtotozviewer.h"

#include <QtDebug>
#include <QCursor>
#include <QLabel>
#include <QGraphicsGridLayout>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QVBoxLayout>

#define TAB_BOOKMARKS_INDEX 0
#define TAB_SEARCH_INDEX 1

#define TOTOZMANAGER_OBJECT_NAME "QQTotozManager"

#define TOTOZMANAGER_TITLE	"Totoz Manager"

QQTotozManager::QQTotozManager(QQSettings * settings, QWidget *parent) :
	QDockWidget(TOTOZMANAGER_TITLE, parent),
	ui(new Ui::QQTotozManager)
{
	setObjectName(TOTOZMANAGER_OBJECT_NAME);
	setFeatures(QDockWidget::DockWidgetClosable |
				QDockWidget::DockWidgetMovable |
				QDockWidget::DockWidgetFloatable);

	m_settings = settings;

	m_requester = new QQTMRequester(this, settings);
	connect(m_requester, SIGNAL(requestFinished()), this, SLOT(totozSearchFinished()));

	m_totozDownloader = new QQTotozDownloader(settings);

	ui->setupUi(this);
	this->layout()->setContentsMargins(1, 1, 1, 1);
	ui->qqTMTabWidget->widget(TAB_BOOKMARKS_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);
	ui->qqTMTabWidget->widget(TAB_SEARCH_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);

	if(m_settings->value(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH, DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH).toBool() == true)
		ui->qqTMTabWidget->setCurrentIndex(TAB_SEARCH_INDEX);
	else
		ui->qqTMTabWidget->removeTab(TAB_SEARCH_INDEX);

	ui->cancelSearchButton->hide();
	connect(ui->cancelSearchButton, SIGNAL(clicked()), this, SLOT(totozSearchCanceled()));

	ui->serverScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->bookmarkScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->serverScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->bookmarkScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	connect(ui->qqTMTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchTotoz()));
	connect(ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchBookmarks(QString)));

	ui->dockWidgetContents->setMaximumWidth(ui->qqTMTabWidget->width());

	fillBookmarks();
}

QQTotozManager::~QQTotozManager()
{
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
	QString searchStr = ui->searchLineEdit->text();

	if(ui->qqTMTabWidget->currentIndex() == TAB_BOOKMARKS_INDEX)
	{
		searchBookmarks(ui->searchLineEdit->text());
	}
	else if(ui->qqTMTabWidget->currentIndex() == TAB_SEARCH_INDEX && searchStr.length() >= 3)
	{
		ui->cancelSearchButton->show();

		setCursor(QCursor(Qt::BusyCursor));
		ui->searchLineEdit->setCursor(QCursor(Qt::BusyCursor));
		ui->searchLineEdit->setStyleSheet("QLineEdit{background: black;color: white;}");

		m_requester->searchTotoz(searchStr);
	}
}

void QQTotozManager::totozSearchFinished()
{
	ui->cancelSearchButton->hide();

	setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setStyleSheet("");

	QList<QString> results = m_requester->results();

	QWidget * widget = new QWidget(this);
	QVBoxLayout * layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	if(results.size() > 0)
	{
		for(int i = 0; i < results.size(); i++)
		{
			QString result = results.at(i);
			m_totozDownloader->fetchTotoz(result);
		}

		createViewer(ui->serverScrollArea, results, QQTotoz::ADD);

		for(int i = 0; i < results.size(); i++)
		{
			QQTotozViewer * viewer = new QQTotozViewer(this);
			viewer->enableBookmarksAdd();
			viewer->setTotozDownloader(m_totozDownloader);
			viewer->setTotozId(results.at(i));
			connect(viewer, SIGNAL(totozBookmarkAct(QString,QQTotoz::TotozBookmarkAction)), this, SLOT(totozBookmarkDo(QString,QQTotoz::TotozBookmarkAction)));
			connect(viewer, SIGNAL(totozClicked(QString)), this, SLOT(totozSelected(QString)));
			layout->addWidget(viewer);
		}

	}
	else
	{
		QLabel * label = new QLabel("No totoz found", widget);
		layout->addWidget(label);

	}

	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
	widget->setLayout(layout);
	QWidget * oldWidget = ui->serverScrollArea->takeWidget();
	ui->serverScrollArea->setWidget(widget);
	ui->serverScrollAreaContents = widget;

	oldWidget->deleteLater();
}

void QQTotozManager::totozSearchCanceled()
{
	m_requester->cancel();

	ui->cancelSearchButton->hide();

	setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setCursor(QCursor(Qt::ArrowCursor));
	ui->searchLineEdit->setStyleSheet("");

}

void QQTotozManager::focusInEvent(QFocusEvent * event)
{
	QDockWidget::focusInEvent(event);
	ui->searchLineEdit->setFocus();
}

void QQTotozManager::searchBookmarks(QString text)
{
	QStringList matchingTotozIds;
	if(m_settings->contains(SETTINGS_TOTOZ_BOOKMARKLIST))
	{
		QStringList totozIds = m_settings->value(SETTINGS_TOTOZ_BOOKMARKLIST).toStringList();
		if(text.size() == 0)
			matchingTotozIds << totozIds;
		else
		{
			for(int i = 0; i < totozIds.size(); i++)
			{
				QString totozId = totozIds.at(i);
				if(totozId.contains(text, Qt::CaseInsensitive))
					matchingTotozIds.append(totozId);
			}
		}
	}

	createViewer(ui->bookmarkScrollArea, matchingTotozIds, QQTotoz::REMOVE);
}

void QQTotozManager::totozSelected(QString anchor)
{
	//qDebug() << "QQTotozManager::totozClicked, anchor=" << anchor;
	emit totozClicked(anchor);
}

void QQTotozManager::totozBookmarkDo(QString anchor, QQTotoz::TotozBookmarkAction action)
{
	QStringList totozIds;
	if(m_settings->contains(SETTINGS_TOTOZ_BOOKMARKLIST))
		totozIds = m_settings->value(SETTINGS_TOTOZ_BOOKMARKLIST).toStringList();

	bool modified = false;
	if(totozIds.contains(anchor, Qt::CaseInsensitive))
	{
		if(action == QQTotoz::REMOVE)
		{
			totozIds.removeAll(anchor);
			m_settings->setValue(SETTINGS_TOTOZ_BOOKMARKLIST, totozIds);
			modified = true;
		}
	}
	else
	{
		if(action == QQTotoz::ADD)
		{
			totozIds.append(anchor);
			m_settings->setValue(SETTINGS_TOTOZ_BOOKMARKLIST, totozIds);
			modified = true;
		}
	}

	if(modified)
		fillBookmarks();
}

void QQTotozManager::fillBookmarks()
{
	QStringList totozIds;
	if(m_settings->contains(SETTINGS_TOTOZ_BOOKMARKLIST))
		totozIds = m_settings->value(SETTINGS_TOTOZ_BOOKMARKLIST).toStringList();
	else
		m_settings->setValue(SETTINGS_TOTOZ_BOOKMARKLIST, totozIds);

	createViewer(ui->bookmarkScrollArea, totozIds, QQTotoz::REMOVE);
}

void QQTotozManager::createViewer(QScrollArea * dest, const QStringList & ids, QQTotoz::TotozBookmarkAction action)
{
	QWidget * widget = new QWidget(this);
	QVBoxLayout * layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	for(int i = 0; i < ids.size(); i++)
	{
		QQTotozViewer * viewer = new QQTotozViewer(this);
		viewer->setTotozDownloader(m_totozDownloader);

		if(action == QQTotoz::ADD)
			viewer->enableBookmarksAdd();
		else if(action == QQTotoz::REMOVE)
			viewer->enableBookmarksRem();

		viewer->setTotozId(ids.at(i));
		connect(viewer, SIGNAL(totozBookmarkAct(QString,QQTotoz::TotozBookmarkAction)), this, SLOT(totozBookmarkDo(QString,QQTotoz::TotozBookmarkAction)));
		connect(viewer, SIGNAL(totozClicked(QString)), this, SLOT(totozSelected(QString)));
		layout->addWidget(viewer);
	}

	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

	widget->setLayout(layout);
	QWidget * oldWidget = dest->takeWidget();
	dest->setWidget(widget);

	oldWidget->deleteLater();
}

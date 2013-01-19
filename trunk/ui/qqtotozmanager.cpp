#include "qqtotozmanager.h"
#include "ui_qqtotozmanager.h"

#include "core/qqtotozdownloader.h"
#include "totozmanager/qqtmrequester.h"
#include "totozmanager/qqtmlabel.h"

#include <QCursor>
#include <QtDebug>
#include <QLabel>
#include <QGraphicsGridLayout>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QVBoxLayout>

#define TAB_BOOKMARKS_INDEX 0
#define TAB_SEARCH_INDEX 1

QQTotozManager::QQTotozManager(QQSettings * settings, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQTotozManager)
{
	setModal(false);
	m_positionInitialized = false;

	m_settings = settings;

	m_requester = new QQTMRequester(settings);
	connect(m_requester, SIGNAL(requestFinished()), this, SLOT(totozSearchFinished()));

	m_totozDownloader = new QQTotozDownloader(settings);

	ui->setupUi(this);
	ui->qqTMTabWidget->setCurrentIndex(TAB_BOOKMARKS_INDEX);
	ui->qqTMTabWidget->currentWidget()->layout()->setContentsMargins(1, 1, 2, 1);
	this->layout()->setContentsMargins(1, 1, 1, 1);

	ui->searchScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->searchScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	connect(ui->qqTMTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchTotoz()));
}

QQTotozManager::~QQTotozManager()
{
	delete ui;
	m_requester->deleteLater();
	m_totozDownloader->deleteLater();
}

void QQTotozManager::closeEvent(QCloseEvent * e)
{
	QDialog::closeEvent(e);
	m_position = pos();
	m_positionInitialized = true;
}

void QQTotozManager::showEvent(QShowEvent * e)
{
	QDialog::showEvent(e);
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
			connect(label, SIGNAL(totozClicked(QString)), this, SLOT(totozSelected(QString)));
			layout->addWidget(label);
		}

		widget->setLayout(layout);
		QWidget * oldWidget = ui->searchScrollArea->takeWidget();
		ui->searchScrollArea->setWidget(widget);
		ui->searchScrollAreaContents = widget;

		delete oldWidget;
	}

	// To adjust size horizontaly without changing height

	setMinimumWidth(0);
	setMaximumWidth(QWIDGETSIZE_MAX);
	setFixedHeight(height());
	adjustSize();
	setFixedWidth(width());
	setMinimumHeight(0);
	setMaximumHeight(QWIDGETSIZE_MAX);
}

void QQTotozManager::totozSelected(QString anchor)
{
	qDebug() << "QQTotozManager::totozClicked, anchor=" << anchor;
	emit totozClicked(anchor);
}

#include "qqtotozmanager.h"
#include "ui_qqtotozmanager.h"

#include "core/qqtotozdownloader.h"
#include "core/totozmanager/qqtmrequester.h"
#include "ui/qqtotozviewer.h"

#include "core/qutetools.h"

#include <QtDebug>
#include <QCursor>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QVBoxLayout>
#include <QWidget>

#define TAB_TOTOZ_INDEX 0
#define TAB_EMOJI_INDEX 1

#define MIN_TOTOZ_SEARCH_LEN 3

#define TOTOZMANAGER_OBJECT_NAME "QQTotozManager"

#define TOTOZMANAGER_TITLE	"Totoz Manager"

#define EMOJI_SYMBOL "SYMBOL"
#define EMOJI_IS_CAT "IS_CAT"

QQTotozManager::QQTotozManager(QWidget *parent) :
	QDockWidget(TOTOZMANAGER_TITLE, parent),
	m_ui(new Ui::QQTotozManager),
	m_totozSearchEnabled(true)
{
	setObjectName(TOTOZMANAGER_OBJECT_NAME);
	setFeatures(QDockWidget::DockWidgetClosable |
				QDockWidget::DockWidgetMovable |
				QDockWidget::DockWidgetFloatable);

	QQSettings settings;

	m_requester = new QQTMRequester(this);
	connect(m_requester, SIGNAL(requestFinished()), this, SLOT(totozSearchFinished()));

	m_totozDownloader = new QQTotozDownloader(this);

	m_ui->setupUi(this);

	this->layout()->setContentsMargins(1, 1, 1, 1);
	m_ui->qqTMTabWidget->widget(TAB_TOTOZ_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);
	m_ui->qqTMTabWidget->widget(TAB_EMOJI_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);

	totozSearchEnabled(settings.value(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH, DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH).toBool());

	m_ui->cancelSearchButton->hide();
	m_ui->cancelSearchButton->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(m_ui->cancelSearchButton, SIGNAL(clicked()), this, SLOT(totozSearchCanceled()));

	emojis = settings.listEmojis();

#if(QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	m_ui->searchLineEdit->setClearButtonEnabled(true);
#endif

	connect(m_ui->qqTMTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(m_ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchTotoz()));
	connect(m_ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(handleSearchTextChanged(QString)));

	m_ui->dockWidgetContents->setMaximumWidth(m_ui->qqTMTabWidget->width());

	QVBoxLayout *l = new QVBoxLayout();
	l->setContentsMargins(0, 0, 0, 0);
	m_bookmarkHeaderW = new QLabel(m_ui->totozScrollAreaContents);
	m_bookmarkHeaderW->setText("Bookmarked :");
	l->addWidget(m_bookmarkHeaderW);
	m_bookmarkW = new QWidget(m_ui->totozScrollAreaContents);
	l->addWidget(m_bookmarkW);
	m_searchHeaderW= new QLabel(m_ui->totozScrollAreaContents);
	m_searchHeaderW->setText("Remote search :");
	m_searchHeaderW->hide();
	l->addWidget(m_searchHeaderW);
	m_searchW = new QWidget(m_ui->totozScrollAreaContents);
	m_searchW->hide();
	l->addWidget(m_searchW);
	l->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

	m_ui->totozScrollAreaContents->setLayout(l);
}

QQTotozManager::~QQTotozManager()
{
	delete m_ui;
}

void QQTotozManager::tabChanged(int tabIndex)
{
	handleSearchTextChanged(m_ui->searchLineEdit->text());

	if(tabIndex == TAB_EMOJI_INDEX)
	{
		m_ui->searchLineEdit->hide();

		QList<QQEmojiDef> l;
		foreach (QQEmojiCat c, emojis) {
			l.append(c);
		}

		updateEmojiViewer(l);
	}
	else
	{
		if(m_totozSearchEnabled) {
			m_ui->searchLineEdit->show();
			m_ui->searchLineEdit->setFocus();
		}
	}
}

void QQTotozManager::searchTotoz()
{
	QString searchStr = m_ui->searchLineEdit->text();

	if(m_ui->qqTMTabWidget->currentIndex() == TAB_TOTOZ_INDEX && searchStr.length() >= MIN_TOTOZ_SEARCH_LEN)
	{
		m_ui->cancelSearchButton->show();

		m_ui->dockWidgetContents->setCursor(QCursor(Qt::BusyCursor));
		m_ui->searchLineEdit->setStyleSheet("QLineEdit{background: black;color: white;}");

		m_requester->searchTotoz(searchStr);
	}
}

void QQTotozManager::totozSearchFinished()
{
	m_ui->cancelSearchButton->hide();

	m_ui->dockWidgetContents->unsetCursor();
	m_ui->searchLineEdit->setStyleSheet("");

	m_searchResultList = m_requester->results();

	foreach (QString totoz, m_searchResultList) {
		m_totozDownloader->fetchTotoz(totoz);
	}

	updateTotozViewer();
}

void QQTotozManager::totozSearchCanceled()
{
	m_requester->cancel();

	m_ui->cancelSearchButton->hide();

	m_ui->dockWidgetContents->unsetCursor();
	m_ui->searchLineEdit->setStyleSheet("");

	m_searchResultList.clear();
	updateTotozViewer();
}

void QQTotozManager::totozSearchEnabled(bool enabled)
{
	m_totozSearchEnabled = enabled;
	m_ui->searchLineEdit->setVisible(enabled);
}

QStringList QQTotozManager::m_tTZBookmarkListCache;
QStringList QQTotozManager::bookmarkedTotozIds()
{
	//Temporaire
	QQSettings settings;
	if(settings.contains(SETTINGS_TOTOZ_BOOKMARKLIST))
	{
		m_tTZBookmarkListCache = settings.value(SETTINGS_TOTOZ_BOOKMARKLIST, "").toStringList();
		setBookmarkedTotozIds(m_tTZBookmarkListCache);
		settings.remove(SETTINGS_TOTOZ_BOOKMARKLIST);
	}
	//

	if(m_tTZBookmarkListCache.isEmpty())
	{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		QDir dirData(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#else
		QDir dirData(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif
		if(!dirData.exists())
			dirData.mkpath(dirData.path());

		QFile totozBmFile(dirData.filePath(TOTOZ_BOOKMARKS_FILE));

		if(!totozBmFile.open(QIODevice::ReadWrite | QIODevice::Text))
		{
			QFileInfo fInfo(totozBmFile);
			qCritical() << "Can't open" << fInfo.absoluteFilePath() << "in read/write mode";
		}

		while(!totozBmFile.atEnd())
				m_tTZBookmarkListCache.append(QString(totozBmFile.readLine().trimmed()));

		totozBmFile.close();
	}

	return m_tTZBookmarkListCache;
}

void QQTotozManager::setBookmarkedTotozIds(QStringList newList)
{
	m_tTZBookmarkListCache = newList;

#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QDir dirData(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#else
	QDir dirData(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif

	if(!dirData.exists())
		dirData.mkpath(dirData.path());

	QFile totozBmFile(dirData.filePath(TOTOZ_BOOKMARKS_FILE));
	if(!totozBmFile.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		QFileInfo fInfo(totozBmFile);
		qCritical() << "Can't open" << fInfo.absoluteFilePath() << "in read/write mode";
	}

	QTextStream str(& totozBmFile);
	foreach(QString totozId, m_tTZBookmarkListCache)
		str << totozId << "\n";

	totozBmFile.close();
}

//////////////////////////////////////////////////////////////
/// \brief QQTotozManager::setVisible
/// \param visible
///
void QQTotozManager::setVisible(bool visible)
{
	if(! visible)
	{
		m_ui->searchLineEdit->clear();

		if(QuteTools::checkFocusRecurse(this) &&
		   m_oldFocusWidget != NULL &&
		   m_oldFocusWidget->isVisible())
			m_oldFocusWidget->setFocus();

	}
	else
	{
		m_oldFocusWidget = QApplication::focusWidget();
		m_ui->searchLineEdit->setFocus();
	}

	QDockWidget::setVisible(visible);
}

void QQTotozManager::focusInEvent(QFocusEvent *event)
{
	QDockWidget::focusInEvent(event);
	m_ui->searchLineEdit->setFocus();
}

void QQTotozManager::showEvent(QShowEvent *ev)
{
	QDockWidget::showEvent(ev);

	updateTotozViewer();
}

void QQTotozManager::handleSearchTextChanged(QString text)
{
	if(text.size() < MIN_TOTOZ_SEARCH_LEN)
	{
		QLayout *l = m_searchW->layout();
		if(l != NULL)
			delete l;
	}

	if(m_ui->qqTMTabWidget->currentIndex() == TAB_TOTOZ_INDEX)
	{
		QStringList matchingTotozIds;
		QStringList totozIds = bookmarkedTotozIds();
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

		updateTotozViewer();
	}
}

void QQTotozManager::totozBookmarkDo(QString anchor, QQTotoz::TotozBookmarkAction action)
{
	QStringList totozIds = bookmarkedTotozIds();

	bool modified = false;
	if(totozIds.contains(anchor, Qt::CaseInsensitive))
	{
		if(action == QQTotoz::REMOVE)
		{
			totozIds.removeAll(anchor);
			modified = true;
		}
	}
	else
	{
		if(action == QQTotoz::ADD)
		{
			totozIds.append(anchor);
			modified = true;
		}
	}

	if(modified)
	{
		setBookmarkedTotozIds(totozIds);
		updateTotozViewer();
	}
}

void QQTotozManager::updateTotozViewer()
{
	QWidget *widget = new QWidget(this);

	/* Bookmarked */
	QVBoxLayout *l = new QVBoxLayout(widget);
	l->setContentsMargins(0, 0, 0, 0);

	QString searchText = m_ui->searchLineEdit->text();
	QStringList ids;
	if(searchText.length() > 0)
	{
		foreach (QString id, bookmarkedTotozIds())
		{
			if(id.contains(searchText, Qt::CaseInsensitive))
				ids.append(id);
		}
	}
	else
	{
		ids << bookmarkedTotozIds();
	}


	foreach (QString id, ids)
	{
		QQTotozViewer *viewer = new QQTotozViewer(widget);
		viewer->setTotozDownloader(m_totozDownloader);
		viewer->enableBookmarksRem();
		viewer->setShowAtMousePos(false);
		viewer->setTotozId(id);

		connect(viewer, SIGNAL(totozBookmarkAct(QString,QQTotoz::TotozBookmarkAction)), this, SLOT(totozBookmarkDo(QString,QQTotoz::TotozBookmarkAction)));
		connect(viewer, SIGNAL(totozClicked(QString)), this, SIGNAL(totozClicked(QString)));
		l->addWidget(viewer);
	}
	QLayout *oldL = m_bookmarkW->layout();
	if(oldL != NULL)
	{
		QLayoutItem *child;
		while ((child = oldL->takeAt(0)) != 0) {
			delete child->widget();
			delete child;
		}
		delete oldL;
	}

	m_bookmarkW->setLayout(l);

	/*
	for(int i = 0; i < ids.size(); i++)
	{
		QQTotozViewer *viewer = new QQTotozViewer(widget);
		viewer->setTotozDownloader(m_totozDownloader);

		if(action == QQTotoz::ADD)
			viewer->enableBookmarksAdd();
		else if(action == QQTotoz::REMOVE)
			viewer->enableBookmarksRem();
		viewer->setShowAtMousePos(false);

		viewer->setTotozId(ids.at(i));
		connect(viewer, SIGNAL(totozBookmarkAct(QString,QQTotoz::TotozBookmarkAction)), this, SLOT(totozBookmarkDo(QString,QQTotoz::TotozBookmarkAction)));
		connect(viewer, SIGNAL(totozClicked(QString)), this, SIGNAL(totozClicked(QString)));
		layout->addWidget(viewer);
	}


	widget->setLayout(layout);
	QWidget *oldWidget = dest->takeWidget();
	dest->setWidget(widget);

	//Doit etre supprime "plus tard" car ici on peut avoir ete appele par le widget qu'on va detruire ici-meme
	oldWidget->deleteLater();
	*/
}

void QQTotozManager::emojiSelected()
{
	QObject *o = sender();
	if(o != NULL)
	{
		if(o->property(EMOJI_IS_CAT).toBool()) {
			bool found = false;
			foreach (QQEmojiCat c, emojis) {
				if(c.symbol == o->property(EMOJI_SYMBOL)) {
					found = true;

					QList<QQEmojiDef> defs = c.emojis;

					// Creation d'une entree "speciale" permettant de revenir au niveau 0
					//  et ajout en debut de liste
					QQEmojiDef folderUp;
					folderUp.name = "..";
					folderUp.symbol = "📁";
					folderUp.type = CAT;
					defs.prepend(folderUp);

					updateEmojiViewer(defs);
					break;
				}
			}

			if(! found) {
				/* Pas de sub trouve, on retourne au niveau 0 */
				QList<QQEmojiDef> l;
				foreach (QQEmojiCat c, emojis) {
					l.append(c);
				}

				updateEmojiViewer(l);
			}
		}
		else
		{
			emit totozClicked(o->property(EMOJI_SYMBOL).toString());
		}
	}
	else
	{
		qDebug() << Q_FUNC_INFO << "o is NULL";
	}
}


void QQTotozManager::updateEmojiViewer(const QList<QQEmojiDef> &emojis)
{
	QWidget *widget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);

	for(int i = 0; i < emojis.size(); i++)
	{
		QPushButton *b = new QPushButton(widget);
		b->setFlat(true);
		b->setStyleSheet("Text-align: left");
		b->setProperty(EMOJI_SYMBOL, emojis[i].symbol);
		b->setProperty(EMOJI_IS_CAT, emojis[i].type == CAT);
		b->setText(QString(emojis[i].symbol).append(" ").append(emojis[i].name));
		connect(b, SIGNAL(clicked(bool)), this, SLOT(emojiSelected()));
		layout->addWidget(b);
	}

	layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

	widget->setLayout(layout);
	QWidget *oldWidget = m_ui->emojiScrollArea->takeWidget();
	m_ui->emojiScrollArea->setWidget(widget);

	//Doit etre supprime "plus tard" car ici on peut avoir ete appele par le widget qu'on va detruire ici-meme
	oldWidget->deleteLater();
}

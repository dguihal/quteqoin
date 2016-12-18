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
#define REMOTE_SEARCH_LATENCY_MS 500

#define TOTOZMANAGER_OBJECT_NAME "QQTotozManager"

#define TOTOZMANAGER_TITLE	"Totoz Manager"

#define EMOJI_SYMBOL "SYMBOL"
#define EMOJI_IS_CAT "IS_CAT"

QQTotozManager::QQTotozManager(QWidget *parent) :
	QDockWidget(TOTOZMANAGER_TITLE, parent),
	m_ui(new Ui::QQTotozManager),
	m_totozSearchEnabled(true),
	m_searchQueryTemperer(new QTimer(this))
{
	setObjectName(TOTOZMANAGER_OBJECT_NAME);
	setFeatures(QDockWidget::DockWidgetClosable |
				QDockWidget::DockWidgetMovable |
				QDockWidget::DockWidgetFloatable);

	m_searchQueryTemperer->setSingleShot(true);
	m_searchQueryTemperer->setInterval(REMOTE_SEARCH_LATENCY_MS);
	connect(m_searchQueryTemperer, SIGNAL(timeout()),
			this, SLOT(searchTotoz()));

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

#if(QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	m_ui->searchLineEdit->setClearButtonEnabled(true);
#endif

	connect(m_ui->qqTMTabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(m_ui->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchTotoz()));
	connect(m_ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(handleSearchTextChanged(QString)));

	m_ui->dockWidgetContents->setMaximumWidth(m_ui->qqTMTabWidget->width());

	// Construction de la fenetre de visu des totoz
	QVBoxLayout *l = new QVBoxLayout();
	l->setContentsMargins(0, 0, 0, 0);
	m_bookmarkHeaderW = new QLabel(m_ui->totozScrollAreaContents);
	m_bookmarkHeaderW->setText("Bookmarked :");
	m_bookmarkHeaderW->setStyleSheet("QLabel {font: bold}");
	l->addWidget(m_bookmarkHeaderW);
	m_bookmarkW = new QWidget(m_ui->totozScrollAreaContents);
	l->addWidget(m_bookmarkW);
	m_searchHeaderW= new QLabel(m_ui->totozScrollAreaContents);
	m_searchHeaderW->setText("Remote search :");
	m_searchHeaderW->setStyleSheet("QLabel {font: bold}");
	m_searchHeaderW->hide();
	l->addWidget(m_searchHeaderW);
	m_searchW = new QWidget(m_ui->totozScrollAreaContents);
	m_searchW->hide();
	l->addWidget(m_searchW);
	l->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

	m_ui->totozScrollAreaContents->setLayout(l);

	// Construction de la fenetre de visu des emojis
	updateEmojiViewer(settings.listEmojis());

}

QQTotozManager::~QQTotozManager()
{
	delete m_ui;
}

void QQTotozManager::tabChanged(int tabIndex)
{
	m_ui->searchLineEdit->clear();
	handleSearchTextChanged(""); // Reset de l'affichage

	switch(tabIndex)
	{
	case TAB_EMOJI_INDEX:
		m_ui->searchLineEdit->show();
		m_ui->searchLineEdit->setFocus();
	case TAB_TOTOZ_INDEX:
		if(m_totozSearchEnabled)
		{
			m_ui->searchLineEdit->show();
			m_ui->searchLineEdit->setFocus();
		}
		else
			m_ui->searchLineEdit->hide();
	default:
		qWarning() << Q_FUNC_INFO << "Unknown tab index";
	}
}

void QQTotozManager::totozSearchFinished()
{
	m_ui->cancelSearchButton->hide();
	m_searchHeaderW->show();
	m_searchW->show();

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
	m_searchHeaderW->hide();
	m_searchW->hide();

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
	m_searchQueryTemperer->stop();
	m_requester->cancel();

	switch(m_ui->qqTMTabWidget->currentIndex())
	{
	case TAB_TOTOZ_INDEX:
	{
		if(text.size() < MIN_TOTOZ_SEARCH_LEN)
		{
			m_searchHeaderW->hide();
			m_searchW->hide();
		}
		else if(m_totozSearchEnabled)
		{
			m_searchQueryTemperer->start();

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
		}

		updateTotozViewer();
	}
		break;
	case TAB_EMOJI_INDEX:
	{
		QQSettings settings;

		if(text.size() > 0)
		{
			QList<QQEmojiDef> l;
			foreach (QQEmojiCat c, settings.listEmojis()) {
				foreach (QQEmojiDef d, c.emojis) {
					if(d.name.contains(text, Qt::CaseInsensitive))
						l.append(d);
				}
			}
			updateEmojiViewer(l);
		}
		else
			updateEmojiViewer(settings.listEmojis());
	}
		break;
	default:
		qWarning() << Q_FUNC_INFO << "Unknown tab index";
	}
}

void  QQTotozManager::searchTotoz()
{
	QString searchStr = m_ui->searchLineEdit->text();

	if(m_totozSearchEnabled &&
			m_ui->qqTMTabWidget->currentIndex() == TAB_TOTOZ_INDEX &&
			searchStr.length() >= MIN_TOTOZ_SEARCH_LEN)
	{
		m_searchHeaderW->show();
		m_searchW->show();

		m_ui->cancelSearchButton->show();

		m_ui->dockWidgetContents->setCursor(QCursor(Qt::BusyCursor));
		m_ui->searchLineEdit->setStyleSheet("QLineEdit{background: black;color: white;}");

		m_requester->searchTotoz(searchStr);
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
	/* Bookmarked */
	QVBoxLayout *l = new QVBoxLayout();
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
		ids << bookmarkedTotozIds();

	foreach (QString id, ids)
	{
		QQTotozViewer *viewer = new QQTotozViewer(m_bookmarkW);
		viewer->setTotozDownloader(m_totozDownloader);
		viewer->enableBookmarksRem();
		viewer->setShowAtMousePos(false);
		viewer->setTotozId(id);

		connect(viewer, SIGNAL(totozBookmarkAct(QString,QQTotoz::TotozBookmarkAction)),
				this, SLOT(totozBookmarkDo(QString,QQTotoz::TotozBookmarkAction)));
		connect(viewer, SIGNAL(totozClicked(QString)), this, SIGNAL(totozClicked(QString)));
		l->addWidget(viewer);
	}
	QLayout *oldL = m_bookmarkW->layout();
	if(oldL != NULL)
	{
		QLayoutItem *child;
		while ((child = oldL->takeAt(0)) != 0)
			child->widget()->deleteLater();
		delete oldL;
	}

	m_bookmarkW->setLayout(l);


	if(m_searchHeaderW->isHidden())
		return;

	// Recherche server
	l = new QVBoxLayout();
	l->setContentsMargins(0, 0, 0, 0);

	foreach (QString id, m_searchResultList)
	{
		QQTotozViewer *viewer = new QQTotozViewer(m_searchW);
		viewer->setTotozDownloader(m_totozDownloader);
		viewer->enableBookmarksAdd();
		viewer->setShowAtMousePos(false);
		viewer->setTotozId(id);

		connect(viewer, SIGNAL(totozBookmarkAct(QString,QQTotoz::TotozBookmarkAction)),
				this, SLOT(totozBookmarkDo(QString,QQTotoz::TotozBookmarkAction)));
		connect(viewer, SIGNAL(totozClicked(QString)), this, SIGNAL(totozClicked(QString)));
		l->addWidget(viewer);
	}

	if(m_searchResultList.length() == 0)
		l->addWidget(new QLabel("Empty result search", m_searchW));

	oldL = m_searchW->layout();
	if(oldL != NULL)
	{
		QLayoutItem *child;
		while ((child = oldL->takeAt(0)) != 0)
			child->widget()->deleteLater();
		delete oldL;
	}

	m_searchW->setLayout(l);
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
					folderUp.symbol = QString::fromUtf8("\xf0\x9f\x93\x81"); //'FILE FOLDER' (U+1F4C1) -> '📁';
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

	foreach (QQEmojiDef d, emojis) {
		QPushButton *b = new QPushButton(widget);
		b->setFlat(true);
		b->setStyleSheet("Text-align: left");
		b->setProperty(EMOJI_SYMBOL, d.symbol);
		b->setProperty(EMOJI_IS_CAT, d.type == CAT);
		b->setText(QString(d.symbol).append(" ").append(d.name));
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

void QQTotozManager::updateEmojiViewer(const QList<QQEmojiCat> &emojis)
{
	QList<QQEmojiDef> l;
	foreach (QQEmojiCat c, emojis) {
		l.append(c);
	}
	updateEmojiViewer(l);
}

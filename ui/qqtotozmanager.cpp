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
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWidget>
#include <utility>

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
	connect(m_searchQueryTemperer, &QTimer::timeout, this, &QQTotozManager::searchTotoz);

	QQSettings settings;

	m_requester = new QQTMRequester(this);
	connect(m_requester, &QQTMRequester::requestFinished, this, &QQTotozManager::totozSearchFinished);

	m_totozDownloader = new QQTotozDownloader(this);

	m_ui->setupUi(this);

	this->layout()->setContentsMargins(1, 1, 1, 1);
	m_ui->qqTMTabWidget->widget(TAB_TOTOZ_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);
	m_ui->qqTMTabWidget->widget(TAB_EMOJI_INDEX)->layout()->setContentsMargins(0, 1, 0, 1);

	totozSearchEnabled(settings.value(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH, DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH).toBool());

	m_ui->cancelSearchButton->hide();
	m_ui->cancelSearchButton->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(m_ui->cancelSearchButton, &QPushButton::clicked, this, &QQTotozManager::totozSearchCanceled);

	m_ui->searchLineEdit->setClearButtonEnabled(true);

	connect(m_ui->qqTMTabWidget, &QTabWidget::currentChanged, this, &QQTotozManager::tabChanged);
	connect(m_ui->searchLineEdit, &QLineEdit::returnPressed, this, &QQTotozManager::searchTotoz);
	connect(m_ui->searchLineEdit, &QLineEdit::textChanged, this, &QQTotozManager::handleSearchTextChanged);

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
	m_emojis = settings.listEmojis();
	updateEmojiViewer(m_emojis);

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
		break;
	case TAB_TOTOZ_INDEX:
		if(m_totozSearchEnabled)
		{
			m_ui->searchLineEdit->show();
			m_ui->searchLineEdit->setFocus();
		}
		else
			m_ui->searchLineEdit->hide();
		break;
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

	for (QString totoz : std::as_const(m_searchResultList)) {
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
		QDir dirData(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
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

	QDir dirData(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

	if(!dirData.exists())
		dirData.mkpath(dirData.path());

	QFile totozBmFile(dirData.filePath(TOTOZ_BOOKMARKS_FILE));
	if(!totozBmFile.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		QFileInfo fInfo(totozBmFile);
		qCritical() << "Can't open" << fInfo.absoluteFilePath() << "in read/write mode";
	}

	QTextStream str(& totozBmFile);
	for (const QString &totozId : std::as_const(m_tTZBookmarkListCache))
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
		   m_oldFocusWidget != nullptr &&
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
		if(text.size() < MIN_TOTOZ_SEARCH_LEN)
		{
			m_searchHeaderW->hide();
			m_searchW->hide();
			m_searchResultList.clear();
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
		break;
	case TAB_EMOJI_INDEX:
		if(text.size() > 0)
		{
			QList<QQEmojiDef> l;
			for (const QQEmojiCat &c : std::as_const(m_emojis)) {
				for (const QQEmojiDef &d : std::as_const(c.emojis)) {
					if(d.name.contains(text, Qt::CaseInsensitive) ||
					   d.symbol == text)
						l.append(d);
				}
			}
			updateEmojiViewer(l);
		}
		else
			updateEmojiViewer(m_emojis);
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
		const QStringList bookmarks = bookmarkedTotozIds();
		for (const QString &id : bookmarks)
		{
			if(id.contains(searchText, Qt::CaseInsensitive))
				ids.append(id);
		}
	}
	else
		ids << bookmarkedTotozIds();

	for (const QString &id : std::as_const(ids))
	{
		QQTotozViewer *viewer = new QQTotozViewer(m_bookmarkW);
		viewer->setTotozDownloader(m_totozDownloader);
		viewer->enableBookmarksRem();
		viewer->setShowAtMousePos(false);
		viewer->setTotozId(id);

		connect(viewer, &QQTotozViewer::totozBookmarkAct, this, &QQTotozManager::totozBookmarkDo);
		connect(viewer, &QQTotozViewer::totozClicked, this, &QQTotozManager::totozClicked);
		l->addWidget(viewer);
	}
	QLayout *oldL = m_bookmarkW->layout();
	if(oldL != nullptr)
	{
		QLayoutItem *child;
		while ((child = oldL->takeAt(0)) != nullptr)
			child->widget()->deleteLater();
		delete oldL;
	}

	m_bookmarkW->setLayout(l);

	if(m_searchHeaderW->isHidden())
		return;

	// Recherche server
	l = new QVBoxLayout();
	l->setContentsMargins(0, 0, 0, 0);

	const QStringList &results = m_searchResultList;
	for (const QString &id : results)
	{
		QQTotozViewer *viewer = new QQTotozViewer(m_searchW);
		viewer->setTotozDownloader(m_totozDownloader);
		viewer->enableBookmarksAdd();
		viewer->setShowAtMousePos(false);
		viewer->setTotozId(id);

		connect(viewer, &QQTotozViewer::totozBookmarkAct, this, &QQTotozManager::totozBookmarkDo);
		connect(viewer, &QQTotozViewer::totozClicked, this, &QQTotozManager::totozClicked);
		l->addWidget(viewer);
	}

	if(m_searchResultList.length() == 0)
		l->addWidget(new QLabel("Empty result search", m_searchW));

	oldL = m_searchW->layout();
	if(oldL != nullptr)
	{
		QLayoutItem *child;
		while ((child = oldL->takeAt(0)) != nullptr)
			child->widget()->deleteLater();
		delete oldL;
	}

	m_searchW->setLayout(l);
	m_searchW->show();
}

void QQTotozManager::emojiSelected()
{
	QObject *o = sender();
	if(o != nullptr)
	{
		if(o->property(EMOJI_IS_CAT).toBool())
		{
			bool found = false;
			for (const QQEmojiCat &c : std::as_const(m_emojis))
			{
				if(c.symbol == o->property(EMOJI_SYMBOL))
				{
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

			if(! found)
			{
				/* Pas de sub trouve, on retourne au niveau 0 */
				QList<QQEmojiDef> l;
				for (const QQEmojiCat &c : std::as_const(m_emojis)) {
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
		qDebug() << Q_FUNC_INFO << "o is NULL";
}

void QQTotozManager::updateEmojiViewer(const QList<QQEmojiDef> &emojis)
{
	QWidget *widget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);

	QQSettings settings;
	QFont piniFont;
	piniFont.fromString(settings.value(SETTINGS_GENERAL_DEFAULT_FONT, DEFAULT_GENERAL_DEFAULT_FONT).toString());

	for (const QQEmojiDef &d : std::as_const(emojis))
	{
		QPushButton *b = new QPushButton(widget);
		b->setFlat(true);
		b->setStyleSheet("QPushButton {text-align: left}");
		b->setProperty(EMOJI_SYMBOL, d.symbol);
		b->setProperty(EMOJI_IS_CAT, d.type == CAT);
		b->setText(QString(d.symbol).append(" ").append(d.name));
		b->setFont(piniFont);
		connect(b, &QPushButton::clicked, this, &QQTotozManager::emojiSelected);
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
	for (const QQEmojiCat &c : std::as_const(emojis))
	{
		l.append(c);
	}
	updateEmojiViewer(l);
}

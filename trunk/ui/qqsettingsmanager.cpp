#include "qqsettingsmanager.h"
#include "ui_qqsettingsmanager.h"

#include "core/qqsettings.h"
#include "core/qqtypes.h"
#include "ui/settingsmanager/qqboardssettings.h"
#include "ui/settingsmanager/qqfiltersettings.h"
#include "ui/settingsmanager/qqgeneralsettings.h"
#include "ui/settingsmanager/qqpalmisettings.h"
#include "ui/settingsmanager/qqtotozsettings.h"

#include <QtDebug>

#define ITEM_GENERAL_TYPE (QListWidgetItem::UserType)
#define ITEM_TOTOZ_TYPE (QListWidgetItem::UserType + 1)
#define ITEM_BOARDS_TYPE (QListWidgetItem::UserType + 2)
#define ITEM_PALMI_TYPE (QListWidgetItem::UserType + 3)
#define ITEM_FILTER_TYPE (QListWidgetItem::UserType + 4)
#define ITEM_HUNT_TYPE (QListWidgetItem::UserType + 5)


QQSettingsManager::QQSettingsManager(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQSettingsManager)
{
	ui->setupUi(this);
	needPiniFullRepaint = false;
	QListWidget *listSettingsTheme = ui->listSettingsTheme;

	QLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	listSettingsTheme->setSortingEnabled(false);
	listSettingsTheme->setIconSize(QSize(32, 32));

	listSettingsTheme->addItem(
				new QListWidgetItem(QIcon(":/img/general-icon.png"), tr("General"),
									listSettingsTheme, ITEM_GENERAL_TYPE)
				);
	m_generalSettingsW = new QQGeneralSettings(this);
	initGeneralSettings();
	m_generalSettingsW->hide();
	layout->addWidget(m_generalSettingsW);

	listSettingsTheme->addItem(
				new QListWidgetItem(QIcon(":/img/totoz-icon.jpeg"), tr("Totoz"),
									listSettingsTheme, ITEM_TOTOZ_TYPE)
				);
	m_totozSettingsW = new QQTotozSettings(this);
	initTotozSettings();
	m_totozSettingsW->hide();
	layout->addWidget(m_totozSettingsW);

	listSettingsTheme->addItem(
				new QListWidgetItem(QIcon(":/img/board-icon.png"), tr("Boards"),
									listSettingsTheme, ITEM_BOARDS_TYPE)
				);
	m_boardsSettingsW = new QQBoardsSettings(this);
	initBoardsSettings();
	m_boardsSettingsW->hide();
	layout->addWidget(m_boardsSettingsW);

	listSettingsTheme->addItem(
				new QListWidgetItem(QIcon(":/img/palmipede-icon.png"), tr("Palmipede"),
									listSettingsTheme, ITEM_PALMI_TYPE)
				);
	m_palmiSettingsW = new QQPalmiSettings(this);
	initPalmiSettings();
	m_palmiSettingsW->hide();
	layout->addWidget(m_palmiSettingsW);

	listSettingsTheme->addItem(
				new QListWidgetItem(QIcon(":/img/filter-icon.png"), tr("Filters"),
									listSettingsTheme, ITEM_FILTER_TYPE)
				);
	m_filterSettingsW = new QQFilterSettings(this);
	initFilterSettings();
	m_filterSettingsW->hide();
	layout->addWidget(m_filterSettingsW);

	listSettingsTheme->addItem(
				new QListWidgetItem(QIcon(":/img/hunt-icon.png"), tr("Hunt"),
									listSettingsTheme, ITEM_HUNT_TYPE)
				);
	//m_huntSettingsW = new QQHuntSettings(this);
	initHuntSettings();
	//m_huntSettingsW->hide();
	//layout->addWidget(m_huntSettingsW);

	listSettingsTheme->setMaximumWidth(listSettingsTheme->sizeHintForColumn(0) + 15);
	connect(listSettingsTheme, SIGNAL(itemSelectionChanged()),
			this, SLOT(configItemChanged()));

	ui->settingsSelWidget->setLayout(layout);
	listSettingsTheme->setCurrentRow(0);
}

QQSettingsManager::~QQSettingsManager()
{
	delete ui;
}

void QQSettingsManager::accept()
{
	saveBoardsSettings();
	saveFilterSettings();
	saveGeneralSettings();
	saveHuntSettings();
	saveTotozSettings();
	savePalmiSettings();

	if(needPiniFullRepaint)
		emit fullRepaint();

	QDialog::accept();
}

void QQSettingsManager::configItemChanged()
{
	QListWidgetItem *item = ui->listSettingsTheme->currentItem();

	ui->settingsThemeLabel->setText(item->text());

	m_generalSettingsW->hide();
	m_totozSettingsW->hide();
	m_boardsSettingsW->hide();
	m_palmiSettingsW->hide();
	m_filterSettingsW->hide();
	//m_huntSettingsW->hide();
	switch(item->type())
	{
	case ITEM_GENERAL_TYPE:
		m_generalSettingsW->show();
		break;
	case ITEM_TOTOZ_TYPE:
		m_totozSettingsW->show();
		break;
	case ITEM_BOARDS_TYPE:
		m_boardsSettingsW->show();
		break;
	case ITEM_PALMI_TYPE:
		m_palmiSettingsW->show();
		break;
	case ITEM_FILTER_TYPE:
		m_filterSettingsW->show();
		break;
	case ITEM_HUNT_TYPE:
	//	m_huntSettingsW->show();
		break;
	default:
		qWarning() << "Unknown type : " << item->type() << ", ignoring";
	}
}

void QQSettingsManager::initBoardsSettings()
{
	QMap<QString, QQBouchot::QQBouchotSettings> mapBouchotSettings;

	//Les bouchots existant
	QList<QQBouchot *> listBouchots = QQBouchot::listBouchots();
	QQBouchot *bouchot = NULL;
	for(int i = 0; i < listBouchots.size(); i++)
	{
		bouchot = listBouchots.at(i);
		mapBouchotSettings.insert(bouchot->name(), bouchot->settings());
	}
	m_boardsSettingsW->setBouchots(mapBouchotSettings);
}

void QQSettingsManager::saveBoardsSettings()
{
	QQSettings settings;

	QQBouchot *bouchot = NULL;

	// Les bouchots supprimes
	QStringList lstOldBouchots = m_boardsSettingsW->getOldBouchots();
	for(int i = 0; i < lstOldBouchots.size(); i++)
	{
		settings.removeBouchot(lstOldBouchots.at(i));
		bouchot = QQBouchot::bouchot(lstOldBouchots.at(i));
		if(bouchot != NULL)
			delete bouchot;
	}

	// Les bouchots modifies
	QMap<QString, QQBouchot::QQBouchotSettings> mBouchots = m_boardsSettingsW->getModifBouchots();
	QList<QString> bouchotNames = mBouchots.keys();
	for(int i = 0; i < bouchotNames.size(); i++)
	{
		QString bouchotName = bouchotNames.at(i);
		bouchot = QQBouchot::bouchot(bouchotName);
		if(bouchot != NULL)
		{
			bouchot->setSettings(mBouchots.value(bouchotName));
			settings.saveBouchot(bouchotName, mBouchots.value(bouchotName));
		}
	}
	// Les bouchots ajoutes
	mBouchots = m_boardsSettingsW->getNewBouchots();
	bouchotNames = mBouchots.keys();
	for(int i = 0; i < bouchotNames.size(); i++)
	{
		QString bouchotName = bouchotNames.at(i);
		settings.saveBouchot(bouchotName, mBouchots.value(bouchotName));
		emit bouchotCreated(settings.loadBouchot(bouchotName));
	}
}

void QQSettingsManager::initFilterSettings()
{
	QQSettings settings;

	bool enableUrlTransformer = settings.value(SETTINGS_FILTER_SMART_URL_TRANSFORMER,  DEFAULT_FILTER_SMART_URL_TRANSFORMER).toBool();
	m_filterSettingsW->setSmartUrlEnabled(enableUrlTransformer);

	QuteQoin::QQSmartUrlFilerTransformType smartUrlTransformerType = (QuteQoin::QQSmartUrlFilerTransformType) settings.value(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE,  DEFAULT_FILTER_SMART_URL_TRANSFORM_TYPE).toInt();
	m_filterSettingsW->setSmartUrlTransformerType(smartUrlTransformerType);
}

void QQSettingsManager::saveFilterSettings()
{
	QQSettings settings;

	bool enableUrlTransformer = m_filterSettingsW->isSmartUrlEnabled();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_FILTER_SMART_URL_TRANSFORMER, enableUrlTransformer, DEFAULT_FILTER_SMART_URL_TRANSFORMER);

	QuteQoin::QQSmartUrlFilerTransformType trType = m_filterSettingsW->smartUrlTransformerType();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE, trType, DEFAULT_FILTER_SMART_URL_TRANSFORM_TYPE);
}

void QQSettingsManager::initGeneralSettings()
{
	QQSettings settings;

	QString historySize = settings.value(SETTINGS_GENERAL_MAX_HISTLEN, DEFAULT_GENERAL_MAX_HISTLEN).toString();
	m_generalSettingsW->setMaxHistorySize(historySize);

	QString defaultLogin = settings.value(SETTINGS_GENERAL_DEFAULT_LOGIN, DEFAULT_GENERAL_DEFAULT_LOGIN).toString();
	m_generalSettingsW->setDefaultLogin(defaultLogin);

	QString defaultUA = settings.value(SETTINGS_GENERAL_DEFAULT_UA, DEFAULT_GENERAL_DEFAULT_UA).toString();
	m_generalSettingsW->setDefaultUA(defaultUA);

	QString defaultWebSearchUrl = settings.value(SETTINGS_GENERAL_WEBSEARCH_URL, DEFAULT_GENERAL_WEBSEARCH_URL).toString();
	m_generalSettingsW->setDefaultWebSearchUrl(defaultWebSearchUrl);

	QFont defaultFont;
	defaultFont.fromString(settings.value(SETTINGS_GENERAL_DEFAULT_FONT, DEFAULT_GENERAL_DEFAULT_FONT).toString());
	m_generalSettingsW->setDefaultFont(defaultFont);

	QString hColor = settings.value(SETTINGS_GENERAL_HIGHLIGHT_COLOR, DEFAULT_GENERAL_HIGHLIGHT_COLOR).toString();
	m_generalSettingsW->setHightLightColor(hColor);
}

void QQSettingsManager::saveGeneralSettings()
{
	QQSettings settings;

	QString historySize = m_generalSettingsW->maxHistorySize();
	settings.setValueWithDefault(SETTINGS_GENERAL_MAX_HISTLEN, historySize, DEFAULT_GENERAL_MAX_HISTLEN);

	QString defaultLogin = m_generalSettingsW->defaultLogin();
	settings.setValueWithDefault(SETTINGS_GENERAL_DEFAULT_LOGIN, defaultLogin, DEFAULT_GENERAL_DEFAULT_LOGIN);

	QString defaultUA = m_generalSettingsW->defaultUA();
	settings.setValueWithDefault(SETTINGS_GENERAL_DEFAULT_UA, defaultUA, DEFAULT_GENERAL_DEFAULT_UA);

	QString defaultWebSearchUrl = m_generalSettingsW->defaultWebSearchUrl();
	settings.setValueWithDefault(SETTINGS_GENERAL_WEBSEARCH_URL, defaultWebSearchUrl, DEFAULT_GENERAL_WEBSEARCH_URL);

	QString defaultFont = m_generalSettingsW->defaultFont().toString();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_GENERAL_DEFAULT_FONT, defaultFont, DEFAULT_GENERAL_DEFAULT_FONT);

	QString hColor = m_generalSettingsW->highlightColor();
	settings.setValueWithDefault(SETTINGS_GENERAL_HIGHLIGHT_COLOR, hColor, DEFAULT_GENERAL_HIGHLIGHT_COLOR);
}

void QQSettingsManager::initHuntSettings()
{
	//TODO
}

void QQSettingsManager::saveHuntSettings()
{
	//TODO
}

void QQSettingsManager::initPalmiSettings()
{
	QQSettings settings;

	//Palmi shortcuts
	m_palmiSettingsW->setStaticShortcuts(settings.staticPalmiShorcuts());
	m_palmiSettingsW->setUserShortcuts(settings.userPalmiShorcuts());

	//Palmi mini/maxi
	bool isPalmiMini = settings.value(SETTINGS_PALMI_MINI, DEFAULT_PALMI_MINI).toBool();
	m_palmiSettingsW->setPalmiMinimized(isPalmiMini);
}

void QQSettingsManager::savePalmiSettings()
{
	QQSettings settings;

	//Palmi shortcuts
	settings.setUserPalmiShorcuts(m_palmiSettingsW->getUserShortcuts());

	//Palmi mini/maxi
	bool isPalmiMini = m_palmiSettingsW->isPalmiMinimized();
	bool oldStatus = settings.value(SETTINGS_PALMI_MINI, DEFAULT_PALMI_MINI).toBool();

	settings.setValueWithDefault(SETTINGS_PALMI_MINI, isPalmiMini, DEFAULT_PALMI_MINI);

	if(isPalmiMini != oldStatus)
	{
		if(isPalmiMini == true)
			emit minimizePalmi();
		else
			emit maximizePalmi();
	}
}

void QQSettingsManager::initTotozSettings()
{
	QQSettings settings;

	QString totozServerURL = settings.value(SETTINGS_TOTOZ_SERVER_URL, DEFAULT_TOTOZ_SERVER_URL).toString();
	m_totozSettingsW->setTotozServerURL(totozServerURL);

	QString totozServerBaseImg = settings.value(SETTINGS_TOTOZ_SERVER_BASE_IMG, DEFAULT_TOTOZ_SERVER_BASE_IMG).toString();
	m_totozSettingsW->setTotozBaseImgUrl(totozServerBaseImg);

	QString totozServerNameSuffix = settings.value(SETTINGS_TOTOZ_SERVER_NAME_SUFFIX, DEFAULT_TOTOZ_SERVER_NAME_SUFFIX).toString();
	m_totozSettingsW->setTotozNameSuffix(totozServerNameSuffix);

	bool totozServerAllowSearch = settings.value(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH,  DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH).toBool();
	m_totozSettingsW->setTotozAllowSearch(totozServerAllowSearch);

	QString totozServerQueryPattern = settings.value(SETTINGS_TOTOZ_SERVER_QUERY_PATTERN, DEFAULT_TOTOZ_SERVER_QUERY_PATTERN).toString();
	m_totozSettingsW->setTotozQueryPattern(totozServerQueryPattern);

	QString totozServerVisualMode = settings.value(SETTINGS_TOTOZ_VISUAL_MODE, DEFAULT_TOTOZ_VISUAL_MODE).toString();
	m_totozSettingsW->setTotozVisualModes(DEFAULT_TOTOZ_VISUAL_MODES);
	m_totozSettingsW->setTotozVisualMode(totozServerVisualMode);
}

void QQSettingsManager::saveTotozSettings()
{
	QQSettings settings;

	QString totozServerURL = m_totozSettingsW->totozServerURL();
	settings.setValueWithDefault(SETTINGS_TOTOZ_SERVER_URL, totozServerURL, DEFAULT_TOTOZ_SERVER_URL);

	QString totozServerBaseImg = m_totozSettingsW->totozBaseImgUrl();
	settings.setValueWithDefault(SETTINGS_TOTOZ_SERVER_BASE_IMG, totozServerBaseImg, DEFAULT_TOTOZ_SERVER_BASE_IMG);

	QString totozServerNameSuffix = m_totozSettingsW->totozNameSuffix();
	settings.setValueWithDefault(SETTINGS_TOTOZ_SERVER_NAME_SUFFIX, totozServerNameSuffix, DEFAULT_TOTOZ_SERVER_NAME_SUFFIX);

	bool totozServerAllowSearch = m_totozSettingsW->totozAllowSearch();
	if(settings.setValueWithDefault(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH, QVariant(totozServerAllowSearch), DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH))
		emit totozSearchEnabledChanged(totozServerAllowSearch);

	QString totozServerQueryPattern = m_totozSettingsW->totozQueryPattern();
	settings.setValueWithDefault(SETTINGS_TOTOZ_SERVER_QUERY_PATTERN, totozServerQueryPattern, DEFAULT_TOTOZ_SERVER_QUERY_PATTERN);

	QString totozServerVisualMode = m_totozSettingsW->totozVisualMode();
	settings.setValueWithDefault(SETTINGS_TOTOZ_VISUAL_MODE, totozServerVisualMode, DEFAULT_TOTOZ_VISUAL_MODE);
}

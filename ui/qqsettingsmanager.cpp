#include "qqsettingsmanager.h"
#include "ui_qqsettingsmanager.h"

#include "core/qqsettings.h"
#include "core/qqtypes.h"
#include "ui/settingsmanager/qqboardssettings.h"
#include "ui/settingsmanager/qqfiltersettings.h"
#include "ui/settingsmanager/qqgeneralsettings.h"
#include "ui/settingsmanager/qqhuntsettings.h"
#include "ui/settingsmanager/qqnetworksettings.h"
#include "ui/settingsmanager/qqpalmisettings.h"
#include "ui/settingsmanager/qqtotozsettings.h"

#include <QtDebug>
#include <QApplication>

#define ITEM_GENERAL_TYPE (QListWidgetItem::UserType)
#define ITEM_TOTOZ_TYPE (QListWidgetItem::UserType + 1)
#define ITEM_BOARDS_TYPE (QListWidgetItem::UserType + 2)
#define ITEM_PALMI_TYPE (QListWidgetItem::UserType + 3)
#define ITEM_FILTER_TYPE (QListWidgetItem::UserType + 4)
#define ITEM_HUNT_TYPE (QListWidgetItem::UserType + 5)
#define ITEM_NETWORK_TYPE (QListWidgetItem::UserType + 6)


QQSettingsManager::QQSettingsManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QQSettingsManager)
{
	ui->setupUi(this);
	needPiniFullRepaint = false;
	QListWidget *listSettingsTheme = ui->listSettingsTheme;

	QQSettings settings;

	QLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	listSettingsTheme->setSortingEnabled(false);
	listSettingsTheme->setIconSize(QSize(32, 32));

	listSettingsTheme->addItem(
	            new QListWidgetItem(QIcon(":/img/general-icon.png"), tr("General"),
	                                listSettingsTheme, ITEM_GENERAL_TYPE)
	            );
	m_generalSettingsW = new QQGeneralSettings(this);
	initGeneralSettings(settings);
	m_generalSettingsW->hide();
	layout->addWidget(m_generalSettingsW);

	listSettingsTheme->addItem(
	            new QListWidgetItem(QIcon(":/img/totoz-icon.jpeg"), tr("Totoz"),
	                                listSettingsTheme, ITEM_TOTOZ_TYPE)
	            );
	m_totozSettingsW = new QQTotozSettings(this);
	initTotozSettings(settings);
	m_totozSettingsW->hide();
	layout->addWidget(m_totozSettingsW);

	listSettingsTheme->addItem(
	            new QListWidgetItem(QIcon(":/img/board-icon.png"), tr("Boards"),
	                                listSettingsTheme, ITEM_BOARDS_TYPE)
	            );
	m_boardsSettingsW = new QQBoardsSettings(this);
	initBoardsSettings(settings);
	m_boardsSettingsW->hide();
	layout->addWidget(m_boardsSettingsW);

	listSettingsTheme->addItem(
	            new QListWidgetItem(QIcon(":/img/palmipede-icon.png"), tr("Palmipede"),
	                                listSettingsTheme, ITEM_PALMI_TYPE)
	            );
	m_palmiSettingsW = new QQPalmiSettings(this);
	initPalmiSettings(settings);
	m_palmiSettingsW->hide();
	layout->addWidget(m_palmiSettingsW);

	listSettingsTheme->addItem(
	            new QListWidgetItem(QIcon(":/img/filter-icon.png"), tr("Filters"),
	                                listSettingsTheme, ITEM_FILTER_TYPE)
	            );
	m_filterSettingsW = new QQFilterSettings(this);
	initFilterSettings(settings);
	m_filterSettingsW->hide();
	layout->addWidget(m_filterSettingsW);

	listSettingsTheme->addItem(
	            new QListWidgetItem(QIcon(":/img/hunt-icon.png"), tr("Hunt"),
	                                listSettingsTheme, ITEM_HUNT_TYPE)
	            );
	m_huntSettingsW = new QQHuntSettings(this);
	initHuntSettings(settings);
	m_huntSettingsW->hide();
	layout->addWidget(m_huntSettingsW);

	listSettingsTheme->addItem(
	            new QListWidgetItem(QIcon(":/img/network-icon.png"), tr("Network"),
	                                listSettingsTheme, ITEM_NETWORK_TYPE)
	            );
	m_networkSettingsW = new QQNetworkSettings(this);
	initNetworkSettings(settings);
	m_networkSettingsW->hide();
	layout->addWidget(m_networkSettingsW);

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
	QQSettings settings;

	saveBoardsSettings(settings);
	saveFilterSettings(settings);
	saveGeneralSettings(settings);
	saveHuntSettings(settings);
	saveTotozSettings(settings);
	savePalmiSettings(settings);
	saveNetworkSettings(settings);

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
	m_huntSettingsW->hide();
	m_networkSettingsW->hide();
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
		m_huntSettingsW->show();
		break;
	case ITEM_NETWORK_TYPE:
		m_networkSettingsW->show();
		break;
	default:
		qWarning() << "Unknown type : " << item->type() << ", ignoring";
	}
}

void QQSettingsManager::initBoardsSettings(const QQSettings &settings)
{
	QMap<QString, QQBouchot::QQBouchotSettings> mapBouchotSettings;

	//Les bouchots existant
	QList<QQBouchot *> listBouchots = QQBouchot::listBouchots();
	QQBouchot *bouchot = nullptr;
	for(int i = 0; i < listBouchots.size(); i++)
	{
		bouchot = listBouchots.at(i);
		mapBouchotSettings.insert(bouchot->name(), bouchot->settings());
	}
	m_boardsSettingsW->setBouchots(mapBouchotSettings);

//	QQSettings settings;

#ifdef Q_OS_UNIX
	bool isBigornotifyEnabled = settings.value(SETTINGS_BIGORNOTIFY_ENABLED,  DEFAULT_BIGORNOTIFY_ENABLED).toBool();
	m_boardsSettingsW->setBigornotifyEnabled(isBigornotifyEnabled);
#endif
	bool isWebUrlPreviewEnabled = settings.value(SETTINGS_WEB_IMAGE_VIEWER_ENABLED,  DEFAULT_WEB_IMAGE_VIEWER_ENABLED).toBool();
	m_boardsSettingsW->setWebUrlPreviewEnabled(isWebUrlPreviewEnabled);
	int webUrlPreviewSize = settings.value(SETTINGS_WEB_IMAGE_PREVIEW_SIZE,  DEFAULT_WEB_IMAGE_PREVIEW_SIZE).toInt();
	m_boardsSettingsW->setWebUrlPreviewSize(webUrlPreviewSize);
}

void QQSettingsManager::saveBoardsSettings(QQSettings &settings)
{
	//QQSettings settings;

	QQBouchot *bouchot = nullptr;

	// Les bouchots supprimes
	QStringList lstOldBouchots = m_boardsSettingsW->getOldBouchots();
	for(int i = 0; i < lstOldBouchots.size(); i++)
	{
		settings.removeBouchot(lstOldBouchots.at(i));
		bouchot = QQBouchot::bouchot(lstOldBouchots.at(i));
		if(bouchot != nullptr)
			delete bouchot;
	}

	// Les bouchots modifies
	QMap<QString, QQBouchot::QQBouchotSettings> mBouchots = m_boardsSettingsW->getModifBouchots();
	QList<QString> bouchotNames = mBouchots.keys();
	for(int i = 0; i < bouchotNames.size(); i++)
	{
		QString bouchotName = bouchotNames.at(i);
		bouchot = QQBouchot::bouchot(bouchotName);
		if(bouchot != nullptr)
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

#ifdef Q_OS_UNIX
	bool isBigornotifyEnabled = m_boardsSettingsW->isBigornotifyEnabled();
	settings.setValueWithDefault(SETTINGS_BIGORNOTIFY_ENABLED, isBigornotifyEnabled, DEFAULT_BIGORNOTIFY_ENABLED);
#endif
	bool isWebUrlPreviewEnabled = m_boardsSettingsW->isWebUrlPreviewEnabled();
	settings.setValueWithDefault(SETTINGS_WEB_IMAGE_VIEWER_ENABLED, isWebUrlPreviewEnabled, DEFAULT_WEB_IMAGE_VIEWER_ENABLED);
	int webUrlPreviewSize = m_boardsSettingsW->webUrlPreviewSize();
	settings.setValueWithDefault(SETTINGS_WEB_IMAGE_PREVIEW_SIZE, webUrlPreviewSize, DEFAULT_WEB_IMAGE_PREVIEW_SIZE);
}

void QQSettingsManager::initFilterSettings(const QQSettings &settings)
{
	//QQSettings settings;

	bool enableUrlTransformer = settings.value(SETTINGS_FILTER_SMART_URL_TRANSFORMER,  DEFAULT_FILTER_SMART_URL_TRANSFORMER).toBool();
	m_filterSettingsW->setSmartUrlEnabled(enableUrlTransformer);

	QuteQoin::QQSmartUrlFilerTransformType smartUrlTransformerType = static_cast<QuteQoin::QQSmartUrlFilerTransformType>(settings.value(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE,  DEFAULT_FILTER_SMART_URL_TRANSFORM_TYPE).toInt());
	m_filterSettingsW->setSmartUrlTransformerType(smartUrlTransformerType);
}

void QQSettingsManager::saveFilterSettings(QQSettings &settings)
{
	//QQSettings settings;

	bool enableUrlTransformer = m_filterSettingsW->isSmartUrlEnabled();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_FILTER_SMART_URL_TRANSFORMER, enableUrlTransformer, DEFAULT_FILTER_SMART_URL_TRANSFORMER);

	QuteQoin::QQSmartUrlFilerTransformType trType = m_filterSettingsW->smartUrlTransformerType();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE, trType, DEFAULT_FILTER_SMART_URL_TRANSFORM_TYPE);
}

void QQSettingsManager::initGeneralSettings(const QQSettings &settings)
{
	//QQSettings settings;

	QString historySize = settings.value(SETTINGS_GENERAL_MAX_HISTLEN, DEFAULT_GENERAL_MAX_HISTLEN).toString();
	m_generalSettingsW->setMaxHistorySize(historySize);

	QString defaultUA = settings.value(SETTINGS_GENERAL_DEFAULT_UA, DEFAULT_GENERAL_DEFAULT_UA).toString();
	m_generalSettingsW->setDefaultUA(defaultUA);

	QString defaultWebSearchUrl = settings.value(SETTINGS_GENERAL_WEBSEARCH_URL, DEFAULT_GENERAL_WEBSEARCH_URL).toString();
	m_generalSettingsW->setDefaultWebSearchUrl(defaultWebSearchUrl);

	bool stealthModeEnabled = settings.value(SETTINGS_GENERAL_STEALTH_MODE, DEFAULT_GENERAL_STEALTH_MODE).toBool();
	m_generalSettingsW->setEnableStealthMode(stealthModeEnabled);

	QFont defaultFont;
	defaultFont.fromString(settings.value(SETTINGS_GENERAL_DEFAULT_FONT, DEFAULT_GENERAL_DEFAULT_FONT).toString());
	m_generalSettingsW->setDefaultFont(defaultFont);

	QString hColor = settings.value(SETTINGS_GENERAL_HIGHLIGHT_COLOR, DEFAULT_GENERAL_HIGHLIGHT_COLOR).toString();
	m_generalSettingsW->setHightLightColor(hColor);

	QString piniMode = settings.value(SETTINGS_GENERAL_PINI_MODE, DEFAULT_GENERAL_PINI_MODE).toString();
	m_generalSettingsW->setPiniMode(piniMode);

	//ASCII Login
	m_generalSettingsW->setAsciiLogin(settings.value(SETTINGS_GENERAL_PINI_ASCII_LOGIN, DEFAULT_GENERAL_PINI_ASCII_LOGIN).toBool());
}

void QQSettingsManager::saveGeneralSettings(QQSettings &settings)
{
	//QQSettings settings;

	QString historySize = m_generalSettingsW->maxHistorySize();
	settings.setValueWithDefault(SETTINGS_GENERAL_MAX_HISTLEN, historySize, DEFAULT_GENERAL_MAX_HISTLEN);

	QString defaultUA = m_generalSettingsW->defaultUA();
	settings.setValueWithDefault(SETTINGS_GENERAL_DEFAULT_UA, defaultUA, DEFAULT_GENERAL_DEFAULT_UA);

	QString defaultWebSearchUrl = m_generalSettingsW->defaultWebSearchUrl();
	settings.setValueWithDefault(SETTINGS_GENERAL_WEBSEARCH_URL, defaultWebSearchUrl, DEFAULT_GENERAL_WEBSEARCH_URL);

	bool stealthModeEnabled = m_generalSettingsW->isStealthModeEnabled();
	settings.setValueWithDefault(SETTINGS_GENERAL_STEALTH_MODE, stealthModeEnabled, DEFAULT_GENERAL_STEALTH_MODE);

	QString defaultFont = m_generalSettingsW->defaultFont().toString();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_GENERAL_DEFAULT_FONT, defaultFont, DEFAULT_GENERAL_DEFAULT_FONT);

	QString hColor = m_generalSettingsW->highlightColor();
	settings.setValueWithDefault(SETTINGS_GENERAL_HIGHLIGHT_COLOR, hColor, DEFAULT_GENERAL_HIGHLIGHT_COLOR);

	QString piniMode = m_generalSettingsW->piniMode();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_GENERAL_PINI_MODE, piniMode, DEFAULT_GENERAL_PINI_MODE);

	//ASCII login
	bool asciiLogin = m_generalSettingsW->isAciiLogin();
	needPiniFullRepaint |= settings.setValueWithDefault(SETTINGS_GENERAL_PINI_ASCII_LOGIN, asciiLogin, DEFAULT_GENERAL_PINI_ASCII_LOGIN);
}

void QQSettingsManager::initHuntSettings(const QQSettings &settings)
{
	//QQSettings settings;

	QuteQoin::QQHuntMode huntMode = static_cast<QuteQoin::QQHuntMode>(settings.value(SETTINGS_HUNT_MODE, DEFAULT_HUNT_MODE).toInt());
	m_huntSettingsW->setHuntMode(huntMode);

	QuteQoin::QQSLHuntMode slHuntMode = static_cast<QuteQoin::QQSLHuntMode>(settings.value(SETTINGS_SL_HUNT_MODE, DEFAULT_SL_HUNT_MODE).toInt());
	m_huntSettingsW->setSlHuntMode(slHuntMode);

	bool silentHuntEnable = settings.value(SETTINGS_HUNT_SILENT_ENABLED, DEFAULT_HUNT_SILENT_ENABLED).toBool();
	m_huntSettingsW->setSilentHunt(silentHuntEnable);

	int maxHuntableItems = settings.value(SETTINGS_HUNT_MAX_ITEMS, DEFAULT_HUNT_MAX_ITEMS).toInt();
	m_huntSettingsW->setMaxHuntableItems(maxHuntableItems);
}

void QQSettingsManager::saveHuntSettings(QQSettings &settings)
{
	//QQSettings settings;

	QuteQoin::QQHuntMode huntMode = m_huntSettingsW->huntMode();
	settings.setValueWithDefault(SETTINGS_HUNT_MODE, huntMode, DEFAULT_HUNT_MODE);

	QuteQoin::QQSLHuntMode slHuntMode = m_huntSettingsW->slHuntMode();
	settings.setValueWithDefault(SETTINGS_SL_HUNT_MODE, slHuntMode, DEFAULT_SL_HUNT_MODE);

	bool silentHuntEnable = m_huntSettingsW->silentHuntEnabled();
	settings.setValueWithDefault(SETTINGS_HUNT_SILENT_ENABLED, silentHuntEnable, DEFAULT_HUNT_SILENT_ENABLED);

	unsigned int maxHuntableItems = m_huntSettingsW->maxHuntableItems();
	settings.setValueWithDefault(SETTINGS_HUNT_MAX_ITEMS, maxHuntableItems, DEFAULT_HUNT_MAX_ITEMS);
}

void QQSettingsManager::initNetworkSettings(const QQSettings &settings)
{
	QString networkMode = settings.value(SETTINGS_NETWORK_MODE, DEFAULT_NETWORK_MODE).toString();
	if(! SETTINGS_NETWORK_MODES.contains(networkMode))
		networkMode = DEFAULT_NETWORK_MODE;

	if(networkMode == SETTINGS_NETWORK_MODE_DIRECT)
		m_networkSettingsW->setNetworkSettingsMode(QQNetworkSettings::DIRECT);
	else if(networkMode == SETTINGS_NETWORK_MODE_SYSTEM)
		m_networkSettingsW->setNetworkSettingsMode(QQNetworkSettings::SYSTEM);
	else // if(networkMode == SETTINGS_NETWORK_MODE_MANUAL)
	{
		m_networkSettingsW->setNetworkSettingsMode(QQNetworkSettings::MANUAL);
		QString proxyHost = settings.value(SETTINGS_NETWORK_PROXY_HOST, DEFAULT_NETWORK_PROXY_HOST).toString();
		QString proxyPort = settings.value(SETTINGS_NETWORK_PROXY_PORT, DEFAULT_NETWORK_PROXY_PORT).toString();
		m_networkSettingsW->setProxySettings(proxyHost, proxyPort);
		bool isHttpProxy = settings.value(SETTINGS_NETWORK_PROXY_IS_HTTP, DEFAULT_NETWORK_PROXY_IS_HTTP).toBool();
		m_networkSettingsW->setProxyIsHTTP(isHttpProxy);
	}
}

void QQSettingsManager::saveNetworkSettings(QQSettings &settings)
{
	bool hasChanged = false;
	QQNetworkSettings::Mode mode = m_networkSettingsW->networkSettingsMode();
	QString modeStr;
	switch(mode)
	{
	case QQNetworkSettings::DIRECT:
		modeStr = SETTINGS_NETWORK_MODE_DIRECT;
		break;
	case QQNetworkSettings::SYSTEM:
		modeStr = SETTINGS_NETWORK_MODE_SYSTEM;
		break;
	default: //QQNetworkSettings::MANUAL
		modeStr = SETTINGS_NETWORK_MODE_MANUAL;
		break;
	}
	hasChanged |= settings.setValueWithDefault(SETTINGS_NETWORK_MODE, modeStr, DEFAULT_NETWORK_MODE);

	if(mode == QQNetworkSettings::MANUAL)
	{
		QString val = m_networkSettingsW->proxySettingsHost();
		hasChanged |= settings.setValueWithDefault(SETTINGS_NETWORK_PROXY_HOST, val, DEFAULT_NETWORK_PROXY_HOST);
		val = m_networkSettingsW->proxySettingsPort();
		hasChanged |= settings.setValueWithDefault(SETTINGS_NETWORK_PROXY_PORT, val, DEFAULT_NETWORK_PROXY_PORT);
		hasChanged |= settings.setValueWithDefault(SETTINGS_NETWORK_PROXY_IS_HTTP,
		                             m_networkSettingsW->proxyIsHTTP(), DEFAULT_NETWORK_PROXY_IS_HTTP);
	}
	else
	{
		settings.remove(SETTINGS_NETWORK_PROXY_HOST);
		settings.remove(SETTINGS_NETWORK_PROXY_PORT);
		settings.remove(SETTINGS_NETWORK_PROXY_IS_HTTP);
	}

	if(hasChanged)
		emit networkProxySettingsChanged();
}

void QQSettingsManager::initPalmiSettings(const QQSettings &settings)
{
	//QQSettings settings;

	//Palmi shortcuts
	m_palmiSettingsW->setStaticShortcuts(settings.staticPalmiShorcuts());
	m_palmiSettingsW->setUserShortcuts(settings.userPalmiShorcuts());

	//Palmi mini/maxi
	bool isPalmiMini = settings.value(SETTINGS_PALMI_MINI, DEFAULT_PALMI_MINI).toBool();
	m_palmiSettingsW->setPalmiMinimized(isPalmiMini);

	//Palmi fixed/docked
	bool isPalmiDocked = settings.value(SETTINGS_PALMI_DOCKED, DEFAULT_PALMI_DOCKED).toBool();
	m_palmiSettingsW->setPalmiDocked(isPalmiDocked);

	//Palmi short norlogeds
	bool isShortNorlogesEnabled = settings.value(SETTINGS_PALMI_SHORT_NORLOGE_ENABLED, DEFAULT_PALMI_SHORT_NORLOGE_ENABLED).toBool();
	m_palmiSettingsW->setShortNorlogesEnabled(isShortNorlogesEnabled);

	QString fsService = settings.value(SETTINGS_FILE_SHARING_SERVICE, DEFAULT_FILE_SHARING_SERVICE).toString();
	m_palmiSettingsW->setFileSharingServices(DEFAULT_FILE_SHARING_SERVICES);
	m_palmiSettingsW->setFileSharingService(fsService);
}

void QQSettingsManager::savePalmiSettings(QQSettings &settings)
{
	//QQSettings settings;

	//Palmi shortcuts
	settings.setUserPalmiShorcuts(m_palmiSettingsW->getUserShortcuts());

	//Palmi mini/maxi
	bool isPalmiMini = m_palmiSettingsW->isPalmiMinimized();
	bool oldState = settings.value(SETTINGS_PALMI_MINI, DEFAULT_PALMI_MINI).toBool();

	settings.setValueWithDefault(SETTINGS_PALMI_MINI, isPalmiMini, DEFAULT_PALMI_MINI);

	bool palmiMinimizedStatusChanged = (oldState != isPalmiMini);

	//Palmi fixed/docked
	bool isPalmiDocked = m_palmiSettingsW->isPalmiDocked();
	oldState = settings.value(SETTINGS_PALMI_DOCKED, DEFAULT_PALMI_DOCKED).toBool();

	settings.setValueWithDefault(SETTINGS_PALMI_DOCKED, isPalmiDocked, DEFAULT_PALMI_DOCKED);

	bool palmiDockedStatusChanged = (oldState != isPalmiDocked);

	if(palmiMinimizedStatusChanged || palmiDockedStatusChanged)
		emit palmiStatusChanged(isPalmiMini, isPalmiDocked);

	//Palmi short norlogeds
	bool isShortNorlogesEnabled = m_palmiSettingsW->isShortNorlogesEnabled();
	settings.setValueWithDefault(SETTINGS_PALMI_SHORT_NORLOGE_ENABLED, QVariant(isShortNorlogesEnabled), DEFAULT_PALMI_SHORT_NORLOGE_ENABLED);

	QString fsService = m_palmiSettingsW->getFileSharingService();
	settings.setValueWithDefault(SETTINGS_FILE_SHARING_SERVICE, fsService, DEFAULT_FILE_SHARING_SERVICE);
}

void QQSettingsManager::initTotozSettings(const QQSettings &settings)
{
	//QQSettings settings;

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

void QQSettingsManager::saveTotozSettings(QQSettings &settings)
{
	//QQSettings settings;

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

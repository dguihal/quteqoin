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

QQSettingsManager::QQSettingsManager(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQSettingsManager)
{
	ui->setupUi(this);
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
	saveTotozSettings();
	savePalmiSettings();

	QDialog::accept();
}

void QQSettingsManager::configItemChanged()
{
	QListWidgetItem *item = ui->listSettingsTheme->currentItem();

	ui->settingsThemeLabel->setText(item->text());

	m_generalSettingsW->hide();
	m_filterSettingsW->hide();
	m_totozSettingsW->hide();
	m_boardsSettingsW->hide();
	m_palmiSettingsW->hide();
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
	settings.setValueWithDefault(SETTINGS_FILTER_SMART_URL_TRANSFORMER, enableUrlTransformer, DEFAULT_FILTER_SMART_URL_TRANSFORMER);

	QuteQoin::QQSmartUrlFilerTransformType trType = m_filterSettingsW->smartUrlTransformerType();
	settings.setValueWithDefault(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE, trType, DEFAULT_FILTER_SMART_URL_TRANSFORM_TYPE);

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
}

void QQSettingsManager::saveGeneralSettings()
{
	QQSettings settings;

	QString historySize = m_generalSettingsW->maxHistorySize();
	if(historySize.size() == 0 || historySize == QString::number(DEFAULT_GENERAL_MAX_HISTLEN))
		settings.remove(SETTINGS_GENERAL_MAX_HISTLEN);
	else
		settings.setValue(SETTINGS_GENERAL_MAX_HISTLEN, historySize.toInt());

	QString defaultLogin = m_generalSettingsW->defaultLogin();
	if(defaultLogin.size() == 0 || defaultLogin == DEFAULT_GENERAL_DEFAULT_LOGIN)
		settings.remove(SETTINGS_GENERAL_DEFAULT_LOGIN);
	else
		settings.setValue(SETTINGS_GENERAL_DEFAULT_LOGIN, defaultLogin);

	QString defaultUA = m_generalSettingsW->defaultUA();
	if(defaultUA.size() == 0 || defaultUA == DEFAULT_GENERAL_DEFAULT_UA)
		settings.remove(SETTINGS_GENERAL_DEFAULT_UA);
	else
		settings.setValue(SETTINGS_GENERAL_DEFAULT_UA, defaultUA);

	QString defaultWebSearchUrl = m_generalSettingsW->defaultWebSearchUrl();
	if(defaultWebSearchUrl.size() == 0 || defaultWebSearchUrl == DEFAULT_GENERAL_WEBSEARCH_URL)
		settings.remove(SETTINGS_GENERAL_WEBSEARCH_URL);
	else
		settings.setValue(SETTINGS_GENERAL_WEBSEARCH_URL, defaultWebSearchUrl);

	QString defaultFont = m_generalSettingsW->defaultFont().toString();
	if(defaultFont == DEFAULT_GENERAL_DEFAULT_FONT)
		settings.remove(SETTINGS_GENERAL_DEFAULT_FONT);
	else
		settings.setValue(SETTINGS_GENERAL_DEFAULT_FONT, defaultFont);
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

	//Palmi hidden
	bool isPalmiHidden = settings.value(SETTINGS_PALMI_HIDDEN, DEFAULT_PALMI_HIDDEN).toBool();
	m_palmiSettingsW->setPalmiHidden(isPalmiHidden);
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

	//Palmi hidden
	bool isPalmiHidden = m_palmiSettingsW->isPalmiHidden();
	bool oldVisibility = settings.value(SETTINGS_PALMI_HIDDEN, DEFAULT_PALMI_HIDDEN).toBool();

	settings.setValueWithDefault(SETTINGS_PALMI_HIDDEN, isPalmiMini, DEFAULT_PALMI_HIDDEN);

	if(isPalmiHidden != oldVisibility)
		emit hidePalmi(isPalmiHidden);
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
	if(totozServerURL.size() == 0 || totozServerURL == DEFAULT_TOTOZ_SERVER_URL)
		settings.remove(SETTINGS_TOTOZ_SERVER_URL);
	else
		settings.setValue(SETTINGS_TOTOZ_SERVER_URL, totozServerURL);

	QString totozServerBaseImg = m_totozSettingsW->totozBaseImgUrl();
	if(totozServerBaseImg.size() == 0 || totozServerBaseImg == DEFAULT_TOTOZ_SERVER_BASE_IMG)
		settings.remove(SETTINGS_TOTOZ_SERVER_BASE_IMG);
	else
		settings.setValue(SETTINGS_TOTOZ_SERVER_BASE_IMG, totozServerBaseImg);

	QString totozServerNameSuffix = m_totozSettingsW->totozNameSuffix();
	if(totozServerNameSuffix.size() == 0 || totozServerNameSuffix == DEFAULT_TOTOZ_SERVER_NAME_SUFFIX)
		settings.remove(SETTINGS_TOTOZ_SERVER_NAME_SUFFIX);
	else
		settings.setValue(SETTINGS_TOTOZ_SERVER_NAME_SUFFIX, totozServerNameSuffix);

	bool totozServerAllowSearch = m_totozSettingsW->totozAllowSearch();
	emit totozSearchEnabledChanged(totozServerAllowSearch);
	if(totozServerAllowSearch == DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH)
		settings.remove(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH);
	else
		settings.setValue(SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH, QVariant(totozServerAllowSearch));

	QString totozServerQueryPattern = m_totozSettingsW->totozQueryPattern();
	if(totozServerQueryPattern.size() == 0 || totozServerQueryPattern == DEFAULT_TOTOZ_SERVER_QUERY_PATTERN)
		settings.remove(SETTINGS_TOTOZ_SERVER_QUERY_PATTERN);
	else
		settings.setValue(SETTINGS_TOTOZ_SERVER_QUERY_PATTERN, totozServerQueryPattern);

	QString totozServerVisualMode = m_totozSettingsW->totozVisualMode();
	if(totozServerVisualMode.size() == 0 || totozServerVisualMode == DEFAULT_TOTOZ_VISUAL_MODE)
		settings.remove(SETTINGS_TOTOZ_VISUAL_MODE);
	else
		settings.setValue(SETTINGS_TOTOZ_VISUAL_MODE, totozServerVisualMode);
}

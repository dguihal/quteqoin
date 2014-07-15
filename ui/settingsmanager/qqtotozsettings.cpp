#include "qqtotozsettings.h"
#include "ui_qqtotozsettings.h"

#include "core/qqsettings.h"

#include <QtDebug>

QQTotozSettings::QQTotozSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQTotozSettings)
{
	ui->setupUi(this);

	QQSettings settings;
	ui->presetCB->clear();
	QStringList srvPresets = settings.listTotozSrvPresets();

	ui->presetCB->insertItem(0, "");
#if(QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	srvPresets.sort(Qt::CaseInsensitive);
#else
	srvPresets.sort();
#endif
	foreach (QString preset, srvPresets)
	{
		QString label = settings.getTotozSrvPreset(preset, true).label;
		ui->presetCB->insertItem(INT_MAX, label, preset);
	}

	connect(ui->srvTotozLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(totozServerURLChanged(QString)));
	connect(ui->srvTotozBaseImgLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(totozBaseImgUrlChanged(QString)));
	connect(ui->srvNameSuffixlineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(totozNameSuffixChanged(QString)));
	connect(ui->srvAllowSearch, SIGNAL(stateChanged(int)),
			this, SLOT(totozAllowSearchChanged(int)));
	connect(ui->srvTotozQueryPatternLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(totozQueryPatternChanged(QString)));
	connect(ui->presetCB, SIGNAL(activated(int)),
			this, SLOT(loadTotozSrvPreset(int)));
	connect(ui->totozModeComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(totozVisualModeChanged(QString)));
}

QQTotozSettings::~QQTotozSettings()
{
	delete ui;
}

//PROPERTY TotozServerURL
void QQTotozSettings::setTotozServerURL(const QString &totozServerURL)
{
	ui->srvTotozLineEdit->setText(totozServerURL);
}

QString QQTotozSettings::totozServerURL()
{
	return m_totozServerURL;
}

void QQTotozSettings::totozServerURLChanged(const QString &)
{
	m_totozServerURL = ui->srvTotozLineEdit->text();
}


//PROPERTY TotozBaseImgUrl
void QQTotozSettings::setTotozBaseImgUrl(const QString &totozBaseImgUrl)
{
	ui->srvTotozBaseImgLineEdit->setText(totozBaseImgUrl);
}

QString QQTotozSettings::totozBaseImgUrl()
{
	return m_totozBaseImgUrl;
}

void QQTotozSettings::totozBaseImgUrlChanged(const QString &)
{
	m_totozBaseImgUrl = ui->srvTotozBaseImgLineEdit->text();
}


//PROPERTY TotozNameSuffix
void QQTotozSettings::setTotozNameSuffix(const QString &totozNameSuffix)
{
	ui->srvNameSuffixlineEdit->setText(totozNameSuffix);
}

QString QQTotozSettings::totozNameSuffix()
{
	return m_totozNameSuffix;
}

void QQTotozSettings::totozNameSuffixChanged(const QString &)
{
	m_totozNameSuffix = ui->srvNameSuffixlineEdit->text();
}


//PROPERTY TotozBaseAllowSearch
void QQTotozSettings::setTotozAllowSearch(const bool &totozAllowSearch)
{
	ui->srvAllowSearch->setChecked(totozAllowSearch);
	totozAllowSearchChanged(ui->srvAllowSearch->checkState());
}

bool QQTotozSettings::totozAllowSearch()
{
	return m_totozAllowSearch;
}

void QQTotozSettings::totozAllowSearchChanged(int)
{
	Qt::CheckState state = ui->srvAllowSearch->checkState();
	if(state == Qt::Checked)
	{
		m_totozAllowSearch = true;
		ui->srvTotozQueryPatternLineEdit->setReadOnly(false);
		ui->srvTotozQueryPatternLineEdit->setText(m_totozQueryPattern);
	}
	else
	{
		m_totozAllowSearch = false;
		ui->srvTotozQueryPatternLineEdit->setReadOnly(true);
		ui->srvTotozQueryPatternLineEdit->clear();
	}
}


//PROPERTY TotozQueryPattern
void QQTotozSettings::setTotozQueryPattern(const QString &totozQueryPattern)
{
	if(ui->srvAllowSearch->checkState() == Qt::Checked)
		ui->srvTotozQueryPatternLineEdit->setText(totozQueryPattern);
	else
		m_totozQueryPattern = totozQueryPattern;
}

QString QQTotozSettings::totozQueryPattern()
{
	if(ui->srvAllowSearch->checkState() == Qt::Checked)
		return m_totozQueryPattern;
	else
		return "";
}

void QQTotozSettings::totozQueryPatternChanged(const QString &)
{
	m_totozQueryPattern = ui->srvTotozQueryPatternLineEdit->text();
}


//PROPERTY TotozVisualModes
void QQTotozSettings::setTotozVisualModes(const QStringList &totozVisualModes)
{
	ui->totozModeComboBox->clear();
	for(int i = 0; i < totozVisualModes.size(); i++)
		ui->totozModeComboBox->addItem(totozVisualModes.at(i));
}

void QQTotozSettings::setTotozVisualMode(const QString &totozVisualMode)
{
	int index = ui->totozModeComboBox->findText(totozVisualMode, Qt::MatchCaseSensitive);
	if(index >= 0)
		ui->totozModeComboBox->setCurrentIndex(index);
}

QString QQTotozSettings::totozVisualMode()
{
	return m_totozVisualMode;
}

void QQTotozSettings::totozVisualModeChanged(const QString &)
{
	m_totozVisualMode = ui->totozModeComboBox->currentText();
}


// Totoz Server Preset
void QQTotozSettings::loadTotozSrvPreset(int index)
{
	QQSettings settings;
	QString presetName = ui->presetCB->itemData(index).toString();

	if(presetName.isEmpty())
		return;

	QQTotozSrvPreset preset = settings.getTotozSrvPreset(presetName, false);

	ui->srvTotozLineEdit->setText(preset.url);
	ui->srvTotozBaseImgLineEdit->setText(preset.baseImgDir);
	ui->srvNameSuffixlineEdit->setText(preset.imgSuffix);
	ui->srvAllowSearch->setChecked(! preset.searchPattern.isEmpty());
	ui->srvTotozQueryPatternLineEdit->setText(preset.searchPattern);
}

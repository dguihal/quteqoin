#include "qqtotozsettings.h"
#include "ui_qqtotozsettings.h"

QQTotozSettings::QQTotozSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQTotozSettings)
{
	ui->setupUi(this);

	connect(ui->srvTotozLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(totozServerURLChanged(QString)));
	connect(ui->srvTotozBaseImgLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(totozBaseImgUrlChanged(QString)));
	connect(ui->srvAllowSearch, SIGNAL(stateChanged(int)),
			this, SLOT(totozAllowSearchChanged(int)));
	connect(ui->srvTotozQueryPatternLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(totozQueryPatternChanged(QString)));
	connect(ui->totozModeComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(totozVisualModeChanged(QString)));
}

QQTotozSettings::~QQTotozSettings()
{
	delete ui;
}

//void QQTotozSettings::TotozPresetChanged(QFont &font);

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

//PROPERTY TotozBaseAllowSearch
void QQTotozSettings::setTotozAllowSearch(const bool &totozAllowSearch)
{
	ui->srvAllowSearch->setChecked(totozAllowSearch);
}

bool QQTotozSettings::totozAllowSearch()
{
	return m_totozAllowSearch;
}

void QQTotozSettings::totozAllowSearchChanged(int)
{
	m_totozAllowSearch = (ui->srvAllowSearch->checkState() != Qt::Unchecked);
}


//PROPERTY TotozQueryPattern
void QQTotozSettings::setTotozQueryPattern(const QString &totozQueryPattern)
{
	ui->srvTotozQueryPatternLineEdit->setText(totozQueryPattern);
}

QString QQTotozSettings::totozQueryPattern()
{
	return m_totozQueryPattern;
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


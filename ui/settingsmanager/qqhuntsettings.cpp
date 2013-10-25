#include "qqhuntsettings.h"
#include "ui_qqhuntsettings.h"

QQHuntSettings::QQHuntSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQHuntSettings)
{
	ui->setupUi(this);

	connect(ui->silentModeCB, SIGNAL(toggled(bool)), this, SLOT(setSilentHunt(bool)));
	connect(ui->maxObjSB, SIGNAL(valueChanged(int)), this, SLOT(setMaxHuntableItems(int)));
	connect(ui->HuntAutoRB, SIGNAL(clicked()), this, SLOT(huntModeChanged()));
	connect(ui->huntDisabledRB, SIGNAL(clicked()), this, SLOT(huntModeChanged()));
	connect(ui->huntManualRB, SIGNAL(clicked()), this, SLOT(huntModeChanged()));
}

QQHuntSettings::~QQHuntSettings()
{
	delete ui;
}

void QQHuntSettings::sethuntMode(QuteQoin::QQHuntMode huntMode)
{
	m_huntMode = huntMode;
	switch (m_huntMode) {
	case QuteQoin::Auto:
		ui->HuntAutoRB->setChecked(true);
		break;
	case QuteQoin::Disabled:
		ui->huntDisabledRB->setChecked(true);
		break;
	default: // QuteQoin::Manual:
		ui->huntManualRB->setChecked(true);
		break;
	}
}

void QQHuntSettings::setMaxHuntableItems(int maxHuntableItems)
{
	m_maxHuntableItems = maxHuntableItems;
	if(ui->maxObjSB->value() != maxHuntableItems)
		ui->maxObjSB->setValue(m_maxHuntableItems);
}

void QQHuntSettings::setSilentHunt(bool enabled)
{
	m_isSilentHunt = enabled;
	if(ui->silentModeCB->isChecked() != m_isSilentHunt)
		ui->silentModeCB->toggle();
}

void QQHuntSettings::huntModeChanged()
{
	if(ui->HuntAutoRB->isChecked())
		m_huntMode = QuteQoin::Auto;
	else if(ui->huntDisabledRB->isChecked())
		m_huntMode = QuteQoin::Disabled;
	else
		m_huntMode = QuteQoin::Manual;
}

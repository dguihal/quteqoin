#include "qqfiltersettings.h"
#include "ui_qqfiltersettings.h"

QQFilterSettings::QQFilterSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQFilterSettings)
{
	ui->setupUi(this);

	connect(ui->smartUrlCB, SIGNAL(clicked(bool)), this, SLOT(setSmartUrlEnabled(bool)));
	connect(ui->fullHostUrlRB, SIGNAL(clicked()), this, SLOT(smartUrlMethodChanged()));
	connect(ui->shortHostUrlRB, SIGNAL(clicked()), this, SLOT(smartUrlMethodChanged()));
	connect(ui->shorterHostUrlRB, SIGNAL(clicked()), this, SLOT(smartUrlMethodChanged()));
}

QQFilterSettings::~QQFilterSettings()
{
	delete ui;
}

bool QQFilterSettings::isSmartUrlEnabled()
{
	return ui->smartUrlCB->isChecked();
}

void QQFilterSettings::setSmartUrlTransformerType(QuteQoin::QQSmartUrlFilerTransformType transformType)
{
	m_smartUrlTransformType = transformType;
	switch(transformType)
	{
	case QuteQoin::Full:
		ui->fullHostUrlRB->setChecked(true);
		break;
	case QuteQoin::Short:
		ui->shortHostUrlRB->setChecked(true);
		break;
	case QuteQoin::Shorter:
		ui->shorterHostUrlRB->setChecked(true);
		m_smartUrlTransformType = QuteQoin::Shorter;
		break;
	}
}

void QQFilterSettings::setSmartUrlEnabled(bool checked)
{
	ui->smartUrlCB->setChecked(checked);
	ui->fullHostUrlRB->setEnabled(checked);
	ui->shortHostUrlRB->setEnabled(checked);
	ui->shorterHostUrlRB->setEnabled(checked);
}

void QQFilterSettings::smartUrlMethodChanged()
{
	if(ui->fullHostUrlRB->isChecked())
		m_smartUrlTransformType = QuteQoin::Full;
	else if(ui->shortHostUrlRB->isChecked())
		m_smartUrlTransformType = QuteQoin::Short;
	else
		m_smartUrlTransformType = QuteQoin::Shorter;
}

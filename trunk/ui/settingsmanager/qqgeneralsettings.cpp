#include "qqgeneralsettings.h"
#include "ui_qqgeneralsettings.h"

QQGeneralSettings::QQGeneralSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQGeneralSettings)
{
	ui->setupUi(this);

	QValidator *validator = new QIntValidator(1, INT_MAX, this);
	ui->maxHistLineEdit->setValidator(validator);

	connect(ui->fontFamComboB, SIGNAL(currentFontChanged(const QFont &)),
			this, SLOT(fontChanged(const QFont &)));
	connect(ui->maxHistLineEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(maxHistorySizeChanged(const QString &)));
	connect(ui->defaultUALlineEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(defaultUAChanged(const QString &)));
	connect(ui->defaultLoginLineEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(defaultLoginChanged(const QString &)));

	QFont currentFont;
	ui->fontFamComboB->setCurrentFont(currentFont);
	fontChanged(currentFont);


}

QQGeneralSettings::~QQGeneralSettings()
{
	delete ui;
}

void QQGeneralSettings::fontChanged(const QFont &font)
{
	QFontDatabase fDB;

	QList<int> listSizes = fDB.pointSizes(font.family(), font.styleName());

	QString value = ui->fontSizeComboB->currentText();
	if(value.size() == 0)
		value = QString::number(font.pointSize());

	ui->fontSizeComboB->clear();
	for(int i = 0; i < listSizes.size(); i++)
		ui->fontSizeComboB->addItem(QString::number(listSizes.at(i)));

	int index = ui->fontSizeComboB->findText(value);
	if(index >= 0)
		ui->fontSizeComboB->setCurrentIndex(index);

}

//PROPERTY MaxHistorySize
void QQGeneralSettings::setMaxHistorySize(const QString &maxSize)
{
	ui->maxHistLineEdit->setText(maxSize);
}

QString QQGeneralSettings::maxHistorySize()
{
	return m_maxHistorySize;
}

void QQGeneralSettings::maxHistorySizeChanged(const QString &)
{
	m_maxHistorySize = ui->maxHistLineEdit->text();
}

//PROPERTY DefaultLogin
void QQGeneralSettings::setDefaultLogin(const QString &defaultLogin)
{
	ui->defaultLoginLineEdit->setText(defaultLogin);
}

QString QQGeneralSettings::defaultLogin()
{
	return m_defaultLogin;
}

void QQGeneralSettings::defaultLoginChanged(const QString &)
{
	m_defaultLogin = ui->defaultLoginLineEdit->text();
}

//PROPERTY DefaultUA
void QQGeneralSettings::setDefaultUA(const QString &defaultUA)
{
	ui->defaultUALlineEdit->setText(defaultUA);
}

QString QQGeneralSettings::defaultUA()
{
	return m_defaultUA;
}

void QQGeneralSettings::defaultUAChanged(const QString &)
{
	m_defaultUA = ui->defaultUALlineEdit->text();
}

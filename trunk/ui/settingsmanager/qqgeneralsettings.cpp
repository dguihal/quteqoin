#include "qqgeneralsettings.h"
#include "ui_qqgeneralsettings.h"

#include "core/qqsettingsparams.h"

#include <QtDebug>
#include <QColorDialog>

QQGeneralSettings::QQGeneralSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQGeneralSettings)
{
	ui->setupUi(this);

	QValidator *validator = new QIntValidator(1, INT_MAX, this);
	ui->maxHistLineEdit->setValidator(validator);

	connect(ui->fontFamComboB, SIGNAL(currentFontChanged(const QFont &)),
			this, SLOT(fontChanged(const QFont &)));
	connect(ui->fontSizeComboB, SIGNAL(activated(int)),
			this, SLOT(fontSizeChanged(int)));
	connect(ui->colorChooserPB, SIGNAL(clicked(bool)),
			this, SLOT(showColorDialog()));

	m_defaultFont = QFont();
	ui->fontFamComboB->setCurrentFont(m_defaultFont);
	fontChanged(m_defaultFont);

}

QQGeneralSettings::~QQGeneralSettings()
{
	delete ui;
}

//PROPERTY DefaultFont
void QQGeneralSettings::setDefaultFont(const QFont &font)
{
	m_defaultFont = font;
	ui->fontFamComboB->setCurrentFont(m_defaultFont);
	fontChanged(m_defaultFont);
}

QFont QQGeneralSettings::defaultFont()
{
	return m_defaultFont;
}

void QQGeneralSettings::fontChanged(const QFont &font)
{
	QFontDatabase fDB;

	QList<int> listSizes = fDB.pointSizes(font.family());

	int fontSize = m_defaultFont.pointSize();
	if(! listSizes.contains(fontSize))
		fontSize = font.pointSize();
	QString fontSizeStr = QString::number(fontSize);

	ui->fontSizeComboB->clear();
	for(int i = 0; i < listSizes.size(); i++)
		ui->fontSizeComboB->addItem(QString::number(listSizes.at(i)));

	int index = ui->fontSizeComboB->findText(fontSizeStr);
	if(index >= 0)
		ui->fontSizeComboB->setCurrentIndex(index);

	m_defaultFont = font;
	m_defaultFont.setPointSize(fontSize);
}

void QQGeneralSettings::fontSizeChanged(int index)
{
	Q_UNUSED(index);
	QString fontSizeStr = ui->fontSizeComboB->currentText();

	m_defaultFont.setPointSize(fontSizeStr.toInt());
}

//PROPERTY MaxHistorySize
void QQGeneralSettings::setMaxHistorySize(const QString &maxSize)
{
	ui->maxHistLineEdit->setText(maxSize);
}

QString QQGeneralSettings::maxHistorySize()
{
	return ui->maxHistLineEdit->text();
}

//PROPERTY DefaultLogin
void QQGeneralSettings::setDefaultLogin(const QString &defaultLogin)
{
	ui->defaultLoginLineEdit->setText(defaultLogin);
}

QString QQGeneralSettings::defaultLogin()
{
	return ui->defaultLoginLineEdit->text();
}

//PROPERTY DefaultUA
void QQGeneralSettings::setDefaultUA(const QString &defaultUA)
{
	ui->defaultUALlineEdit->setText(defaultUA);
}

QString QQGeneralSettings::defaultUA()
{
	return ui->defaultUALlineEdit->text();
}

//PROPERTY DefaultwebSearchUrl
void QQGeneralSettings::setDefaultWebSearchUrl(const QString &defaultWebSearchUrl)
{
	ui->defaultWebSearchUrlLineEdit->setText(defaultWebSearchUrl);
}

QString QQGeneralSettings::defaultWebSearchUrl()
{
	return ui->defaultWebSearchUrlLineEdit->text();
}

//PROPERTY STEALTHMODE
void QQGeneralSettings::setEnableStealthMode(bool isEnabled)
{
	ui->stealthModeCB->setChecked(isEnabled);
}

bool QQGeneralSettings::isStealthModeEnabled()
{
	return ui->stealthModeCB->isChecked();
}

//PROPERTY DynamicHighlight
void QQGeneralSettings::setHightLightColor(const QString &hColor)
{
	m_color = hColor;
	QColor color(hColor);
	if(color.isValid())
	{
		ui->fixedHighLightColorRB->setChecked(true);
		ui->colorChooserPB->setStyleSheet("QPushButton{background-color: " + m_color + ";}");
	}
	else
		ui->dynamicHighLightColorRB->setChecked(true);
}

QString QQGeneralSettings::highlightColor()
{
	if(ui->dynamicHighLightColorRB->isChecked())
		return QString();
	else
		return m_color;
}

void QQGeneralSettings::showColorDialog()
{
	QColor color(m_color);
	if(! color.isValid())
		color.setNamedColor(DEFAULT_GENERAL_HIGHLIGHT_COLOR);

	color = QColorDialog::getColor(color, this);
	if(color.isValid())
	{
		m_color = color.name();
		ui->colorChooserPB->setStyleSheet("QPushButton{background-color: " + m_color + ";}");
	}
}

#include "qqboardwizardolccs.h"

#include "core/qqsettings.h"
#include "ui/settingsmanager/qqboardwizard.h"

#include <QtDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMovie>
#include <QStringList>
#include <QXmlStreamReader>

QQOlccsWizardPMainNetAccessor::QQOlccsWizardPMainNetAccessor(QObject *parent) :
		QQNetworkAccessor(parent)
{
}

QStringList QQOlccsWizardPMainNetAccessor::boardList()
{
	return m_boardList;
}

void QQOlccsWizardPMainNetAccessor::updateBoardList()
{
	m_boardList.clear();

	QUrl url("http://olccs.halifirien.info/boards.xml");
	QNetworkRequest request(url);

	QQSettings settings;
	QString ua = settings.value(SETTINGS_GENERAL_DEFAULT_UA, DEFAULT_GENERAL_DEFAULT_UA).toString();
	if(ua.isEmpty())
		ua=QString(DEFAULT_GENERAL_DEFAULT_UA);
	request.setRawHeader(QString::fromLatin1("User-Agent").toLatin1(), QString(DEFAULT_GENERAL_DEFAULT_UA).toLatin1());
	httpGet(request);
}

void QQOlccsWizardPMainNetAccessor::requestFinishedSlot(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		QString errMsg = reply->errorString();
		emit error(errMsg);
	}
	else
	{
		QXmlStreamReader xml;
		xml.setDevice(reply);

		QString currentElement;
		while (!xml.atEnd())
		{
			xml.readNext();
			switch (xml.tokenType())
			{
			case QXmlStreamReader::StartElement:
				currentElement = xml.name().toString();

				if(currentElement == "site")
				{
					QXmlStreamAttributes attr = xml.attributes();
					if(attr.hasAttribute("name"))
						m_boardList.append(attr.value("name").toString());
				}
				break;
			default:
				break;
			}
		}
		emit boardListAvailable();
	}
}

QQBoardWizardOlccs::QQBoardWizardOlccs(QWidget *parent) :
	QWizardPage(parent)
{
	setTitle(tr("Board Selection"));
	setSubTitle(tr("Please for the list of boards to be available and select one to continue"));

	m_boardSelectorCB = new QComboBox(this);
	m_boardSelectorCB->setEditable(false);
	connect(m_boardSelectorCB, SIGNAL(currentIndexChanged(int)), this, SLOT(nameSelectionChanged(int)));
	m_waitLabel = new QLabel(this);
	QMovie *waitMovie = new QMovie(this);
	waitMovie->setFileName(":/img/totoz-loader.gif");
	m_waitLabel->setMovie(waitMovie);
	waitMovie->start();

	QVBoxLayout *layout = new QVBoxLayout;

	QHBoxLayout *l1 = new QHBoxLayout;
	l1->addWidget(m_boardSelectorCB);
	m_emptyListLabel = new QLabel(tr("No new board available via olccs"), this);
	l1->addWidget(m_emptyListLabel);
	m_emptyListLabel->hide();
	l1->addWidget(m_waitLabel);

	layout->addItem(l1);

	QFrame *line = new QFrame(this);
	line->setObjectName(QString::fromUtf8("line"));
	line->setGeometry(QRect(320, 150, 118, 3));
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	layout->addWidget(line);

	QFormLayout *l2 = new QFormLayout;
	m_groupSelCB = new QComboBox(this);
	l2->addRow(new QLabel(tr("group :"), this), m_groupSelCB);
	m_colorLE = new QLineEdit(this);
	m_colorLE->setPlaceholderText("exemple : #fec4c4");
	connect(m_colorLE, SIGNAL(textChanged(QString)), this, SLOT(colorChanged()));
	l2->addRow(new QLabel(tr("color :")), m_colorLE);
	QLineEdit *cookieLE = new QLineEdit(this);
	l2->addRow(new QLabel(tr("cookie :"), this), cookieLE);
	m_advSettingsCB = new QCheckBox(this);
	m_advSettingsCB->setEnabled(false);
	connect(m_advSettingsCB, SIGNAL(clicked()), this, SLOT(advSettingsTriggered()));
	l2->addRow(new QLabel(tr("<i>Show adv. settings page (TODO)</i> :"), this), m_advSettingsCB);

	layout->addItem(l2);

	registerField("Name*", m_boardSelectorCB);
	registerField("Color*", m_colorLE);
	registerField("Group", m_groupSelCB);
	registerField("Cookie", cookieLE);

	setLayout(layout);

	m_netAccessor = new QQOlccsWizardPMainNetAccessor(this);
	connect(m_netAccessor, SIGNAL(boardListAvailable()), this, SLOT(boardListAvailable()));
	m_netAccessor->updateBoardList();
}

void QQBoardWizardOlccs::setListGroups(QStringList& listGroups)
{
	m_groupSelCB->setEditable(true);
	m_groupSelCB->clear();
	m_groupSelCB->addItems(listGroups);
}

int	QQBoardWizardOlccs::nextId () const
{
	if(m_advSettingsCB->isChecked())
		return QQBoardWizard::Page_Adv;
	else
		return -1;
}

void QQBoardWizardOlccs::boardListAvailable()
{
	QStringList boards = m_netAccessor->boardList();
	QList<QQBouchot *> lB = QQBouchot::listBouchots();
	foreach (QQBouchot *b, lB)
		boards.removeAll(b->name());

	if(boards.isEmpty())
	{
		m_boardSelectorCB->hide();
		m_emptyListLabel->show();
	}
	else
	{
		m_boardSelectorCB->clear();
		m_boardSelectorCB->addItems(boards);
		m_boardSelectorCB->setCurrentIndex(0);
	}

	m_waitLabel->hide();
	if(m_waitLabel->movie() != NULL)
		m_waitLabel->movie()->stop();
}

void QQBoardWizardOlccs::advSettingsTriggered()
{
	setFinalPage(! m_advSettingsCB->isChecked());
}

void QQBoardWizardOlccs::colorChanged()
{
	QColor color(m_colorLE->text());
	if(color.isValid())
		m_colorLE->setStyleSheet("QLineEdit{background-color: " + m_colorLE->text() + ";}");
	else
		m_colorLE->setStyleSheet("QLineEdit{}");
}

void QQBoardWizardOlccs::nameSelectionChanged(int index)
{
	Q_UNUSED(index)
	QString name = m_boardSelectorCB->currentText();
	QColor color = QQBouchot::getBouchotDef(name).color();
	if(color.isValid())
		m_colorLE->setText(color.name());
	else
		m_colorLE->setText("");
}

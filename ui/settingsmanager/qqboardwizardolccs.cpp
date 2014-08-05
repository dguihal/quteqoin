#include "qqboardwizardolccs.h"

#include "core/qqsettings.h"
#include "ui/settingsmanager/qqboardwizard.h"

#include <QtDebug>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
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
	setSubTitle(tr("Please wait for the list of supported boards to be available, and select one to continue"));

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
	m_emptyListLabel->setStyleSheet("QLabel { color: red; font-weight : bold; font-size: 1.5em;}");
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
	l2->setHorizontalSpacing(5);
	m_groupSelCB = new QComboBox(this);
	l2->addRow(new QLabel(tr("group :"), this), m_groupSelCB);

	m_colorLE = new QLineEdit(this);
	m_colorLE->setPlaceholderText("exemple : #fec4c4");
	connect(m_colorLE, SIGNAL(textChanged(QString)), this, SLOT(colorChanged()));
	l2->addRow(new QLabel(tr("color :")), m_colorLE);

	m_cookieLE = new QLineEdit(this);
	l2->addRow(new QLabel(tr("cookie :"), this), m_cookieLE);

	m_advSettingsCB = new QCheckBox(this);
	m_advSettingsCB->setEnabled(true);
	l2->addRow(new QLabel(tr("<i>Show adv. settings page</i> :"), this), m_advSettingsCB);

	layout->addItem(l2);

	registerField("Olccs_Name*", m_boardSelectorCB);
	registerField("Olccs_Color*", m_colorLE);
	registerField("Olccs_Group", m_groupSelCB);
	registerField("Olccs_Cookie", m_cookieLE);
	registerField("Olccs_AdvSettings", m_advSettingsCB);

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

bool QQBoardWizardOlccs::isComplete() const
{
	bool ret = (! m_groupSelCB->currentText().isEmpty()) &&
			(! m_colorLE->text().isEmpty());
	return ret;
}

int	QQBoardWizardOlccs::nextId () const
{
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
		m_boardSelectorCB->setEnabled(false);
		m_colorLE->setEnabled(false);
		m_cookieLE->setEnabled(false);
		m_groupSelCB->setEnabled(false);
		m_advSettingsCB->setEnabled(false);
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

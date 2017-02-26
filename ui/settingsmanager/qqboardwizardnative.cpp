#include "qqboardwizardnative.h"

#include "core/qqbouchot.h"
#include "ui/settingsmanager/qqboardwizard.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>

QQBoardWizardNative::QQBoardWizardNative(QWidget *parent) :
	QWizardPage(parent)
{
	setTitle(tr("Board Selection"));
	setSubTitle(tr("Please select a template"));

	QVBoxLayout *layout = new QVBoxLayout;

	QHBoxLayout *l1 = new QHBoxLayout;

	QStringList boardsTpl = QQBouchot::getBouchotDefNameList();
	QList<QQBouchot *> lB = QQBouchot::listBouchots();
	foreach (QQBouchot *b, lB)
		boardsTpl.removeAll(b->name());
	bool hasNewItems = ! boardsTpl.isEmpty();

	m_boardSelectorCB = new QComboBox(this);
	registerField("Native_Name*", m_boardSelectorCB);
	l1->addWidget(m_boardSelectorCB);

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
	m_groupSelCB->setEnabled(hasNewItems);
	l2->addRow(new QLabel(tr("group :"), this), m_groupSelCB);

	m_colorLE = new QLineEdit(this);
	m_colorLE->setPlaceholderText("exemple : #fec4c4");
	connect(m_colorLE, SIGNAL(textChanged(QString)), this, SLOT(colorChanged()));
	l2->addRow(new QLabel(tr("color :")), m_colorLE);

	QLineEdit *cookieLE = new QLineEdit(this);
	cookieLE->setEnabled(hasNewItems);
	l2->addRow(new QLabel(tr("cookie :"), this), cookieLE);

	m_advSettingsCB = new QCheckBox(this);
	m_advSettingsCB->setEnabled(hasNewItems);
	l2->addRow(new QLabel(tr("<i>Show adv. settings page (TODO)</i> :"), this), m_advSettingsCB);

	layout->addItem(l2);

	registerField("Native_Name*", m_boardSelectorCB);
	registerField("Native_Group", m_groupSelCB);
	registerField("Native_Color*", m_colorLE);
	registerField("Native_Cookie", cookieLE);
	registerField("Native_AdvSettings", m_advSettingsCB);

	if(hasNewItems)
	{
		m_boardSelectorCB->setEditable(false);
		connect(m_boardSelectorCB, SIGNAL(currentIndexChanged(int)), this, SLOT(nameSelectionChanged(int)));
		m_boardSelectorCB->addItems(boardsTpl);
		m_boardSelectorCB->setCurrentIndex(0);
		m_emptyListLabel = NULL;
	}
	else
	{
		m_boardSelectorCB->hide();
		m_emptyListLabel = new QLabel(tr("No new board template available"), this);
		m_emptyListLabel->setStyleSheet("QLabel { color: red; font-weight : bold; font-size: 1.5em;}");
		l1->addWidget(m_emptyListLabel);
	}

	setLayout(layout);
}

void QQBoardWizardNative::setListGroups(QStringList& listGroups)
{
	m_groupSelCB->setEditable(true);
	m_groupSelCB->clear();
	m_groupSelCB->addItems(listGroups);
}

bool QQBoardWizardNative::isComplete() const
{
	bool ret = (m_groupSelCB->currentIndex() >= 0) &&
			(! m_colorLE->text().isEmpty());
	return ret;
}

int	QQBoardWizardNative::nextId () const
{
	return -1;
}

void QQBoardWizardNative::colorChanged()
{
	QColor color(m_colorLE->text());
	if(color.isValid())
		m_colorLE->setStyleSheet("QLineEdit{background-color: " + m_colorLE->text() + ";}");
	else
		m_colorLE->setStyleSheet("QLineEdit{}");
}

void QQBoardWizardNative::nameSelectionChanged(int index)
{
	Q_UNUSED(index)
	QString name = m_boardSelectorCB->currentText();
	QColor color = QQBouchot::getBouchotDef(name).color();
	if(color.isValid())
		m_colorLE->setText(color.name());
	else
		m_colorLE->setText("");
}

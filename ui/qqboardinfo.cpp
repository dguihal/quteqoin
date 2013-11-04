#include "qqboardinfo.h"
#include "ui_qqboardinfo.h"

#include "core/qqbouchot.h"

#include <QtDebug>

QQBoardInfo::QQBoardInfo(QQBouchot *board, QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::QQBoardInfo),
	m_board(board)
{
	if(m_board == NULL)
		return;

	m_ui->setupUi(this);


	m_ui->bodyWidget->hide();
	m_ui->showBtn->setText("+");
	connect(m_ui->showBtn, SIGNAL(clicked()), this, SLOT(toggleExpandedView()));

	m_ui->refreshPB->setTextVisible(true);
	m_ui->refreshPB->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_ui->refreshPB->setFormat(m_board->name());
	m_ui->refreshPB->setStyleSheet(QString("QProgressBar { border: none; background-color: rgba(0, 0, 0, 0%)} ")
								   .append("QProgressBar::chunk {background-color: ")
								   .append(m_board->settings().color().name())
								   .append("; width: 5px; margin: 0.5px;}"));

	m_pctPollAnimation.setStartValue(0);
	m_pctPollAnimation.setEndValue(100);
	m_pctPollAnimation.setEasingCurve(QEasingCurve::Linear);
	m_pctPollAnimation.setTargetObject(m_ui->refreshPB);
	m_pctPollAnimation.setPropertyName("value");
	rearmRefreshPB();
	connect(m_board, SIGNAL(lastPostersUpdated()), this, SLOT(updateUserList()));
	connect(m_board, SIGNAL(refreshStarted()), this, SLOT(rearmRefreshPB()));
}

QQBoardInfo::~QQBoardInfo()
{
	delete m_ui;
}

void QQBoardInfo::rearmRefreshPB()
{
	m_pctPollAnimation.stop();
	m_ui->refreshPB->setValue(0);
	m_pctPollAnimation.setDuration(m_board->settings().refresh() * 1000);
	m_pctPollAnimation.start();
}

void QQBoardInfo::toggleExpandedView()
{
	if(m_ui->bodyWidget->isVisible())
	{
		m_ui->bodyWidget->hide();
		m_ui->showBtn->setText("+");
	}
	else
	{
		m_ui->bodyWidget->show();
		m_ui->showBtn->setText("-");
	}
}

#define MAX_ITEMS 10
void QQBoardInfo::updateUserList()
{
	QList<QQMussel> lastPosters = m_board->lastPosters();

	QWidget *boardInfoWidget = new QWidget(m_ui->usrDspSA);

	if(! lastPosters.isEmpty())
	{
		QVBoxLayout *lastPostersWidgetLayout = new QVBoxLayout(boardInfoWidget);
		lastPostersWidgetLayout->setSpacing(1);
		lastPostersWidgetLayout->setContentsMargins(0, 0, 0, 0);

		QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		policy.setHorizontalStretch(0);
		policy.setVerticalStretch(0);

		int vSpace = 0;
		for(int i = 0; i < lastPosters.size(); i++)
		{
			QString name = lastPosters.at(i).name();
			if(! lastPosters.at(i).isAuth())
				name.prepend("<i>").append("</i>");

			QLabel *lbl = new QLabel(name, boardInfoWidget);
			lbl->setSizePolicy(policy);
			lastPostersWidgetLayout->addWidget(lbl);
			if(i < MAX_ITEMS)
				vSpace += lbl->sizeHint().height() + 1;
		}

		QWidget *old = m_ui->usrDspSA->takeWidget();
		if(old != NULL)
			delete old;

		m_ui->usrDspSA->setWidget(boardInfoWidget);

		m_ui->usrDspSA->setMaximumHeight(vSpace);
		m_ui->usrDspSA->setMinimumHeight(vSpace);

		m_ui->usrDspSA->show();
	}
	else
	{
		m_ui->usrDspSA->hide();
	}
}

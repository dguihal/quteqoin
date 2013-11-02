#include "qqboardinfo.h"
#include "ui_qqboardinfo.h"

#include "core/qqbouchot.h"

#include <QtDebug>
#include <QPalette>

QQBoardInfo::QQBoardInfo(QQBouchot *board, QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::QQBoardInfo),
	m_board(board),
	m_pctPoll(0)
{
	if(m_board == NULL)
		return;

	m_ui->setupUi(this);

	m_ui->bodyWidget->hide();
	m_ui->showBtn->setText("+");
	connect(m_ui->showBtn, SIGNAL(clicked()), this, SLOT(toggleExpandedView()));

	m_ui->boardNameLbl->setText(m_board->name());
	connect(m_board, SIGNAL(lastPostersUpdated()), this, SLOT(updateUserList()));
}

QQBoardInfo::~QQBoardInfo()
{
	delete m_ui;
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

void QQBoardInfo::setPctPoll(int pctPoll)
{
	m_pctPoll = pctPoll;

	QPalette palette;
	QLinearGradient linearGradient(QPointF(0, 0), QPointF(width(), 0));
	QColor baseColor = m_board->settings().color();
	linearGradient.setColorAt(width() * m_pctPoll / 100.0, baseColor);
	baseColor.setAlpha(0);
	linearGradient.setColorAt(width(), baseColor);
	palette.setBrush(QPalette::Window,*(new QBrush(linearGradient)));

	m_ui->headWidget->setPalette(palette);
	m_ui->headWidget->update();
}

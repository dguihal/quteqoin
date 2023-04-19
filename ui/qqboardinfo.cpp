#include "qqboardinfo.h"
#include "ui_qqboardinfo.h"

#include "core/qqbouchot.h"
#include "core/qqboardstatechangeevent.h"
#include "core/qutetools.h"
#include "ui/qqmusselinfo.h"

#include <QtDebug>
#include <QFontMetrics>

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::QQBoardInfo
/// \param board
/// \param parent
///
QQBoardInfo::QQBoardInfo(QQBouchot *board, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::QQBoardInfo),
    m_board(board)
{
	if(m_board == nullptr)
		return;

	m_ui->setupUi(this);

	m_ui->usrDspSA->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_ui->usrDspSA->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	m_ui->bodyWidget->hide();
	m_ui->showBtn->setText("+");
	connect(m_ui->showBtn, &QToolButton::clicked, this, &QQBoardInfo::toggleExpandedView);

	m_ui->refreshPB->setTextVisible(true);
	m_ui->refreshPB->setBoardColor(m_board->settings().color());
	m_ui->refreshPB->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_ui->refreshPB->setBoardName(board->name());
	connect(m_ui->refreshPB, &QQBoardInfoProgressBar::bouchotSelected,
	        this, &QQBoardInfo::toggleBoardVisibility);

	QFontInfo fi = m_ui->refreshPB->fontInfo();
	int size = fi.pixelSize() + 2;
	m_ui->refreshPB->setFixedHeight(size);
	m_ui->showBtn->setFixedHeight(size);
	m_ui->showBtn->setFixedWidth(size);

	m_pctPollAnimation.setStartValue(100);
	m_pctPollAnimation.setEndValue(0);
	m_pctPollAnimation.setEasingCurve(QEasingCurve::Linear);
	m_pctPollAnimation.setTargetObject(m_ui->refreshPB);
	m_pctPollAnimation.setPropertyName("value");

	rearmRefreshPB();
	connect(m_board, &QQBouchot::lastPostersUpdated, this, &QQBoardInfo::updateUserList);
	connect(m_board, &QQBouchot::refreshStarted, this, &QQBoardInfo::rearmRefreshPB);
	connect(m_board, &QQBouchot::refreshOK, this, &QQBoardInfo::resetFromErrorState);
	connect(m_board, &QQBouchot::refreshError, this, &QQBoardInfo::showRefreshError);
	connect(m_board, &QQBouchot::visibilitychanged, this, &QQBoardInfo::updateNameWithStatus);
	board->registerForEventNotification(this, QQBouchot::StateChanged);
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::~QQBoardInfo
///
QQBoardInfo::~QQBoardInfo()
{
	delete m_ui;
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::sizeHint
/// \return
///
QSize QQBoardInfo::sizeHint() const
{
	QFontMetrics fm = m_ui->usrDspSA->fontMetrics();
	int minWidth = fm.boundingRect("moules").width();
	return QSize(minWidth + m_ui->labelUsers->minimumWidth(), m_ui->labelUsers->minimumHeight());
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::musselSelected
/// \param mussel
///
void QQBoardInfo::musselSelected(QQMussel mussel)
{
	Q_UNUSED(mussel)
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::rearmRefreshPB
///
void QQBoardInfo::rearmRefreshPB()
{
	disconnect(m_board, SIGNAL(refreshOK()), this, SLOT(rearmRefreshPB()));

	m_pctPollAnimation.stop();
	m_ui->refreshPB->setValue(0);
	m_pctPollAnimation.setDuration(m_board->currentRefreshInterval());
	m_pctPollAnimation.start();
}


//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::resetFromErrorState
///
void QQBoardInfo::resetFromErrorState()
{
	m_ui->refreshPB->setOnError(false);
	m_ui->refreshPB->setToolTip("");

	updateNameWithStatus();
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::showRefreshError
/// \param errMsg
///
void QQBoardInfo::showRefreshError(QString &errMsg)
{
	m_ui->refreshPB->setOnError(true);
	m_ui->refreshPB->setToolTip(errMsg);

	connect(m_board, SIGNAL(refreshOK()), this, SLOT(rearmRefreshPB()));

	updateNameWithStatus();
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::toggleBoardVisibility
///
void QQBoardInfo::toggleBoardVisibility()
{
	m_board->toggleVisibility();
	updateNameWithStatus();
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::toggleExpandedView
///
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
//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::updateUserList
///
void QQBoardInfo::updateUserList()
{
	QList<QQMussel> lastPosters = m_board->lastPosters();

	QWidget *boardInfoWidget = new QWidget(m_ui->usrDspSA);

	QHash<QQMussel, QQMusselInfo *> newMusselInfoHash;

	QVBoxLayout *lastPostersWidgetLayout = new QVBoxLayout(boardInfoWidget);
	lastPostersWidgetLayout->setSpacing(1);
	lastPostersWidgetLayout->setContentsMargins(0, 0, 0, 0);

	QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	policy.setHorizontalStretch(0);
	policy.setVerticalStretch(0);

	// TODO Keep all datas, but mark them as hidden to
	//		1/ keep tracks of slow posting mussels
	//		2/ Avoid mem leaks

	int vSpace = 0;
	for(int i = 0; i < lastPosters.size(); i++)
	{
		QQMussel mussel = lastPosters.at(i);
		QQMusselInfo *mi = nullptr;
		if(m_musselInfoHash.contains(mussel))
		{
			mi = m_musselInfoHash.take(mussel);
			mi->setParent(boardInfoWidget);
		}
		else
		{
			mi = new QQMusselInfo(mussel, boardInfoWidget);
			QObject::connect(mi, &QQMusselInfo::musselSelected, this, &QQBoardInfo::musselSelected);
			mi->setSizePolicy(policy);
		}

		newMusselInfoHash.insert(mussel, mi);
		lastPostersWidgetLayout->addWidget(mi);
		if(i < MAX_ITEMS)
			vSpace += mi->sizeHint().height() + 1;
	}

	QWidget *old = m_ui->usrDspSA->takeWidget();
	if(old != nullptr)
		delete old;
	m_ui->usrDspSA->setWidget(boardInfoWidget);

	if(! lastPosters.isEmpty())
	{
		m_ui->usrDspSA->setMaximumHeight(vSpace);
		m_ui->usrDspSA->setMinimumHeight(vSpace);

		m_ui->usrDspSA->show();
	}
	else
		m_ui->usrDspSA->hide();


	m_musselInfoHash = newMusselInfoHash;
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::event
/// \param e
/// \return
///
bool QQBoardInfo::event(QEvent *e)
{
	if(e->type() == QQBoardStateChangeEvent::BOARD_STATE_CHANGED)
		updateNameWithStatus();
	else
		QWidget::event(e);

	return true;
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfo::updateNameWithStatus
///
void QQBoardInfo::updateNameWithStatus()
{
	QString flags = QuteTools::statusStringFromState(m_board->boardState());
	m_ui->refreshPB->setBoardStatusFlags(flags);
}

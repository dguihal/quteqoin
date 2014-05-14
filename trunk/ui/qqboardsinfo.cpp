#include "qqboardsinfo.h"
#include "ui_qqboardsinfo.h"

#include "core/qqbouchot.h"
#include "ui/qqboardinfo.h"

#include <QtDebug>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSpacerItem>

#define BOARDSINFO_OBJECT_NAME "QQBoardInfo"

#define BOARDSINFO_TITLE	"Board Info"

//////////////////////////////////////////////////////////////
/// \brief QQBoardsInfo::QQBoardsInfo
/// \param parent
///
QQBoardsInfo::QQBoardsInfo(QWidget *parent) :
	QDockWidget(BOARDSINFO_TITLE, parent), m_ui(new Ui::QQBoardsInfo)
{
	setObjectName(BOARDSINFO_OBJECT_NAME);
	setFeatures(QDockWidget::DockWidgetClosable |
				QDockWidget::DockWidgetMovable |
				QDockWidget::DockWidgetFloatable);

	m_ui->setupUi(this);
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardsInfo::~QQBoardsInfo
///
QQBoardsInfo::~QQBoardsInfo()
{
	delete m_ui;
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardsInfo::updateBoardList
///
void QQBoardsInfo::updateBoardList()
{
	QList<QQBouchot *> listBouchots = QQBouchot::listBouchots();

	QWidget *boardsInfoWidget = new QWidget(m_ui->scrollArea);
	QVBoxLayout *boardsInfoWidgetLayout = new QVBoxLayout(boardsInfoWidget);
	boardsInfoWidgetLayout->setSpacing(3);
	boardsInfoWidgetLayout->setContentsMargins(1, 1, 1, 1);
	QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	policy.setHorizontalStretch(0);
	policy.setVerticalStretch(0);

	QFrame *line = NULL;
	QHash<QString, QPointer<QQBoardInfo> > newhashbInfo;
	bool first = true;
	foreach(QQBouchot *board, listBouchots)
	{
		QQBoardInfo *boardInfo = NULL;
		QPointer<QQBoardInfo> bInfoPtr = m_hashbInfo.value(board->name());
		if(! bInfoPtr.isNull())
		{
			newhashbInfo.insert(board->name(), bInfoPtr);
			boardInfo = bInfoPtr.data();
			boardInfo->setParent(boardsInfoWidget);
		}
		else
		{
			boardInfo = new QQBoardInfo(board, boardsInfoWidget);
			connect(boardInfo, SIGNAL(bouchotSelected(QString)), this, onBouchotSelected(QString));
			newhashbInfo.insert(board->name(), QPointer<QQBoardInfo>(boardInfo));
			boardInfo->setSizePolicy(policy);
		}

		if(! first)
		{
			line = new QFrame(boardsInfoWidget);
			line->setObjectName(QString::fromUtf8("line"));
			line->setFrameShape(QFrame::HLine);
			line->setFrameShadow(QFrame::Sunken);
			boardsInfoWidgetLayout->addWidget(line);
		}
		else
			first = false;
		boardsInfoWidgetLayout->addWidget(boardInfo);
	}
	boardsInfoWidgetLayout->addSpacerItem(new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

	boardsInfoWidget->setLayout(boardsInfoWidgetLayout);

	QWidget *oldWidget = m_ui->scrollArea->takeWidget();
	if(oldWidget != NULL)
		delete oldWidget;

	m_hashbInfo = newhashbInfo;
	m_ui->scrollArea->setWidget(boardsInfoWidget);
	boardsInfoWidget->show();
}

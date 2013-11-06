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

QQBoardsInfo::QQBoardsInfo(QWidget *parent) :
	QDockWidget(BOARDSINFO_TITLE, parent), m_ui(new Ui::QQBoardsInfo)
{
	setObjectName(BOARDSINFO_OBJECT_NAME);
	setFeatures(QDockWidget::DockWidgetClosable |
				QDockWidget::DockWidgetMovable |
				QDockWidget::DockWidgetFloatable);

	m_ui->setupUi(this);
}

QQBoardsInfo::~QQBoardsInfo()
{
	delete m_ui;
}

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
	foreach(QQBouchot *board, listBouchots)
	{
		connect(board, SIGNAL(destroyed()), this, SLOT(updateBoardList()));
		QQBoardInfo *boardInfo = new QQBoardInfo(board, boardsInfoWidget);
		boardInfo->setSizePolicy(policy);
		boardsInfoWidgetLayout->addWidget(boardInfo);

		line = new QFrame(boardsInfoWidget);
		line->setObjectName(QString::fromUtf8("line"));
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Sunken);
		boardsInfoWidgetLayout->addWidget(line);
	}
	boardsInfoWidgetLayout->removeWidget(line);
	delete line;
	boardsInfoWidgetLayout->addSpacerItem(new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

	boardsInfoWidget->setLayout(boardsInfoWidgetLayout);

	QWidget *oldWidget = m_ui->scrollArea->takeWidget();
	if(oldWidget != NULL)
		delete oldWidget;

	m_ui->scrollArea->setWidget(boardsInfoWidget);
	boardsInfoWidget->show();
}

#include "qqboardinfoprogressbar.h"

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::QQBoardInfoProgressBar
/// \param parent
/// \param boardName
///
QQBoardInfoProgressBar::QQBoardInfoProgressBar(QWidget *parent, const QString &boardName) :
	QProgressBar(parent),
	m_boardName(boardName)
{
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::setBoardStatusFlags
/// \param flags
///
void QQBoardInfoProgressBar::setBoardStatusFlags(const QString &flags)
{
	QString info = m_boardName;
	if(flags.size() > 0)
		info.append(" (").append(flags).append(")");

	setFormat(m_boardName);
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::mousePressEvent
/// \param event
///
void QQBoardInfoProgressBar::mousePressEvent(QMouseEvent *event)
{
	QProgressBar::mousePressEvent(event);
	emit bouchotSelected(m_boardName);
}

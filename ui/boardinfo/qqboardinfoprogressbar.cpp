#include "qqboardinfoprogressbar.h"

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::QQBoardInfoProgressBar
/// \param parent
/// \param boardName
///
QQBoardInfoProgressBar::QQBoardInfoProgressBar(QWidget *parent) :
	QProgressBar(parent)
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

	setFormat(info);
}

//////////////////////////////////////////////////////////////
void QQBoardInfoProgressBar::setBoardName(const QString &boardName)
{
	m_boardName = boardName;
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

#include "qqboardinfoprogressbar.h"

#include <QPainter>
#include <QtDebug>

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::QQBoardInfoProgressBar
/// \param parent
/// \param boardName
///
QQBoardInfoProgressBar::QQBoardInfoProgressBar(QWidget *parent) :
    QProgressBar(parent),
    m_onError(false)
{
#ifdef Q_OS_WIN32
	QFont font("Segoe UI Symbol", 10);
	setFont(font);
#endif
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
/// \brief QQBoardInfoProgressBar::setBoardName
/// \param boardName
///
void QQBoardInfoProgressBar::setBoardName(const QString &boardName)
{
	m_boardName = boardName;
	setFormat(m_boardName);
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::setOnError
/// \param onError
///
void QQBoardInfoProgressBar::setOnError(const bool &onError)
{
	if(m_onError != onError)
	{
		m_onError = onError;
		update();
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::mousePressEvent
/// \param event
///
void QQBoardInfoProgressBar::mousePressEvent(QMouseEvent *event)
{
	QProgressBar::mousePressEvent(event);
	emit bouchotSelected();
}

//////////////////////////////////////////////////////////////
/// \brief QQBoardInfoProgressBar::paintEvent
/// \param event
///
void QQBoardInfoProgressBar::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QRect rect = this->geometry();
	rect.setWidth(static_cast<int>(rect.width() * (static_cast<double>(value()) / static_cast<double> (maximum()))));

	//Fill
	QPainter rectPainter(this);

	// gradient
	QLinearGradient gradient(0, 0, 0, rect.height());
	gradient.setColorAt(0.5, Qt::transparent);
	gradient.setColorAt(0.65, m_boardColor);
	gradient.setColorAt(0.85, m_boardColor);
	gradient.setColorAt(1.0, Qt::transparent);

	// brush
	QBrush brush(gradient);
	rectPainter.setBrush(brush);
	rectPainter.setPen(Qt::NoPen);
	rectPainter.setOpacity(1);
	rectPainter.drawRect(rect);

	//Text
	rect = this->geometry();
	rect.setX(rect.x() + 5);
	QPainter textPainter(this);
	if(m_onError)
		textPainter.setPen(Qt::red);
	textPainter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text());
}

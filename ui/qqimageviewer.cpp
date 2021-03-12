#include "qqimageviewer.h"

#include <QDebug>
#include <QToolTip>

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::QQImageViewer
/// \param parent
///
QQImageViewer::QQImageViewer(QWidget *parent) :
	QLabel(parent),
	m_showAtMousePos(true)
{
	m_imgMovie.setCacheMode(QMovie::CacheAll);

	setTextFormat(Qt::PlainText);
	setTextInteractionFlags(Qt::NoTextInteraction);
	setWordWrap(false);

	setScaledContents(false);
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::hideEvent
/// \param event
///
void QQImageViewer::hideEvent(QHideEvent *event)
{
	if(m_imgMovie.isValid())
		m_imgMovie.stop();
	QLabel::hideEvent(event);
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::showEvent
/// \param event
///
void QQImageViewer::showEvent(QShowEvent *event)
{
	if(m_imgMovie.isValid())
		m_imgMovie.start();
	QLabel::showEvent(event);
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::displayText
/// \param text
///
void QQImageViewer::displayText(const QString &text)
{
	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 192); color : black; }");
	QFontMetricsF fm(font());
	QSizeF txtSize = fm.size(Qt::TextSingleLine, text);
	setText(text);
	setMinimumSize(txtSize.toSize());
	setMaximumSize(txtSize.toSize());
	adjustSize();
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::displayMovie
///
void QQImageViewer::displayMovie()
{
	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 0); color : black; }");
	m_imgMovie.disconnect(SIGNAL(finished()));

	m_imgMovie.jumpToFrame(0);

	setMovie(&m_imgMovie);
	if(isVisible())
		m_imgMovie.start();
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::displayImage
/// \param image
///
void QQImageViewer::displayImage(QImage &image)
{
	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 0); color : black; }");
	QPixmap imgPixmap = QPixmap::fromImage(image);
	setPixmap(imgPixmap);
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::updateImg
/// \param imgData
///
bool QQImageViewer::updateImg(const QByteArray &imgData, const QSize &maxSize)
{
	m_imgDataBuffer.close();
	m_imgDataBuffer.setData(imgData);
	m_imgDataBuffer.open(QIODevice::ReadOnly);

	QImage image;
	if(! image.loadFromData(imgData))
	{
		displayWaitMovie();
		return false;
	}

	QSize imgSize = image.size();
	if(maxSize.isValid())
		imgSize.scale(imgSize.boundedTo(maxSize), Qt::KeepAspectRatio);

	m_imgMovie.stop();
	m_imgMovie.setDevice(&m_imgDataBuffer);
	if(m_imgMovie.isValid())
	{
		m_imgMovie.setScaledSize(imgSize);

		displayMovie();
	}
	else
		displayImage(image);
	setFixedSize(imgSize);
	adjustSize();

	return true;
}

//////////////////////////////////////////////////////////////
/// \brief displayWaitMovie
///
void QQImageViewer::displayWaitMovie()
{
	m_imgDataBuffer.close();
	m_imgMovie.setScaledSize(QSize());
	m_imgMovie.setFileName(":/img/totoz-loader.gif");

	displayMovie();

	QSize vidSize = m_imgMovie.currentPixmap().size();

	setMinimumSize(vidSize);
	setMaximumSize(vidSize);
	adjustSize();
}

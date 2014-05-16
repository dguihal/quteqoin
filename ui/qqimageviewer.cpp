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
	setPos();
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::displayMovie
///
void QQImageViewer::displayMovie()
{
	setStyleSheet("QLabel { background-color : rgba(255, 255, 255, 0); color : black; }");
	m_imgMovie.disconnect(SIGNAL(finished()));

	if(m_imgMovie.frameCount() > 1)
		connect(&m_imgMovie, SIGNAL(finished()), &m_imgMovie, SLOT(start()));

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
	if(maxSize.isValid() &&
			(imgSize.width() > maxSize.width() ||
			 imgSize.height() > maxSize.height())
			)
	{
		imgSize.scale(maxSize, Qt::KeepAspectRatio);
		m_imgMovie.setScaledSize(imgSize);
		image = image.scaled(maxSize);
	}
	else
		m_imgMovie.setScaledSize(QSize());

	m_imgMovie.setDevice(&m_imgDataBuffer);

	if(m_imgMovie.isValid())
		displayMovie();
	else
		displayImage(image);

	setMinimumSize(imgSize);
	setMaximumSize(imgSize);
	adjustSize();
	setPos();

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
	setPos();
}

//////////////////////////////////////////////////////////////
/// \brief QQImageViewer::setPos
///
void QQImageViewer::setPos()
{
	QWidget* parent = parentWidget();
	if(m_showAtMousePos && parent != NULL)
	{
		bool shouldMoveToolTip = true;
		QPoint totozViewerPos = parent->mapFromGlobal(QCursor::pos());
		QSize parentSize = parent->size();
		if(totozViewerPos.x() > (parentSize.width() / 2)) // a droite
		{
			totozViewerPos.rx() -= width();
			shouldMoveToolTip = false;
		}
		if(totozViewerPos.y() > (parentSize.height() / 2)) // en dessous
		{
			totozViewerPos.ry() -= height();
			shouldMoveToolTip = false;
		}

		move(totozViewerPos);

		if(shouldMoveToolTip && QToolTip::isVisible())
		{
			QString txt = QToolTip::text();

			QPoint ttPos = totozViewerPos;
			ttPos.ry() += height();

			QToolTip::showText(parent->mapToGlobal(ttPos), "Lorem Ipsum", parent); // Ne fonctionne pas avec un texte vide
			QToolTip::showText(parent->mapToGlobal(ttPos), txt, parent);
		}
	}
}

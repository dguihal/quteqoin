#include "qqwebimageviewer.h"

#include <QDebug>

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::QQWebImageViewer
/// \param parent
///
QQWebImageViewer::QQWebImageViewer(QWidget *parent) :
	QQImageViewer(parent)
{
	imgDownloader = new QQWebImageDownloader(this);
	connect(imgDownloader, SIGNAL(ready()), this, SLOT(imgReady()));
}

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::showImg
/// \param url
///
void QQWebImageViewer::showImg(const QUrl &url)
{
	displayWaitMovie();
	imgDownloader->getImage(url);
}

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::imgReady
///
void QQWebImageViewer::imgReady()
{
	if(! updateImg(imgDownloader->imgData(), m_imgMaxSize))
		displayText(tr("Unrecognised or invalid image"));
}

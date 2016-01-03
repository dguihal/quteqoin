#include "qqwebimageviewer.h"

#include <QDebug>

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::QQWebImageViewer
/// \param parent
///
QQWebImageViewer::QQWebImageViewer(QWidget *parent) :
	QQImageViewer(parent)
{
	m_downloader = new QQWebDownloader(this);
	connect(m_downloader, SIGNAL(ready()), this, SLOT(imgReady()));
}

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::showImg
/// \param url
///
void QQWebImageViewer::showImg(const QUrl &url)
{
	displayWaitMovie();
	m_downloader->getURL(url);
}

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::imgReady
///
void QQWebImageViewer::imgReady()
{
	QString dataCType = m_downloader->dataContentType();
	if(dataCType.startsWith("image/"))
	{
		if(! updateImg(m_downloader->imgData(), m_imgMaxSize))
			displayText(tr("Unrecognised or invalid image"));
	}
	// TODO : else if (dataCType.startsWith("video/"))
	else
		displayText(tr("Not an image, preview not supported"));
}

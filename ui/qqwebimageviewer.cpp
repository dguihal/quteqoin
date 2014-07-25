#include "qqwebimageviewer.h"

#include <QDebug>

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::QQWebImageViewer
/// \param parent
///
QQWebImageViewer::QQWebImageViewer(QWidget *parent) :
	QQImageViewer(parent)
{
	m_imgDownloader = new QQWebImageDownloader(this);
	connect(m_imgDownloader, SIGNAL(ready()), this, SLOT(imgReady()));
}

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::showImg
/// \param url
///
void QQWebImageViewer::showImg(const QUrl &url)
{
	displayWaitMovie();
	m_imgDownloader->getImage(url);
}

//////////////////////////////////////////////////////////////
/// \brief QQWebImageViewer::imgReady
///
void QQWebImageViewer::imgReady()
{
	QString dataCType = m_imgDownloader->dataContentType();
	if(! dataCType.startsWith("image/"))
	{
		displayText(tr("Not an image, preview not supported"));
		return;
	}
	if(! updateImg(m_imgDownloader->imgData(), m_imgMaxSize))
		displayText(tr("Unrecognised or invalid image"));
}

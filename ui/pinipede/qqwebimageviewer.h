#ifndef QQWEBIMAGEVIEWER_H
#define QQWEBIMAGEVIEWER_H

#include "ui/qqimageviewer.h"
#include "core/qqwebdownloader.h"

class QQWebImageViewer : public QQImageViewer
{
	Q_OBJECT
public:
	explicit QQWebImageViewer(QWidget *parent = 0);

	//////////////////////////////////////////////////////////////
	/// \brief setImgMaxSize
	/// \param imgMaxSize
	///
	void setImgMaxSize(const QSize &imgMaxSize) { m_imgMaxSize = imgMaxSize; }

	void showImg(const QUrl &url);

protected slots:
	void imgReady();

private:
	QQWebDownloader *m_downloader;

	QSize m_imgMaxSize;
};

#endif // QQWEBIMAGEVIEWER_H

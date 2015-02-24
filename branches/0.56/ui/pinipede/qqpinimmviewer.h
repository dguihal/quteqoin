#ifndef QQPINIMMVIEWER_H
#define QQPINIMMVIEWER_H

#include "ui/qqimageviewer.h"
#include "core/qqwebimagedownloader.h"

#include <QWidget>

class QQWebImageViewer;

class QQPiniMMViewer : public QWidget
{
	Q_OBJECT
public:
	explicit QQPiniMMViewer(QWidget *parent = 0);
	~QQPiniMMViewer();

	//////////////////////////////////////////////////////////////
	/// \brief setViewMaxSize
	/// \param imgMaxSize
	///
	void setViewMaxSize(const QSize &viewMaxSize) { m_viewMaxSize = viewMaxSize; }

	void showImg(const QUrl &url);
signals:

public slots:

private:
	QQWebImageDownloader *m_downloader;

	QSize m_viewMaxSize;
};

#endif // QQPINIMMVIEWER_H

#ifndef QQIMAGEVIEWER_H
#define QQIMAGEVIEWER_H

#include <QBuffer>
#include <QLabel>
#include <QMovie>

class QQImageViewer : public QLabel
{
	Q_OBJECT
public:
	explicit QQImageViewer(QWidget *parent = 0);

	bool updateImg(const QByteArray &imgData, const QSize &maxSize = QSize());

	bool showAtMousePos() const { return m_showAtMousePos; }
	void setShowAtMousePos(bool showAtMousePos) { m_showAtMousePos = showAtMousePos; }

	void displayWaitMovie();

signals:

public slots:

protected:
	virtual void hideEvent(QHideEvent *event);
	virtual void showEvent(QShowEvent *event);

	void displayText(const QString &text);
	void displayMovie();
	void displayImage(QImage &image);

private:
	void setPos();

	QBuffer m_imgDataBuffer;
	QMovie m_imgMovie;

	bool m_showAtMousePos;
};

#endif // QQIMAGEVIEWER_H

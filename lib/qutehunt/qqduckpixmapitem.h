#ifndef QQDUCKPIXMAPITEM_H
#define QQDUCKPIXMAPITEM_H

#include <QGenericMatrix>
#include <QGraphicsPixmapItem>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QVector>

typedef QGenericMatrix<1, 2, qreal> QQMatrix1x2;

class QQDuckPixmapItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
	explicit QQDuckPixmapItem(QString srcBouchot, QString postId, QObject *parent = 0);
	~QQDuckPixmapItem();

signals:

public slots:
	void animate();
	void animateKill();
	void animateMove();
	void loadNextPixMap();

private:
	void nextMoveStep();

	QSequentialAnimationGroup *m_animation;

	QQMatrix1x2 m_speedVec;

	class QQPixmapProp
	{
	public:
		explicit QQPixmapProp() : m_pixmapName() { m_rotationAngle = 0.0, m_mirrorX = false, m_mirrorY = false; }
		QString m_pixmapName;
		qreal m_rotationAngle;
		bool m_mirrorX;
		bool m_mirrorY;
	};
	QString m_srcBouchot;
	QString m_postId;

	QVector<QQPixmapProp> m_listPixmapProp;
	int m_animPixmapNum;
	bool m_animPixmapNumInc;
	bool m_animPixmapTimerEnabled;
	QTimer m_animPixmapTimer;
	QString m_currPixmapName;
};

#endif // QQDUCKPIXMAPITEM_H

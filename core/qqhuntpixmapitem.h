#ifndef QQHUNTPIXMAPITEM_H
#define QQHUNTPIXMAPITEM_H

#include <QGenericMatrix>
#include <QGraphicsPixmapItem>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QVector>

typedef QGenericMatrix<1, 2, float> QQMatrix1x2;
#define X_VALUE 0,0
#define Y_VALUE 1,0

typedef QGenericMatrix<2, 2, float> QQMatrix2x2;

class QQHuntPixmapItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
	explicit QQHuntPixmapItem(QString srcBouchot, QString postId, bool selfItem, QObject *parent = 0);
	~QQHuntPixmapItem();

	QString bouchotName() const { return m_srcBouchot; }
	QString postId() const { return m_postId; }
	bool isSelfItem() const { return m_isSelfItem; }

signals:

public slots:
	void animate();
	void animateKill();
	void animateMove();
	void loadNextPixMap();

protected:

	class QQPixmapProp
	{
	public:
		explicit QQPixmapProp() : m_pixmapName() { m_rotationAngle = 0.0, m_mirrorX = false, m_mirrorY = false; }
		QString m_pixmapName;
		float m_rotationAngle;
		bool m_mirrorX;
		bool m_mirrorY;
	};

	virtual QQPixmapProp animPixmapProp(float angle) = 0;
	virtual QQPixmapProp fallPixmapProp() = 0;
	virtual QQPixmapProp killPixmapProp() = 0;
	void nextMoveStep();

	QSequentialAnimationGroup *m_animation;
	QTimer m_animPixmapTimer;
	bool m_animPixmapTimerEnabled;
	QVector<QQPixmapProp> m_listPixmapProp;
	int m_maxAnimPixmapNumInc;
	int m_minAnimPixmapNumInc;
	int m_animPixmapNum;
	bool m_animPixmapNumInc;
	QString m_currPixmapName;

	QQMatrix1x2 m_speedVec;

	QString m_srcBouchot;
	QString m_postId;
	bool m_isSelfItem;
};

#endif // QQHUNTPIXMAPITEM_H

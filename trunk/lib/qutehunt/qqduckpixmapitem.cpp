#include "qqduckpixmapitem.h"

#include "qutetools.h"

#include <qmath.h>

#include <QGraphicsScene>
#include <QImage>
#include <QPropertyAnimation>

#define X_VALUE 0,0
#define Y_VALUE 1,0

#define BASE_DUCK_PIX QString(":/anims/")

QQDuckPixmapItem::QQDuckPixmapItem(QString srcBouchot, QString postId, QObject *parent) :
	QObject(parent)
{
	setCacheMode(ItemCoordinateCache);
	m_srcBouchot = srcBouchot;
	m_postId = postId;

	m_animation = new QSequentialAnimationGroup(this);
	connect(m_animation, SIGNAL(finished()), this, SLOT(animate()));

	m_speedVec(X_VALUE) = 1.0;
	m_speedVec(Y_VALUE) = 0.0;

	m_animPixmapNum = 1;
	m_animPixmapNumInc = true;

	m_animPixmapTimerEnabled = true;
	m_animPixmapTimer.setInterval(200);
	m_animPixmapTimer.setSingleShot(false);
	connect(&m_animPixmapTimer, SIGNAL(timeout()), this, SLOT(animateMove()));
}

QQDuckPixmapItem::~QQDuckPixmapItem()
{
	qDebug() << "delete";
}

#define SPEED_FACTOR 200
#define MAX_DURATION 3000
#define MIN_DURATION 1000
#if !defined(M_PI_6)
#define M_PI_6 (M_PI_2 / 3)
#endif

void QQDuckPixmapItem::animate()
{
	m_animation->clear();
	m_listPixmapProp.clear();
	int timeMs = QuteTools::randInt(MIN_DURATION, MAX_DURATION);

	//Recuperation des bornes
	QRectF sceneRect = scene()->sceneRect();
	qreal maxX = sceneRect.width() - boundingRect().width();
	qreal maxY = sceneRect.height() - boundingRect().height();

	//Et du point initial
	QPointF curPos = pos();

	//Calcul du nouveau vecteur vitesse
	qreal angle = (((qreal) qrand()) / INT_MAX) * M_PI_2;
	angle -= M_PI_4;

	QMatrix2x2 rotMatrix;
	rotMatrix(0, 0) = qCos(angle);
	rotMatrix(0, 1) = qSin(angle);
	rotMatrix(1, 0) = 0.0 - rotMatrix(0,1);
	rotMatrix(1, 1) = rotMatrix(0,0);

	m_speedVec = rotMatrix * m_speedVec;

	//Decoupage du temps d'animation en tranche selon les rencontres avec les bords.
	while(timeMs > 0)
	{
		QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "pos");
		pAnimation->setStartValue(curPos);
		connect(pAnimation, SIGNAL(finished()), this, SLOT(loadNextPixMap()));

		qreal angle = qAcos(m_speedVec(X_VALUE)); // 0 <= angle <= Pi
		QQPixmapProp pixmapProp;
		if((0 <= angle) && (angle < M_PI_6))
			pixmapProp.m_pixmapName = "duck_hor/duck_hor_%1.png";
		else if((M_PI_6 <= angle) && (angle < 2 * M_PI_6))
		{
			pixmapProp.m_pixmapName = "duck_diag/duck_diag_%1.png";
			if(m_speedVec(Y_VALUE) > 0)
				pixmapProp.m_rotationAngle = M_PI_2;
		}
		else if((2 * M_PI_6 <= angle) && (angle < 4 * M_PI_6))
		{
			pixmapProp.m_pixmapName = "duck_vert/duck_vert_%1.png";
			if(m_speedVec(Y_VALUE) > 0)
				pixmapProp.m_mirrorY = true;
		}
		else if((4 * M_PI_6 <= angle) && (angle < 5 * M_PI_6))
		{
			pixmapProp.m_pixmapName = "duck_diag/duck_diag_%1.png";
			pixmapProp.m_mirrorX = true;
			if(m_speedVec(Y_VALUE) > 0)
				pixmapProp.m_rotationAngle = M_PI_2;
		}
		else
		{
			pixmapProp.m_pixmapName = "duck_hor/duck_hor_%1.png";
			pixmapProp.m_mirrorX = true;
		}
		m_listPixmapProp.append(pixmapProp);

		QQMatrix1x2 resSpeedVec = m_speedVec * ((qreal) SPEED_FACTOR)
											 * (timeMs / (qreal) (MAX_DURATION - MIN_DURATION));
		qreal destPosX = curPos.x() + resSpeedVec(X_VALUE);
		qreal destPosY = curPos.y() + resSpeedVec(Y_VALUE);

		qreal xFactor = 1;
		if(destPosX < 0)
			xFactor = (0.0 - curPos.x() / (destPosX - curPos.x()));
		else if(destPosX > maxX)
			xFactor = (maxX - curPos.x()) / (destPosX - curPos.x());

		qreal yFactor = 1;
		if(destPosY < 0)
			yFactor = (0.0 - curPos.y() / (destPosY - curPos.y()));
		else if(destPosY > maxY)
			yFactor = (maxY - curPos.y()) / (destPosY - curPos.y());

		//qDebug() << "xFactor" << xFactor << "yFactor" << yFactor;
		//Collision de bord detectee
		if(xFactor < 1 || yFactor < 1)
		{
			qreal realtime;
			if(xFactor <= yFactor) // on touche gauche/droite avant haut/bas
			{
				realtime = timeMs * xFactor;
				curPos = QPointF(curPos.x() + (destPosX - curPos.x()) * xFactor,
								 curPos.y() + (destPosY - curPos.y()) * xFactor);

				m_speedVec(X_VALUE) = 0.0 - m_speedVec(X_VALUE);
				if(xFactor == yFactor)
					m_speedVec(Y_VALUE) = 0.0 - m_speedVec(Y_VALUE);
			}
			else if(xFactor > yFactor) // on touche haut/bas avant gauche/droite
			{
				realtime = timeMs * yFactor;
				curPos = QPointF(curPos.x() + (destPosX - curPos.x()) * yFactor,
								 curPos.y() + (destPosY - curPos.y()) * yFactor);

				m_speedVec(Y_VALUE) = 0.0 - m_speedVec(Y_VALUE);
			}

			pAnimation->setDuration(realtime);
			timeMs -= realtime;
			pAnimation->setEndValue(curPos);
		}
		else
		{
			pAnimation->setDuration(timeMs);
			timeMs = 0;
			pAnimation->setEndValue(QPointF(destPosX, destPosY));
		}
		m_animation->addAnimation(pAnimation);
	}
	loadNextPixMap();
	m_animation->start(QAbstractAnimation::KeepWhenStopped);
}

#define FALL_SPEED_FACTOR 5
void QQDuckPixmapItem::animateKill()
{
	m_animation->disconnect();
	m_animation->clear();
	m_listPixmapProp.clear();
	m_animPixmapTimerEnabled = false;
	m_animPixmapTimer.stop();

	QPointF curPos = pos();
	qreal maxY = scene()->sceneRect().height() - boundingRect().height();

	QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "pos");
	pAnimation->setStartValue(curPos);
	connect(pAnimation, SIGNAL(finished()), this, SLOT(loadNextPixMap()));
	pAnimation->setDuration(1000);
	pAnimation->setEndValue(curPos);
	m_animation->addAnimation(pAnimation);

	QQPixmapProp pixmapProp;
	pixmapProp.m_pixmapName = "duck_kill/duck_kill_%1.png";
	m_listPixmapProp.append(pixmapProp);
	m_animPixmapNum = 1;
	m_animPixmapNumInc = true;


	pAnimation = new QPropertyAnimation(this, "pos");
	pAnimation->setStartValue(curPos);
	connect(pAnimation, SIGNAL(finished()), this, SLOT(deleteLater()));
	QPointF endPos = curPos;
	endPos.setY(maxY);
	pAnimation->setDuration((maxY - curPos.y()) * FALL_SPEED_FACTOR);
	pAnimation->setEndValue(endPos);
	m_animation->addAnimation(pAnimation);

	m_listPixmapProp.append(pixmapProp);

	loadNextPixMap();
	m_animation->start(QAbstractAnimation::KeepWhenStopped);

}

void QQDuckPixmapItem::loadNextPixMap()
{
	if(! m_listPixmapProp.isEmpty())
	{
		m_animPixmapTimer.stop();
		QQPixmapProp pixmapProp = m_listPixmapProp.at(0);
		m_listPixmapProp.remove(0);
		m_currPixmapName = BASE_DUCK_PIX.append(pixmapProp.m_pixmapName);

		QPixmap p(m_currPixmapName.arg(m_animPixmapNum));
		nextMoveStep();

		int scaleX = pixmapProp.m_mirrorX ? -1 : 1;
		int scaleY = pixmapProp.m_mirrorY ? -1 : 1;
		setTransformOriginPoint(p.width() / 2, p.height() / 2);
		setTransform(QTransform().scale(scaleX, scaleY).rotateRadians(pixmapProp.m_rotationAngle, Qt::ZAxis));
		setPixmap(p);
		if(m_animPixmapTimerEnabled)
			m_animPixmapTimer.start();
	}
}

void QQDuckPixmapItem::animateMove()
{
	setPixmap(QPixmap(m_currPixmapName.arg(m_animPixmapNum)));
	nextMoveStep();
}

void QQDuckPixmapItem::nextMoveStep()
{
	if(m_animPixmapNumInc)
	{
		m_animPixmapNum++;
		if(m_animPixmapNum == 3)
			m_animPixmapNumInc = false;
	}
	else
	{
		m_animPixmapNum--;
		if(m_animPixmapNum == 1)
			m_animPixmapNumInc = true;
	}
}

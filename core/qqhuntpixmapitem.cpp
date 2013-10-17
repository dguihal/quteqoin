#include "qqhuntpixmapitem.h"

#include "core/qutetools.h"

#include <qmath.h>

#include <QGraphicsScene>
#include <QPixmap>
#include <QPropertyAnimation>

#define BASE_ANIM_PIX QString(":/anims/")

QQHuntPixmapItem::QQHuntPixmapItem(QString srcBouchot, QString postId, QObject *parent) :
	QObject(parent)
{
	setCacheMode(ItemCoordinateCache);
	m_srcBouchot = srcBouchot;
	m_postId = postId;

	m_animation = new QSequentialAnimationGroup(this);
	connect(m_animation, SIGNAL(finished()), this, SLOT(animate()));

	m_speedVec(X_VALUE) = 1.0;
	m_speedVec(Y_VALUE) = 0.0;

	m_maxAnimPixmapNumInc = 1;
	m_minAnimPixmapNumInc = 1;
	m_animPixmapNum = 1;
	m_animPixmapNumInc = true;

	m_animPixmapTimerEnabled = true;
	m_animPixmapTimer.setInterval(200);
	m_animPixmapTimer.setSingleShot(false);
	connect(&m_animPixmapTimer, SIGNAL(timeout()), this, SLOT(animateMove()));
}

QQHuntPixmapItem::~QQHuntPixmapItem()
{
	delete m_animation;
}

#define SPEED_FACTOR 200
#define MAX_DURATION 3000
#define MIN_DURATION 1000

void QQHuntPixmapItem::animate()
{
	m_animation->clear();
	m_listPixmapProp.clear();
	int timeMs = QuteTools::randInt(MIN_DURATION, MAX_DURATION);

	//Recuperation des bornes
	QRectF sceneRect = scene()->sceneRect();
	float maxX = sceneRect.width() - boundingRect().width();
	float maxY = sceneRect.height() - boundingRect().height();

	//Et du point initial
	QPointF curPos = pos();

	//Calcul du nouveau vecteur vitesse
	float angle = (((float) qrand()) / INT_MAX) * M_PI_2;
	angle -= M_PI_4;

	QQMatrix2x2 rotMatrix;
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

		float angle = qAcos(m_speedVec(X_VALUE)); // 0 <= angle <= Pi
		QQPixmapProp pixmapProp = animPixmapProp(angle);
		m_listPixmapProp.append(pixmapProp);

		QQMatrix1x2 resSpeedVec = m_speedVec * ((float) SPEED_FACTOR)
											 * (timeMs / (float) (MAX_DURATION - MIN_DURATION));
		float destPosX = curPos.x() + resSpeedVec(X_VALUE);
		float destPosY = curPos.y() + resSpeedVec(Y_VALUE);

		float xFactor = 1;
		if(destPosX < 0)
			xFactor = (0.0 - curPos.x() / (destPosX - curPos.x()));
		else if(destPosX > maxX)
			xFactor = (maxX - curPos.x()) / (destPosX - curPos.x());

		float yFactor = 1;
		if(destPosY < 0)
			yFactor = (0.0 - curPos.y() / (destPosY - curPos.y()));
		else if(destPosY > maxY)
			yFactor = (maxY - curPos.y()) / (destPosY - curPos.y());

		//qDebug() << "xFactor" << xFactor << "yFactor" << yFactor;
		//Collision de bord detectee
		if(xFactor < 1 || yFactor < 1)
		{
			float realtime = 0;
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
void QQHuntPixmapItem::animateKill()
{
	m_animation->disconnect();
	m_animation->clear();
	m_listPixmapProp.clear();
	m_animPixmapTimerEnabled = false;
	m_animPixmapTimer.stop();

	QPointF curPos = pos();
	float maxY = scene()->sceneRect().height() - boundingRect().height();

	QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "pos");
	pAnimation->setStartValue(curPos);
	connect(pAnimation, SIGNAL(finished()), this, SLOT(loadNextPixMap()));
	pAnimation->setDuration(1000);
	pAnimation->setEndValue(curPos);
	m_animation->addAnimation(pAnimation);

	m_listPixmapProp.append(killPixmapProp());
	m_animPixmapNumInc = false;

	pAnimation = new QPropertyAnimation(this, "pos");
	pAnimation->setStartValue(curPos);
	connect(pAnimation, SIGNAL(finished()), this, SLOT(deleteLater()));
	QPointF endPos = curPos;
	endPos.setY(maxY);
	pAnimation->setDuration((maxY - curPos.y()) * FALL_SPEED_FACTOR);
	pAnimation->setEndValue(endPos);
	m_animation->addAnimation(pAnimation);

	m_listPixmapProp.append(fallPixmapProp());

	loadNextPixMap();
	m_animation->start(QAbstractAnimation::KeepWhenStopped);
}


void QQHuntPixmapItem::animateMove()
{
	if(m_currPixmapName.contains("%1"))
	{
		setPixmap(QPixmap(m_currPixmapName.arg(m_animPixmapNum)));
		nextMoveStep();
	}
	else
		setPixmap(QPixmap(m_currPixmapName));
}

void QQHuntPixmapItem::loadNextPixMap()
{
	if(! m_listPixmapProp.isEmpty())
	{
		m_animPixmapTimer.stop();
		QQPixmapProp pixmapProp = m_listPixmapProp.at(0);
		m_listPixmapProp.remove(0);
		m_currPixmapName = BASE_ANIM_PIX.append(pixmapProp.m_pixmapName);

		animateMove();
		QPixmap p = pixmap();

		int scaleX = pixmapProp.m_mirrorX ? -1 : 1;
		int scaleY = pixmapProp.m_mirrorY ? -1 : 1;
		setTransformOriginPoint(p.width() / 2, p.height() / 2);
		setTransform(QTransform().scale(scaleX, scaleY).rotateRadians(pixmapProp.m_rotationAngle, Qt::ZAxis));
		setPixmap(p);
		if(m_animPixmapTimerEnabled)
			m_animPixmapTimer.start();
	}
}

void QQHuntPixmapItem::nextMoveStep()
{
	if(m_animPixmapNumInc)
	{
		m_animPixmapNum++;
		if(m_animPixmapNum == m_maxAnimPixmapNumInc)
			m_animPixmapNumInc = false;
	}
	else
	{
		m_animPixmapNum--;
		if(m_animPixmapNum == m_minAnimPixmapNumInc)
			m_animPixmapNumInc = true;
	}
}

#include "qqduck.h"

QQDuck::QQDuck(QObject *parent) :
	QObject(parent)
{
	m_pixmapItem = NULL;
}

QQDuck::QQDuck(QString tribuneName, QString postId, QObject *parent) :
	QObject(parent)
{
	m_tribuneName = tribuneName;
	m_postId = postId;
	m_pixmapItem = NULL;
}

QQDuck::~QQDuck()
{
	if(m_pixmapItem != NULL)
		delete m_pixmapItem;
}

void QQDuck::kill()
{
	if(m_pixmapItem != NULL)
	{
		m_pixmapItem->animateKill(); //S'autodetruira a la fin
		m_pixmapItem = NULL;
	}
}

void QQDuck::launch()
{
	if(m_pixmapItem != NULL)
		m_pixmapItem->animate();
}

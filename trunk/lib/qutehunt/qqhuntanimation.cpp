#include "qqhuntanimation.h"

#include <QtDebug>
#include <QDirIterator>

#define QQHUNTANIMATION_PREFIX "anims"

QQHuntAnimation::QQHuntAnimation()
{
}

QStringList QQHuntAnimation::huntAnimations()
{
	QStringList anims;
	QDirIterator it(":/" QQHUNTANIMATION_PREFIX "/", QDirIterator::NoIteratorFlags);

	while (it.hasNext())
	{
		it.next();
		QString anim = it.fileName();
		anims.append(anim);
	}

	return QStringList();
}

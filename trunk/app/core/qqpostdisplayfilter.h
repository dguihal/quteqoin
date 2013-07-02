#ifndef QQPOSTDISPLAYFILTER_H
#define QQPOSTDISPLAYFILTER_H

#include "core/qqpost.h"

class QQPostDisplayFilter
{
public:
	QQPostDisplayFilter();

	virtual bool filter(const QQPost *post) = 0;
};

#endif // QQPOSTDISPLAYFILTER_H

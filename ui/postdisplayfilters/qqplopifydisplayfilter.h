#ifndef QQPLOPIFYDISPLAYFILTER_H
#define QQPLOPIFYDISPLAYFILTER_H

#include "ui/qqpostdisplayfilter.h"

class QQPlopifyDisplayFilter : public QQPostDisplayFilter
{
public:
	QQPlopifyDisplayFilter() : QQPostDisplayFilter() {}
	virtual ~QQPlopifyDisplayFilter() {}

	virtual bool filterMatch(const QQPost *post);
};

#endif // QQPLOPIFYDISPLAYFILTER_H

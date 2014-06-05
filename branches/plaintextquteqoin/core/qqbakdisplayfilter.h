#ifndef QQBAKDISPLAYFILTER_H
#define QQBAKDISPLAYFILTER_H

#include "core/qqpostdisplayfilter.h"

class QQBakDisplayFilter : public QQPostDisplayFilter
{
public:
	QQBakDisplayFilter() : QQPostDisplayFilter() {}
	virtual ~QQBakDisplayFilter() {}

	virtual bool filterMatch(const QQPost *post);
};

#endif // QQBAKDISPLAYFILTER_H

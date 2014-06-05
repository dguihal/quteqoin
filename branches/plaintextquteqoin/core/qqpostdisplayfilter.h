#ifndef QQPOSTDISPLAYFILTER_H
#define QQPOSTDISPLAYFILTER_H

class QQPost;

class QQPostDisplayFilter
{
public:
	QQPostDisplayFilter() {}
	virtual ~QQPostDisplayFilter() {}

	virtual bool filterMatch(const QQPost *post) = 0;
};

#endif // QQPOSTDISPLAYFILTER_H

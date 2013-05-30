#ifndef QQMESSAGETRANSFORMFILTER_H
#define QQMESSAGETRANSFORMFILTER_H

#include <QString>

class QQPost;

class QQMessageTransformFilter
{
public:
	explicit QQMessageTransformFilter();

	virtual void transformMessage(const QQPost *post, QString &message) = 0;

};

#endif // QQMESSAGETRANSFORMFILTER_H

#ifndef QQMESSAGETRANSFORMFILTER_H
#define QQMESSAGETRANSFORMFILTER_H

#include <QString>

class QQMessageTransformFilter
{
public:
	explicit QQMessageTransformFilter();

	virtual void transformMessage(const QString &bouchot, QString &message) = 0;

};

#endif // QQMESSAGETRANSFORMFILTER_H

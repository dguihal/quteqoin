#ifndef QQSIMPLEPOSTDISPLAYFILTER_H
#define QQSIMPLEPOSTDISPLAYFILTER_H

#include "core/qqpostdisplayfilter.h"

#include <QList>
#include <QString>

class QQSimplePostDisplayFilter : public QQPostDisplayFilter
{
public:
	QQSimplePostDisplayFilter();

	class Rule
	{
	public:
		enum Type { EQUALS, CONTAINS, MATCHES };
		enum Field { ID, NORLOGE, LOGIN, UA, LOGIN_UA, MESSAGE, BOUCHOT, GROUP };

		bool validate(QQPost *post);

		Type type;
		Field field;
		QString value;

	private:
		QString getPostValue(QQPost *post);
	};

	virtual bool filter(const QQPost *post);

	addRule(const QQSimplePostDisplayFilter::Rule &newRule);

private:
	QList<Rule> m_rules;
};

#endif // QQSIMPLEPOSTDISPLAYFILTER_H

#include "qqsimplepostdisplayfilter.h"

#include "core/qqbouchot.h"

#include <QRegExp>

//////////////////////////////////////////////////////////////
/// \brief QQSimplePostDisplayFilter::QQSimplePostDisplayFilter
///
QQSimplePostDisplayFilter::QQSimplePostDisplayFilter()
{
}

//////////////////////////////////////////////////////////////
/// \brief QQSimplePostDisplayFilter::filter
/// \param post
/// \return true if post match filter
///
bool QQSimplePostDisplayFilter::filter(const QQPost *post)
{
	bool rep = true;
	for(int i = 0; i < m_rules.size() && rep; i++)
		rep = m_rules.at(i).validate(post);

	return rep;
}

//////////////////////////////////////////////////////////////
/// \brief QQSimplePostDisplayFilter::addRule
/// \param newRule
///
void QQSimplePostDisplayFilter::addRule(const QQSimplePostDisplayFilter::Rule &newRule)
{
	m_rules.append(newRule);
}

//////////////////////////////////////////////////////////////
/// \brief QQSimplePostDisplayFilter::Rule::validate
/// \param post
/// \return true if the rule is valid for this post
///
bool QQSimplePostDisplayFilter::Rule::validate(QQPost *post)
{
	QString postValue = getPostValue(post);
	switch(type)
	{
	case EQUALS:
		return postValue == value;
	case CONTAINS:
		return postValue.contains(value);
	case MATCHES:
	{
		QRegExp reg(value);
		if(reg.isValid())
			return postValue.contains(reg);
		else
			return false;
	}
	}
	return false;
}

//////////////////////////////////////////////////////////////
/// \brief QQSimplePostDisplayFilter::Rule::getPostValue
/// \param post
/// \return
///
QString QQSimplePostDisplayFilter::Rule::getPostValue(QQPost *post)
{
	QString rep;
	switch(field)
	{
	case ID:
		rep = post->id();
		break;
	case NORLOGE:
		rep = post->norlogeComplete();
		break;
	case LOGIN:
	case LOGIN_UA:
		rep = post->login();
		if(rep.length() > 0 || field == LOGIN)
			break;
	case UA:
		rep = post->UA();
		break;
	case MESSAGE:
		rep = post->message();
		break;
	case BOUCHOT:
		rep = post->bouchot()->name();
		break;
	case GROUP:
		rep = post->bouchot()->settings().group();
		break;
	default:
		rep.clear();
	}
	return rep;
}

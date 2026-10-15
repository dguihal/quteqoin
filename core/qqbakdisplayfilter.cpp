#include "qqbakdisplayfilter.h"

#include "qqbouchot.h"
#include "qqpost.h"

//////////////////////////////////////////////////////////////
/// \brief QQBakDisplayFilter::filterMatch
/// \param post
/// \return false if post match filter, true otherwise
///
bool QQBakDisplayFilter::filterMatch(const QQPost *post)
{
	if(post == nullptr)
		return false;

	QQBouchot *b = post->bouchot();
	QString login = post->login();
	if(login.size() > 0)
		return b->isBaked(login, true);

	return b->isBaked(post->UA(), false);
}

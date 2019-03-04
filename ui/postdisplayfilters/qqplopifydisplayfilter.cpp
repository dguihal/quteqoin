#include "qqplopifydisplayfilter.h"

#include "core/qqbouchot.h"
#include "core/qqpost.h"

//////////////////////////////////////////////////////////////
/// \brief QQplopifyDisplayFilter::filterMatch
/// \param post
/// \return false if post match filter, true otherwise
///
bool QQPlopifyDisplayFilter::filterMatch(const QQPost *post)
{
	if(post == nullptr)
		return false;

	auto b = post->bouchot();
	auto login = post->login();
	return login.size() > 0 ? b->isPlopified(login, true) :
	                          b->isPlopified(post->UA(), false);
}

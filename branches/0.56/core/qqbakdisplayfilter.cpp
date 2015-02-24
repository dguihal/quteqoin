#include "qqbakdisplayfilter.h"

#include "core/qqbouchot.h"
#include "core/qqpost.h"

//////////////////////////////////////////////////////////////
/// \brief QQBakDisplayFilter::filterMatch
/// \param post
/// \return false if post match filter, true otherwise
///
bool QQBakDisplayFilter::filterMatch(const QQPost *post)
{
	if(post == NULL)
		return false;

	QQBouchot *b = post->bouchot();
	QString login = post->login();
	if(login.size() > 0)
		return b->isBaked(login, true);
	else
		return b->isBaked(post->UA(), false);
}

#include "qutetools.h"

#include <QWidget>

QuteTools::QuteTools()
{
}

//////////////////////////////////////////////////////////////
/// \brief checkFocusRecurse
/// \param parent
/// \return
///
bool QuteTools::checkFocusRecurse(QWidget *parent)
{
	bool focus = parent->hasFocus();
	if(! focus)
	{
		foreach(QObject *child, parent->children())
		{
			if(child->isWidgetType())
			{
				if((focus = QuteTools::checkFocusRecurse((QWidget *)child)) == true)
					break;
			}
		}
	}
	return focus;
}

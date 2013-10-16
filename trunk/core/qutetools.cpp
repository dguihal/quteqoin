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

//////////////////////////////////////////////////////////////
/// \brief QuteTools::randInt
/// \param low
/// \param high
/// \return Random number between low and high
///
int QuteTools::randInt(int low, int high)
{
	return qrand() % ((high + 1) - low) + low;
}

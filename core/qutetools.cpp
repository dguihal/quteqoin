#include "qutetools.h"

#include <QWidget>

QuteTools::QuteTools()
= default;

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
				focus = QuteTools::checkFocusRecurse(dynamic_cast<QWidget *>(child));
				if(focus)
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

//////////////////////////////////////////////////////////////
/// \brief QuteTools::statusStringFromState
/// \param s
/// \return
///
QString QuteTools::statusStringFromState(QuteQoin::QQBoardStates s)
{
	QString flags;
	if(s.hasResponse)
		flags.append(QString::fromUtf8("\u2709")); //'ENVELOPE' (U+2709) -> '✉';
	else if(s.hasNewPosts)
		flags.append(QString::fromUtf8("*"));

	if(s.hasBigorno)
		flags.append(QString::fromUtf8("\u260F")); //'WHITE TELEPHONE' (U+26A0) -> '☏';

	if(s.hasError)
		flags.append(QString::fromUtf8("\u26A0")); //'WARNING SIGN' (U+26A0) -> '⚠';

	if(s.isHidden)
		flags.append(QString::fromUtf8("\xf0\x9f\x99\x88")); //'SEE-NO-EVIL MONKEY' (U+1F648) -> '🙈';
	return flags;
}

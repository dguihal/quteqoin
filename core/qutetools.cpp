#include "qutetools.h"

#include <QObject>
#include <QWidget>
#include <QRandomGenerator>

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
		for (QObject *child : parent->children())
		{
			if(child->isWidgetType())
			{
				focus = QuteTools::checkFocusRecurse(qobject_cast<QWidget *>(child));
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
	return QRandomGenerator::global()->bounded(low, high);
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
		flags.append(QStringLiteral(u"\u2709")); //'ENVELOPE' (U+2709) -> '✉';
	else if(s.hasNewPosts)
		flags.append(QStringLiteral("*"));

	if(s.hasBigorno)
		flags.append(QStringLiteral(u"\u260F")); //'WHITE TELEPHONE' (U+26A0) -> '☏';

	if(s.hasError)
		flags.append(QStringLiteral(u"\u26A0")); //'WARNING SIGN' (U+26A0) -> '⚠';

	if(s.isHidden)
		flags.append(QStringLiteral(u"\xf0\x9f\x99\x88")); //'SEE-NO-EVIL MONKEY' (U+1F648) -> '🙈';
	return flags;
}

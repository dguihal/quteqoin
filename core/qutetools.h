#ifndef QUTETOOLS_H
#define QUTETOOLS_H

#include "core/qqtypes.h"

#include <QString>

class QWidget;

class QuteTools
{

public:
	QuteTools();

	static bool checkFocusRecurse(QWidget *parent);
	static int randInt(int low, int high);
	static QString statusStringFromState(QuteQoin::QQBoardStates s);
};

#endif // QUTETOOLS_H

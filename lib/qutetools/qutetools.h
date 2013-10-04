#ifndef QUTETOOLS_H
#define QUTETOOLS_H

class QWidget;

class QuteTools
{

public:
	QuteTools();

	static bool checkFocusRecurse(QWidget *parent);
	static int randInt(int low, int high);
};

#endif // QUTETOOLS_H

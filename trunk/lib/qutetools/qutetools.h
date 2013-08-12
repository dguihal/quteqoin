#ifndef QUTETOOLS_H
#define QUTETOOLS_H

class QWidget;

class QuteTools
{

public:
	QuteTools();

	static bool checkFocusRecurse(QWidget *parent);
};

#endif // QUTETOOLS_H

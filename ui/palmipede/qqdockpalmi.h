#ifndef QQDOCKPALMI_H
#define QQDOCKPALMI_H

#include <QDockWidget>

class QQDockPalmi : public QDockWidget
{
public:
	QQDockPalmi(QWidget *parent = 0);
	~QQDockPalmi();

	void takePalmiWidget(QWidget *p);
	void releasePalmiWidget();

public slots:
	virtual void setVisible(bool visible);

private:
	bool m_wasVisible;

	QWidget *m_centralW;
	QWidget *m_palmiW;
};

#endif // QQDOCKPALMI_H

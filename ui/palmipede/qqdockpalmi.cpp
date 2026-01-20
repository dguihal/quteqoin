#include "qqdockpalmi.h"

#include <QVBoxLayout>

#define DOCKPALMI_OBJECT_NAME "QQDockPalmipede"
#define DOCKPALMI_WINDOW_NAME "Palmipede Editor"

QQDockPalmi::QQDockPalmi(QWidget *parent) :
	QDockWidget(parent),
	m_wasVisible(true),
	m_centralW(new QWidget(this)),
	m_palmiW(nullptr)
{
	setObjectName(DOCKPALMI_WINDOW_NAME);
	setWindowTitle(tr(DOCKPALMI_WINDOW_NAME));

	setFeatures(QDockWidget::DockWidgetClosable |
				QDockWidget::DockWidgetMovable |
				QDockWidget::DockWidgetFloatable);

	setAllowedAreas(Qt::LeftDockWidgetArea |
					Qt::RightDockWidgetArea);

}

QQDockPalmi::~QQDockPalmi()
{

}

void QQDockPalmi::takePalmiWidget(QWidget *p)
{
	if(p == nullptr)
		return;

	setWidget(nullptr);

	setWidget(p);
	p->show();
	adjustSize();
}

void QQDockPalmi::releasePalmiWidget()
{
	setWidget(nullptr);
}


void QQDockPalmi::setVisible(bool visible)
{
	QDockWidget::setVisible(visible);
	m_wasVisible = visible;
}

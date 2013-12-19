#include "qqmusselinfo.h"

#include <QtDebug>

QQMusselInfo::QQMusselInfo(QQMussel mussel, QWidget *parent) :
	QLabel(parent),
	m_mussel(mussel),
	m_selected(false)
{
	setText(m_mussel.name());
	updateStyleSheet();
}

void QQMusselInfo::mousePressEvent(QMouseEvent *event)
{
	QLabel::mousePressEvent(event);
	if(! selected())
		setSelected();
	else
		setUnselected();

	updateStyleSheet();
}


void QQMusselInfo::mouseReleaseEvent(QMouseEvent *event)
{
	QLabel::mouseReleaseEvent(event);
}

void QQMusselInfo::updateStyleSheet()
{
	QString style("QLabel {%1}");

	if(selected())
	{
		QPalette curPalette = palette();
		style = style.arg(
					QString("border: 2px solid; border-color: %1; border-radius: 4px; padding: 0px; %2")
					.arg(curPalette.text().color().name()));
	}
	else
		style = style.arg("padding: 2px; %1");


	if(m_mussel.isAuth())
		style = style.arg("");
	else
		style = style.arg("font-style: italic;");

	qDebug() << style;
	setStyleSheet(style);
}

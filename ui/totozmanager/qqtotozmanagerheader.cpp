#include "qqtotozmanagerheader.h"
#include "ui_qqtotozmanagerheader.h"

QQtotozManagerHeader::QQtotozManagerHeader(QWidget *parent) : QWidget(parent),
	m_isCollapsed(false)
{
	updateBtnIcon();
}


void QQtotozManagerHeader::setLabel(QString lbl)
{
	m_ui->label->setText(lbl);
}

void QQtotozManagerHeader::collapse(bool collapsed)
{
	m_isCollapsed = collapsed;

	updateBtnIcon();
}

void QQtotozManagerHeader::updateBtnIcon()
{
	if(m_isCollapsed)
		m_ui->toolButton->setArrowType(Qt::RightArrow);
	else
		m_ui->toolButton->setArrowType(Qt::DownArrow);
}

#ifndef QQTOTOZMANAGERHEADER_H
#define QQTOTOZMANAGERHEADER_H

#include <QWidget>


namespace Ui {
class QQtotozManagerHeader;
}

class QQtotozManagerHeader : public QWidget
{
	Q_OBJECT
public:
	explicit QQtotozManagerHeader(QWidget *parent = 0);

	void setLabel(QString lbl);
signals:

public slots:
	void collapse(bool collapsed);

private:
	void updateBtnIcon();
	Ui::QQtotozManagerHeader *m_ui;

	bool m_isCollapsed;
};

#endif // QQTOTOZMANAGERHEADER_H

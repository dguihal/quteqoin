#ifndef QQPALMISETTINGS_H
#define QQPALMISETTINGS_H

#include <QList>
#include <QWidget>

class QTableWidgetItem;

namespace Ui {
class QQPalmiSettings;
}

class QQPalmiSettings : public QWidget
{
	Q_OBJECT

public:
	explicit QQPalmiSettings(QWidget *parent = 0);
	~QQPalmiSettings();

	void setStaticShortcuts(QList<QPair<QChar, QString> > list);
	void setUserShortcuts(QList<QPair<QChar, QString> > list);
	QList<QPair<QChar, QString> > getUserShortcuts();

protected slots:

private:
	Ui::QQPalmiSettings *ui;
};

#endif // QQPALMISETTINGS_H

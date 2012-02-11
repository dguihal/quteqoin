#ifndef QQBOUCHOTSETTINGSDIALOG_H
#define QQBOUCHOTSETTINGSDIALOG_H

#include <QDialog>

class QQBouchot;

namespace Ui {
    class QQBouchotSettingsDialog;
}

class QQBouchotSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QQBouchotSettingsDialog(QWidget *parent = 0);
	~QQBouchotSettingsDialog();

	QQBouchot * getConfiguredBouchot();

public slots:
	void okPressed();
	void cancelPressed();
	void usePresetPressed();

private:
    Ui::QQBouchotSettingsDialog *ui;
	QQBouchot *newBouchot;
};

#endif // QQBOUCHOTSETTINGSDIALOG_H

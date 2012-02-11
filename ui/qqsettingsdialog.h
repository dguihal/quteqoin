#ifndef QQSETTINGSDIALOG_H
#define QQSETTINGSDIALOG_H

#include <QDialog>
#include <QList>

class QQBouchot;
class QUrl;

namespace Ui {
    class QQSettingsDialog;
}

class QQSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QQSettingsDialog(QWidget *parent = 0);
    ~QQSettingsDialog();

	void setMaxHistoryLength(uint);
	uint maxHistoryLength();

	void setDefaultUA(const QString&);
	QString defaultUA();

	void setTotozServerUrl(const QUrl&);
	QUrl totozServerUrl();

	void setTotozMode(int);
	int totozMode();

	void setDefaultLogin(const QString&);
	QString defaultLogin();

	void setListBouchots(const QList<QQBouchot *>&);
	QList<QQBouchot *> listBouchots();
	QList<QQBouchot *> newBouchots();
	QList<QQBouchot *> oldBouchots();

public slots:
	void addBouchot();
	void deleteBouchot();

private:
    Ui::QQSettingsDialog *ui;
	QList<QQBouchot *> m_listBouchots;
};

#endif // QQSETTINGSDIALOG_H

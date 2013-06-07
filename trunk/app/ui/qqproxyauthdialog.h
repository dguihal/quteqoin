#ifndef QQPROXYAUTHDIALOG_H
#define QQPROXYAUTHDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class QQProxyAuthDialog;
}

class QQProxyAuthDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit QQProxyAuthDialog(QWidget *parent = 0);
	~QQProxyAuthDialog();

	void setLogin(QString & login);
	QString login();
	void setPasswd(QString & passwd);
	QString passwd();
	
private:
	Ui::QQProxyAuthDialog *ui;
};

#endif // QQPROXYAUTHDIALOG_H

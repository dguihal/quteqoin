#include "qqproxyauthdialog.h"
#include "ui_qqproxyauthdialog.h"

QQProxyAuthDialog::QQProxyAuthDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQProxyAuthDialog)
{
	ui->setupUi(this);
	setModal(true);
}

QQProxyAuthDialog::~QQProxyAuthDialog()
{
	delete ui;
}

void QQProxyAuthDialog::setLogin(QString & login)
{
	return ui->loginLineEdit->setText(login);

}

QString QQProxyAuthDialog::login()
{
	return ui->loginLineEdit->text();
}

void QQProxyAuthDialog::setPasswd(QString & passwd)
{
	return ui->passwdLineEdit->setText(passwd);
}

QString QQProxyAuthDialog::passwd()
{
	return ui->passwdLineEdit->text();
}

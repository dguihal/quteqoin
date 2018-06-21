#include "qqproxyauthdialog.h"
#include "ui_qqproxyauthdialog.h"

#include <QApplication>
#include <QStyle>

QQProxyAuthDialog::QQProxyAuthDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QQProxyAuthDialog)
{
	ui->setupUi(this);
	ui->iconLabel->setText(QString());
	ui->iconLabel->setPixmap(QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, parent).pixmap(32, 32));
	setModal(true);
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(ui->passwordLineEdit, SIGNAL(returnPressed()), this, SLOT(accept()));
}

QQProxyAuthDialog::~QQProxyAuthDialog()
{
	delete ui;
}

void QQProxyAuthDialog::setLogin(QString & login)
{
	return ui->userNameLineEdit->setText(login);

}

QString QQProxyAuthDialog::login()
{
	return ui->userNameLineEdit->text();
}

void QQProxyAuthDialog::setPasswd(QString & passwd)
{
	return ui->passwordLineEdit->setText(passwd);
}

QString QQProxyAuthDialog::passwd()
{
	return ui->passwordLineEdit->text();
}

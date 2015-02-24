#ifndef QQNETWORKSETTINGS_H
#define QQNETWORKSETTINGS_H

#include <QWidget>

namespace Ui {
class QQNetworkSettings;
}

class QQNetworkSettings : public QWidget
{
	Q_OBJECT

public:
	explicit QQNetworkSettings(QWidget *parent = 0);
	~QQNetworkSettings();

	enum Mode {DIRECT, SYSTEM, MANUAL};
	void setNetworkSettingsMode(const Mode &mode);
	Mode networkSettingsMode() const;

	void setProxySettings(const QString &host, const QString &port);
	QString proxySettingsHost() const;
	QString proxySettingsPort() const;

	void setProxyIsHTTP(const bool &proxyIsHttp);
	bool proxyIsHTTP() const;

protected slots:
	void onNetworkModeChanged();
	void onProxyPortValuechanged(QString text);

private:
	Ui::QQNetworkSettings *ui;
};

#endif // QQNETWORKSETTINGS_H

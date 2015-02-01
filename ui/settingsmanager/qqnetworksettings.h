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

private:
	Ui::QQNetworkSettings *ui;
};

#endif // QQNETWORKSETTINGS_H

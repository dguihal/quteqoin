#ifndef QQGENERALSETTINGS_H
#define QQGENERALSETTINGS_H

#include <QWidget>

namespace Ui {
class QQGeneralSettings;
}

class QQGeneralSettings : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString m_maxHistorySize READ maxHistorySize WRITE setMaxHistorySize)
	Q_PROPERTY(QString m_defaultUA READ defaultUA WRITE setDefaultUA)
	Q_PROPERTY(QString m_defaultLogin READ defaultLogin WRITE setDefaultLogin)

public:
	explicit QQGeneralSettings(QWidget *parent = 0);
	~QQGeneralSettings();

	void setMaxHistorySize(const QString &maxSize);
	QString maxHistorySize();

	void setDefaultLogin(const QString &defaultLogin);
	QString defaultLogin();

	void setDefaultUA(const QString &defaultUA);
	QString defaultUA();

protected slots:
	void fontChanged(const QFont &font);
	void maxHistorySizeChanged(const QString &);
	void defaultLoginChanged(const QString &);
	void defaultUAChanged(const QString &);

private:
	Ui::QQGeneralSettings *ui;

	QString m_maxHistorySize;
	QString m_defaultLogin;
	QString m_defaultUA;
};

#endif // QQGENERALSETTINGS_H

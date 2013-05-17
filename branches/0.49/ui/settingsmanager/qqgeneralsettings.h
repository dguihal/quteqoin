#ifndef QQGENERALSETTINGS_H
#define QQGENERALSETTINGS_H

#include <QWidget>

namespace Ui {
class QQGeneralSettings;
}

class QQGeneralSettings : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QFont m_defaultFont READ defaultFont WRITE setDefaultFont)
	Q_PROPERTY(QString m_maxHistorySize READ maxHistorySize WRITE setMaxHistorySize)
	Q_PROPERTY(QString m_defaultUA READ defaultUA WRITE setDefaultUA)
	Q_PROPERTY(QString m_defaultLogin READ defaultLogin WRITE setDefaultLogin)
	Q_PROPERTY(QString m_defaultwebSearchUrl READ defaultLogin WRITE setDefaultLogin)

public:
	explicit QQGeneralSettings(QWidget *parent = 0);
	~QQGeneralSettings();

	void setDefaultFont(const QFont &font);
	QFont defaultFont();

	void setMaxHistorySize(const QString &maxSize);
	QString maxHistorySize();

	void setDefaultLogin(const QString &defaultLogin);
	QString defaultLogin();

	void setDefaultUA(const QString &defaultUA);
	QString defaultUA();

	void setDefaultWebSearchUrl(const QString &defaultWebSearchUrl);
	QString defaultWebSearchUrl();

protected slots:
	void fontChanged(const QFont &font);
	void fontSizeChanged(int index);
	void maxHistorySizeChanged(const QString &maxHistorySize);
	void defaultLoginChanged(const QString &defaultLogin);
	void defaultUAChanged(const QString &defaultUA);
	void defaultWebSearchUrl(const QString &defaultWebSearchUrl);

private:
	Ui::QQGeneralSettings *ui;

	QFont m_defaultFont;
	QString m_maxHistorySize;
	QString m_defaultLogin;
	QString m_defaultUA;
	QString m_defaultwebSearchUrl;


};

#endif // QQGENERALSETTINGS_H

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

	void setEnableStealthMode(bool isEnabled);
	bool isStealthModeEnabled();

	void setHightLightColor(const QString &hColor);
	QString highlightColor();

protected slots:
	void fontChanged(const QFont &font);
	void fontSizeChanged(int index);
	void showColorDialog();

private:
	Ui::QQGeneralSettings *ui;

	QFont m_defaultFont;
	QString m_color;
};

#endif // QQGENERALSETTINGS_H

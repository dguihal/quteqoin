#ifndef QQTOTOZSETTINGS_H
#define QQTOTOZSETTINGS_H

#include <QWidget>

namespace Ui {
class QQTotozSettings;
}

class QQTotozSettings : public QWidget
{
	Q_OBJECT

public:
	explicit QQTotozSettings(QWidget *parent = 0);
	~QQTotozSettings();

	void setTotozServerURL(const QString &totozServerURL);
	QString totozServerURL();

	void setTotozBaseImgUrl(const QString &totozBaseImgUrl);
	QString totozBaseImgUrl();

	void setTotozNameSuffix(const QString &totozNameSuffix);
	QString totozNameSuffix();

	void setTotozAllowSearch(const bool &totozAllowSearch);
	bool totozAllowSearch();

	void setTotozQueryPattern(const QString &totozQueryPattern);
	QString totozQueryPattern();

	void setTotozVisualModes(const QStringList &totozVisualModes);
	void setTotozVisualMode(const QString &totozVisualMode);
	QString totozVisualMode();

protected slots:
	//void TotozPresetChanged(QFont &font);
	void totozServerURLChanged(const QString &);
	void totozBaseImgUrlChanged(const QString &);
	void totozNameSuffixChanged(const QString &);
	void totozAllowSearchChanged(int);
	void totozQueryPatternChanged(const QString &);
	void totozVisualModeChanged(const QString &);
	void loadTotozSrvPreset(int index);

private:
	Ui::QQTotozSettings *ui;

	QString m_totozServerURL;
	QString m_totozBaseImgUrl;
	QString m_totozNameSuffix;
	bool m_totozAllowSearch;
	QString m_totozQueryPattern;

	QString m_totozVisualMode;
};

#endif // QQTOTOZSETTINGS_H

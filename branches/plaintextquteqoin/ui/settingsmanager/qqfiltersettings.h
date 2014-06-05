#ifndef QQFILTERSETTINGS_H
#define QQFILTERSETTINGS_H

#include "core/qqtypes.h"

#include <QWidget>

namespace Ui {
class QQFilterSettings;
}

class QQFilterSettings : public QWidget
{
	Q_OBJECT

public:
	explicit QQFilterSettings(QWidget *parent = 0);
	~QQFilterSettings();

	bool isSmartUrlEnabled();

	void setSmartUrlTransformerType(QuteQoin::QQSmartUrlFilerTransformType transformType);
	QuteQoin::QQSmartUrlFilerTransformType smartUrlTransformerType() const { return m_smartUrlTransformType; }

public slots:
	void setSmartUrlEnabled(bool checked = false);

protected slots:
	void smartUrlMethodChanged();

private:
	Ui::QQFilterSettings *ui;

	QuteQoin::QQSmartUrlFilerTransformType m_smartUrlTransformType;
};


#endif // QQFILTERSETTINGS_H

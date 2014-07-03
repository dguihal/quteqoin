#ifndef QQBOARDWIZARDNATIVE_H
#define QQBOARDWIZARDNATIVE_H

#include <QStringList>
#include <QWizardPage>

class QCheckBox;
class QComboBox;
class QLabel;

class QQBoardWizardNative : public QWizardPage
{
	Q_OBJECT
public:
	explicit QQBoardWizardNative(QWidget *parent = 0);

	void setListGroups(QStringList &listGroups);

	virtual int	nextId () const;
signals:

public slots:

protected slots:
	void advSettingsTriggered();
	void nameSelectionChanged(int index);

private:

	QCheckBox *m_advSettingsCB;
	QComboBox *m_boardSelectorCB;
	QComboBox *m_groupSelCB;
	QLabel *m_emptyListLabel;
};

#endif // QQBOARDWIZARDNATIVE_H

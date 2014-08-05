#ifndef QQBOARDWIZARDNATIVE_H
#define QQBOARDWIZARDNATIVE_H

#include <QStringList>
#include <QWizardPage>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;

class QQBoardWizardNative : public QWizardPage
{
	Q_OBJECT
public:
	explicit QQBoardWizardNative(QWidget *parent = 0);

	void setListGroups(QStringList &listGroups);

	virtual int	nextId () const;
	virtual bool isComplete() const;
signals:

public slots:

protected slots:
	void colorChanged();
	void nameSelectionChanged(int index);

private:

	QCheckBox *m_advSettingsCB;
	QComboBox *m_boardSelectorCB;
	QComboBox *m_groupSelCB;
	QLineEdit *m_colorLE;
	QLabel *m_emptyListLabel;
};

#endif // QQBOARDWIZARDNATIVE_H

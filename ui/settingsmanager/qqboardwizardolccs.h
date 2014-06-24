#ifndef OLCCSWIZARDP1_H
#define OLCCSWIZARDP1_H

#include "core/qqnetworkaccessor.h"

#include <QStringList>
#include <QWizardPage>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;

class QQOlccsWizardPMainNetAccessor : public QQNetworkAccessor
{
	Q_OBJECT

public:
	explicit QQOlccsWizardPMainNetAccessor(QObject *parent = 0);

	QStringList boardList();
	void updateBoardList();

	signals:
		void boardListAvailable();
		void error(QString errMsg);

	protected slots:
		void requestFinishedSlot(QNetworkReply *reply);

private:
		QStringList m_boardList;
};

class QQBoardWizardOlccs : public QWizardPage
{
	Q_OBJECT
public:
	explicit QQBoardWizardOlccs(QWidget *parent = 0);

	void setListGroups(QStringList &listGroups);

	virtual int	nextId () const;

signals:

public slots:
	void boardListAvailable();

protected slots:
	void advSettingsTriggered();
	void colorChanged();
	void nameSelectionChanged(int index);

private:
	QQOlccsWizardPMainNetAccessor *m_netAccessor;

	QCheckBox *m_advSettingsCB;
	QComboBox *m_boardSelectorCB;
	QComboBox *m_groupSelCB;
	QLabel *m_waitLabel;
	QLabel *m_emptyListLabel;
	QLineEdit *m_colorLE;

};



#endif // OLCCSWIZARDP1_H

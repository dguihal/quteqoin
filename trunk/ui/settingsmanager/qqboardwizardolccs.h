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
	virtual bool isComplete() const;

signals:

public slots:
	void boardListAvailable();

protected slots:
	void colorChanged();
	void nameSelectionChanged(int index);

private:
	QQOlccsWizardPMainNetAccessor *m_netAccessor;

	QCheckBox *m_advSettingsCB;
	QComboBox *m_boardSelectorCB;
	QLineEdit *m_colorLE;
	QLineEdit *m_cookieLE;
	QComboBox *m_groupSelCB;
	QLabel *m_waitLabel;
	QLabel *m_emptyListLabel;

};



#endif // OLCCSWIZARDP1_H

#ifndef QQBOARDWIZARDINTRO_H
#define QQBOARDWIZARDINTRO_H

#include <QWizardPage>

class QRadioButton;

class QQBoardWizardIntro : public QWizardPage
{
	Q_OBJECT
public:
	explicit QQBoardWizardIntro(QWidget *parent = 0);

	virtual int	nextId () const;

signals:

public slots:

private:
	QRadioButton *m_modeNativeRB;
	QRadioButton *m_modeOlccsRB;
};

#endif // QQBOARDWIZARDINTRO_H

#ifndef QQPALMIPEDE_H
#define QQPALMIPEDE_H

#include <QDockWidget>
#include <QString>

class QColor;
class QShortcut;

namespace Ui {
class QQPalmipede;
}

class QQPalmipede : public QDockWidget
{
	Q_OBJECT

public:
	explicit QQPalmipede(QWidget *parent = 0);
	~QQPalmipede();

	//QString getCurrentBouchot();

	void insertText(const QString &);
	void addBouchot(const QString & newBouchot, const QColor & newBouchotColor);
	void removeBouchot(const QString & oldBouchot);

	void setMinimal(bool minimal);

signals:
	void postMessage(const QString & bouchot, const QString & message);

public slots:
	void changeBoard(bool next);
	void insertReplaceText(const QString &bouchot, const QString &tag);
	void insertReplaceText(const QString &tag);
	virtual void setVisible(bool visible);

protected:
	virtual void focusInEvent(QFocusEvent * event);

private:
	void changeNorloges(const QString & bouchot);

	Ui::QQPalmipede *m_ui;

	QString m_oldBouchot;

	bool m_minimal;
	bool m_wasVisible;

	int palmiTextSelectionStart;
	QString palmiSelectedText;

	QString m_windowTitle;

private slots:
	void blamPafActivated(const QString & text);
	void bouchotSelectorActivated(int index);
	void postPushButtonClicked();
};

#endif // QQPALMIPEDE_H

#ifndef QQPINISEARCHWIDGET_H
#define QQPINISEARCHWIDGET_H

#include <QWidget>

namespace Ui {
class QQPiniSearchWidget;
}

class QQPiniSearchWidget : public QWidget
{
	Q_OBJECT

public:
	explicit QQPiniSearchWidget(QWidget *parent = 0);
	~QQPiniSearchWidget();

public slots:
	virtual void setVisible(bool visible);

signals:
	void search(const QString &text, bool forward);

protected:
	void focusInEvent(QFocusEvent *event);
	void keyPressEvent(QKeyEvent *event);

protected slots:
	void searchTextChanged(const QString &text);
	void searchTextNextFwd();
	void searchTextNextBwd();

private:
	Ui::QQPiniSearchWidget *ui;
};

#endif // PINISEARCHWIDGET_H

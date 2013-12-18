#ifndef QQMUSSELINFO_H
#define QQMUSSELINFO_H

#include "core/qqmussel.h"

#include <QLabel>

class QQMusselInfo : public QLabel
{
    Q_OBJECT
public:
	explicit QQMusselInfo(QQMussel mussel, QWidget *parent = 0);

    bool selected() { return m_selected; }
	void setSelected() { m_selected = true; emit selected(m_mussel); }
	void setUnselected() { m_selected = false; }

signals:
	void selected(QQMussel mussel);

public slots:

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
	void updateStyleSheet();
	QQMussel m_mussel;

	bool m_selected;
};

#endif // QQMUSSELINFO_H

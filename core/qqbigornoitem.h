#ifndef QQBIGORNOITEM_H
#define QQBIGORNOITEM_H

#include <QString>

class QQBigornoItem
{
public:
	QQBigornoItem(int postition, const QString & word);

	int position() { return m_position; }
	QString word() { return m_word; }
	bool wasNotified() { return m_wasNotified; }
	void setNotified() { m_wasNotified = true; }

private:
	int m_position;
	QString m_word;
	bool m_wasNotified;
};

#endif // QQBIGORNOITEM_H

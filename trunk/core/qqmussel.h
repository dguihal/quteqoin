#ifndef QQMUSSEL_H
#define QQMUSSEL_H

#include <QString>

class QQMussel
{
public:
	explicit QQMussel(QString name, QString board, bool isAuth = true);

	QString name() const { return m_name; }
	QString board() const { return m_board; }
	bool isAuth() const { return m_isAuth; }

	QString fullName() const;
	bool equals(const QQMussel mussel) const;
	bool operator== (const QQMussel mussel) const { return this->equals(mussel); }

private:
	QString m_name;
	QString m_board;
	bool m_isAuth;
};

#endif // QQMUSSEL_H

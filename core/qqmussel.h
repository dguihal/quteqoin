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
	bool isMe() const;

	void bak(bool isBaked);
	bool isBaked() const { return m_isBaked; }

	void plopify(bool isPlopified);
	bool isPlopified() const { return m_isPlopified; }

	QString fullName() const;
	bool equals(const QQMussel mussel) const;
	bool operator== (const QQMussel mussel) const { return this->equals(mussel); }
	bool operator!= (const QQMussel mussel) const { return ! this->equals(mussel); }

	uint hash() const;

private:
	QString m_name;
	QString m_board;
	bool m_isAuth;
	bool m_isBaked;
	bool m_isPlopified;
};

inline uint qHash(const QQMussel &c) {
	   return (c.hash());
}
#endif // QQMUSSEL_H

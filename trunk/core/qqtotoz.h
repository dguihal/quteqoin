#ifndef QQTOTOZ_H
#define QQTOTOZ_H

#include <QImage>
#include <QMovie>
#include <QObject>
#include <QSharedPointer>
#include <QString>

class QQTotoz
{
public:
	enum TypeTotoz {
		QQTOTOZ_NOTFOUND,
		QQTOTOZ_UNDEF,
		QQTOTOZ_STATIC,
		QQTOTOZ_ANIMATED
	};

	explicit QQTotoz();
	QQTotoz(const QString & totozId, int posInMessage);
	QQTotoz(const QQTotoz & totoz);
	~QQTotoz();

	bool isValid() { return m_typeTotoz != QQTotoz::QQTOTOZ_NOTFOUND; }

	QString getOrigString() { return (QString::fromAscii("[:")).append(m_id).append(QString::fromAscii("]")); }
	QString getId() { return m_id; }
	int getPosInMessage() { return m_posInMessage; }
	void setPosInMessage(const int posInMessage) { m_posInMessage = posInMessage; }
	QQTotoz::TypeTotoz getType() { return m_typeTotoz; }

	QSharedPointer<QImage> getStaticData() { return m_totozStaticImgPtr; }
	void setStaticData(QSharedPointer<QImage> staticImage) { m_totozStaticImgPtr = staticImage; m_typeTotoz = QQTOTOZ_STATIC; }
	QSharedPointer<QMovie> getDynData() { return m_totozDynImgPtr; }
	void setDynData(QSharedPointer<QMovie> dynImage) { m_totozDynImgPtr = dynImage; m_typeTotoz = QQTOTOZ_ANIMATED; }

signals:

public slots:

private:
	QString m_id;
	TypeTotoz m_typeTotoz;
	QSharedPointer<QImage> m_totozStaticImgPtr;
	QSharedPointer<QMovie> m_totozDynImgPtr;
	int m_posInMessage;

};

#endif // QQTOTOZ_H

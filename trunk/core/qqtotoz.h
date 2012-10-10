#ifndef QQTOTOZ_H
#define QQTOTOZ_H

#include <QDesktopServices>
#include <QFile>
#include <QObject>
#include <QSharedPointer>
#include <QString>

class QQTotoz
{
public:
	explicit QQTotoz();
	QQTotoz(const QString & totozId, int posInMessage);
	QQTotoz(const QQTotoz & totoz);
	~QQTotoz();

	QString getOrigString() { return (QString::fromAscii("[:")).append(m_id).append(QString::fromAscii("]")); }
	QString getId() { return m_id; }
	int getPosInMessage() { return m_posInMessage; }
	void setPosInMessage(const int posInMessage) { m_posInMessage = posInMessage; }

	bool isValid() { return m_id.length() > 0 && QFile::exists(this->getPath()); }

	QString getPath() { return QQTotoz::getPath(m_id); }

	static QString getPath(QString id) { return QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
				.append(id)
				.append(QString::fromAscii(".gif"));
					  }

signals:

public slots:

private:
	QString m_id;
	int m_posInMessage;

};

#endif // QQTOTOZ_H

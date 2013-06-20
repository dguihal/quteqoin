#ifndef QQTOTOZ_H
#define QQTOTOZ_H

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QString>


class QQTotoz
{
public:
	explicit QQTotoz();
	QQTotoz(const QString & totozId);
	QQTotoz(const QQTotoz & totoz);
	~QQTotoz();

	QString getOrigString() const { return (QString::fromLatin1("[:")).append(m_id).append(QString::fromLatin1("]")); }
	QString getId() { return m_id; }

	bool isValid() { return m_id.length() > 0 && cacheExists(); }
	bool cacheExists();
	QString cacheDirPath() { return getPath(m_id); }

	QByteArray data() { return m_totozData; }
	void setData(QByteArray array) { m_totozData = array; m_wasmodfied = true; }
	QStringList getTags() { return m_tags; }
	bool isNSFW() { return m_isNSFW; }
	void setCacheExpireDate(const QDateTime &cacheExpireDate) { m_cacheExpireDate = cacheExpireDate; }
	bool isCacheExpired();

	void save();

	enum TotozBookmarkAction { ADD, REMOVE };

	static bool cacheExists(QString id) { return QFile::exists(getPath(id)); }
	static void invalidateCache(QString id) { QFile::remove(getPath(id)); }

signals:

public slots:

private:
	void load();

	static QString getPath(QString id);

	QString m_id;

	QByteArray m_totozData;
	QStringList m_tags;
	bool m_isNSFW;
	QDateTime m_cacheExpireDate;

	bool m_wasmodfied;
};

#endif // QQTOTOZ_H

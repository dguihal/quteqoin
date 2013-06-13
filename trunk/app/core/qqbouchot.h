#ifndef QQBOUCHOT_H
#define QQBOUCHOT_H

#include "core/qqnetworkaccessor.h"
#include "core/qqpost.h"

#include <QColor>
#include <QHash>
#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QUrl>

class QNetworkReply;
class QQXmlParser;
class QQSettings;


class QQBouchot : public QQNetworkAccessor
{
	Q_OBJECT

public:
	enum TypeSlip { SlipTagsEncoded = 0, SlipTagsRaw = 1 };
	enum TypeRequest { UnknownRequest = 0, BackendRequest = 1, PostRequest = 2 };

	class QQBouchotSettings
	{

	public:
		QStringList aliases() const { return m_aliases; }
		void setAliases(const QStringList &newAliases) { m_aliases = newAliases; }
		QString aliasesToString() const { return this->m_aliases.join(QChar::fromAscii(Separator)); }
		void setAliasesFromString(const QString &newAliasesString) { m_aliases = newAliasesString.split(QChar::fromAscii(Separator)); }
		bool containsAlias(const QString & alias) { return m_aliases.contains(alias); }

		QString backendUrl() const { return m_backendUrl; }
		void setBackendUrl(const QString &newBackendUrl) { m_backendUrl = newBackendUrl; }

		QColor color() const { return m_color; }
		QColor colorLight() const { return m_color.lighter(110); }
		void setColor(const QColor &newColor) { m_color = newColor; }
		QString colorToString() const { return m_color.name(); }
		void setColorFromString(const QString &newColorString) { m_color.setNamedColor(newColorString); }

		QString cookie() const { return m_cookie; }
		void setCookie(const QString &newCookie) { m_cookie = newCookie; }

		QString login() const { return m_login; }
		void setLogin(const QString &newLogin) { m_login = newLogin; }

		QString postData() const { return m_postData; }
		void setPostData(const QString &newPostData) { m_postData = newPostData; }

		QString postUrl() const { return m_postUrl; }
		void setPostUrl(const QString &newPostUrl) { m_postUrl = newPostUrl; }

		int refresh() const { return m_refresh; }
		void setRefresh(int newRefresh) { m_refresh = newRefresh; }
		QString refreshToString() const { return QString::number(this->m_refresh); }
		void setRefreshFromString(const QString &newRefreshString) { m_refresh = newRefreshString.toInt(); }

		QQBouchot::TypeSlip slipType() const { return m_slipType; }
		void setSlipType(QQBouchot::TypeSlip newSlipType) { m_slipType = newSlipType; }

		QString ua() const { return m_ua; }
		void setUa(const QString &newUA) { m_ua = newUA; }

		QString group() const { return m_group; }
		void setGroup(const QString & newGroup) { m_group = newGroup; }

	private:
		QStringList m_aliases;
		QString m_backendUrl;
		QColor m_color;
		QString m_cookie;
		QString m_login;
		QString m_postData;
		QString m_postUrl;
		int m_refresh;
		TypeSlip m_slipType;
		QString m_ua;
		QString m_group;

		static const char Separator = ',';
	};

	QQBouchot(const QString &name, QObject *parent = 0);
	~QQBouchot();

	QString name() { return m_name; }
	//void setName(const QString &newName) { m_name = newName; }

	void parseBackend(const QByteArray &data);
	void postMessage(const QString &message);

	QQBouchotSettings settings() { return m_bSettings; }
	void setSettings(QQBouchot bouchotRef) { setSettings(bouchotRef.settings()); }
	void setSettings(const QQBouchotSettings &newSettings);

	void startRefresh();
	void stopRefresh();

	QList<QQPost *> takeNewPosts();
	QList<QQPost *> getPostsHistory() {	return m_history; }
	void setNewPostsFromHistory();

	void setPiniWidget(QWidget *widget) { m_piniWidget = widget; }

	virtual bool event(QEvent *e);

	static QQBouchotSettings getBouchotDef(const QString &bouchotName);
	static QStringList getBouchotDefNameList();
	static QQBouchot * bouchot(const QString &bouchotName);
	static QList<QQBouchot *> listBouchots();
	static QList<QQBouchot *> listBouchotsGroup(const QString &groupName);

public slots:
	void slotSslErrors(const QList<QSslError> &errors);

signals:
	void removed(QString name, QString groupName);
	void destroyed(QQBouchot *bouchot);
	void groupChanged(QQBouchot *bouchot, QString oldGroupName);

protected slots:
	void fetchBackend();
	void requestFinishedSlot(QNetworkReply *reply);
	void insertNewPost(QQPost &newPost);
	void parsingFinished();

private:
	void checkGroupModified(const QString &oldGroupName);
	void askPiniUpdate();

	QList<QQPost *> m_history;
	bool m_hasXPostId; //false = unknown
	QList<QString> m_xPostIds;
	int m_lastId;
	QString m_name;
	QList<QQPost *> m_newPostHistory;
	QQBouchotSettings m_bSettings;
	QTimer timer;

	QQXmlParser *m_xmlParser;

	QWidget *m_piniWidget;

	static QHash<QString, QQBouchot *> s_hashBouchots;
};

#endif // QQBOUCHOT_H

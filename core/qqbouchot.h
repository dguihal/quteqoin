#ifndef QQBOUCHOT_H
#define QQBOUCHOT_H

#include "qqpost.h"

#include <QColor>
#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

class QQBouchot : public QObject
{
	Q_OBJECT

public:
	enum TypeSlip { SlipTagsEncoded = 0, SlipTagsRaw = 1 };

	QQBouchot();
	QQBouchot(QObject *);
	QQBouchot(const QString &, QObject *);
	/** Clone constructor */
	QQBouchot(QQBouchot *);
	~QQBouchot();

	QStringList aliases();
	void setAliases(const QStringList &);
	QString aliasesToString();
	void setAliasesFromString(const QString &);

	QString backendUrl();
	void setBackendUrl(const QString &);

	QColor color();
	void setColor(const QColor &);
	QString colorToString();
	void setColorFromString(const QString &);

	QString cookie();
	void setCookie(const QString &);

	QString login();
	void setLogin(const QString &);

	QString name();
	void setName(const QString &);

	QString postData();
	void setPostData(const QString &);

	QString postUrl();
	void setPostUrl(const QString &);

	int refresh();
	void setRefresh(int);
	QString refreshToString();
	void setRefreshFromString(const QString &);

	QQBouchot::TypeSlip slipType();
	void setSlipType(QQBouchot::TypeSlip);

	QString ua();
	void setUa(const QString &);

	QString group();
	void setGroup(const QString &);

	void startRefresh();
	void stopRefresh();

	QList<QQPost *> getNewPosts();

	static const char Separator = ';';
	static QQBouchot* getBouchotDef(const QString &);
	static QStringList getBouchotDefNameList();

signals:
	void newPostsInserted(QQBouchot *);

protected slots:
	void fetchBackend();
	void replyFinished(QNetworkReply *);
	void insertNewPost(QQPost &);

private:
	void init();

	QString m_name;
	QColor m_color;
	QStringList m_aliases;
	QString m_login;
	QString m_cookie;
	QString m_backendUrl;
	QString m_postUrl;
	QString m_postData;
	int m_refresh;
	TypeSlip m_slipType;
	QString m_ua;

	QString m_group;

	QTimer timer;
	QNetworkAccessManager *m_netManager;
	QList<QQPost *> m_history;
	QList<QQPost *> m_newPostHistory;
	int m_lastId;
};

/*
var see = new Board('see', false);
see.getUrl = 'http://tout.essaye.sauf.ca/tribune.xml';
see.postUrl = 'http://tout.essaye.sauf.ca/tribune/post';
see.color = '#ffd0d0';
see.alias = "schee,seeschloss";
GlobalBoards['see'] = see;

var moules = new Board('moules', false);
moules.getUrl = 'http://moules.org/board/backend';
moules.postUrl = 'http://moules.org/board/add';
moules.color = '#ffe3c9';
GlobalBoards['moules'] = moules;

var bouchot = new Board('bouchot', false);
bouchot.getUrl = 'http://bouchot.org/tribune/remote?last=%i';
bouchot.postUrl = 'http://bouchot.org/tribune/post_coincoin';
bouchot.postData = "missive=%m";
bouchot.color = '#e9e9e9';
GlobalBoards['bouchot'] = bouchot;

var finss = new Board('finss', false);
finss.getUrl = 'http://finss.free.fr/drupal/?q=tribune.xml';
finss.postUrl = 'http://finss.free.fr/drupal/?q=tribune/post';
finss.color = '#d0ffd0';
GlobalBoards['finss'] = finss;

var shoop = new Board('shoop', false);
shoop.getUrl = 'http://dax.sveetch.net/tribune/remote.xml'; // ?last=%i inopÃ©rant pour le moment
shoop.postUrl = 'http://dax.sveetch.net/tribune/post.xml';
shoop.postData = "content=%m";
shoop.alias = "sveetch,dax";
shoop.color = '#EDEDDB';
GlobalBoards['shoop'] = shoop;

var tif = new Board('tifauv', false);
tif.getUrl = 'http://tribune.tifauv.homeip.net/tribune/remote?last=%i';
tif.postUrl = 'http://tribune.tifauv.homeip.net/tribune/post_coincoin';
tif.postData = "missive=%m";
tif.alias = "tif";
tif.color = '#a9f9b9';
GlobalBoards['tifauv'] = tif;

var olo = new Board('olo', false);
olo.getUrl = 'http://board.olivierl.org/remote.xml';
olo.postUrl = 'http://board.olivierl.org/add.php';
olo.color = '#80dafc';
olo.alias = "olivierl";
olo.slip = SLIP_TAGS_RAW;
GlobalBoards['olo'] = olo;

var ygllo = new Board('ygllo', false);
ygllo.getUrl = 'http://ygllo.com/?q=tribune.xml';
ygllo.postUrl = 'http://ygllo.com/?q=tribune/post';
ygllo.color = '#eee887';
ygllo.alias = "yg,llo,fdg";
GlobalBoards['ygllo'] = ygllo;

var kad = new Board('kadreg', false);
kad.getUrl = 'http://kadreg.org/board/backend.php';
kad.postUrl = 'http://kadreg.org/board/add.php';
kad.color = '#dae6e6';
kad.alias = "kad,rincevent";
kad.slip = SLIP_TAGS_RAW;
GlobalBoards['kadreg'] = kad;

var dae = new Board('darkside', false);
dae.getUrl = 'http://quadaemon.free.fr/remote.xml';
dae.postUrl = 'http://quadaemon.free.fr/add.php';
dae.color = '#daedae';
dae.alias = "dae,daemon";
dae.slip = SLIP_TAGS_RAW; // Protection temporaire
GlobalBoards['darkside'] = dae;

var axel = new Board('hadoken', false);
axel.getUrl = 'http://hadoken.free.fr/board/remote.php';
axel.postUrl = 'http://hadoken.free.fr/board/post.php';
axel.color = '#77AADD';
axel.alias = "axel,waf";
GlobalBoards['hadoken'] = axel;

var lo = new Board('comptoir', false);
lo.getUrl = 'http://lordoric.free.fr/daBoard/remote.xml';
lo.postUrl = 'http://lordoric.free.fr/daBoard/add.php';
lo.color = '#dedede';
lo.alias = "lo,lordoric";
lo.slip = SLIP_TAGS_RAW; // Protection temporaire
GlobalBoards['comptoir'] = lo;

var gabu = new Board('gabuzomeu', false);
gabu.getUrl = 'http://gabuzomeu.fr/tribune.xml';
gabu.postUrl = 'http://gabuzomeu.fr/tribune/post';
gabu.color = '#aaffbb';
GlobalBoards['gabuzomeu'] = gabu;
*/
#endif // QQBOUCHOT_H

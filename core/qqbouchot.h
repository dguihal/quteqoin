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

    class QQBouchotSettings {

    public:
        QStringList aliases() { return m_aliases; }
        void setAliases(const QStringList &newAliases) { m_aliases = newAliases; }
        QString aliasesToString() { return this->m_aliases.join(QChar::fromAscii(Separator)); }
        void setAliasesFromString(const QString &newAliasesString) { m_aliases = newAliasesString.split(QChar::fromAscii(Separator)); }

        QString backendUrl() { return m_backendUrl; }
        void setBackendUrl(const QString &newBackendUrl) { m_backendUrl = newBackendUrl; }

        QColor color() { return m_color; }
        QColor colorLight() { return m_color.lighter(110); }
        void setColor(const QColor &newColor) { m_color = newColor; }
        QString colorToString() { return m_color.name(); }
        void setColorFromString(const QString &newColorString) { m_color.setNamedColor(newColorString); }

        QString cookie() { return m_cookie; }
        void setCookie(const QString &newCookie) { m_cookie = newCookie; }

        QString login() { return m_login; }
        void setLogin(const QString &newLogin) { m_login = newLogin; }

        QString postData() { return m_postData; }
        void setPostData(const QString &newPostData) { m_postData = newPostData; }

        QString postUrl() { return m_postUrl; }
        void setPostUrl(const QString &newPostUrl) { m_postUrl = newPostUrl; }

        int refresh() { return m_refresh; }
        void setRefresh(int newRefresh) { m_refresh = newRefresh; }
        QString refreshToString() { return QString::number(this->m_refresh); }
        void setRefreshFromString(const QString &newRefreshString) { m_refresh = newRefreshString.toInt(); }

        QQBouchot::TypeSlip slipType() { return m_slipType; }
        void setSlipType(QQBouchot::TypeSlip newSlipType) { m_slipType = newSlipType; }

        QString ua() { return m_ua; }
        void setUa(const QString &newUA) { m_ua = newUA; }

        QString group() { return m_group; }
        void setGroup(const QString& newGroup) { m_group = newGroup; }

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
    };

    QQBouchot(const QString& name, QObject* parent = 0);
    ~QQBouchot();

    QString name() { return m_name; }
    //void setName(const QString &newName) { m_name = newName; }

    void postMessage(const QString & message);

    QQBouchotSettings settings() { return m_settings; }
    void setSettings(QQBouchot bouchotRef) { setSettings(bouchotRef.settings()); }
    void setSettings(QQBouchotSettings newSettings) { m_settings = newSettings; }

    void startRefresh();
    void stopRefresh();

    QList<QQPost *> getNewPosts();

    static const char Separator = ';';
    static QQBouchotSettings getBouchotDef(const QString &);
    static QStringList getBouchotDefNameList();

signals:
    void newPostsInserted(QQBouchot *);

protected slots:
    void fetchBackend();
    void replyFinished(QNetworkReply *);
    void insertNewPost(QQPost &);

private:

    QList<QQPost *> m_history;
    int m_lastId;
    QString m_name;
    QNetworkAccessManager *m_netManager;
    QList<QQPost *> m_newPostHistory;
    QQBouchotSettings m_settings;
    QTimer timer;
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

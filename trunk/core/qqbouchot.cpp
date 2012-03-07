#include "qqbouchot.h"

#include "xml/qqxmlparser.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

int defaultRefreshRate = 30;
typedef struct bouchotDef
{
    char name[16];
    char getUrl[64];
    char postUrl[64];
    char postData[64];
    char color[16];
    char alias[64];
    char cookieProto[64];
    QQBouchot::TypeSlip typeSlip;
} bouchotDefStruct;

//Définition des bouchots préconfigurés
// tiré d'olcc by Chrisix
int bouchotsDefSize = 4;
bouchotDefStruct bouchotsDef[] =
{
    { "dlfp", "http://linuxfr.org/board/index.xml", "http://linuxfr.org/board", "board[message]=%m",
      "#dac0de", "linuxfr,beyrouth,passite,dapassite", "_linuxfr.org_session=", QQBouchot::SlipTagsEncoded },
    { "batavie", "http://batavie.leguyader.eu/remote.xml", "http://batavie.leguyader.eu/index.php/add", "",
      "#ffccaa", "llg", "", QQBouchot::SlipTagsRaw },
    { "euromussels", "http://euromussels.eu/?q=tribune.xml&last_id=%i", "http://euromussels.eu/?q=tribune/post", "",
      "#d0d0ff", "euro,euroxers", "", QQBouchot::SlipTagsEncoded },
    { "finss", "http://finss.free.fr/drupal/?q=tribune.xml", "http://finss.free.fr/drupal/?q=tribune/post", "",
      "#d0ffd0", "finss", QQBouchot::SlipTagsEncoded }
};

QQBouchot::QQBouchot(const QString& name, QObject* parent) :
    QObject(parent)
{
    m_name = name;
    m_settings.setRefresh(0);
    this->m_netManager = new QNetworkAccessManager(this);
    m_history.clear();
    m_newPostHistory.clear();
    m_lastId=-1;
    connect(m_netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

QQBouchot::~QQBouchot()
{
}

QQBouchot::QQBouchotSettings QQBouchot::settings()
{
    return m_settings;
}

void QQBouchot::setSettings(QQBouchotSettings newSettings)
{
    m_settings = newSettings;
}

void QQBouchot::setSettings(QQBouchot bouchotRef)
{
    setSettings(bouchotRef.settings());
}

void QQBouchot::startRefresh()
{
    qDebug() << "QQBouchot::startRefresh";
    if(m_settings.refresh() <= 0)
        return;

    //Connection du signal
    connect(&timer, SIGNAL(timeout()), this, SLOT(fetchBackend()));

    //Première récuperation
    fetchBackend();
}

void QQBouchot::stopRefresh()
{
    qDebug() << "QQBouchot::stopRefresh";
    timer.disconnect();
    timer.stop();
}

QList<QQPost *> QQBouchot::getNewPosts()
{
    return m_newPostHistory;
}

void QQBouchot::fetchBackend()
{
    QString url = m_settings.backendUrl();
    QString lastId;

    //on bloque le timer
    timer.stop();

    lastId.setNum(m_lastId);
    url.replace(QString::fromAscii("%i"), lastId);

    qDebug() << "QQBouchot::fetchBackend url=" << url;

    QNetworkRequest request(QUrl::fromUserInput(url));

    if(m_settings.ua().isEmpty() == false)
        request.setRawHeader(QString::fromAscii("User-Agent").toAscii(), m_settings.ua().toAscii());

    if(m_settings.cookie().isEmpty() == false)
        request.setRawHeader(QString::fromAscii("Cookie").toAscii(), m_settings.cookie().toAscii());

    m_netManager->get(request);

    timer.setInterval(m_settings.refresh() * 1000);
    timer.start();
}

void QQBouchot::replyFinished(QNetworkReply *reply)
{
    qDebug() << "QQBouchot::replyFinished isFinished=" << reply->isFinished()
                << ", error=" << reply->errorString();

    m_newPostHistory.clear();

    QXmlSimpleReader xmlReader;
    QXmlInputSource xmlSource;
    QQXmlParser xmlParser;
    xmlParser.setLastId(m_lastId);

    connect(&xmlParser, SIGNAL(newPostReady(QQPost&)), this, SLOT(insertNewPost(QQPost&)));
    xmlSource.setData(reply->readAll());
    xmlReader.setContentHandler(&xmlParser);
    xmlReader.setErrorHandler(&xmlParser);
    xmlReader.parse(&xmlSource);

    reply->deleteLater();

    if( m_newPostHistory.size() > 0 )
    {
        qDebug() << "QQBouchot::replyFinished, newPostsInserted emis";
        m_history.append(m_newPostHistory);
        m_lastId = m_history.last()->id().toInt();
        emit newPostsInserted(this);
    }
}

void QQBouchot::insertNewPost(QQPost &newPost)
{
    //qDebug() << "MainWindow::insertNewPost ;; " << newPost.norloge();
    QQPost * tmpNewPost = new QQPost(newPost);
    tmpNewPost->setParent( this );

    m_newPostHistory.prepend( tmpNewPost );

}

/////////////////////////
// Static
/////////////////////////

QQBouchot::QQBouchotSettings QQBouchot::getBouchotDef(const QString &nameBouchot)
{

    QQBouchot::QQBouchotSettings settings;

    int i = 0;
    for(; i < bouchotsDefSize; i++)
        if(QString::compare(nameBouchot, QLatin1String(bouchotsDef[i].name)) == 0)
            break;

    if(i < bouchotsDefSize)
    {
        settings.setAliasesFromString(bouchotsDef[i].alias);
        settings.setBackendUrl(bouchotsDef[i].getUrl);
        settings.setColorFromString(bouchotsDef[i].color);
        settings.setPostData(bouchotsDef[i].postData);
        settings.setPostUrl(bouchotsDef[i].postUrl);
        settings.setRefresh(defaultRefreshRate);
        settings.setSlipType(bouchotsDef[i].typeSlip);
        settings.setCookie(bouchotsDef[i].cookieProto);
    }

    return settings;
}

QStringList QQBouchot::getBouchotDefNameList()
{
    int i = 0;
    QStringList res;
    for(i = 0; i < bouchotsDefSize; i++)
        res.append(QString::fromAscii(bouchotsDef[i].name));

    return res;
}

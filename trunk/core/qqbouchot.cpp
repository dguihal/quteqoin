#include "qqbouchot.h"

#include "core/qqsettings.h"
#include "xml/qqxmlparser.h"

#include <QDateTime>
#include <QDebug>
#include <QNetworkProxyFactory>
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
	{ "batavie", "http://batavie.leguyader.eu/remote.xml", "http://batavie.leguyader.eu/index.php/add", "message=%m",
	  "#ffccaa", "llg", "", QQBouchot::SlipTagsRaw },
	{ "euromussels", "http://euromussels.eu/?q=tribune.xml&last_id=%i", "http://euromussels.eu/?q=tribune/post", "message=%m",
	  "#d0d0ff", "euro,euroxers", "", QQBouchot::SlipTagsEncoded },
	{ "finss", "http://finss.free.fr/drupal/?q=tribune.xml", "http://finss.free.fr/drupal/?q=tribune/post", "message=%m",
	  "#d0ffd0", "finss", "", QQBouchot::SlipTagsEncoded }
};

QQBouchot::QQBouchot(const QString & name, QQSettings * parent) :
	QObject(parent)
{
	m_name = name;
	m_settings.setRefresh(0);
	m_netManager.proxyFactory()->setUseSystemConfiguration(true);
	m_history.clear();
	m_newPostHistory.clear();
	m_lastId=0;
	connect(& m_netManager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(replyFinished(QNetworkReply*)));
	connect(& m_netManager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
			parent, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
}

QQBouchot::~QQBouchot()
{
}

void QQBouchot::postMessage(const QString &message)
{
	QString url = m_settings.postUrl();
	QByteArray postData = m_settings.postData().toAscii();
	QByteArray mark("%m");

	if(postData.contains(mark))
		postData.replace(mark, QUrl::toPercentEncoding(message));
	else
		postData=QUrl::toPercentEncoding(message);

	QNetworkRequest request(QUrl::fromUserInput(url));
	request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

	if(m_settings.ua().isEmpty() == false)
		request.setRawHeader(QString::fromAscii("User-Agent").toAscii(), m_settings.ua().toAscii());
	else
		request.setRawHeader(QString::fromAscii("User-Agent").toAscii(), ((QQSettings *)this->parent())->defaultUA().toAscii());

	if(m_settings.cookie().isEmpty() == false)
		request.setRawHeader(QString::fromAscii("Cookie").toAscii(), m_settings.cookie().toAscii());

	m_netManager.post(request, postData);
}


void QQBouchot::startRefresh()
{
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQBouchot::startRefresh";
	if(m_settings.refresh() <= 0)
		return;

	//Connection du signal
	connect(&timer, SIGNAL(timeout()), this, SLOT(fetchBackend()));

	//Première récuperation
	fetchBackend();
}

void QQBouchot::stopRefresh()
{
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQBouchot::stopRefresh";
	timer.disconnect();
	timer.stop();
}


void QQBouchot::setNewPostsFromHistory()
{
	QQPost * firstNew = m_newPostHistory.first();
	int index = m_history.indexOf(firstNew);
	while (--index >= 0)
		m_newPostHistory.prepend(m_history.at(index));
}

void QQBouchot::fetchBackend()
{
	QString url = m_settings.backendUrl();
	QString lastId;

	//on bloque le timer
	timer.stop();

	lastId.setNum(m_lastId);
	url.replace(QString::fromAscii("%i"), lastId);

	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQBouchot::fetchBackend url=" << url;

	QNetworkRequest request(QUrl::fromUserInput(url));

	if(m_settings.ua().isEmpty() == false)
		request.setRawHeader(QString::fromAscii("User-Agent").toAscii(), m_settings.ua().toAscii());

	if(m_settings.cookie().isEmpty() == false)
		request.setRawHeader(QString::fromAscii("Cookie").toAscii(), m_settings.cookie().toAscii());

	m_netManager.get(request);

	timer.setInterval(m_settings.refresh() * 1000);
	timer.start();
}

void QQBouchot::replyFinished(QNetworkReply *reply)
{
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQBouchot::replyFinished isFinished=" << reply->isFinished();

	if(reply->error() != QNetworkReply::NoError)
	{
		 qWarning() << "QQBouchot::replyFinished, error=" << reply->errorString();
		 return;
	}

	m_newPostHistory.clear();

	QXmlSimpleReader xmlReader;
	QXmlInputSource xmlSource;
	QQXmlParser xmlParser;
	xmlParser.setLastId(m_lastId);
	xmlParser.setTypeSlip(this->m_settings.slipType());

	connect(&xmlParser, SIGNAL(newPostReady(QQPost&)), this, SLOT(insertNewPost(QQPost&)));
	xmlSource.setData(reply->readAll());
	xmlReader.setContentHandler(&xmlParser);
	xmlReader.setErrorHandler(&xmlParser);
	xmlReader.parse(&xmlSource);

	reply->deleteLater();

	if( m_newPostHistory.size() > 0 )
	{
		qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
				 << "QQBouchot::replyFinished, newPostsInserted emis";
		m_history.append(m_newPostHistory);
		m_lastId = m_history.last()->id().toInt();
		emit newPostsInserted(this);
	}
}

void QQBouchot::insertNewPost(QQPost &newPost)
{
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

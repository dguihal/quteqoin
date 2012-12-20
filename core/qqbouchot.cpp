#include "qqbouchot.h"

#include "core/qqsettings.h"
#include "core/qqpurgebouchothistoevent.h"
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
	  "#dac0de", "linuxfr,beyrouth,passite,dapassite", "linuxfr.org_session=", QQBouchot::SlipTagsEncoded },
	{ "batavie", "http://batavie.leguyader.eu/remote.xml", "http://batavie.leguyader.eu/index.php/add", "message=%m",
	  "#ffccaa", "llg", "", QQBouchot::SlipTagsRaw },
	{ "euromussels", "http://euromussels.eu/?q=tribune.xml&last_id=%i", "http://euromussels.eu/?q=tribune/post", "message=%m",
	  "#d0d0ff", "euro,euroxers", "", QQBouchot::SlipTagsEncoded },
	{ "finss", "http://finss.fr/drupal/node/95/xml", "http://finss.fr/drupal/node/95/post", "message=%m",
	  "#d0ffd0", "finss", "", QQBouchot::SlipTagsEncoded }
};

QQBouchot::QQBouchot(const QString & name, QQSettings * settings) :
	QQNetworkAccessor(settings)
{
	m_name = name;
	m_settings.setRefresh(0);
	m_history.clear();
	m_newPostHistory.clear();
	m_lastId=0;
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
	request.setAttribute(QNetworkRequest::User, QQBouchot::PostRequest);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");

	if(m_settings.ua().isEmpty() == false)
		request.setRawHeader("User-Agent", m_settings.ua().toAscii());
	else
		request.setRawHeader("User-Agent", ((QQSettings *)this->parent())->defaultUA().toAscii());

	if(m_settings.cookie().isEmpty() == false)
		request.setRawHeader("Cookie", m_settings.cookie().toAscii());

	request.setRawHeader("Accept", "*/*");
	request.setRawHeader("Accept-Encoding","gzip, defalte");
	request.setRawHeader("Referer", request.url().toString().toAscii());
	httpPost(request, postData);
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

QList<QQPost *> QQBouchot::takeNewPosts()
{
	QList <QQPost *> list(m_newPostHistory);
	m_newPostHistory.clear();
	return list;
}

void QQBouchot::setNewPostsFromHistory()
{
	int index = m_history.length();
	if(! m_newPostHistory.isEmpty())
	{
		QQPost * firstNew = m_newPostHistory.first();
		index = m_history.indexOf(firstNew);
	}
	while (--index >= 0)
		m_newPostHistory.prepend(m_history.at(index));
}

bool QQBouchot::event(QEvent * e)
{
	if(e->type() == QQPurgeBouchotHistoEvent::PURGE_BOUCHOT_HISTO)
	{
		QString maxId = ((QQPurgeBouchotHistoEvent *) e)->maxId();

		// Il faut au moins garder le dernier post dans l'histo
		bool notFound = true;
		while(notFound && m_history.size() > 1)
		{
			if(m_history.at(0)->id() == maxId)
				notFound = false;

			delete m_history.takeAt(0);
		}
	}
	else
		QObject::event(e);

	return true;
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
	request.setAttribute(QNetworkRequest::User, QQBouchot::BackendRequest);
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
						 QNetworkRequest::AlwaysNetwork);

	if(m_settings.ua().isEmpty() == false)
		request.setRawHeader(QString::fromAscii("User-Agent").toAscii(), m_settings.ua().toAscii());

	if(m_settings.cookie().isEmpty() == false)
		request.setRawHeader(QString::fromAscii("Cookie").toAscii(), m_settings.cookie().toAscii());

	httpGet(request);

	timer.setInterval(m_settings.refresh() * 1000);
	timer.start();
}

void QQBouchot::requestFinishedSlot(QNetworkReply *reply)
{
	qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			 << "QQBouchot::requestFinishedSlot isFinished=" << reply->isFinished();

	// Recuperation du Statut HTTP
	//QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	if(reply->error() != QNetworkReply::NoError)
	{
		qWarning() << QDateTime::currentDateTime().currentMSecsSinceEpoch()
				   << " : QQBouchot::requestFinishedSlot, error : " << reply->error()
				   << ", msg : " << reply->errorString();
		stopRefresh();
	}
	else
	{
		switch(reply->request().attribute(QNetworkRequest::User, QQBouchot::UnknownRequest).toInt(0))
		{
		case QQBouchot::PostRequest:
			qDebug() << "QQBouchot::requestFinishedSlot post Request detected, refresh du backend ";
			fetchBackend();
			break;

		case QQBouchot::BackendRequest:
			qDebug() << "QQBouchot::requestFinishedSlot fetch backend detected";
			parseBackend(reply->readAll());
			break;

		default:
			qWarning()  << "QQBouchot::requestFinishedSlot, reply->request().attribute(QNetworkRequest::User).toInt() unknown";
		}
	}
	reply->deleteLater();
}

void QQBouchot::parseBackend(const QByteArray & data)
{
	QXmlSimpleReader xmlReader;
	QXmlInputSource xmlSource;
	QQXmlParser xmlParser;
	xmlParser.setLastId(m_lastId);
	xmlParser.setTypeSlip(this->m_settings.slipType());

	connect(&xmlParser, SIGNAL(newPostReady(QQPost&)), this, SLOT(insertNewPost(QQPost&)));
	xmlSource.setData(data);
	xmlReader.setContentHandler(&xmlParser);
	xmlReader.setErrorHandler(&xmlParser);
	xmlReader.parse(&xmlSource);


	if( m_newPostHistory.size() > 0 )
	{
		qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
				 << "QQBouchot::replyFinished, newPostsInserted emis";
		m_history.append(m_newPostHistory);
		m_lastId = m_history.last()->id().toInt();
		emit newPostsAvailable(m_settings.group());
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

QQBouchot::QQBouchotSettings QQBouchot::getBouchotDef(const QString & nameBouchot)
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


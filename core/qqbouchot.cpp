#include "qqbouchot.h"

#include "xml/qqxmlparser.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

int defaultRefreshRate = 30;
typedef struct
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
int bouchotsDefSize = 3;
bouchotDefStruct bouchotsDef[] =
{
	{ "dlfp", "http://linuxfr.org/board/index.xml", "http://linuxfr.org/board", "board[message]=%m",
	  "#dac0de", "linuxfr,beyrouth,passite,dapassite", "_linuxfr.org_session=", QQBouchot::SlipTagsEncoded },
	{ "batavie", "http://batavie.leguyader.eu/remote.xml", "http://batavie.leguyader.eu/index.php/add", "",
	  "#ffccaa", "llg", "", QQBouchot::SlipTagsRaw },
	{ "euromussels", "http://euromussels.eu/?q=tribune.xml&last_id=%i", "http://euromussels.eu/?q=tribune/post", "",
	  "#d0d0ff", "euro,euroxers", "", QQBouchot::SlipTagsEncoded }
};

QQBouchot::QQBouchot() :
	QObject(NULL)
{
	init();
}

QQBouchot::QQBouchot(QObject* parent) :
	QObject(parent)
{
	init();
}

QQBouchot::QQBouchot(const QString& name, QObject* parent) :
	QObject(parent)
{
	this->m_name = name;
	init();
}

QQBouchot::QQBouchot(QQBouchot * original)
{
	this->setAliases(original->aliases());
	this->setBackendUrl(original->backendUrl());
	this->setColor(original->color());
	this->setCookie(original->cookie());
	this->setLogin(original->login());
	this->setName(original->name());
	this->setParent(original->parent());
	this->setPostData(original->postData());
	this->setPostUrl(original->postUrl());
	this->setRefresh(original->refresh());
	this->setSlipType(original->slipType());
	this->setUa(original->ua());
	init();
}

void QQBouchot::init()
{
	this->m_refresh = 0;
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

QStringList QQBouchot::aliases()
{
	return this->m_aliases;
}

void QQBouchot::setAliases(const QStringList& aliases)
{
	qDebug() << "QQBouchot::setAliases = " << aliases.join(QString::fromAscii(";"));
	this->m_aliases = aliases;
}

QString QQBouchot::aliasesToString()
{
	return this->m_aliases.join(QChar::fromAscii(Separator));
}

void QQBouchot::setAliasesFromString(const QString& string)
{
	qDebug() << "QQBouchot::setAliasesFromString = " << string;
	this->m_aliases = string.split(QChar::fromAscii(Separator));
}

QString QQBouchot::backendUrl()
{
	return this->m_backendUrl;
}

void QQBouchot::setBackendUrl(const QString& url)
{
	qDebug() << "QQBouchot::setBackendUrl = " << url;
	this->m_backendUrl = url;
}

QColor QQBouchot::color()
{
	return this->m_color;
}

void QQBouchot::setColor(const QColor& color)
{
	qDebug() << "QQBouchot::setColor = " << color;
	this->m_color = color;
}

QString QQBouchot::colorToString()
{
	return this->m_color.name();
}

void QQBouchot::setColorFromString(const QString& namedColor)
{
	qDebug() << "QQBouchot::setColorFromString = " << namedColor;
	this->m_color.setNamedColor(namedColor);
}

QString QQBouchot::cookie()
{
	return this->m_cookie;
}

void QQBouchot::setCookie(const QString& cookie)
{
	qDebug() << "QQBouchot::setCookie = " << cookie;
	this->m_cookie = cookie;
}

QString QQBouchot::name()
{
	return this->m_name;
}

void QQBouchot::setName(const QString& name)
{
	qDebug() << "QQBouchot::setName = " << name;
	this->m_name = name;
}

QString QQBouchot::login()
{
	return this->m_login;
}

void QQBouchot::setLogin(const QString& login)
{
	qDebug() << "QQBouchot::setLogin = " << login;
	this->m_login = login;
}

QString QQBouchot::postData()
{
	return this->m_postData;
}

void QQBouchot::setPostData(const QString& postData)
{
	qDebug() << "QQBouchot::setPostData = " << postData;
	this->m_postData = postData;
}

QString QQBouchot::postUrl()
{
	return this->m_postUrl;
}

void QQBouchot::setPostUrl(const QString& url)
{
	qDebug() << "QQBouchot::setPostUrl = " << url;
	this->m_postUrl = url;
}

int QQBouchot::refresh()
{
	return this->m_refresh;
}

void QQBouchot::setRefresh(int refresh)
{
	qDebug() << "QQBouchot::setRefresh = " << refresh;
	this->m_refresh= refresh;
}

QString QQBouchot::refreshToString()
{
	return QString::number(this->m_refresh);
}

void QQBouchot::setRefreshFromString(const QString& refresh)
{
	qDebug() << "QQBouchot::setRefreshFromString = " << refresh;
	this->m_refresh = refresh.toInt();
}

QQBouchot::TypeSlip QQBouchot::slipType()
{
	return this->m_slipType;
}

void QQBouchot::setSlipType(QQBouchot::TypeSlip slipType)
{
	qDebug() << "QQBouchot::setSlipType = " << slipType;
	this->m_slipType = slipType;
}

QString QQBouchot::ua()
{
	return this->m_ua;
}

void QQBouchot::setUa(const QString& ua)
{
	qDebug() << "QQBouchot::setUa = " << ua;
	this->m_ua = ua;
}

QString QQBouchot::group()
{
	return this->m_group;
}

void QQBouchot::setGroup(const QString &newGroup)
{
	this->m_group = newGroup;
}


void QQBouchot::startRefresh()
{
	qDebug() << "QQBouchot::startRefresh";
	if(m_refresh <= 0)
		return;

	connect(&timer, SIGNAL(timeout()), this, SLOT(fetchBackend()));
	timer.start(m_refresh * 1000);
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

QQBouchot* QQBouchot::getBouchotDef(const QString &nameBouchot)
{
	int i = 0;
	for(i = 0; i < bouchotsDefSize; i++)
		if(nameBouchot == bouchotsDef[i].name)
			break;

	if(i >= bouchotsDefSize)
		return NULL;

	QQBouchot *res = new QQBouchot(nameBouchot, NULL);
	res->setAliasesFromString(bouchotsDef[i].alias);
	res->setBackendUrl(bouchotsDef[i].getUrl);
	res->setColorFromString(bouchotsDef[i].color);
	res->setPostData(bouchotsDef[i].postData);
	res->setPostUrl(bouchotsDef[i].postUrl);
	res->setRefresh(defaultRefreshRate);
	res->setSlipType(bouchotsDef[i].typeSlip);
	res->setCookie(bouchotsDef[i].cookieProto);

	return res;
}

QStringList QQBouchot::getBouchotDefNameList()
{
	int i = 0;
	QStringList res;
	for(i = 0; i < bouchotsDefSize; i++)
		res.append(QString::fromAscii(bouchotsDef[i].name));

	return res;
}

void QQBouchot::fetchBackend()
{
	QString url = m_backendUrl;
	QString lastId;
	lastId.setNum(m_lastId);
	url.replace(QString::fromAscii("%i"), lastId);

	qDebug() << "QQBouchot::fetchBackend url=" << url;

	QNetworkRequest request(QUrl::fromUserInput(url));

	if(m_ua.isEmpty() == false)
		request.setRawHeader(QString::fromAscii("User-Agent").toAscii(), m_ua.toAscii());

	if(m_cookie.isEmpty() == false)
		request.setRawHeader(QString::fromAscii("Cookie").toAscii(), m_cookie.toAscii());

	m_netManager->get(request);

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

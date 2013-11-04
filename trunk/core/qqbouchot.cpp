#include "qqbouchot.h"

#include "core/qqbackendupdatedevent.h"
#include "core/qqpurgebouchothistoevent.h"
#include "core/qqsettings.h"
#include "xml/qqxmlparser.h"

#include <QApplication>
#include <QDateTime>
#include <QtDebug>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>
#include <QSslError>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#define X_POST_ID_HEADER "X-Post-Id"

int defaultRefreshRate = 30;
typedef struct QQBouchotDef
{
	char name[16];
	char getUrl[64];
	char postUrl[64];
	char postData[64];
	char color[16];
	char alias[64];
	char cookieProto[64];
	QQBouchot::TypeSlip typeSlip;
} QQBouchotDef;

//Définition des bouchots préconfigurés
// tiré d'olcc by Chrisix
int bouchotsDefSize = 10;
QQBouchotDef bouchotsDef[] =
{
	{ "dlfp", "http://linuxfr.org/board/index.xml", "http://linuxfr.org/board", "board[message]=%m",
	  "#dac0de", "linuxfr,beyrouth,passite,dapassite", "linuxfr.org_session=", QQBouchot::SlipTagsEncoded },
	{ "batavie", "http://batavie.leguyader.eu/remote.xml", "http://batavie.leguyader.eu/index.php/add", "message=%m",
	  "#ffccaa", "llg", "", QQBouchot::SlipTagsRaw },
	{ "euromussels", "http://euromussels.eu/?q=tribune.xml&last_id=%i", "http://euromussels.eu/?q=tribune/post", "message=%m",
	  "#d0d0ff", "euro,euroxers", "", QQBouchot::SlipTagsEncoded },
	{ "finss", "http://finss.fr/drupal/node/95/xml", "http://finss.fr/drupal/node/95/post", "message=%m",
	  "#d0ffd0", "finss", "", QQBouchot::SlipTagsRaw },
	{ "shoop", "http://dax.sveetch.net/tribune/remote.xml?last=%i", "http://dax.sveetch.net/tribune/post.xml", "content=%m",
	  "#ededdb", "sveetch,dax", "shoop_sessionid=", QQBouchot::SlipTagsEncoded },
	{ "moules", "http://moules.org/board/backend", "http://moules.org/board/add", "message=%m",
	  "#ffe3c9", "", "", QQBouchot::SlipTagsEncoded },
	{ "hadoken", "http://hadoken.free.fr/board/remote.php", "http://hadoken.free.fr/board/post.php", "message=%m",
	  "#77aadd", "axel,waf", "", QQBouchot::SlipTagsEncoded },
	{ "gabuzomeu", "http://gabuzomeu.fr/tribune.xml", "http://gabuzomeu.fr/tribune/post", "message=%m",
	  "#aaffbb", "", "", QQBouchot::SlipTagsRaw },
	{ "see", "http://tout.essaye.sauf.ca/tribune.xml", "http://tout.essaye.sauf.ca/tribune/post", "message=%m",
	  "#ffd0d0", "schee,seeschloss", "", QQBouchot::SlipTagsRaw },
	{ "devnewton", "http://devnewton.bci.im/fr/chat/xml", "http://devnewton.bci.im/fr/chat/post", "message=%m",
	  "#666666", "", "", QQBouchot::SlipTagsRaw }
};

QQBouchot::QQBouchot(const QString &name, QObject *parent) :
	QQNetworkAccessor(parent),
	m_hasXPostId(false), // unknown
	m_lastId(-1),
	m_name(name),
	m_xmlParser(new QQXmlParser()),
	m_piniWidget(NULL),
	m_deltaTimeH(-1) // unknown
{
	m_bSettings.setRefresh(0);

	connect(m_xmlParser, SIGNAL(newPostReady(QQPost&)), this, SLOT(insertNewPost(QQPost&)));
	connect(m_xmlParser, SIGNAL(finished()), this, SLOT(parsingFinished()));

	QQBouchot::s_hashBouchots.insert(m_name, this);
}

QQBouchot::~QQBouchot()
{
	QQBouchot::s_hashBouchots.remove(m_name);
	emit destroyed(this);
	delete m_xmlParser;
}

void QQBouchot::postMessage(const QString &message)
{
	QString url = m_bSettings.postUrl();
	QByteArray postData = m_bSettings.postData().toLatin1();
	QByteArray mark("%m");

	if(postData.contains(mark))
		postData.replace(mark, QUrl::toPercentEncoding(message));
	else
		postData=QUrl::toPercentEncoding(message);

	QNetworkRequest request(QUrl::fromUserInput(url));
	request.setAttribute(QNetworkRequest::User, QQBouchot::PostRequest);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");

	QString ua = m_bSettings.ua();
	if(ua.isEmpty())
	{
		QQSettings settings;
		ua = settings.value(SETTINGS_GENERAL_DEFAULT_UA, DEFAULT_GENERAL_DEFAULT_UA).toString();
		if(ua.isEmpty())
			ua=QString(DEFAULT_GENERAL_DEFAULT_UA);
	}

	request.setRawHeader("User-Agent", ua.toLatin1());

	if(m_bSettings.cookie().isEmpty() == false)
		request.setRawHeader("Cookie", m_bSettings.cookie().toLatin1());

	request.setRawHeader("Accept", "*/*");
	request.setRawHeader("Accept-Encoding","gzip, deflate");
	request.setRawHeader("Referer", request.url().toString().toLatin1());
	httpPost(request, postData);
}

void QQBouchot::setSettings(const QQBouchotSettings &newSettings)
{
	QString oldGroup = m_bSettings.group();
	m_bSettings = newSettings;
	checkGroupModified(oldGroup);
}


void QQBouchot::startRefresh()
{
	if(m_bSettings.refresh() <= 0)
		return;

	//Connection du signal
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(fetchBackend()));

	//Première récuperation
	fetchBackend();
}

void QQBouchot::stopRefresh()
{
	m_timer.disconnect();
	m_timer.stop();
}

QQListPostPtr QQBouchot::takeNewPosts()
{
	QQListPostPtr list(m_newPostHistory);
	m_newPostHistory.clear();
	return list;
}

void QQBouchot::setNewPostsFromHistory()
{
	int index = m_history.length();
	if(! m_newPostHistory.isEmpty())
	{
		QQPostPtr firstNew = m_newPostHistory.first();
		index = m_history.indexOf(firstNew);
	}
	while (--index >= 0)
		m_newPostHistory.prepend(m_history.at(index));

	askPiniUpdate();
}

bool QQBouchot::event(QEvent *e)
{
	if(e->type() == QQPurgeBouchotHistoEvent::PURGE_BOUCHOT_HISTO)
	{
		QMutableListIterator<QQPostPtr> i(m_history);
		while (i.hasNext())
		{
			QQPostPtr post = i.next();
			if(post.isNull())
				i.remove();
			else
				break;
		}
	}
	else
		QObject::event(e);

	return true;
}

void QQBouchot::fetchBackend()
{
	QString url = m_bSettings.backendUrl();
	QString lastId;

	//on bloque le timer
	m_timer.stop();

	if(m_lastId < 0)
	{
		//1° appel au backend, on supprime le last/last_id/... de l'url
		QRegExp reg;
		reg.setCaseSensitivity(Qt::CaseSensitive);
		reg.setPatternSyntax(QRegExp::RegExp2);
		reg.setPattern("&?\\w+=%i");
		url.replace(reg, "");
		//Nettoyage de l'url finale
		reg.setPattern("\\?$");
		url.replace(reg, "");
		reg.setPatternSyntax(QRegExp::FixedString);
		reg.setPattern("?&");
		url.replace(reg, "?");
	}
	else
	{
		lastId.setNum(m_lastId);
		url.replace(QString::fromLatin1("%i"), lastId);
	}

	QNetworkRequest request(QUrl::fromUserInput(url));
	request.setAttribute(QNetworkRequest::User, QQBouchot::BackendRequest);
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
						 QNetworkRequest::AlwaysNetwork);

	request.setRawHeader(QString::fromLatin1("User-Agent").toLatin1(), QString(DEFAULT_GENERAL_DEFAULT_UA).toLatin1());

	if(m_bSettings.cookie().isEmpty() == false)
		request.setRawHeader(QString::fromLatin1("Cookie").toLatin1(), m_bSettings.cookie().toLatin1());

	QNetworkReply * reply = httpGet(request);
	connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(const QList<QSslError>&)));

	emit refreshStarted();
	m_timer.setInterval(m_bSettings.refresh() * 1000);
	m_timer.start();
}


void QQBouchot::slotSslErrors(const QList<QSslError> &errors)
{
	foreach(QSslError err, errors)
	{
		if(err.error() != QSslError::NoError)
		{
			qDebug() << "QQNetworkAccessor::slotNetworkReplyError: " << err.errorString();
		}
	}
}

void QQBouchot::requestFinishedSlot(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		qWarning() << "QQBouchot::requestFinishedSlot, error : " << reply->error()
				   << ", msg : " << reply->errorString();
	}
	else
	{
		switch(reply->request().attribute(QNetworkRequest::User, QQBouchot::UnknownRequest).toInt(0))
		{
		case QQBouchot::PostRequest:
			if(reply->hasRawHeader(X_POST_ID_HEADER))
			{
				m_hasXPostId = true;
				m_xPostIds.append(QString(reply->rawHeader(X_POST_ID_HEADER)));
			}

			fetchBackend();
			break;
		case QQBouchot::BackendRequest:
			parseBackend(reply->readAll());
			break;
		default:
			qWarning()  << "QQBouchot::requestFinishedSlot, reply->request().attribute(QNetworkRequest::User).toInt() unknown";
		}
	}
	reply->deleteLater();
}

void QQBouchot::parseBackend(const QByteArray &data)
{
	QXmlSimpleReader xmlReader;
	QXmlInputSource xmlSource;

	m_xmlParser->setLastId(m_lastId);
	m_xmlParser->setTypeSlip(m_bSettings.slipType());

	xmlSource.setData(data);
	xmlReader.setContentHandler(m_xmlParser);
	xmlReader.setErrorHandler(m_xmlParser);
	xmlReader.parse(&xmlSource);
}

void QQBouchot::insertNewPost(QQPost &newPost)
{
	QQPost * tmpNewPost = new QQPost(newPost);
	tmpNewPost->setParent(this);

	QString postId = tmpNewPost->id();
	if(m_xPostIds.contains(postId))
	{
		tmpNewPost->setSelfPost();
		m_xPostIds.removeOne(postId);
	}
	//Si la tribune gere le X-Post-Id mais que l'id du post ne fait pas
	// partie de la liste alors il est d'une autre moule

	//Pas du tout efface, il peut s'agir d'un post provenant d'un autre canard
	//else if(m_hasXPostId)
	//	tmpNewPost->setSelfPost(false);

	m_newPostHistory.prepend(tmpNewPost);
}


void QQBouchot::parsingFinished()
{
	if(m_newPostHistory.size() > 0)
	{
		QDateTime currentDateTime = QDateTime::currentDateTime();
		if(m_deltaTimeH == -1 &&
		   m_history.size() > 0 &&
		   m_newPostHistory.size() > 0)
		{
			//Le delta de TZ ne peut etre determine efficacement que lors d'un
			// refresh de backend (pas lors du chargement initial).
			QQPost *last = m_newPostHistory.last();
			QDateTime postDateTime = QDateTime::fromString(last->norloge(), "yyyyMMddHHmmss");
			m_deltaTimeH = postDateTime.secsTo(currentDateTime) / 3600; //Secondes vers Heures
		}

		m_history.append(m_newPostHistory);
		m_lastPosters.clear();
		{
			int deltaTimeH = m_deltaTimeH;
			// Si on ne sait pas on considere que c'est 0 en attendant d'en savoir plus
			if(deltaTimeH < 0)
				deltaTimeH = 0;

			currentDateTime.addSecs( deltaTimeH * 3600 ); // on se met sur le meme TZ que le bouchot
			for(int i = m_history.size() - 1; i >= 0; i--)
			{
				QQPost *post = m_history.at(i);
				QDateTime postDateTime = QDateTime::fromString(post->norloge(), "yyyyMMddHHmmss");
				if(qAbs(currentDateTime.secsTo(postDateTime)) < (2 * 3600))
				{
					QString name;
					bool isAuth;
					if(post->login().size() > 0)
					{
						name = post->login();
						isAuth = true;
					}
					else if((post->UA().size() > 0) && ! post->UA().contains('/'))
					{
						name = post->UA();
						isAuth = false;
					}

					if(name.length() > 0)
					{
						QQMussel mussel(name, m_name, isAuth);
						bool found = false;
						foreach (QQMussel lastMussels, m_lastPosters) {
							if(lastMussels == mussel)
							{
								found = true;
								break;
							}
						}
						if(! found)
							m_lastPosters.append(mussel);
					}
				}
				else
					break;
			}
			emit lastPostersUpdated();
		}
		m_lastId = m_xmlParser->maxId();
		askPiniUpdate();
	}
}

void QQBouchot::askPiniUpdate()
{
	QApplication::postEvent(
				(QObject *) m_piniWidget,
				new QQBackendUpdatedEvent(
					QQBackendUpdatedEvent::BACKEND_UPDATED,
					m_bSettings.group()
					),
				Qt::LowEventPriority
				);
}

void QQBouchot::checkGroupModified(const QString &oldGroupName)
{
	if(m_bSettings.group() != oldGroupName)
		emit groupChanged(this, oldGroupName);
}

/////////////////////////
// Static
/////////////////////////

QQBouchot::QQBouchotSettings QQBouchot::getBouchotDef(const QString &bouchotName)
{

	QQBouchot::QQBouchotSettings settings;

	int i = 0;
	for(; i < bouchotsDefSize; i++)
		if(QString::compare(bouchotName, QLatin1String(bouchotsDef[i].name)) == 0)
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
		res.append(QString::fromLatin1(bouchotsDef[i].name));

	return res;
}

QHash<QString, QQBouchot *> QQBouchot::s_hashBouchots;

QQBouchot * QQBouchot::bouchot(const QString &bouchotName)
{
	QQBouchot * ret = NULL;
	if(s_hashBouchots.contains(bouchotName))
		ret = s_hashBouchots.value(bouchotName);
	else
	{
		foreach (QQBouchot *bouchot, listBouchots())
		{
			if(bouchot->settings().aliases().contains(bouchotName))
			{
				ret = bouchot;
				break;
			}
		}
	}

	return ret;
}

QList<QQBouchot *> QQBouchot::listBouchots()
{
	return s_hashBouchots.values();
}

QList<QQBouchot *> QQBouchot::listBouchotsGroup(const QString &groupName)
{
	QHashIterator<QString, QQBouchot *> i(s_hashBouchots);
	QQBouchot *bouchot;
	QList<QQBouchot *> res;

	while (i.hasNext()) {
		bouchot = i.next().value();
		if(bouchot->settings().group() == groupName)
			res.append(bouchot);
	}
	return res;
}

QList<QString> QQBouchot::listGroups()
{
	QList<QString> listGroups;
	foreach (QQBouchot *bouchot, s_hashBouchots.values())
	{
		QString group = bouchot->settings().group();
		if(! listGroups.contains(group))
			listGroups.append(group);
	}
	return listGroups;
}

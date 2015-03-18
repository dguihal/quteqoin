#include "qqbouchot.h"

#include "core/qqbackendupdatedevent.h"
#include "core/qqboardstatechangeevent.h"
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
	{ "shoop", "http://www.sveetch.net/tribune/remote/xml/?last=%i", "http://www.sveetch.net/tribune/post/xml", "content=%m",
	  "#ededdb", "sveetch,dax", "shoop_sessionid=", QQBouchot::SlipTagsRaw },
	{ "moules", "http://moules.org/board/backend", "http://moules.org/board/add", "message=%m",
	  "#ffe3c9", "", "", QQBouchot::SlipTagsEncoded },
	{ "hadoken", "http://hadoken.free.fr/board/remote.php", "http://hadoken.free.fr/board/post.php", "message=%m",
	  "#77aadd", "axel,waf", "", QQBouchot::SlipTagsEncoded },
	{ "gabuzomeu", "http://gabuzomeu.fr/tribune.xml", "http://gabuzomeu.fr/tribune/post", "message=%m",
	  "#aaffbb", "", "", QQBouchot::SlipTagsRaw },
	{ "see", "http://tout.essaye.sauf.ca/tribune.xml", "http://tout.essaye.sauf.ca/tribune/post", "message=%m",
	  "#ffd0d0", "schee,seeschloss", "", QQBouchot::SlipTagsRaw },
	{ "devnewton", "http://b3.bci.im/legacy/xml", "http://b3.bci.im/legacy/post", "message=%m",
	  "#666666", "", "", QQBouchot::SlipTagsEncoded }
};

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::QQBouchot
/// \param name
/// \param parent
///
QQBouchot::QQBouchot(const QString &name, QObject *parent) :
	QQNetworkAccessor(parent),
	m_hasXPostId(false), // unknown
	m_lastId(-1),
	m_name(name),
	m_xmlParser(new QQXmlParser()),
	m_deltaTimeH(-1) // unknown
{
	m_bSettings.setRefreshFromString(DEFAULT_BOUCHOT_REFRESH);

	m_state.hasBigorno = false;
	m_state.hasError = false;
	m_state.hasNewPosts = false;
	m_state.hasResponse = false;

	connect(m_xmlParser, SIGNAL(newPostReady(QQPost&)), this, SLOT(insertNewPost(QQPost&)));
	connect(m_xmlParser, SIGNAL(finished()), this, SLOT(parsingFinished()));

	QQBouchot::s_hashBouchots.insert(m_name, this);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::~QQBouchot
///
QQBouchot::~QQBouchot()
{
	QQBouchot::s_hashBouchots.remove(m_name);
	emit destroyed(this);
	delete m_xmlParser;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::postMessage
/// \param message
///
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

	if(! m_bSettings.cookie().isEmpty())
		request.setRawHeader("Cookie", m_bSettings.cookie().toLatin1());

	request.setRawHeader("Accept", "*/*");
	request.setRawHeader("Accept-Encoding","gzip, deflate");
	request.setRawHeader("Referer", request.url().toString().toLatin1());
	httpPost(request, postData);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::setSettings
/// \param newSettings
///
void QQBouchot::setSettings(const QQBouchotSettings &newSettings)
{
	QString oldGroup = m_bSettings.group();
	m_bSettings = newSettings;
	checkGroupModified(oldGroup);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::startRefresh
///
void QQBouchot::startRefresh()
{
	if(m_bSettings.refresh() <= 0)
		return;

	//Connection du signal
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(fetchBackend()));

	//Première récuperation
	fetchBackend();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::stopRefresh
///
void QQBouchot::stopRefresh()
{
	m_timer.disconnect();
	m_timer.stop();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::takeNewPosts
/// \return
///
QQListPostPtr QQBouchot::takeNewPosts()
{
	QQListPostPtr list(m_newPostHistory);
	m_newPostHistory.clear();
	return list;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::setNewPostsFromHistory
///
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

	m_state.hasNewPosts = true;
	sendBouchotEvents();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::addToBak
/// \param name
/// \param isAuth
///
void QQBouchot::addToBak(const QString &name, const bool isAuth)
{
	QPair<QString, bool> newItem = qMakePair(name, isAuth);
	if(! m_bakList.contains(newItem))
		m_bakList.append(newItem);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::isBaked
/// \param name
/// \param isAuth
///
bool QQBouchot::isBaked(const QString &name, const bool isAuth) const
{
	QPair<QString, bool> newItem = qMakePair(name, isAuth);
	return m_bakList.contains(newItem);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::removeFromBak
/// \param name
/// \param isAuth
///
void QQBouchot::removeFromBak( const QString &name, const bool isAuth)
{
	QPair<QString, bool> newItem = qMakePair(name, isAuth);
	m_bakList.removeAll(newItem);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::addToPlopify
/// \param name
/// \param isAuth
///
void QQBouchot::addToPlopify(const QString &name, const bool isAuth)
{
	QPair<QString, bool> newItem = qMakePair(name, isAuth);
	if(! m_plopifyList.contains(newItem))
		m_plopifyList.append(newItem);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::isPlopified
/// \param name
/// \param isAuth
///
bool QQBouchot::isPlopified(const QString &name, const bool isAuth) const
{
	QPair<QString, bool> newItem = qMakePair(name, isAuth);
	return m_plopifyList.contains(newItem);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::removeFromPlopify
/// \param name
/// \param isAuth
///
void QQBouchot::removeFromPlopify(const QString &name, const bool isAuth)
{
	QPair<QString, bool> newItem = qMakePair(name, isAuth);
	m_plopifyList.removeAll(newItem);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::boardState
/// \return
///
QuteQoin::QQBoardStates QQBouchot::boardState()
{
	return m_state;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::resetStatus
///
void QQBouchot::resetStatus()
{
	m_state.hasNewPosts = false;
	m_state.hasBigorno = false;
	m_state.hasResponse = false;
	sendBouchotEvents();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::setHasNewResponse
///
void QQBouchot::setHasNewResponse()
{
	m_state.hasResponse = true;
	sendBouchotEvents();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::setHasBigorno
///
void QQBouchot::setHasBigorno()
{
	m_state.hasBigorno = true;
	sendBouchotEvents();
}

//////////////////////////////////////////////////////////////
/// \brief registerForEventNotification
/// \param receiver
/// \param events
///
void QQBouchot::registerForEventNotification(QObject *receiver, QQBouchotEvents events)
{
	if(receiver)
	{
		connect(receiver, SIGNAL(destroyed(QObject *)),
				this, SLOT(unregisterForEventNotification(QObject*)));
		unregisterForEventNotification(receiver);
		QQBouchot::EventReceiver evRcv;
		evRcv.acceptedEvents = events;
		evRcv.receiver = receiver;
		m_listEventReceivers.append(evRcv);
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::event
/// \param e
/// \return
///
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

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::fetchBackend
///
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

	QNetworkReply *reply = httpGet(request);
	connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(const QList<QSslError>&)));

	emit refreshStarted();
	m_timer.setInterval(m_bSettings.refresh() * 1000);
	m_timer.start();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::slotSslErrors
/// \param errors
///
void QQBouchot::slotSslErrors(const QList<QSslError> &errors)
{
	QString msgs;
	foreach(QSslError err, errors)
	{
		switch (err.error()) {
			case QSslError::SelfSignedCertificate:
			case QSslError::SelfSignedCertificateInChain:
			case QSslError::UnableToGetLocalIssuerCertificate:
			case QSslError::UnableToVerifyFirstCertificate:
			case QSslError::CertificateUntrusted:
				if(m_bSettings.isStrictHttpsCertif() == true)
					msgs.append(err.errorString()).append("\n");
				break;
			case QSslError::NoError:
				break;
			default:
				msgs.append(err.errorString()).append("\n");
				break;
		}
	}
	if(msgs.length() > 0)
		emit refreshError(msgs);
	else
		((QNetworkReply *)sender())->ignoreSslErrors();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::unregisterForEventNotification
/// \param receiver
///
void QQBouchot::unregisterForEventNotification(QObject *receiver)
{
	if(receiver)
	{
		QMutableListIterator<EventReceiver> i(m_listEventReceivers);
		while(i.hasNext())
		{
			if(i.next().receiver == receiver)
			{
				disconnect(receiver, SIGNAL(destroyed(QObject *)),
						this, SLOT(unregisterForEventNotification(QObject*)));
				i.remove();
			}
		}
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::requestFinishedSlot
/// \param reply
///
void QQBouchot::requestFinishedSlot(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		QString errMsg = reply->errorString();
		m_state.hasError = true;
		qWarning() << Q_FUNC_INFO
				   << "error : " << reply->error()
				   << "msg : " << reply->errorString();
		switch(reply->request().attribute(QNetworkRequest::User, QQBouchot::UnknownRequest).toInt(0))
		{
			case QQBouchot::BackendRequest:
				emit refreshError(errMsg);
				break;
			default:
				qWarning() << Q_FUNC_INFO
						   << "reply->request().attribute(QNetworkRequest::User).toInt() unknown"
						   << reply->request().attribute(QNetworkRequest::User, QQBouchot::UnknownRequest).toInt(0);
		}
	}
	else
	{
		m_state.hasError = false;
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
			emit refreshOK();
			break;
			qWarning() << Q_FUNC_INFO
					   << reply->error()
					   << "QQBouchot::requestFinishedSlot, reply->request().attribute(QNetworkRequest::User).toInt() unknown";
		}
	}
	reply->deleteLater();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::parseBackend
/// \param data
///
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

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::insertNewPost
/// \param newPost
///
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

#define MAX_TIME_USER_ACTIVE_S (30 * 60)
//////////////////////////////////////////////////////////////
/// \brief QQBouchot::parsingFinished
///
void QQBouchot::parsingFinished()
{
	if(m_newPostHistory.size() > 0)
	{
		if(m_deltaTimeH == -1 &&
		   m_history.size() > 0) //Ne peut-etre fait sur le 1° backend recupere
		{
			//Le delta de TZ ne peut etre determine efficacement que lors d'un
			// refresh de backend (pas lors du chargement initial).
			QQPost *last = m_newPostHistory.last();
			QDateTime postDateTime = QDateTime::fromString(last->norloge(), "yyyyMMddHHmmss");
			m_deltaTimeH = postDateTime.secsTo(QDateTime::currentDateTime()) / 3600; //Secondes vers Heures
		}

		m_history.append(m_newPostHistory);
		m_lastId = m_xmlParser->maxId();
		m_state.hasNewPosts = true;
		sendBouchotEvents();
	}

	updateLastUsers();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::checkGroupModified
/// \param oldGroupName
///
void QQBouchot::checkGroupModified(const QString &oldGroupName)
{
	if(m_bSettings.group() != oldGroupName)
		emit groupChanged(this, oldGroupName);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::updateLastUsers
///
void QQBouchot::updateLastUsers()
{
	m_lastPosters.clear();

	int deltaTimeH = m_deltaTimeH;
	// Si on ne sait pas on considere que c'est 0 en attendant d'en savoir plus
	if(deltaTimeH < 0)
		deltaTimeH = 0;

	QDateTime currentDateTime = QDateTime::currentDateTime();
	currentDateTime.addSecs( deltaTimeH * 3600 ); // on se met sur le meme TZ que le bouchot
	for(int i = m_history.size() - 1; i >= 0; i--)
	{
		QQPost *post = m_history.at(i);
		QDateTime postDateTime = QDateTime::fromString(post->norloge(), "yyyyMMddHHmmss");
		if(qAbs(currentDateTime.secsTo(postDateTime)) < MAX_TIME_USER_ACTIVE_S)
		{
			QString name;
			bool isAuth = false;
			if(post->login().size() > 0)
			{
				name = post->login();
				isAuth = true;
			}
			else if((post->UA().size() > 0) && ! post->UA().contains('/'))
				name = post->UA();

			if(name.length() > 0)
			{
				QQMussel mussel(name, m_name, isAuth);
				if(!m_lastPosters.contains(mussel))
					m_lastPosters.append(mussel);
			}
		}
		else
			break;
	}
	emit lastPostersUpdated();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::postStateChangedEvent
///
void QQBouchot::sendBouchotEvents()
{
	foreach (EventReceiver evRcv, m_listEventReceivers)
	{
		if(evRcv.acceptedEvents.testFlag(NewPostsAvailable) && m_state.hasNewPosts)
		{
			QApplication::postEvent(evRcv.receiver,
									new QQBackendUpdatedEvent(
										QQBackendUpdatedEvent::BACKEND_UPDATED,
										m_bSettings.group()),
									Qt::LowEventPriority
									);
		}
		if(evRcv.acceptedEvents.testFlag(StateChanged))
		{
			QApplication::postEvent(evRcv.receiver,
									new QQBoardStateChangeEvent(m_name),
									Qt::LowEventPriority
									);
		}
	}
}


/////////////////////////
// Static
/////////////////////////

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::getBouchotDef
/// \param bouchotName
/// \return
///
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
		settings.setRefreshFromString(DEFAULT_BOUCHOT_REFRESH);
		settings.setSlipType(bouchotsDef[i].typeSlip);
		settings.setCookie(bouchotsDef[i].cookieProto);
	}

	return settings;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::getBouchotDefNameList
/// \return
///
QStringList QQBouchot::getBouchotDefNameList()
{
	int i = 0;
	QStringList res;
	for(i = 0; i < bouchotsDefSize; i++)
		res.append(QString::fromLatin1(bouchotsDef[i].name));

	return res;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::s_hashBouchots
///
QHash<QString, QQBouchot *> QQBouchot::s_hashBouchots;

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::bouchot
/// \param bouchotName
/// \return
///
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

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::listBouchots
/// \return
///
QList<QQBouchot *> QQBouchot::listBouchots()
{
	return s_hashBouchots.values();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::listBouchotsGroup
/// \param groupName
/// \return
///
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

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::listGroups
/// \return
///
QStringList QQBouchot::listGroups()
{
	QStringList listGroups;
	foreach (QQBouchot *bouchot, s_hashBouchots.values())
	{
		QString group = bouchot->settings().group();
		if(! listGroups.contains(group))
			listGroups.append(group);
	}
	return listGroups;
}

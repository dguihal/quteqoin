#include "qqbouchot.h"

#include "core/qqbackendupdatedevent.h"
#include "core/qqboardstatechangeevent.h"
#include "core/qqpurgebouchothistoevent.h"
#include "core/qqsettings.h"
#include "core/parsers/qqbackendparser.h"
#include "core/parsers/qqtsvparser.h"
#include "core/parsers/qqxmlparser.h"

#include <QApplication>
#include <QBuffer>
#include <QDateTime>
#include <QtDebug>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>
#include <QRegularExpression>
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
#define BOUCHOTS_DEF_SIZE 11
static QQBouchotDef bouchotsDef[] =
{
    { "dlfp", "https://linuxfr.org/board/index.xml", "https://linuxfr.org/board", "board%5Bmessage%5D=%m",
      "#dac0de", "linuxfr,beyrouth,passite,dapassite", "linuxfr.org_session=;remember_account_token=", QQBouchot::SlipTagsEncoded },
    { "batavie", "http://batavie.leguyader.eu/remote.xml", "http://batavie.leguyader.eu/index.php/add", "message=%m",
      "#ffccaa", "llg", "", QQBouchot::SlipTagsRaw },
    { "euromussels", "http://faab.euromussels.eu/data/backend.xml", "http://faab.euromussels.eu/add.php", "message=%m",
      "#d0d0ff", "euro,euroxers,eurofaab", "", QQBouchot::SlipTagsRaw },
    { "sveetch", "http://sveetch.net/tribune/remote/xml/?last=%i", "http://sveetch.net/tribune/post/xml", "content=%m",
      "#ededdb", "shoop,dax", "", QQBouchot::SlipTagsRaw },
    { "moules", "http://moules.org/board/last.php?backend=tsv&id=%i&order=desc", "http://moules.org/board/add.php", "message=%m",
      "#ffe3c9", "", "", QQBouchot::SlipTagsRaw },
    { "gabuzomeu", "http://gabuzomeu.fr/tribune.xml", "http://gabuzomeu.fr/tribune/post", "message=%m",
      "#aaffbb", "", "", QQBouchot::SlipTagsRaw },
    { "devnewton", "https://jb3.devnewton.fr/legacy/xml?last=%i", "https://jb3.devnewton.fr/legacy/post", "message=%m",
      "#666666", "", "", QQBouchot::SlipTagsEncoded },
    { "faab", "http://ratatouille.leguyader.eu/data/backend.xml", "http://ratatouille.leguyader.eu/add.php", "message=%m",
      "#C5D068", "ratatouille", "", QQBouchot::SlipTagsRaw },
    { "goboard", "https://ototu.euromussels.eu/goboard/backend/tsv&last=%i", "https://ototu.euromussels.eu/goboard/post", "message=%m",
      "#fffabb", "goboard", "", QQBouchot::SlipTagsEncoded },
    { "sauf.ca", "http://sauf.ca/feeds/all.tsv", "", "",
      "#4aff47", "", "", QQBouchot::SlipTagsRaw },
    { "42", "http://www.miaoli.im/tribune/42/tsv", "http://www.miaoli.im/tribune/42/post", "message=%m",
      "#ffd0d0", "", "", QQBouchot::SlipTagsRaw }
};

#define REFRESH_RATIOS_SIZE 15
#define REFRESH_RATIOS_MID 7
static float refreshRatiosArr[] = {
    0.1f, 0.3f, 0.5, 0.7f, 0.8f, 0.9f,
    1.0, 1.0, 1.0,
    1.5, 2.0, 3.0, 5.0, 7.0, 10.0
};
#define REFRESH_NEW_POSTS_THRESHOLD 5
#define QQ_MIN_REFRESH_DELAY_S 3

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::QQBouchot
/// \param name
/// \param parent
///
QQBouchot::QQBouchot(const QString &name, QObject *parent) :
    QQNetworkAccessor(parent),
    m_hasXPostId(false), // unknown
    m_lastId(-1),
    m_lastModifiedBackend(""),
    m_name(name),
    m_parser(nullptr),
    m_deltaTimeH(-1) // unknown
{
	m_bSettings.setRefresh(DEFAULT_BOUCHOT_REFRESH);

	m_state.hasBigorno = false;
	m_state.hasError = false;
	m_state.hasNewPosts = false;
	m_state.hasResponse = false;
	m_state.isHidden = false;

	QQBouchot::s_hashBouchots.insert(m_name, this);

	m_refreshRatioIndex = REFRESH_RATIOS_MID;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::~QQBouchot
///
QQBouchot::~QQBouchot()
{
	QQBouchot::s_hashBouchots.remove(m_name);
	emit destroyed(this);
	delete m_parser;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::postMessage
/// \param message
///
void QQBouchot::postMessage(const QString &message)
{
	//Si l'on poste on remet un refresh "normal" si en mode lent
	if(currentRefreshRatio() > 1.0f)
		m_refreshRatioIndex = REFRESH_RATIOS_MID;

	QString url = m_bSettings.postUrl();
	QByteArray postData = m_bSettings.postData().toUtf8();
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

	request.setRawHeader("User-Agent", ua.toUtf8());
	request.setRawHeader("Accept", "*/*");
	request.setRawHeader("Accept-Encoding","gzip, deflate");
	request.setRawHeader("Referer", request.url().toString().toUtf8());
	httpPost(request, postData);
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::setSettings
/// \param newSettings
///
void QQBouchot::setSettings(const QQBouchotSettings &newSettings)
{
	QString oldGroup = m_bSettings.group();
	QString oldBackendURL = m_bSettings.backendUrl();

	//Clean up old cookies
	QUrl postUrl(newSettings.postUrl());
	clearCookiesForUrl(postUrl);

	//Set new cookies
	QList<QNetworkCookie> qlCookies;
	QStringList cookies=newSettings.cookies().split(QRegularExpression("\\s*;\\s*"));
	foreach (QString cookie, cookies) {
		QStringList splittedCookie=cookie.split("=");
		if(splittedCookie.size() >= 2)
		{
			QString cName=splittedCookie.takeFirst();
			QNetworkCookie c(cName.toUtf8(), splittedCookie.join("=").toUtf8());
			qlCookies.append(c);
		}
	}
	if(qlCookies.size() > 0)
		setCookiesFromUrl(qlCookies, postUrl);

	m_bSettings = newSettings;
	checkGroupModified(oldGroup);
	checkBackendUrlModified(oldBackendURL);
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

	m_refreshRatioIndex = REFRESH_RATIOS_MID;
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::currentRefreshInterval
/// \return
///
int QQBouchot::currentRefreshInterval()
{
	return qMax(int(m_bSettings.refresh() * 1000 * currentRefreshRatio()), QQ_MIN_REFRESH_DELAY_S);
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
	m_state.isHidden = false;
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
/// \brief QQBouchot::isReadOnly
/// \return
///
bool QQBouchot::isReadOnly()
{
	return m_bSettings.postUrl().isEmpty();
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
/// \brief QQBouchot::toggleVisibility
///
void QQBouchot::toggleVisibility()
{
	m_state.isHidden = !m_state.isHidden;
	emit visibilitychanged(m_name);
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
	if(m_lastModifiedBackend.length() > 0)
		request.setRawHeader(QString::fromLatin1("If-Modified-Since").toLatin1(), m_lastModifiedBackend.toLatin1());

	if(m_bSettings.cookies().isEmpty() == false)
		request.setRawHeader(QString::fromLatin1("Cookie").toLatin1(), m_bSettings.cookies().toLatin1());

	QNetworkReply *reply = httpGet(request);
	connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(slotSslErrors(const QList<QSslError>&)));

	emit refreshStarted();
	m_timer.setInterval(currentRefreshInterval());
	qDebug() << Q_FUNC_INFO << m_name << currentRefreshRatio() << currentRefreshInterval();
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
		qobject_cast<QNetworkReply *>(sender())->ignoreSslErrors();
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
		           << "Bouchot :" << m_name
		           << "error :" << reply->error()
		           << "msg :" << reply->errorString();
		switch(reply->request().attribute(QNetworkRequest::User, QQBouchot::UnknownRequest).toInt())
		{
			case QQBouchot::BackendRequest:
				emit refreshError(errMsg);
				break;
			default:
				qWarning() << Q_FUNC_INFO
				           << "Bouchot :" << m_name
				           << "reply->request().attribute(QNetworkRequest::User).toInt() unknown :"
				           << reply->request().attribute(QNetworkRequest::User, QQBouchot::UnknownRequest).toInt();
		}
	}
	else
	{
		m_state.hasError = false;
		switch(reply->request().attribute(QNetworkRequest::User, QQBouchot::UnknownRequest).toInt())
		{
			case QQBouchot::PostRequest:
#ifndef QT_NO_DEBUG
				qDebug() << Q_FUNC_INFO << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
				foreach (QByteArray ba, reply->rawHeaderList()) {
					qDebug() << Q_FUNC_INFO << ba << QString(reply->rawHeader(ba));
				}
				qDebug() << Q_FUNC_INFO << reply->readAll();

				qDebug() << Q_FUNC_INFO << reply->request().url();
				foreach (QByteArray ba, reply->request().rawHeaderList()) {
					qDebug() << Q_FUNC_INFO << ba << QString(reply->request().rawHeader(ba));
				}
#endif

				if(reply->hasRawHeader(X_POST_ID_HEADER))
				{
					m_hasXPostId = true;
					m_xPostIds.append(QString(reply->rawHeader(X_POST_ID_HEADER)));
				}

				fetchBackend();
				break;
			case QQBouchot::BackendRequest:
				if(reply->hasRawHeader("Last-Modified"))
					m_lastModifiedBackend = QString(reply->rawHeader("Last-Modified"));

				if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt() == 200)
				{
					QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
					parseBackend(reply->readAll(), contentType);
				}
				emit refreshOK();
				break;
		}
	}
	reply->deleteLater();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::parseBackend
/// \param data
///
void QQBouchot::parseBackend(const QByteArray &data, const QString &contentType)
{
	if(contentType.startsWith("text/xml") ||
	        contentType.startsWith("application/xml"))
		parseBackendXML(data);
	else if(contentType.startsWith("text/tab-separated-values"))
		parseBackendTSV(data);
	else
	{
		QBuffer b;
		b.setData(data);
		b.open(QIODevice::ReadOnly);

		bool parserFound = false;
		if(b.canReadLine())
		{
			QString l = b.readLine();
			qDebug() << Q_FUNC_INFO << l;
			if(l.startsWith("<?xml ") ||
			        l.contains(QRegExp("^<\\w+ ")))
			{
				qDebug() << Q_FUNC_INFO << "XML found";
				parseBackendXML(data);
				parserFound = true;
			}
			else if(l.contains(QRegExp("^\\d+\t")))
			{
				qDebug() << Q_FUNC_INFO << "TSV found";
				parseBackendTSV(data);
				parserFound = true;
			}
		}
		b.close();

		if(! parserFound)
			qWarning() << Q_FUNC_INFO
			           << "Bouchot :" << m_name
			           << "Unsupported backend format (yet) :"
			           << contentType;
	}
}

void QQBouchot::parseBackendTSV(const QByteArray &data)
{
	QQTsvParser *p = qobject_cast<QQTsvParser *>(m_parser);
	if(p == nullptr)
	{
		if(m_parser != nullptr)
			delete m_parser;

		p = new QQTsvParser(this);

		connect(p, SIGNAL(newPostReady(QQPost&)), this, SLOT(insertNewPost(QQPost&)));
		connect(p, SIGNAL(finished()), this, SLOT(parsingFinished()));

		m_parser=p;
	}

	p->setLastId(m_lastId);
	p->parseBackend(data);
}

void QQBouchot::parseBackendXML(const QByteArray &data)
{
	QQXmlParser *p = qobject_cast<QQXmlParser *>(m_parser);
	if(p == nullptr)
	{
		if(m_parser != nullptr)
			delete m_parser;

		p = new QQXmlParser(this);

		connect(p, SIGNAL(newPostReady(QQPost&)), this, SLOT(insertNewPost(QQPost&)));
		connect(p, SIGNAL(finished()), this, SLOT(parsingFinished()));

		m_parser=p;
	}

	QXmlSimpleReader xmlReader;
	QXmlInputSource xmlSource;

	p->setTypeSlip(m_bSettings.slipType());
	p->setLastId(m_lastId);

	xmlSource.setData(data);
	xmlReader.setContentHandler(p);
	xmlReader.setErrorHandler(p);
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
		if(m_newPostHistory.size() >= REFRESH_NEW_POSTS_THRESHOLD
		        && m_refreshRatioIndex > 0)
		{
			m_refreshRatioIndex --; // faster
			if(currentRefreshRatio() > 1.0f)
				m_refreshRatioIndex = REFRESH_RATIOS_MID;
		}

		if(m_deltaTimeH == -1 &&
		        m_history.size() > 0) //Ne peut-etre fait sur le 1° backend recupere
		{
			//Le delta de TZ ne peut etre determine efficacement que lors d'un
			// refresh de backend (pas lors du chargement initial).
			QQPost *last = m_newPostHistory.last();
			QDateTime postDateTime = QDateTime::fromString(last->norloge(), "yyyyMMddHHmmss");
			m_deltaTimeH = static_cast<int>(postDateTime.secsTo(QDateTime::currentDateTime()) / 3600ll); //Secondes vers Heures
		}

		//On s'assure qu'ils sont ranges du plus petit id au plus grand
		std::sort(m_newPostHistory.begin(), m_newPostHistory.end(),
		          // Ca fait rever les lambdas en C++ ....
		          [](const QQPost *a, const QQPost *b) -> bool
		{
			return (* a) < (* b);
		});

		//On verifie pour chaque post s'il est le seul pour la minute
		QPointer<QQPost> prevPost;
		if(m_history.isEmpty())
		{
			prevPost = m_newPostHistory.first();
		}
		else
		{
			//Eventuellement on corrige le statut du dernier post de l'historique actuel vu qu'on dispose du contexte complet desormais
			if(m_history.size() >= 2 &&
			   m_history.at(m_history.size() -2)->norlogeMinute() != m_history.last()->norlogeMinute() &&
			   m_history.last()->norlogeMinute() != m_newPostHistory.first()->norlogeMinute())
			{
				m_history.last()->setAloneInMinute(true);
			}
			prevPost = m_history.last();
		}
		foreach(auto curPost, m_newPostHistory)
		{
			if(prevPost->norlogeMinute() == curPost->norlogeMinute())
			{
				prevPost->setAloneInMinute(false);
				curPost->setAloneInMinute(false);
			}
			prevPost = curPost;
		}
		m_newPostHistory.last()->setAloneInMinute(false); //Pas de norloges raccourcies pour le dernier post tant qu'on a pas tout le contexte

		m_history.append(m_newPostHistory);
		m_lastId = m_parser->maxId();
		m_state.hasNewPosts = true;

		sendBouchotEvents();
	}
	else if(m_refreshRatioIndex < REFRESH_RATIOS_SIZE - 1)
		m_refreshRatioIndex ++; // slower

	updateLastUsers();
}

//////////////////////////////////////////////////////////////
/// \brief QQBouchot::checkBackendUrlModified
/// \param oldBackendUrl
///
void QQBouchot::checkBackendUrlModified(const QString &oldBackendUrl)
{
	if(m_bSettings.backendUrl() != oldBackendUrl)
	{
		if(m_parser != nullptr)
		{
			delete m_parser;
			m_parser = nullptr;
		}
	}
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

	// On se met sur le meme TZ que le bouchot
	QDateTime currentDateTime = QDateTime::currentDateTime().addSecs( deltaTimeH * 3600 );
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
		if(evRcv.acceptedEvents.testFlag(NewPostsAvailable) && m_newPostHistory.size() > 0)
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

//////////////////////////////////////////////////////////////
/// \brief currentRefreshRatio
/// \return
///
float QQBouchot::currentRefreshRatio()
{
	if(m_refreshRatioIndex >= 0 && m_refreshRatioIndex < REFRESH_RATIOS_SIZE)
		return refreshRatiosArr[m_refreshRatioIndex];
	else
	{
		m_refreshRatioIndex = REFRESH_RATIOS_MID;
		return refreshRatiosArr[REFRESH_RATIOS_MID];
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
	for(; i < BOUCHOTS_DEF_SIZE; i++)
		if(QString::compare(bouchotName, QLatin1String(bouchotsDef[i].name)) == 0)
			break;

	if(i < BOUCHOTS_DEF_SIZE)
	{
		settings.setAliasesFromString(bouchotsDef[i].alias);
		settings.setBackendUrl(bouchotsDef[i].getUrl);
		settings.setColorFromString(bouchotsDef[i].color);
		settings.setPostData(bouchotsDef[i].postData);
		settings.setPostUrl(bouchotsDef[i].postUrl);
		settings.setRefresh(DEFAULT_BOUCHOT_REFRESH);
		settings.setSlipType(bouchotsDef[i].typeSlip);
		settings.setCookies(bouchotsDef[i].cookieProto);
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
	for(i = 0; i < BOUCHOTS_DEF_SIZE; i++)
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
	QQBouchot * ret = nullptr;
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

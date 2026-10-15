#include "core/qqbouchot.h"
#include "core/qqboardstatechangeevent.h"
#include "core/qqsettings.h"
#include "ui/pinipede/qqpiniurlhelper.h"
#include "ui/pinipede/qqpinioverlay.h"
#include "ui/pinipede/qqtextbrowser.h"
#include "core/qqwebdownloader.h"
#include "ui/settingsmanager/qqtotozsettings.h"

#include <QBuffer>
#include <QDateTime>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QImage>
#include <QNetworkProxy>
#include <QScopeGuard>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTemporaryDir>
#include <QToolTip>
#include <QtTest>
#include <limits>
#include <memory>

class TestBouchot : public QQBouchot
{
public:
	TestBouchot() : QQBouchot("test") {}
	using QQBouchot::fetchBackend;
};

class EventReceiver : public QObject
{
public:
	int stateChanges = 0;
	int destructionConnections() const { return receivers(SIGNAL(destroyed(QObject*))); }

	bool event(QEvent *event) override
	{
		if(event->type() == QQBoardStateChangeEvent::BOARD_STATE_CHANGED)
		{
			++stateChanges;
			return true;
		}
		return QObject::event(event);
	}
};

class TestTextBrowser : public QQTextBrowser
{
public:
	TestTextBrowser() : QQTextBrowser("test") {}
	using QQTextBrowser::leaveEvent;
	using QQTextBrowser::onAnchorHighlighted;
	using QQTextBrowser::onThumbnailUrlAvailable;
};

class LocalHttpServer : public QObject
{
public:
	QTcpServer server;
	QList<QByteArray> requests;
	QList<QPointer<QTcpSocket>> sockets;

	LocalHttpServer()
	{
		connect(&server, &QTcpServer::newConnection, this, [this] {
			while(server.hasPendingConnections())
			{
				auto *socket = server.nextPendingConnection();
				connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
				connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
					const QByteArray request = socket->property("request").toByteArray() + socket->readAll();
					socket->setProperty("request", request);
					if(!request.contains("\r\n\r\n") || socket->property("handled").toBool())
						return;
					socket->setProperty("handled", true);
					requests.append(request);
					sockets.append(socket);
				});
			}
		});
	}

	QString url() const
	{
		return QString("http://127.0.0.1:%1/board").arg(server.serverPort());
	}

	void respond(int index, const QByteArray &response)
	{
		sockets.at(index)->write(response);
		sockets.at(index)->disconnectFromHost();
	}
};

class BouchotBehaviorTests : public QObject
{
	Q_OBJECT

private:
	QTemporaryDir m_settingsDir;
	const QByteArray m_success = "HTTP/1.1 200 OK\r\nContent-Type: text/tab-separated-values\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";

private slots:
	void initTestCase()
	{
		QVERIFY(m_settingsDir.isValid());
		QCoreApplication::setOrganizationName("QuteqoinBehaviorTests");
		QCoreApplication::setApplicationName("QuteqoinBehaviorTests");
		QSettings::setDefaultFormat(QSettings::IniFormat);
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, m_settingsDir.path());
		QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, m_settingsDir.path());
		QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
	}

	void init()
	{
		QQSettings settings;
		settings.clear();
	}

	void settingsHaveSafeDefaults()
	{
		const QQBouchot::QQBouchotSettings settings;
		QCOMPARE(settings.refresh(), uint(DEFAULT_BOUCHOT_REFRESH));
		QCOMPARE(settings.slipType(), QQBouchot::SlipTagsEncoded);
		QVERIFY(settings.isStrictHttpsCertif());
		QVERIFY(settings.isShortNorlogeEnabled());
		QVERIFY(QQBouchot::getBouchotDef("dlfp").isStrictHttpsCertif());
	}

	void totozSettingsConnectionsUpdateProperties()
	{
		QQTotozSettings widget;
		widget.setTotozServerURL("https://example.com");
		QCOMPARE(widget.totozServerURL(), QString("https://example.com"));
		widget.setTotozBaseImgUrl("images");
		QCOMPARE(widget.totozBaseImgUrl(), QString("images"));
		widget.setTotozNameSuffix(".gif");
		QCOMPARE(widget.totozNameSuffix(), QString(".gif"));
		widget.setTotozAllowSearch(true);
		QVERIFY(widget.totozAllowSearch());
		widget.setTotozQueryPattern("search?q=%t");
		QCOMPARE(widget.totozQueryPattern(), QString("search?q=%t"));
		widget.setTotozAllowSearch(false);
		QVERIFY(!widget.totozAllowSearch());
		QVERIFY(widget.totozQueryPattern().isEmpty());
		widget.setTotozVisualModes({TOTOZ_VISUAL_MODE_DISABLED, TOTOZ_VISUAL_MODE_ON_HOVER});
		QCOMPARE(widget.totozVisualMode(), QString(TOTOZ_VISUAL_MODE_DISABLED));
		widget.setTotozVisualMode(TOTOZ_VISUAL_MODE_ON_HOVER);
		QCOMPARE(widget.totozVisualMode(), QString(TOTOZ_VISUAL_MODE_ON_HOVER));
		widget.setTotozVisualMode(TOTOZ_VISUAL_MODE_DISABLED);
		QCOMPARE(widget.totozVisualMode(), QString(TOTOZ_VISUAL_MODE_DISABLED));
	}

	void shortenedUrlLabels_data()
	{
		QTest::addColumn<QString>("host");
		QTest::addColumn<int>("mode");
		QTest::addColumn<QString>("label");
		QTest::newRow("x-shorter") << QString("x.com") << int(QuteQoin::Shorter) << QString("x");
		QTest::newRow("mozilla-shorter") << QString("developer.mozilla.org") << int(QuteQoin::Shorter) << QString("mozilla");
		QTest::newRow("x-short") << QString("x.com") << int(QuteQoin::Short) << QString("x.com");
		QTest::newRow("mozilla-short") << QString("developer.mozilla.org") << int(QuteQoin::Short) << QString("mozilla.org");
		QTest::newRow("mozilla-full") << QString("developer.mozilla.org") << int(QuteQoin::Full) << QString("developer.mozilla.org");
		QTest::newRow("dotless-short") << QString("localhost") << int(QuteQoin::Short) << QString("localhost");
		QTest::newRow("ipv4-shorter") << QString("127.0.0.1") << int(QuteQoin::Shorter) << QString("127.0.0.1");
	}

	void shortenedUrlLabels()
	{
		QFETCH(QString, host);
		QFETCH(int, mode);
		QFETCH(QString, label);
		QQSettings settings;
		settings.setValue(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE, mode);
		QQPiniUrlHelper helper;
		const QString anchor = QString("<a href=\"https://%1/path\">").arg(host);
		QString message = anchor + "[url]</a>";
		helper.transformMessage("test", message);
		QCOMPARE(message, anchor + '[' + label + "]</a>");
	}

	void shortenedUrlsIncludeEveryLink()
	{
		QQSettings settings;
		settings.setValue(SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE, int(QuteQoin::Shorter));
		QQPiniUrlHelper helper;
		QString message = "prefix <a href=\"https://developer.mozilla.org/path\">[url]</a> "
		                  "<a href=\"https://x.com/path\">[https]</a> "
		                  "<a href=\"https://www.example.com/path\">[http]</a>";
		helper.transformMessage("test", message);
		QCOMPARE(message, QString("prefix <a href=\"https://developer.mozilla.org/path\">[mozilla]</a> "
		                          "<a href=\"https://x.com/path\">[x]</a> "
		                          "<a href=\"https://www.example.com/path\">[example]</a>"));
	}

	void loadingMissingBoardRestoresSettingsGroup()
	{
		QQSettings settings;
		QVERIFY(settings.loadBouchot("missing") == nullptr);
		QVERIFY(settings.group().isEmpty());
	}

	void loadingRefreshSettings_data()
	{
		QTest::addColumn<QVariant>("stored");
		QTest::addColumn<uint>("expected");
		QTest::newRow("missing") << QVariant() << uint(DEFAULT_BOUCHOT_REFRESH);
		QTest::newRow("invalid") << QVariant("invalid") << uint(DEFAULT_BOUCHOT_REFRESH);
		QTest::newRow("negative") << QVariant(-1) << uint(DEFAULT_BOUCHOT_REFRESH);
		QTest::newRow("disabled") << QVariant(0) << uint(0);
		QTest::newRow("configured") << QVariant(42) << uint(42);
	}

	void loadingRefreshSettings()
	{
		QFETCH(QVariant, stored);
		QFETCH(uint, expected);
		QQSettings settings;
		settings.setValue("bouchot/test/backendUrl", "http://127.0.0.1/board");
		settings.setValue("bouchot/test/slipType", 99);
		if(stored.isValid())
			settings.setValue("bouchot/test/refresh", stored);
		std::unique_ptr<QQBouchot> board(settings.loadBouchot("test"));
		QVERIFY(board != nullptr);
		QCOMPARE(board->settings().refresh(), expected);
		QCOMPARE(board->settings().slipType(), QQBouchot::SlipTagsEncoded);
		QVERIFY(board->settings().isStrictHttpsCertif());
		QVERIFY(settings.group().isEmpty());
	}

	void refreshIntervalKeepsFractionalRatios()
	{
		TestBouchot board;
		QCOMPARE(board.currentRefreshInterval(), 30000);
		const QList<int> expected = {30000, 27000, 24000, 21000, 15000, 9000, 3000};
		int postId = 0;
		for(const int interval : expected)
		{
			QByteArray data;
			for(int postIndex = 0; postIndex < 5; ++postIndex)
				data += QByteArray::number(++postId) + '\t'
				        + QDateTime::currentDateTime().toString("yyyyMMddHHmmss").toUtf8()
				        + "\ttest-agent\ttest-user\tmessage\n";
			board.parseBackendTSV(data);
			board.takeNewPosts();
			QCOMPARE(board.currentRefreshInterval(), interval);
		}
	}

	void refreshIntervalIsBounded()
	{
		TestBouchot board;
		auto settings = board.settings();
		settings.setRefresh(0);
		board.setSettings(settings);
		QCOMPARE(board.currentRefreshInterval(), 3000);
		settings.setRefresh(std::numeric_limits<uint>::max());
		board.setSettings(settings);
		QCOMPARE(board.currentRefreshInterval(), std::numeric_limits<int>::max());
	}

	void repeatedSubscriptionKeepsDestructionConnection()
	{
		TestBouchot board;
		auto receiver = std::make_unique<EventReceiver>();
		board.registerForEventNotification(receiver.get(), QQBouchot::StateChanged);
		board.registerForEventNotification(receiver.get(), QQBouchot::StateChanged);
		QCOMPARE(receiver->destructionConnections(), 1);
		board.setHasBigorno();
		QCoreApplication::sendPostedEvents(receiver.get());
		QCOMPARE(receiver->stateChanges, 1);
		receiver.reset();
		board.setHasBigorno();
		QCoreApplication::sendPostedEvents();
	}

	void refreshRequestsDoNotOverlap()
	{
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		TestBouchot board;
		auto settings = board.settings();
		settings.setBackendUrl(server.url());
		board.setSettings(settings);
		QSignalSpy started(&board, &QQBouchot::refreshStarted);
		QSignalSpy finished(&board, &QQBouchot::refreshOK);
		board.fetchBackend();
		board.fetchBackend();
		board.fetchBackend();
		QCOMPARE(started.count(), 1);
		QTRY_COMPARE(server.requests.size(), 1);
		server.respond(0, m_success);
		QTRY_COMPARE(server.requests.size(), 2);
		QCOMPARE(started.count(), 2);
		server.respond(1, m_success);
		QTRY_COMPARE(finished.count(), 2);
		QCOMPARE(server.requests.size(), 2);
	}

	void stoppingRefreshCancelsPendingAndPeriodicRequests()
	{
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		TestBouchot board;
		auto settings = board.settings();
		settings.setBackendUrl(server.url());
		settings.setRefresh(1);
		board.setSettings(settings);
		QSignalSpy started(&board, &QQBouchot::refreshStarted);
		QSignalSpy finished(&board, &QQBouchot::refreshOK);
		board.startRefresh();
		board.startRefresh();
		board.fetchBackend();
		QCOMPARE(started.count(), 1);
		QTRY_COMPARE(server.requests.size(), 1);
		board.stopRefresh();
		server.respond(0, m_success);
		QTRY_COMPARE(finished.count(), 1);
		QTest::qWait(3100);
		QCOMPARE(started.count(), 1);
		QCOMPARE(server.requests.size(), 1);
	}

	void leavingLinkClosesOverlayWithoutTooltip()
	{
		TestTextBrowser browser;
		QSignalSpy hidden(&browser, &QQTextBrowser::hideViewers);
		browser.onAnchorHighlighted(QUrl("duck://test"));
		QToolTip::hideText();
		hidden.clear();
		browser.onAnchorHighlighted(QUrl());
		QCOMPARE(hidden.count(), 1);
		browser.onAnchorHighlighted(QUrl("duck://test"));
		hidden.clear();
		QEvent leave(QEvent::Leave);
		browser.leaveEvent(&leave);
		QCOMPARE(hidden.count(), 1);
	}

	void lateThumbnailDoesNotReopenOverlay()
	{
		TestTextBrowser browser;
		int displayed = 0;
		connect(&browser, &QQTextBrowser::displayMmdaData, this,
		        [&displayed](const QUrl &, QString &) { ++displayed; });
		browser.onAnchorHighlighted(QUrl("duck://test"));
		QUrl thumbnail("http://127.0.0.1/image.png");
		QString contentType("image/png");
		browser.onThumbnailUrlAvailable(thumbnail, contentType);
		QCOMPARE(displayed, 1);
		browser.onAnchorHighlighted(QUrl());
		browser.onThumbnailUrlAvailable(thumbnail, contentType);
		QCOMPARE(displayed, 1);
	}

	void leavingVideoCancelsPendingPlayer()
	{
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		QQPiniOverlay overlay;
		QString contentType("video/mp4");
		overlay.showUrl(QUrl(server.url()), contentType);
		QPointer<QMediaPlayer> player = overlay.findChild<QMediaPlayer *>();
		QVERIFY(!player.isNull());
		auto *video = qobject_cast<QGraphicsVideoItem *>(player->videoOutput());
		QVERIFY(video != nullptr);
		overlay.clearOverview();
		QCOMPARE(player->playbackState(), QMediaPlayer::StoppedState);
		QVERIFY(!video->isVisible());
		player->playbackStateChanged(QMediaPlayer::PlayingState);
		player->errorOccurred(QMediaPlayer::ResourceError, "late video error");
		for(auto *item : overlay.scene()->items())
			QVERIFY(!item->isVisible());
		overlay.clearOverview();
		QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
		QVERIFY(player.isNull());
	}

	void switchingVideoIgnoresOldPlayer()
	{
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		QQPiniOverlay overlay;
		QString contentType("video/mp4");
		overlay.showUrl(QUrl(server.url() + "?first"), contentType);
		auto *oldPlayer = overlay.findChild<QMediaPlayer *>();
		QVERIFY(oldPlayer != nullptr);
		overlay.showUrl(QUrl(server.url() + "?second"), contentType);
		auto *newPlayer = overlay.findChildren<QMediaPlayer *>().constLast();
		QVERIFY(newPlayer != oldPlayer);
		auto *video = qobject_cast<QGraphicsVideoItem *>(newPlayer->videoOutput());
		QVERIFY(video != nullptr);
		const bool wasVisible = video->isVisible();
		const auto itemCount = overlay.scene()->items().size();
		oldPlayer->playbackStateChanged(QMediaPlayer::PlayingState);
		oldPlayer->errorOccurred(QMediaPlayer::ResourceError, "obsolete video error");
		QCOMPARE(video->isVisible(), wasVisible);
		QCOMPARE(overlay.scene()->items().size(), itemCount);
		newPlayer->playbackStateChanged(QMediaPlayer::PlayingState);
		QVERIFY(video->isVisible());
		overlay.clearOverview();
		QVERIFY(!video->isVisible());
		QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
	}

	void closingWindowWithActivePreview_data()
	{
		QTest::addColumn<QString>("contentType");
		QTest::newRow("image") << QString("image/png");
		QTest::newRow("video") << QString("video/mp4");
	}

	void closingWindowWithActivePreview()
	{
		QFETCH(QString, contentType);
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		for(int iteration = 0; iteration < 3; ++iteration)
		{
			auto window = std::make_unique<QWidget>();
			window->resize(400, 300);
			auto *browser = new TestTextBrowser;
			browser->setParent(window.get());
			browser->resize(window->size());
			auto *overlay = new QQPiniOverlay(window.get());
			connect(browser, &QQTextBrowser::hideViewers, overlay, &QQPiniOverlay::clearOverview);
			window->show();
			browser->onAnchorHighlighted(QUrl("duck://test"));
			overlay->showUrl(QUrl(server.url() + QString("?close=%1").arg(iteration)), contentType);
			QTest::qWait(20);
			window->close();
			window.reset();
			QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
		}
	}

	void lateImageDownloadDoesNotReopenOverlay()
	{
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		QQPiniOverlay overlay;
		QString contentType("image/png");
		int downloaded = 0;
		auto *downloader = overlay.findChild<QQWebDownloader *>();
		QVERIFY(downloader != nullptr);
		connect(downloader, &QQWebDownloader::ready, this, [&downloaded](QUrl &) { ++downloaded; });
		overlay.showUrl(QUrl(server.url()), contentType);
		QTRY_COMPARE(server.requests.size(), 1);
		overlay.clearOverview();
		QImage image(2, 2, QImage::Format_RGB32);
		image.fill(Qt::red);
		QByteArray body;
		QBuffer buffer(&body);
		QVERIFY(buffer.open(QIODevice::WriteOnly));
		QVERIFY(image.save(&buffer, "PNG"));
		server.respond(0, "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: "
		                  + QByteArray::number(body.size()) + "\r\nConnection: close\r\n\r\n" + body);
		QTRY_COMPARE(downloaded, 1);
		for(auto *item : overlay.scene()->items())
			QVERIFY(!item->isVisible());
	}

	void debugContentMatchesBuildMode()
	{
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		TestBouchot board;
		auto settings = board.settings();
		settings.setBackendUrl(server.url());
		settings.setPostUrl(server.url());
		settings.setPostData("message=%m");
		board.setSettings(settings);
		QSignalSpy finished(&board, &QQBouchot::refreshOK);

		static QStringList messages;
		messages.clear();
		const auto previousHandler = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &, const QString &message) {
			if(type == QtDebugMsg)
				messages.append(message);
		});
		const auto restoreHandler = qScopeGuard([previousHandler] { qInstallMessageHandler(previousHandler); });

		board.postMessage("message");
		QTRY_COMPARE(server.requests.size(), 1);
		const QByteArray postBody = "debug-response-body";
		server.respond(0, "HTTP/1.1 200 OK\r\nSet-Cookie: session=debug-cookie-value; Path=/\r\nContent-Length: "
		                  + QByteArray::number(postBody.size()) + "\r\nConnection: close\r\n\r\n" + postBody);
		QTRY_COMPARE(server.requests.size(), 2);
		const QByteArray backendBody = "1\t" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss").toUtf8()
		                               + "\ttest-agent\ttest-user\tdebug-backend-content\n";
		server.respond(1, "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: "
		                  + QByteArray::number(backendBody.size()) + "\r\nConnection: close\r\n\r\n" + backendBody);
		QTRY_COMPARE(finished.count(), 1);
		const QString logs = messages.join('\n');
#ifdef QT_DEBUG
		QVERIFY(logs.contains("debug-response-body"));
		QVERIFY(logs.contains("debug-cookie-value"));
		QVERIFY(logs.contains("debug-backend-content"));
#else
		QVERIFY(!logs.contains("debug-response-body"));
		QVERIFY(!logs.contains("debug-cookie-value"));
		QVERIFY(!logs.contains("debug-backend-content"));
#endif
	}

	void networkErrorPreservesSessionCookies()
	{
		LocalHttpServer server;
		QVERIFY(server.server.listen(QHostAddress::LocalHost));
		TestBouchot board;
		auto settings = board.settings();
		settings.setBackendUrl(server.url());
		settings.setPostUrl(server.url());
		settings.setPostData("message=%m");
		board.setSettings(settings);
		auto *manager = board.findChild<QNetworkAccessManager *>();
		QVERIFY(manager != nullptr);
		QCOMPARE(manager->transferTimeout(), 60000);
		QSignalSpy finished(&board, &QQBouchot::refreshOK);
		int errors = 0;
		connect(&board, &QQBouchot::refreshError, this, [&errors](QString &) { ++errors; });
		board.fetchBackend();
		QTRY_COMPARE(server.requests.size(), 1);
		server.respond(0, "HTTP/1.1 200 OK\r\nSet-Cookie: session=test-token; Path=/\r\nContent-Type: text/tab-separated-values\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
		QTRY_COMPARE(finished.count(), 1);
		board.fetchBackend();
		QTRY_COMPARE(server.requests.size(), 2);
		server.respond(1, "HTTP/1.1 503 Service Unavailable\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
		QTRY_COMPARE(errors, 1);
		QCOMPARE(board.findChild<QNetworkAccessManager *>(), manager);
		board.postMessage("message");
		QTRY_COMPARE(server.requests.size(), 3);
		QVERIFY(server.requests.at(2).startsWith("POST "));
		QVERIFY(server.requests.at(2).contains("session=test-token"));
		server.respond(2, m_success);
		QTRY_COMPARE(server.requests.size(), 4);
		server.respond(3, m_success);
		QTRY_COMPARE(finished.count(), 2);
	}
};

QTEST_MAIN(BouchotBehaviorTests)
#include "tst_qqbouchot.moc"
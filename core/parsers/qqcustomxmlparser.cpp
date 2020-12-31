#include "qqcustomxmlparser.h"

#include <QBuffer>

constexpr static auto POST_ATTR_START_MARK = "<post ";
constexpr static auto POST_ATTR_END_MARK = "</post>";
constexpr static auto TIME_ATTR_START_MARK = "time=\"";
constexpr static auto TIME_ATTR_END_MARK = "\"";
constexpr static auto ID_ATTR_START_MARK = "id=\"";
constexpr static auto ID_ATTR_END_MARK = "\"";
constexpr static auto INFO_TAG_START_MARK = "<info>";
constexpr static auto INFO_TAG_END_MARK = "</info>";
constexpr static auto MESSAGE_TAG_START_MARK = "<message>";
constexpr static auto MESSAGE_TAG_END_MARK = "</message>";
constexpr static auto LOGIN_TAG_START_MARK = "<login>";
constexpr static auto LOGIN_TAG_END_MARK = "</login>";


QQCustomXmlParser::QQCustomXmlParser(QObject *parent) : QQBackendParser(parent)
{

}

bool QQCustomXmlParser::parseBackend(const QByteArray &data)
{
	QBuffer f;
	f.setData(data);
	f.open(QIODevice::ReadOnly);

	while (f.canReadLine())
	{
		QByteArray l = f.readLine();

		auto indexStartOfPost = l.indexOf(POST_ATTR_START_MARK);
		while (indexStartOfPost >= 0)
		{
			QBuffer postBuffer;
			postBuffer.open(QBuffer::ReadWrite);
			postBuffer.write(l);

			while (f.canReadLine())
			{
				auto indexEndOfPost = l.indexOf(POST_ATTR_END_MARK);
				if (indexEndOfPost >= 0)
				{
					if(parsePost(postBuffer.data()))
						emit newPostReady(m_currentPost);
					postBuffer.close();
					postBuffer.setData("");
					postBuffer.open(QBuffer::ReadWrite);
					break;
				}

				l = f.readLine();
				postBuffer.write(l);
			}

			indexStartOfPost = l.indexOf(POST_ATTR_START_MARK, indexStartOfPost + strlen(POST_ATTR_START_MARK));
		}
	}

	m_lastId = m_maxId;

	f.close();
	emit finished();

	return true;
}

bool QQCustomXmlParser::parsePost(const QByteArray &data)
{
	m_currentPost.reset();

	// Extract Id
	auto indexStart = data.indexOf(ID_ATTR_START_MARK);
	if (indexStart < 0)
		return false;
	indexStart += strlen(ID_ATTR_START_MARK);

	auto indexEnd = data.indexOf(ID_ATTR_END_MARK, indexStart);
	if (indexEnd < 0)
		return false;
	auto postId = data.mid(indexStart, indexEnd - indexStart);

	bool ok = true;
	qlonglong id = postId.toLongLong(&ok);
	if(!ok || id <= m_lastId)
		return false;
	if(id > m_maxId)
		m_maxId = id;

	m_currentPost.setId(postId);

	// Extract Norloge
	indexStart = data.indexOf(TIME_ATTR_START_MARK);
	if (indexStart < 0)
		return false;
	indexStart += strlen(TIME_ATTR_START_MARK);

	indexEnd = data.indexOf(TIME_ATTR_END_MARK, indexStart);
	if (indexEnd < 0)
		return false;
	m_currentPost.setNorloge(data.mid(indexStart, indexEnd - indexStart));

	// Extract login
	indexStart = data.indexOf(LOGIN_TAG_START_MARK);
	if (indexStart < 0)
		return false;
	indexStart += strlen(LOGIN_TAG_START_MARK);

	indexEnd = data.indexOf(LOGIN_TAG_END_MARK, indexStart);
	if (indexEnd < 0)
		return false;
	m_currentPost.setLogin(data.mid(indexStart, indexEnd - indexStart));

	// Extract info
	indexStart = data.indexOf(INFO_TAG_START_MARK);
	if (indexStart < 0)
		return false;
	indexStart += strlen(INFO_TAG_START_MARK);

	indexEnd = data.indexOf(INFO_TAG_END_MARK, indexStart);
	if (indexEnd < 0)
		return false;
	m_currentPost.setUA(data.mid(indexStart, indexEnd - indexStart));

	// Extract message
	indexStart = data.indexOf(MESSAGE_TAG_START_MARK);
	if (indexStart < 0)
		return false;
	indexStart += strlen(MESSAGE_TAG_START_MARK);

	indexEnd = data.indexOf(MESSAGE_TAG_END_MARK, indexStart);
	if (indexEnd < 0)
		return false;
	auto rawMessage = data.mid(indexStart, indexEnd - indexStart);

	if (m_typeSlip == QQBouchot::SlipTagsEncoded)
		m_currentPost.setMessage(
		            QString(rawMessage).
		                replace("&lt;", "<").
		                replace("&gt;", ">").
		                replace("&quote;", "\"").
		                replace("&amp;", "&")
		            );
	else
		m_currentPost.setMessage(rawMessage);

	qDebug() << Q_FUNC_INFO << m_currentPost.message();

	return true;
}

#include "qqpostparser.h"

#include "core/qqnorlogeref.h"
#include "core/qqsettings.h"
#include "ui/qqmessageblockuserdata.h"
#include "ui/pinipede/qqpiniurlhelper.h"

#include <QtDebug>
#include <QRegularExpression>
#include <QString>
#include <QTextCursor>
#include <QTextDocument>

#define NORLOGE_COLOR "#0000DD"
#define NORLOGE_REP_COLOR "#DD0000"
#define DUCK_COLOR "#9933cc"
#define TOTOZ_COLOR "#00AA11"

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::QQPostParser
/// \param parent
///
QQPostParser::QQPostParser(QObject *parent) :
    QObject(parent)
{
	QQSettings settings;
	if(settings.value(SETTINGS_FILTER_SMART_URL_TRANSFORMER, DEFAULT_FILTER_SMART_URL_TRANSFORMER).toBool())
		m_listMessageTransformFilters.append(new QQPiniUrlHelper(this));

	m_indexShit = 0;
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::formatMessage
/// \param post
/// \return
///
QTextDocumentFragment* QQPostParser::formatMessage(QQPost *post, QQMessageBlockUserData *userData)
{
	QString message = applyMessageTransformFilters(post->message(), post->bouchot()->name(), post->date());
	QList<QTextDocumentFragment> msgFragments = splitMessage(message, post, userData);
	applyMessageFragmentTransformFilters(msgFragments, post->bouchot()->name(), post->date());
	QTextDocument doc;
	QTextCursor cursor(&doc);
	foreach (QTextDocumentFragment fragment, msgFragments) {
		cursor.insertFragment(fragment);
	}

	colorizeBigorno(doc, post, userData);
	colorizeDuck(doc, userData);
	colorizeTableVolante(doc, userData);
	colorizeTotoz(doc, userData);
	detectLecon(doc, userData);

	return new QTextDocumentFragment(&doc);
}

//////////////////////////////////////////////////////////
/// \brief QQPostParser::splitMessage
/// \param message
/// \param post
/// \param userData
/// \return
///
QList<QTextDocumentFragment> QQPostParser::splitMessage(const QString &message, QQPost *post, QQMessageBlockUserData *userData)
{
	QList<QTextDocumentFragment> res;

	if(message.length() > 0)
	{
		QTextDocument doc;
		doc.setHtml(message);
		QTextCursor docCursor(&doc);
		QTextCursor norlogeCursor(&doc);

		QTextCharFormat fmt = docCursor.blockCharFormat();
		fmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
		fmt.setAnchor(true);

		QRegularExpression norlogeReg = QQNorlogeRef::norlogeRegexp();
		while(! (norlogeCursor = doc.find(norlogeReg, norlogeCursor)).isNull())
		{
			QQNorlogeRef nRef = QQNorlogeRef(*post, norlogeCursor.selectedText());
			linkNorlogeRef(&nRef);
			int index = userData->appendNorlogeRef(nRef);

			fmt.setForeground(nRef.isReponse() ? QColor(NORLOGE_REP_COLOR) : QColor(NORLOGE_COLOR));
			fmt.setFontWeight(nRef.isReponse() ? QFont::DemiBold : QFont::Normal);

			QString nRefUrl = QString("nref://bouchot?board=%1&postId=%2&index=%3")
			                  .arg(post->bouchot()->name(), post->id(), QString::number(index));
			fmt.setAnchorHref(nRefUrl);

			norlogeCursor.mergeCharFormat(fmt);

			int selectionStart = norlogeCursor.selectionStart();
			if(selectionStart != 0)
			{
				docCursor.setPosition(selectionStart, QTextCursor::KeepAnchor);
				if(! docCursor.selection().isEmpty())
					res.append(docCursor.selection());
				docCursor.setPosition(selectionStart, QTextCursor:: MoveAnchor);
			}
		}
		docCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		if(! docCursor.selection().isEmpty())
			res.append(docCursor.selection());
	}
	else
		res.append(QTextDocumentFragment());

	return res;
}

//////////////////////////////////////////////////////////
/// \brief QQPostParser::applyMessageFragmentTransformFilters
/// \param listMsgFragments
/// \param bouchot
///
void QQPostParser::applyMessageFragmentTransformFilters(QList<QTextDocumentFragment> &listMsgFragments, const QString &bouchot, const QDate &postDate)
{
	Q_UNUSED(bouchot);

	// Rectify ISO 9601 norloges
	QTextDocument tmp;
	for(int i = 0; i < listMsgFragments.size(); i++)
	{
		tmp.clear();
		QTextCursor c(&tmp);
		c.insertFragment(listMsgFragments[i]);
		c.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);

		auto moveSuccess = c.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 5);
		QString st = c.selectedText();
		// If it starts with "....-" pattern, it is a ISO norloge. Let's reformat it
		if(moveSuccess && c.charFormat().anchorHref().startsWith("nref://") &&
		    st.at(4) == QChar('-'))
		{
			c.beginEditBlock();

			if(st.left(4) == QString::number(postDate.year()))
				c.removeSelectedText();
			else
				c.insertText(QString("%1/").arg(st.left(4)));

			// Process next "..-..T" characters
			c.setPosition(c.anchor());
			c.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 6);
			st = c.selectedText();
			if(st.left(2) == QString("%1").arg(postDate.month(), 2, 10, QChar('0')) &&
			    st.mid(3, 2) == QString("%1").arg(postDate.day(), 2, 10, QChar('0')))
				c.removeSelectedText();
			else
				c.insertText(QString("%1/%2#").arg(st.left(2), st.mid(3, 2)));

			c.endEditBlock();
			listMsgFragments[i] = QTextDocumentFragment(&tmp);
		}
	}
}

//////////////////////////////////////////////////////////
/// \brief QQPostParser::applyMessageTransformFilters
/// \param message
/// \param bouchot
/// \param data
/// \return
///
QString QQPostParser::applyMessageTransformFilters(const QString &message, const QString &bouchot, const QDate &postDate)
{
	Q_UNUSED(postDate);
	QString newMessage = message;

	QQMessageTransformFilter *messageTransformFilter;
	foreach(messageTransformFilter, m_listMessageTransformFilters)
		messageTransformFilter->transformMessage(bouchot, newMessage);

	return newMessage;
}


//////////////////////////////////////////////////////////////
/// \brief QQPostParser::colorizeBigorno
/// \param doc
/// \param post
/// \param data
///
void QQPostParser::colorizeBigorno(QTextDocument &doc, QQPost *post, QQMessageBlockUserData *userData)
{
	QString id = post->bouchot()->settings().login();
	if(id.length() == 0)
		id = post->bouchot()->settings().ua();

	QString bigorno = QString::fromLatin1("\\b(?:");
	if(id.length() > 0)
		bigorno.append(id).append("|");
	bigorno.append("moules)<");

	QRegularExpression bigornoReg = QRegularExpression(bigorno, QRegularExpression::CaseInsensitiveOption);

	QTextCursor cursor(&doc);
	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(Qt::red));

	while(! (cursor = doc.find(bigornoReg, cursor)).isNull())
	{
		QQBigornoItem bigItem(m_indexShit + cursor.selectionStart(), cursor.selectedText());
		userData->addBigornoZone(bigItem);

		QString callerId = post->login();
		if(callerId.length() == 0)
			callerId = post->UA();

		QString name = post->bouchot()->name();

		emit bigorNotify(name, callerId,
		                 (cursor.selectedText().compare("moules<", Qt::CaseInsensitive) == 0));
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::colorizeDuck
/// \param doc
/// \param post
/// \param data
///
void QQPostParser::colorizeDuck(QTextDocument &doc, QQMessageBlockUserData *userData)
{
	QString tete = QString::fromLatin1("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");

	QList<QRegularExpression> regexes;
	regexes << QRegularExpression(QString::fromLatin1("\\\\_").append(tete).append(QString::fromLatin1("<")), QRegularExpression::NoPatternOption);

	regexes << QRegularExpression(QString::fromLatin1(">").append(tete).append(QString::fromLatin1("_\\/")), QRegularExpression::NoPatternOption);

	regexes << QRegularExpression(QString::fromLatin1("coin ?! ?coin ?!"), QRegularExpression::NoPatternOption);

	QTextCursor cursor(&doc);

	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(DUCK_COLOR));

	QQPost *post = userData->post();

	foreach(QRegularExpression reg, regexes)
	{
		cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);

		while(! (cursor = doc.find(reg, cursor)).isNull())
		{
			QString duckUrl = QString("duck://bouchot?board=%1&postId=%2&self=%3")
			                  .arg(post->bouchot()->name(), post->id(),
			                       post->isSelfPost() ? "true" : "false");
			fmt.setAnchorHref(duckUrl);
			cursor.mergeCharFormat(fmt);
			userData->setHasDuck();
		}
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::linkNorlogeRef
/// \param nRef
///
void QQPostParser::linkNorlogeRef(QQNorlogeRef *nRef)
{
	QQBouchot *bouchot = QQBouchot::bouchot(nRef->dstBouchot());
	if(bouchot == NULL)
		return;

	QQListPostPtr history = bouchot->postsHistory();

	// Parcourir du plus recent au plus ancien devrait etre plus rapide car
	// les reponse sont souvent proches du poste d'origine;
	bool targetFound = false;
	for(int i = history.length() - 1; i >= 0; i--)
	{
		QQPost *post = history.at(i);

		if(nRef->matchesPost(post))
			targetFound = true;
		else if(targetFound) // On ne quitte pas avant au cas de match multiple
			break;
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::colorizeTableVolante
/// \param doc
/// \param post
/// \param data
///
void QQPostParser::colorizeTableVolante(QTextDocument &doc, QQMessageBlockUserData *userData)
{
	QRegularExpression tvReg = QRegularExpression(QString::fromLatin1("(?:flap ?flap)|(?:table[ _]volante)"), QRegularExpression::NoPatternOption);

	QTextCursor cursor(&doc);
	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(DUCK_COLOR));

	QQPost *post = userData->post();

	while(! (cursor = doc.find(tvReg, cursor)).isNull())
	{
		QString duckUrl = QString("tablev://bouchot?board=%1&postId=%2&self=%3")
		                  .arg(post->bouchot()->name(), post->id(), post->isSelfPost() ? "true" : "false");
		fmt.setAnchorHref(duckUrl);
		cursor.mergeCharFormat(fmt);
	}
}

#define MAX_TOTOZ_PREFETCH_POST 3
#define MAX_LAST_TOTOZ_IDS_COUNT 50
//////////////////////////////////////////////////////////////
/// \brief QQPostParser::colorizeTotoz
/// \param doc
/// \param post
/// \param data
///
void QQPostParser::colorizeTotoz(QTextDocument &doc, QQMessageBlockUserData *userData)
{
	Q_UNUSED(userData);

	QRegularExpression totozReg = QRegularExpression(QString::fromLatin1("(\\[\\:[^\\t\\)\\]]+\\])"), QRegularExpression::NoPatternOption); //[:[^\t\)\]]


	QTextCursor cursor(&doc);
	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(TOTOZ_COLOR));
	fmt.setFontWeight(QFont::Bold);
	fmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
	fmt.setAnchor(true);

	int totozCount = 0;
	while(! (cursor = doc.find(totozReg, cursor)).isNull())
	{
		QString totozId = cursor.selectedText();

		QString totozName = totozId.mid(2, totozId.length() - 3);
		fmt.setAnchorHref(QString("totoz://name/%1").arg(totozName));
		//Antiflood : Maximum 3 requetes et sur des totoz différents
		if(!lastTotozIds.contains(totozName) && totozCount <= MAX_TOTOZ_PREFETCH_POST)
		{
			totozCount++;
			lastTotozIds.enqueue(totozName);
			while(lastTotozIds.count() > MAX_LAST_TOTOZ_IDS_COUNT)
				lastTotozIds.dequeue();

			emit totozRequired(totozName);
		}

		cursor.mergeCharFormat(fmt);
	}
}

#define LECON_BASE_URL "http://lecons.ssz.fr/lecon/"
//////////////////////////////////////////////////////////////
/// \brief QQPostParser::detectLecon
/// \param doc
/// \param userData
///
void QQPostParser::detectLecon(QTextDocument &doc, QQMessageBlockUserData *userData)
{
	Q_UNUSED(userData)
	QRegularExpression totozReg = QRegularExpression(QString::fromUtf8("(le[cç]on\\s+\\d+)"), QRegularExpression::CaseInsensitiveOption);

	QTextCursor cursor(&doc);
	while(! (cursor = doc.find(totozReg, cursor)).isNull())
	{
		QString numLesson = cursor.selectedText().split(QRegularExpression("\\s")).at(1);
		QString url = QString::fromLatin1("<a href=\"")
		              .append(LECON_BASE_URL)
		              .append(numLesson)
		              .append(QString::fromLatin1("\">"))
		              .append(cursor.selectedText())
		              .append(QString::fromLatin1("</a>"));
		cursor.insertHtml(url);
	}
}

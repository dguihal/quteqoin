#include "qqpostparser.h"

#include "core/qqnorlogeref.h"
#include "core/qqpiniurlhelper.h"
#include "core/qqsettings.h"
#include "ui/qqmessageblockuserdata.h"

#include <QRegExp>
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
/// \brief QQPostParser::applyMessageTransformFilters
/// \param message
/// \param bouchot
/// \param data
/// \return
///
QString QQPostParser::applyMessageTransformFilters(const QString &message, const QString &bouchot)
{
	QString newMessage = message;

	QQMessageTransformFilter *messageTransformFilter;
	foreach(messageTransformFilter, m_listMessageTransformFilters)
		messageTransformFilter->transformMessage(bouchot, newMessage);

	return newMessage;
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::formatMessage
/// \param post
/// \return
///
QTextDocumentFragment* QQPostParser::formatMessage(QQPost *post, QQMessageBlockUserData *userData)
{
	QTextDocument doc;

	QString message = applyMessageTransformFilters(post->message(), post->bouchot()->name());
	doc.setHtml(message);

	colorizeBigorno(doc, post, userData);
	colorizeDuck(doc, userData);
	colorizeNRef(doc, post, userData);
	colorizeTableVolante(doc, userData);
	colorizeTotoz(doc, userData);

	return new QTextDocumentFragment(&doc);
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

	QString bigorno = QString::fromAscii("\\b(?:");
	if(id.length() > 0)
		bigorno.append(id).append("|");
	bigorno.append("moules)<");

	QRegExp m_bigornoReg = QRegExp(bigorno,
								   Qt::CaseInsensitive,
								   QRegExp::RegExp);

	QTextCursor cursor(&doc);
	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(Qt::red));

	while(! (cursor = doc.find(m_bigornoReg, cursor)).isNull())
	{
		QQBigornoItem bigItem(m_indexShit + cursor.selectionStart(), cursor.selectedText());
		userData->addBigornoZone(bigItem);

		emit notifyBigorno();

		cursor.movePosition(QTextCursor::NextCharacter);
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
	QString tete = QString::fromAscii("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");

	QList<QRegExp> regexes;
	regexes << QRegExp(QString::fromAscii("\\\\_").append(tete).append(QString::fromAscii("<")),
								Qt::CaseSensitive,
								QRegExp::RegExp);

	regexes << QRegExp(QString::fromAscii(">").append(tete).append(QString::fromAscii("_\\/")),
					   Qt::CaseSensitive,
					   QRegExp::RegExp);

	regexes << QRegExp(QString::fromAscii("coin ?! ?coin ?!"),
					   Qt::CaseSensitive,
					   QRegExp::RegExp);

	QTextCursor cursor(&doc);

	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(DUCK_COLOR));

	foreach(QRegExp reg, regexes)
	{
		cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);

		while(! (cursor = doc.find(reg, cursor)).isNull())
		{
			userData->addDuckZone(m_indexShit + cursor.selectionStart(), cursor.selectedText());
			cursor.mergeCharFormat(fmt);
			cursor.movePosition(QTextCursor::NextCharacter);
		}
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::colorizeNRef
/// \param doc
/// \param post
/// \param data
///
void QQPostParser::colorizeNRef(QTextDocument &doc, QQPost *post, QQMessageBlockUserData *userData)
{
	QRegExp m_norlogeReg = QQNorlogeRef::norlogeRegexp();

	QTextCursor cursor = doc.find(m_norlogeReg, QTextCursor(&doc));
	QTextCharFormat repFmt = cursor.blockCharFormat();
	repFmt.setForeground(QColor(NORLOGE_REP_COLOR));
	repFmt.setFontWeight(QFont::DemiBold);

	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(NORLOGE_COLOR));

	while(! cursor.isNull())
	{
		QQNorlogeRef nRef = QQNorlogeRef(post->bouchot()->name(),
										 post->norloge(),
										 cursor.selectedText(),
										 m_indexShit + cursor.selectionStart());
		linkNorlogeRef(&nRef);
		userData->addNorlogeRefZone(nRef);

		cursor.mergeCharFormat(nRef.isReponse() ? repFmt : fmt);
		cursor.movePosition(QTextCursor::NextCharacter);

		cursor = doc.find(m_norlogeReg, cursor);
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::linkNorlogeRef
/// \param nRef
///
void QQPostParser::linkNorlogeRef(QQNorlogeRef *nRef)
{
		QQBouchot * bouchot = QQBouchot::bouchot(nRef->dstBouchot());
		if(bouchot == NULL)
				return;

		QList<QQPost *> history = bouchot->getPostsHistory();

		// Parcourir du plus recent au plus ancien devrait etre plus rapide car
		// les reponse sont souvent proches du poste d'origine;
		bool targetFound = false;
		for(int i = history.length() - 1; i >= 0; i--)
		{
			QQPost * post = history.at(i);

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
	QRegExp m_tvReg = QRegExp(QString::fromAscii("(?:flap ?flap)|(?:table[ _]volante)"),
							  Qt::CaseSensitive,
							  QRegExp::RegExp);

	QTextCursor cursor(&doc);
	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(DUCK_COLOR));

	while(! (cursor = doc.find(m_tvReg, cursor)).isNull())
	{
		userData->addTableVZone(m_indexShit + cursor.selectionStart(), cursor.selectedText());
		cursor.mergeCharFormat(fmt);
		cursor.movePosition(QTextCursor::NextCharacter);
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPostParser::colorizeTotoz
/// \param doc
/// \param post
/// \param data
///
void QQPostParser::colorizeTotoz(QTextDocument &doc, QQMessageBlockUserData *userData)
{
	QRegExp m_totozReg = QRegExp(QString::fromAscii("(\\[\\:[^\\t\\)\\]]+\\])"), //[:[^\t\)\]]
								 Qt::CaseSensitive,
								 QRegExp::RegExp);

	QTextCursor cursor(&doc);
	QTextCharFormat fmt = cursor.blockCharFormat();
	fmt.setForeground(QColor(TOTOZ_COLOR));
	fmt.setFontWeight(QFont::Bold);

	while(! (cursor = doc.find(m_totozReg, cursor)).isNull())
	{
		QString totozId = cursor.selectedText();
		userData->addTotozZone(m_indexShit + cursor.selectionStart(), totozId);

		QString totozName = totozId.mid(2, totozId.length() - 3);
		emit totozRequired(totozName);

		cursor.mergeCharFormat(fmt);
		cursor.movePosition(QTextCursor::NextCharacter);
	}
}

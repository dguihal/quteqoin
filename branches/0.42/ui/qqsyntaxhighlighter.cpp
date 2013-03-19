#include "qqsyntaxhighlighter.h"

#include "core/qqbigornoitem.h"
#include "core/qqbouchot.h"
#include "core/qqnorlogeref.h"
#include "core/qqpost.h"
#include "core/qqtotozdownloader.h"
#include "ui/qqmessageblockuserdata.h"

#include <QDateTime>
#include <QtDebug>
#include <QIcon>
#include <QRegExp>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QWidget>

QQSyntaxHighlighter::QQSyntaxHighlighter(QQSettings * settings, QTextDocument * parent) :
	QSyntaxHighlighter(parent)
{
	m_notifWindow = NULL;
	m_settings = settings;
}

QQSyntaxHighlighter::~QQSyntaxHighlighter()
{
}

void QQSyntaxHighlighter::highlightBlock(const QString &text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *)(currentBlockUserData());

	setCurrentBlockState(QQSyntaxHighlighter::NOT_HIGHLIGHTED);
	if(text.length() > 1 &&
	   userData != NULL)
	{
		setCurrentBlockState(QQSyntaxHighlighter::NORMAL);

		highlightBlockForNRef();
		highlightNorloge(text);
		highlightDuck(text);
		highlightTableVolante(text);
		highlightTotoz(text);
		highlightBigorno(text);

		userData->setParsed();
	}
}

void QQSyntaxHighlighter::highlightBlockForNRef()
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	QTextBlockFormat fmt = currentBlock().blockFormat();
	fmt.setBackground(userData->post()->bouchot()->settings().colorLight());

	if(m_nRef.matchesPost(userData->post()))
	{
		fmt.setBackground(QColor("#FFE940"));
		int blockState = currentBlockState();
		blockState |= QQSyntaxHighlighter::FULL_HIGHLIGHTED;
		setCurrentBlockState(blockState);
	}

	QTextCursor curs(currentBlock());
	curs.beginEditBlock();
	curs.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	curs.mergeBlockFormat(fmt);
	curs.endEditBlock();
}

void QQSyntaxHighlighter::highlightNorloge(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	if(userData->wasParsed())
	{
		QList<QQNorlogeRef> nRefs = userData->norlogeRefs();
		for(int i = 0; i < nRefs.size(); i ++)
		{
			QQNorlogeRef nRef = nRefs.at(i);
			formatNRef(nRef);
		}
	}
	else
	{
		QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

		QRegExp m_norlogeReg = QQNorlogeRef::norlogeRegexp();

		int index = text.indexOf(m_norlogeReg, messageRange.begin);
		while (index >= 0)
		{
			int length = m_norlogeReg.matchedLength();

			QQNorlogeRef nRef = QQNorlogeRef(userData->post()->bouchot()->name(),
											 userData->post()->norloge(),
											 text.mid(index, length),
											 index);

			linkNorlogeRef(nRef);
			userData->addNorlogeRefZone(nRef);
			formatNRef(nRef);

			index = text.indexOf(m_norlogeReg, index + length);
		}
	}
}

void QQSyntaxHighlighter::linkNorlogeRef(QQNorlogeRef & nRef)
{
	QQBouchot * bouchot = m_settings->bouchot(nRef.dstBouchot());
	if(bouchot == NULL)
		return;

	QList<QQPost *> history = bouchot->getPostsHistory();

	// Parcourir du plus recent au plus ancien devrait etre plus rapide car
	// les reponse sont souvent proches du poste d'origine;
	bool targetFound = false;
	for(int i = history.length() - 1; i >= 0; i--)
	{
		QQPost * post = history.at(i);

		if(nRef.matchesPost(post))
		{
			targetFound = true;
		}
		else if(targetFound)
		{
			// On a deja trouve un cible, inutile d'aller plus loin
			// On ne quitte pas avant au cas de match multiple
			break;
		}
	}
}

void QQSyntaxHighlighter::formatNRef(QQNorlogeRef & nRef)
{
	QColor highlightColor("#FFE940");
	QColor printColor("#0000DD");
	QColor repColor("#DD0000");

	QTextCharFormat fmt = format(nRef.getPosInMessage());

	if(nRef.isReponse())
	{
		fmt.setForeground(repColor);
		fmt.setFontWeight(QFont::DemiBold);
	}
	else
		fmt.setForeground(printColor);

	if(m_nRef.matchesNRef(nRef))
	{
		fmt.setBackground(highlightColor);
		int blockState = currentBlockState();
		blockState |= QQSyntaxHighlighter::NORLOGE_HIGHLIGHTED;
		setCurrentBlockState(blockState);
	}

	setFormat(nRef.getPosInMessage(), nRef.getOrigNRef().length(), fmt);
}

void QQSyntaxHighlighter::highlightDuck(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	if(userData->wasParsed())
	{
		QList<int> duckIndexes = userData->duckIndexes();
		for(int i = 0; i < duckIndexes.size(); i ++)
		{
			int duckIndex = duckIndexes.at(i);
			formatDuck(duckIndex, userData->duckForIndex(duckIndex).second.length());
		}
	}
	else
	{
		QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

		QString tete = QString::fromAscii("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");

		QRegExp m_duckReg = QRegExp(QString::fromAscii("\\\\_").append(tete).append(QString::fromAscii("<")),
									Qt::CaseSensitive,
									QRegExp::RegExp);

		int index = text.indexOf(m_duckReg, messageRange.begin );
		while (index >= 0)
		{
			int length = m_duckReg.matchedLength();

			userData->addDuckZone(index, text.mid(index, length));
			formatDuck(index, length);

			index = text.indexOf(m_duckReg, index + length);
		}

		m_duckReg = QRegExp(QString::fromAscii(">").append(tete).append(QString::fromAscii("_\\/")),
							Qt::CaseSensitive,
							QRegExp::RegExp);
		index = text.indexOf(m_duckReg, messageRange.begin );
		while (index >= 0) {
			int length = m_duckReg.matchedLength();

			userData->addDuckZone(index, text.mid(index, length));
			formatDuck(index, length);

			index = text.indexOf(m_duckReg, index + length);
		}

		m_duckReg = QRegExp(QString::fromAscii("coin ?! ?coin ?!"),
							Qt::CaseSensitive,
							QRegExp::RegExp);

		index = text.indexOf(m_duckReg, messageRange.begin );
		while (index >= 0)
		{
			int length = m_duckReg.matchedLength();

			if(! userData->wasParsed())
				userData->addDuckZone(index, text.mid(index, length));

			formatDuck(index, length);

			index = text.indexOf(m_duckReg, index + length);
		}
	}
}


void QQSyntaxHighlighter::formatDuck(int duckIndex, int duckStringLength)
{
	QColor color = QColor("#9933CC");
	setFormat(duckIndex, duckStringLength, color);
}

void QQSyntaxHighlighter::highlightTableVolante(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	if(userData->wasParsed())
	{
		QList<int> tableVIndexes = userData->tableVIndexes();
		for(int i = 0; i < tableVIndexes.size(); i ++)
		{
			int tableVIndex = tableVIndexes.at(i);
			formatTableV(tableVIndex, userData->tableVForIndex(tableVIndex).second.length());
		}
	}
	else
	{
		QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

		QRegExp m_tvReg = QRegExp(QString::fromAscii("(?:flap ?flap)|(?:table[ _]volante)"),
								  Qt::CaseSensitive,
								  QRegExp::RegExp);
		int index = text.indexOf(m_tvReg, messageRange.begin );
		while (index >= 0)
		{
			int length = m_tvReg.matchedLength();

			userData->addTableVZone(index, text.mid(index, length));
			formatTableV(index, length);

			index = text.indexOf(m_tvReg, index + length);
		}
	}
}

void QQSyntaxHighlighter::formatTableV(int tableVIndex, int tableVStringLength)
{
	QColor color = QColor("#9933CC");
	setFormat(tableVIndex, tableVStringLength, color);
}


void QQSyntaxHighlighter::highlightTotoz(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	if(userData->wasParsed())
	{
		QList<int> totozIndexes = userData->totozIndexes();
		for(int i = 0; i < totozIndexes.size(); i ++)
		{
			int index = totozIndexes.at(i);
			formatTotoz(index, userData->totozIdForIndex(index));
		}
	}
	else
	{

		QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

		QRegExp m_totozReg = QRegExp(QString::fromAscii("(\\[\\:[^\\t\\)\\]]+\\])"), //[:[^\t\)\]]
									 Qt::CaseSensitive,
									 QRegExp::RegExp);

		int index = text.indexOf(m_totozReg, messageRange.begin );
		while (index >= 0)
		{
			int length = m_totozReg.matchedLength();

			QString totozId = text.mid(index, length);
			userData->addTotozZone(index, totozId);

			QString totozName = totozId.mid(2, totozId.length() - 3);
			emit totozRequired(totozName);

			formatTotoz(index, totozId);

			index = text.indexOf(m_totozReg, index + length);
		}
	}
}

void QQSyntaxHighlighter::formatTotoz(int index, const QString & totozId)
{
	QTextCharFormat totozMessageFormat = format(index);
	totozMessageFormat.setForeground(QColor("#00AA11"));
	totozMessageFormat.setFontWeight(QFont::Bold);

	setFormat(index, totozId.length(), totozMessageFormat);

}

void QQSyntaxHighlighter::highlightBigorno(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	if(userData->wasParsed())
		return;

	QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

	QString login = userData->post()->bouchot()->settings().login();
	QString bigorno = QString::fromAscii("\\b(?:");
	if(login.length() > 0)
		bigorno.append(login).append("|");
	bigorno.append("moules)<");

	QRegExp m_bigornoReg = QRegExp(bigorno,
								   Qt::CaseInsensitive,
								   QRegExp::RegExp);

	int index = text.indexOf(m_bigornoReg, messageRange.begin );
	while (index >= 0)
	{
		int length = m_bigornoReg.matchedLength();
		QQBigornoItem bigItem(index, text.mid(index, length));

		if(m_notifWindow != NULL)
		{
			QIcon icon = QIcon(QString::fromAscii(":/img/Point_exclamation_rouge.svg"));
			m_notifWindow->setWindowIcon(icon);
			bigItem.setNotified();
		}
		userData->addBigornoZone(bigItem);

		index = text.indexOf(m_bigornoReg, index + length);
	}
}

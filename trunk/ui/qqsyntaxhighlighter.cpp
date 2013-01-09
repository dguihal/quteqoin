#include "qqsyntaxhighlighter.h"

#include "core/qqbigornoitem.h"
#include "core/qqbouchot.h"
#include "core/qqnorlogeref.h"
#include "core/qqpost.h"
#include "core/qqtotozmanager.h"
#include "ui/qqmessageblockuserdata.h"

#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QRegExp>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QWidget>

QQSyntaxHighlighter::QQSyntaxHighlighter(QTextDocument * parent) :
	QSyntaxHighlighter(parent)
{
	m_notifWindow = NULL;
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

	QTextBlockFormat fmt;
	fmt.setBackground(userData->post()->bouchot()->settings().colorLight());

	if(m_nRef.isValid())
	{
		QString dstNorloge = m_nRef.dstNorloge();
		QString dstBouchot = m_nRef.dstBouchot();

		QString currNorloge = userData->post()->norloge();
		QQBouchot * currBouchot = userData->post()->bouchot();

		if( ( dstBouchot == currBouchot->name() || currBouchot->settings().containsAlias(dstBouchot) ) &&
				currNorloge.startsWith(dstNorloge) )
		{
			fmt.setBackground(QColor("#FFE940"));
			setCurrentBlockState(QQSyntaxHighlighter::FULL_HIGHLIGHTED);
		}
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
			formatNRef(nRefs.at(i));
	}
	else
	{
		QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

		QRegExp m_norlogeReg = QQNorlogeRef::norlogeRegexp();

		int index = text.indexOf(m_norlogeReg, messageRange.begin );
		while (index >= 0)
		{
			int length = m_norlogeReg.matchedLength();

			QQNorlogeRef nRef = QQNorlogeRef(userData->post()->bouchot()->name(),
											 userData->post()->norloge(),
											 text.mid(index, length),
											 index);

			userData->addNorlogeRefZone(nRef);
			formatNRef(nRef);

			index = text.indexOf(m_norlogeReg, index + length);
		}
	}
}

void QQSyntaxHighlighter::formatNRef(const QQNorlogeRef & nRef)
{
	QColor highlightColor("#FFE940");
	QColor printColor("#0000DD");

	QTextCharFormat fmt = format(0);
	fmt.setForeground(printColor);
	QTextCharFormat fmtH = fmt;
	fmtH.setBackground(highlightColor);

	if(m_nRef == nRef)
	{
		setCurrentBlockState(QQSyntaxHighlighter::NORLOGE_HIGHLIGHTED);
		setFormat(nRef.getPosInMessage(), nRef.getOrigNRef().length(), fmtH);
	}
	else
	{
		setFormat(nRef.getPosInMessage(), nRef.getOrigNRef().length(), fmt);
	}
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
		QList<QQTotoz> totozes = userData->totozZones();
		for(int i = 0; i < totozes.size(); i ++)
			formatTotoz(totozes.at(i));
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

			QQTotoz totoz = QQTotoz(text.mid(index, length), index);
			emit totozRequired(totoz.getId());
			userData->addTotozZone(totoz);

			index = text.indexOf(m_totozReg, index + length);
		}
	}
}

void QQSyntaxHighlighter::formatTotoz(const QQTotoz & totoz)
{
	QTextCharFormat totozMessageFormat;
	totozMessageFormat.setForeground(QColor("#00AA11"));
	totozMessageFormat.setFontWeight(QFont::Bold);

	setFormat(totoz.getPosInMessage(), totoz.getOrigString().length(), totozMessageFormat);

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
								   Qt::CaseSensitive,
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

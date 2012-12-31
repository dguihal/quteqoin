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
	//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
	//		 << "QQSyntaxHighlighter::highlightBlock";

	QQMessageBlockUserData * userData = (QQMessageBlockUserData *)(currentBlockUserData());

	setCurrentBlockState(QQSyntaxHighlighter::NOT_HIGHLIGHTED);
	if(text.length() > 1 &&
			userData != NULL)
	{
		//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
		//		 << "QQSyntaxHighlighter::highlightBlock text=" << text;
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

	QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

	QColor highlightColor("#FFE940");

	QTextCharFormat fmt = format(0);
	fmt.setForeground(QColor("#0000DD"));
	QTextCharFormat fmtH = fmt;
	fmtH.setBackground(highlightColor);


	QRegExp m_norlogeReg = QQNorlogeRef::norlogeRegexp();

	int index = text.indexOf(m_norlogeReg, messageRange.begin );
	while (index >= 0)
	{
		int length = m_norlogeReg.matchedLength();
		//Q_ASSERT(userData->post() != NULL);
		QQNorlogeRef nRef = QQNorlogeRef(userData->post()->bouchot()->name(),
										 userData->post()->norloge(),
										 text.mid(index, length),
										 index);
		userData->addNorlogeRefZone(nRef);
		if(m_nRef == nRef)
		{
			setCurrentBlockState(QQSyntaxHighlighter::NORLOGE_HIGHLIGHTED);
			setFormat(index, length, fmtH);
		}
		else
		{
			setFormat(index, length, fmt);
		}

		index = text.indexOf(m_norlogeReg, index + length);
	}
}

void QQSyntaxHighlighter::highlightDuck(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

	QColor color = QColor("#9933CC");

	QString tete = QString::fromAscii("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");

	//QRegExp m_duckReg = QRegExp(QString::fromAscii("(\\\\_").append(tete).append(QString::fromAscii("&lt;)")),
	QRegExp m_duckReg = QRegExp(QString::fromAscii("\\\\_").append(tete).append(QString::fromAscii("<")),
								Qt::CaseSensitive,
								QRegExp::RegExp);

	int index = text.indexOf(m_duckReg, messageRange.begin );
	while (index >= 0) {
		int length = m_duckReg.matchedLength();
		if(! userData->wasParsed())
			userData->addDuckZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_duckReg, index + length);
	}

	//m_duckReg = QRegExp(QString::fromAscii("(&gt;").append(tete).append(QString::fromAscii("_\\/)")),
	m_duckReg = QRegExp(QString::fromAscii(">").append(tete).append(QString::fromAscii("_\\/")),
						Qt::CaseSensitive,
						QRegExp::RegExp);
	index = text.indexOf(m_duckReg, messageRange.begin );
	while (index >= 0) {
		int length = m_duckReg.matchedLength();
		if(! userData->wasParsed())
			userData->addDuckZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_duckReg, index + length);
	}

	m_duckReg = QRegExp(QString::fromAscii("coin ?! ?coin ?!"),
						Qt::CaseSensitive,
						QRegExp::RegExp);

	index = text.indexOf(m_duckReg, messageRange.begin );
	while (index >= 0) {
		int length = m_duckReg.matchedLength();
		if(! userData->wasParsed())
			userData->addDuckZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_duckReg, index + length);
	}
}

void QQSyntaxHighlighter::highlightTableVolante(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

	QColor color = QColor("#9933CC");

	QRegExp m_tvReg = QRegExp(QString::fromAscii("(?:flap ?flap)|(?:table[ _]volante)"),
							  Qt::CaseSensitive,
							  QRegExp::RegExp);
	int index = text.indexOf(m_tvReg, messageRange.begin );
	while (index >= 0) {
		int length = m_tvReg.matchedLength();
		if(! userData->wasParsed())
			userData->addTableVZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_tvReg, index + length);
	}
}

void QQSyntaxHighlighter::highlightTotoz(const QString & text)
{
	QQMessageBlockUserData * userData = (QQMessageBlockUserData *) currentBlockUserData();
	if(userData == NULL || ! userData->isValid())
		return;

	QQMessageBlockUserData::ZoneRange messageRange = userData->zRangeForID(QQMessageBlockUserData::MESSAGE);

	QTextCharFormat totozMessageFormat;
	totozMessageFormat.setForeground(QColor("#00AA11"));
	totozMessageFormat.setFontWeight(QFont::Bold);

	QRegExp m_totozReg = QRegExp(QString::fromAscii("(\\[\\:[^\\t\\)\\]]+\\])"), //[:[^\t\)\]]
								 Qt::CaseSensitive,
								 QRegExp::RegExp);

	int index = text.indexOf(m_totozReg, messageRange.begin );
	while (index >= 0)
	{
		int length = m_totozReg.matchedLength();

		QQTotoz totoz = QQTotoz(text.mid(index, length), index);
		emit totozRequired(totoz.getId());

		if(! userData->wasParsed())
			userData->addTotozZone(totoz);
		setFormat(index, length, totozMessageFormat);

		index = text.indexOf(m_totozReg, index + length);
	}
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

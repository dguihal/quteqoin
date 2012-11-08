#include "qqsyntaxhighlighter.h"

#include "core/qqnorlogeref.h"
#include "core/qqpost.h"
#include "core/qqbouchot.h"
#include "core/qqtotozmanager.h"
#include "ui/qqmessageblockuserdata.h"

#include <QDateTime>
#include <QDebug>
#include <QRegExp>
#include <QTextCharFormat>
#include <QTextCursor>

QQSyntaxHighlighter::QQSyntaxHighlighter(QTextDocument * parent) :
	QSyntaxHighlighter(parent)
{
}

QQSyntaxHighlighter::~QQSyntaxHighlighter()
{
}

void QQSyntaxHighlighter::setNorlogeRefToHighlight(const QQNorlogeRef &norlogeRef)
{
	this->m_nRef = norlogeRef;
}

//void QQSyntaxHighlighter::set

bool QQSyntaxHighlighter::highlightLine(QTextCursor & lineSelection, QQMessageBlockUserData * userData)
{

	QString dstNorloge = m_nRef.dstNorloge();
	QString dstBouchot = m_nRef.dstBouchot();

	QString currNorloge = userData->post()->norloge();
	QQBouchot * currBouchot = userData->post()->bouchot();

	if( ( dstBouchot == currBouchot->name() || currBouchot->settings().containsAlias(dstBouchot) ) &&
			currNorloge.startsWith(dstNorloge) )
	{
		QTextCharFormat format;
		QColor highlightColor;
		highlightColor.setNamedColor("#FFE940");
		format.setBackground(highlightColor);
		//format.setBackground(QColor::fromRgb(255, 210, 02));
		//format.setBackground(Qt::yellow);

		lineSelection.mergeBlockCharFormat(format);
		userData->setHighlighted();
		return true;
	}
	return false;
}

void QQSyntaxHighlighter::rehighlightMessageBlockAtCursor (QTextCursor cursor, QQMessageBlockUserData * userData)
{
	if(userData->blockZone() == QQMessageBlockUserData::MESSAGE_ZONE)
	{
		QTextCharFormat format;
		QColor highlightColor;
		highlightColor.setNamedColor("#FFE940");
		format.setBackground(highlightColor);

		QList<QQNorlogeRef> norlogeRefs = userData->norlogeRefs();

		for (int i = 0; i < norlogeRefs.size(); ++i)
		{
			QQNorlogeRef norlogeRef = norlogeRefs.at(i);
			if(m_nRef == norlogeRef)
			{
				cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
				//int pos1 = cursor.positionInBlock();
				cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, norlogeRef.getPosInMessage());
				//int pos2 = cursor.positionInBlock();
				cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, norlogeRef.getOrigNRef().length());
				//qDebug() << "pos1=" << pos1 << ", pos2=" << pos2 << ", pos3=" << cursor.positionInBlock();
				cursor.mergeCharFormat(format);
				userData->setHighlighted();
			}
		}
	}
}

void QQSyntaxHighlighter::highlightBlock(const QString &text)
{
	//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
	//		 << "QQSyntaxHighlighter::highlightBlock";

	QQMessageBlockUserData * userData = dynamic_cast<QQMessageBlockUserData *>(currentBlockUserData());

	if(text.length() > 1 &&
			(userData == NULL || userData->wasParsed() == false))
	{
		if(userData == NULL)
		{
			userData = new QQMessageBlockUserData();
			setCurrentBlockUserData(userData);
		}

		//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
		//		 << "QQSyntaxHighlighter::highlightBlock text=" << text;

		if(userData->blockZone() == QQMessageBlockUserData::MESSAGE_ZONE)
		{
			highlightNorloge(text, userData);
			highlightDuck(text, userData);
			highlightTableVolante(text, userData);
			highlightTotoz(text, userData);
		}
		else
		{
			//qDebug() << QDateTime::currentDateTime().currentMSecsSinceEpoch() << " : "
			//		 << "QQSyntaxHighlighter::highlightBlock, userData is NULL";
		}

	}
}

void QQSyntaxHighlighter::highlightNorloge(const QString & text, QQMessageBlockUserData * userData)
{
	if(userData->post() == NULL)
		return;

	QColor color = QColor("#0000DD");

	QRegExp m_norlogeReg = QQNorlogeRef::norlogeRegexp();

	int index = text.indexOf(m_norlogeReg);
	while (index >= 0)
	{
		int length = m_norlogeReg.matchedLength();
		//Q_ASSERT(userData->post() != NULL);
		QQNorlogeRef nRef = QQNorlogeRef(userData->post()->bouchot()->name(),
										 userData->post()->norloge(),
										 text.mid(index, length),
										 index);
		userData->addNorlogeRefZone(nRef);
		setFormat(index, length, color);

		index = text.indexOf(m_norlogeReg, index + length);
	}
}

void QQSyntaxHighlighter::highlightDuck(const QString & text, QQMessageBlockUserData * userData)
{
	QColor color = QColor("#9933CC");

	QString tete = QString::fromAscii("(?:[o0ô°øòó@]|(?:&ocirc;)|(?:&deg;)|(?:&oslash;)|(?:&ograve;)|(?:&oacute;))");

	//QRegExp m_duckReg = QRegExp(QString::fromAscii("(\\\\_").append(tete).append(QString::fromAscii("&lt;)")),
	QRegExp m_duckReg = QRegExp(QString::fromAscii("\\\\_").append(tete).append(QString::fromAscii("<")),
								Qt::CaseSensitive,
								QRegExp::RegExp);

	int index = text.indexOf(m_duckReg);
	while (index >= 0) {
		int length = m_duckReg.matchedLength();
		userData->addDuckZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_duckReg, index + length);
	}

	//m_duckReg = QRegExp(QString::fromAscii("(&gt;").append(tete).append(QString::fromAscii("_\\/)")),
	m_duckReg = QRegExp(QString::fromAscii(">").append(tete).append(QString::fromAscii("_\\/")),
						Qt::CaseSensitive,
						QRegExp::RegExp);
	index = text.indexOf(m_duckReg);
	while (index >= 0) {
		int length = m_duckReg.matchedLength();
		userData->addDuckZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_duckReg, index + length);
	}

	m_duckReg = QRegExp(QString::fromAscii("coin ?! ?coin ?!"),
						Qt::CaseSensitive,
						QRegExp::RegExp); index = text.indexOf(m_duckReg);
	while (index >= 0) {
		int length = m_duckReg.matchedLength();
		userData->addDuckZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_duckReg, index + length);
	}
}

void QQSyntaxHighlighter::highlightTableVolante(const QString & text, QQMessageBlockUserData * userData)
{
	QColor color = QColor("#9933CC");

	QRegExp m_tvReg = QRegExp(QString::fromAscii("(?:flap ?flap)|(?:table[ _]volante)"),
							  Qt::CaseSensitive,
							  QRegExp::RegExp);
	int index = text.indexOf(m_tvReg);
	while (index >= 0) {
		int length = m_tvReg.matchedLength();
		userData->addTableVZone(index, text.mid(index, length));
		setFormat(index, length, color);

		index = text.indexOf(m_tvReg, index + length);
	}
}

void QQSyntaxHighlighter::highlightTotoz(const QString & text, QQMessageBlockUserData * userData)
{
	QTextCharFormat totozMessageFormat;
	totozMessageFormat.setForeground(QColor("#00AA11"));
	totozMessageFormat.setFontWeight(QFont::Bold);

	QRegExp m_totozReg = QRegExp(QString::fromAscii("(\\[\\:[^\\t\\)\\]]+\\])"), //[:[^\t\)\]]
								 Qt::CaseSensitive,
								 QRegExp::RegExp);

	int index = text.indexOf(m_totozReg);
	while (index >= 0)
	{
		int length = m_totozReg.matchedLength();

		QQTotoz totoz = QQTotoz(text.mid(index, length), index);
		emit totozRequired(totoz.getId());

		userData->addTotozZone(totoz);
		setFormat(index, length, totozMessageFormat);

		index = text.indexOf(m_totozReg, index + length);
	}
}

#include "qqpalmilineedit.h"

#include "core/qqnorlogeref.h"
#include "core/qqsettings.h"

#include <QtDebug>
#include <QFocusEvent>
#include <QKeyEvent>

QQPalmiLineEdit::QQPalmiLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
}

void QQPalmiLineEdit::changeColor(const QColor& newColor)
{
	QPalette p = palette();
	p.setColor(QPalette::Base, newColor);
	setPalette(p);
}

void QQPalmiLineEdit::insertSurroundText(const QString & bTag, const QString & eTag)
{
	if(hasSelectedText())
	{
		QString txt = text();
		QString selTxt = selectedText();
		int selStart = selectionStart();
		txt.insert(selStart, bTag);
		txt.insert(selStart + bTag.length() + selTxt.length(), eTag);
		setText(txt);
		setCursorPosition(selStart + bTag.length() + selTxt.length());
	}
	else
	{
		QString buf(bTag);
		buf.append(eTag);
		insert(buf);
		setFocus(Qt::OtherFocusReason);
		setCursorPosition(cursorPosition() - eTag.length());
	}
}

void QQPalmiLineEdit::insertReplaceText(const QString & tag)
{
	if(hasSelectedText())
	{
		QString txt=text();
		QString selTxt = selectedText();
		int selStart = selectionStart();
		txt.replace(selStart, selTxt.length(), tag);
		setText(txt);
		setCursorPosition(selStart + txt.length());
	}
	else
	{
		insert(tag);
		setFocus(Qt::OtherFocusReason);
		setCursorPosition(cursorPosition() + tag.length());
	}
}

void QQPalmiLineEdit::keyPressEvent(QKeyEvent * e)
{
	bool eventManaged = false;
	if(e->modifiers() == Qt::AltModifier)
	{
		int key = e->key();
		eventManaged = true;
		switch(key)
		{
		case Qt::Key_B:
			insertSurroundText(QString::fromAscii("<b>"), QString::fromAscii("</b>"));
			break;
		case Qt::Key_I:
			insertSurroundText(QString::fromAscii("<i>"), QString::fromAscii("</i>"));
			break;
		case Qt::Key_J:
			insertReplaceText(QString::fromAscii("\\o/"));
			break;
		case Qt::Key_K:
			insertReplaceText(QString::fromAscii("/o\\"));
			break;
		case Qt::Key_M:
			insertSurroundText(QString::fromAscii("====> <b>Moment "), QString::fromAscii("</b> <===="));
			break;
		case Qt::Key_O:
			insertReplaceText(QString::fromAscii("_o/* <b>BLAM</b>! "));
			break;
		case Qt::Key_P:
			insertReplaceText(QString::fromAscii("_o/* <b>paf!</b> "));
			break;
		case Qt::Key_S:
			insertSurroundText(QString::fromAscii("<s>"), QString::fromAscii("</s>"));
			break;
		case Qt::Key_U:
			insertSurroundText(QString::fromAscii("<u>"), QString::fromAscii("</u>"));
			break;
		default :
			eventManaged = false;
			break;
		}
	}

	if(! eventManaged)
	{
		QLineEdit::keyPressEvent(e);

		if(e->key() <= 0x0ff) //Permet de ne pas reagir sur les touches fleches / delete / ...
			completeTotoz();
	}
}

void QQPalmiLineEdit::completeTotoz()
{
	QString txt = text().left(cursorPosition());
	int pos = txt.lastIndexOf("[:");
	if(pos >= 0)
	{
		txt = txt.right(txt.length() - (pos + 2));
		if(! txt.contains(']'))
		{
			for(int i = 0; i < m_listTotoz.size(); ++i)
			{
				if(m_listTotoz.at(i).startsWith(txt, Qt::CaseInsensitive))
				{
					QString totoz = m_listTotoz.at(i);
					totoz.append(']');
					setSelection(cursorPosition(), 0 - txt.length());
					insertReplaceText(totoz);
					setSelection(cursorPosition(), 0 - (totoz.length() - txt.length()));
					break;
				}
			}
		}
	}
}

void QQPalmiLineEdit::focusInEvent(QFocusEvent * e)
{
	Q_UNUSED(e);
	updateTotozCompleter();
	QLineEdit::focusInEvent(e);
}

void QQPalmiLineEdit::updateTotozCompleter()
{
	QQSettings settings;
	m_listTotoz = settings.value(SETTINGS_TOTOZ_BOOKMARKLIST).toStringList();
	m_listTotoz.sort();
}

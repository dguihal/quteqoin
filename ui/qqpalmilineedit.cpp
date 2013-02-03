#include "qqpalmilineedit.h"

#include "core/qqnorlogeref.h"

#include <QtDebug>
#include <QFocusEvent>
#include <QKeyEvent>

QQPalmiLineEdit::QQPalmiLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
}

void QQPalmiLineEdit::keyPressEvent(QKeyEvent * e)
{
	if(e->modifiers() == Qt::AltModifier)
	{
		int key = e->key();
		switch(key)
		{
		case Qt::Key_B:
			insertSurroundText(QString::fromAscii("<b>"), QString::fromAscii("</b>"));
			break;
		case Qt::Key_I:
			insertSurroundText(QString::fromAscii("<i>"), QString::fromAscii("</i>"));
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
			QLineEdit::keyPressEvent(e);
			break;
		}
	}
	else
		QLineEdit::keyPressEvent(e);
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

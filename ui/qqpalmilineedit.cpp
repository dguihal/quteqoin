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

void QQPalmiLineEdit::changeColor(const QColor &newColor)
{
	QPalette p = palette();
	p.setColor(QPalette::Base, newColor);
	setPalette(p);
}

void QQPalmiLineEdit::insertText(const QString &str)
{
	QString newStr = str;
	if(str.contains("%s"))
		newStr.replace("%s", selectedText());
	insert(newStr);
}

void QQPalmiLineEdit::blam()
{
	insertText(QString::fromAscii("_o/* <b>BLAM</b>! "));
}

void QQPalmiLineEdit::bold()
{
	insertText(QString::fromAscii("<b>%s</b>"));
}

void QQPalmiLineEdit::italic()
{
	insertText(QString::fromAscii("<i>%s</i>"));
}

void QQPalmiLineEdit::paf()
{
	insertText(QString::fromAscii("_o/* <b>paf!</b> "));
}

void QQPalmiLineEdit::moment()
{
	insertText(QString::fromAscii("====> <b>Moment %s</b> <===="));
}

void QQPalmiLineEdit::strike()
{
	insertText(QString::fromAscii("<s>%s</s>"));
}

void QQPalmiLineEdit::underline()
{
	insertText(QString::fromAscii("<u>%s</u>"));
}

void QQPalmiLineEdit::keyPressEvent(QKeyEvent *e)
{
	QQSettings settings;

	QList< QPair<QChar, QString> > palmishortcuts = settings.palmiShorcuts();

	bool eventManaged = false;
	int keyInt = e->key();
	Qt::Key key = static_cast<Qt::Key>(keyInt);

	if(e->modifiers() == Qt::AltModifier &&
	   key >= SETTINGS_PALMI_SHORTCUTS_MIN_KEY &&
	   key <= SETTINGS_PALMI_SHORTCUTS_MAX_KEY)
	{
		QChar keyChr = QChar(keyInt).toLower();
		for(int i = 0; i < palmishortcuts.size(); i++)
		{
			if(keyChr == palmishortcuts.at(i).first)
			{
				insertText(palmishortcuts.at(i).second);

				eventManaged = true;
				break;
			}
		}
	}

	if(! eventManaged)
	{
		QLineEdit::keyPressEvent(e);

		if(keyInt <= Qt::Key_ydiaeresis) //Permet de ne pas reagir sur les touches fleches / delete / ...
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
					insertText(totoz.right(totoz.length() - txt.length()));
					setSelection(cursorPosition(), 0 - (totoz.length() - txt.length()));
					break;
				}
			}
		}
	}
}

void QQPalmiLineEdit::focusInEvent(QFocusEvent *e)
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

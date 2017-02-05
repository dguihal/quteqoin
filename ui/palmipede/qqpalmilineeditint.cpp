#include "qqpalmilineeditint.h"

#include "core/qqnetworkaccessor.h"
#include "core/qqnorlogeref.h"
#include "core/qqsettings.h"
#include "ui/qqtotozmanager.h"

#include <QtDebug>
#include <QApplication>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QHttpMultiPart>
#include <QKeyEvent>
#include <QMimeData>
#include <QToolButton>
#include <QStyle>

#define MAX_POST_HISTORY_SIZE 6 // 5 + Current

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::QQPalmiLineEditInt
/// \param parent
///
QQPalmiLineEditInt::QQPalmiLineEditInt(QWidget *parent) :
	QLineEdit(parent),
	m_indexInPostHistory(0),
	m_postHistory()
{
	setAttribute(Qt::WA_InputMethodEnabled, true);
#if(QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	setClearButtonEnabled(true);
	setStyleSheet(QString("QLineEdit {color: black; background-color: transparent; border: 0px;}"));
#else
	m_clearButton = new QToolButton(this);
	QFontMetrics fMetrics(font());
	QPixmap pixmap = QPixmap(":/img/palmi-clear.png").scaledToHeight(fMetrics.height() + 1);
	m_clearButton->setIcon(QIcon(pixmap));
	m_clearButton->setIconSize(pixmap.size());
	m_clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	m_clearButton->hide();
	connect(m_clearButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateCloseButton(const QString&)));
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit {padding-right: %1px; color: black; background-color: transparent; border: 0px;}").arg(m_clearButton->sizeHint().width() + frameWidth + 1));
#endif

	m_postHistory.enqueue("");
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::insertText
/// \param str
///
void QQPalmiLineEditInt::insertText(const QString &str)
{
	QString newStr = str;

	int moveCursorCount = 0;
	if(str.contains("%s"))
	{
		if(!hasFocus())
			setSelection(m_savedSelection.first, m_savedSelection.second);
		QString selection = selectedText();
		newStr.replace("%s", selection);

		if(selection.length() == 0)
			moveCursorCount = str.length() - (str.indexOf("%s") + 2);
	}
	insert(newStr);

	if(moveCursorCount > 0)
		cursorBackward(false, moveCursorCount);

	m_postHistory.last() = text();
	m_indexInPostHistory = m_postHistory.size() - 1; //last
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::pushCurrentToHistory
///
void QQPalmiLineEditInt::pushCurrentToHistory()
{
	QString currText = text();
	if(! m_postHistory.contains(currText))
		m_postHistory.last() = currText;

	m_postHistory.enqueue("");

	while(m_postHistory.size() > MAX_POST_HISTORY_SIZE)
		m_postHistory.dequeue();

	m_indexInPostHistory = m_postHistory.size() - 1; //last
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::focusInEvent
/// \param e
///
void QQPalmiLineEditInt::focusInEvent(QFocusEvent *e)
{
	updateTotozCompleter();
	QLineEdit::focusInEvent(e);
}

//////////////////////////////////////////////////////////////
/// \brief focusOutEvent
/// \param e
///
void QQPalmiLineEditInt::focusOutEvent(QFocusEvent *e)
{
	m_savedSelection = qMakePair(selectionStart(), selectedText().length());

	QLineEdit::focusOutEvent(e);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::keyPressEvent
/// \param e
///
void QQPalmiLineEditInt::keyPressEvent(QKeyEvent *e)
{
	QQSettings settings;
	bool saveHistory = true;

	QList< QPair<QChar, QString> > palmishortcuts = settings.palmiShorcuts();

	bool eventManaged = false;
	int keyInt = e->key();
	Qt::Key key = static_cast<Qt::Key>(keyInt);

	if(e->modifiers() == Qt::AltModifier)
	{
		if(key >= SETTINGS_PALMI_SHORTCUTS_MIN_KEY &&
				key <= SETTINGS_PALMI_SHORTCUTS_MAX_KEY)
		{
			QChar keyChr = QChar(keyInt).toLower();
			for(int i = 0; i < palmishortcuts.size(); i++)
			{
				if(keyChr == palmishortcuts.at(i).first)
				{
					insertText(palmishortcuts.at(i).second);

					eventManaged = true;
					e->accept();
					break;
				}
			}
		}
		else if(key == Qt::Key_Up || key == Qt::Key_Down)
		{
			e->accept();
			emit changeBoard(key == Qt::Key_Down);
		}
	}
	else if(e->modifiers() == Qt::NoModifier)
	{
		if(key == Qt::Key_Escape)
		{
			e->accept();
			clear();
			eventManaged = true;
		}

		else if(key == Qt::Key_Up || key == Qt::Key_Down)
		{
			e->accept();
			rotateHistory(key == Qt::Key_Down);
			saveHistory = false; // Ne pas sauver lors d'une rotation
		}
	}

	if(! eventManaged)
	{
		QLineEdit::keyPressEvent(e);

		if(keyInt <= Qt::Key_ydiaeresis) //Permet de ne pas reagir sur les touches fleches / delete / ...
			completeTotoz();
	}

	if(saveHistory)
	{
		m_postHistory.last() = text();
		m_indexInPostHistory = m_postHistory.size() - 1; //last
	}
}

#if(QT_VERSION < QT_VERSION_CHECK(5, 2, 0))
//////////////////////////////////////////////////////////////
/// \brief resizeEvent
/// \param event
///
void QQPalmiLineEditInt::resizeEvent(QResizeEvent *event)
{
	QSize sz = m_clearButton->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	m_clearButton->move(rect().right() - frameWidth - sz.width(),
						(rect().bottom() + 1 - sz.height())/2);

	QLineEdit::resizeEvent(event);
}
#endif

//////////////////////////////////////////////////////////////
/// \brief LineEdit::updateCloseButton
/// \param text
///
void QQPalmiLineEditInt::updateCloseButton(const QString &text)
{
#if(QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	Q_UNUSED(text)
#else
	m_clearButton->setVisible(!text.isEmpty());
	if(m_indexInPostHistory == 0)
		m_postHistory.enqueue(text);
#endif
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::updateTotozCompleter
///
void QQPalmiLineEditInt::updateTotozCompleter()
{
	m_listTotoz = QQTotozManager::bookmarkedTotozIds();
	m_listTotoz.sort();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::completeTotoz
///
void QQPalmiLineEditInt::completeTotoz()
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

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::rotateHistory
///
void QQPalmiLineEditInt::rotateHistory(bool forward)
{
	if(forward && m_indexInPostHistory < m_postHistory.size() - 1)
		m_indexInPostHistory ++;
	else if((!forward) && m_indexInPostHistory > 0)
		m_indexInPostHistory --;

	setText(m_postHistory.at(m_indexInPostHistory));
}

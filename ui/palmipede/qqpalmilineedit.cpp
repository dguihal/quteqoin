#include "qqpalmilineedit.h"

#include "core/qqnetworkaccessor.h"
#include "core/qqnorlogeref.h"
#include "core/qqsettings.h"
#include "ui/qqtotozmanager.h"

#include <QtDebug>
#include <QFileDialog>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QHttpMultiPart>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QToolButton>
#include <QStyle>

#define MAX_POST_HISTORY_SIZE 6 // 5 + Current

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::QQPalmiLineEdit
/// \param parent
///
QQPalmiLineEdit::QQPalmiLineEdit(QWidget *parent) :
	QLineEdit(parent),
	m_fPoster(this),
	m_indexInPostHistory(0),
	m_postHistory()
{
	setAttribute(Qt::WA_InputMethodEnabled, true);
#if(QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	setClearButtonEnabled(true);
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
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(m_clearButton->sizeHint().width() + frameWidth + 1));
#endif
	connect(&m_fPoster, SIGNAL(finished(QString)), this, SLOT(insertText(QString)));
	connect(&m_fPoster, SIGNAL(postErr(QString)), this, SLOT(joinFileErr(QString)));

	m_postHistory.enqueue("");
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::changeColor
/// \param newColor
///
void QQPalmiLineEdit::changeColor(const QColor &newColor)
{
	QPalette p = palette();
	p.setColor(QPalette::Base, newColor);
	setPalette(p);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::insertText
/// \param str
///
void QQPalmiLineEdit::insertText(const QString &str)
{
	QString newStr = str;

	int moveCursorCount = 0;
	if(str.contains("%s"))
	{
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
/// \brief QQPalmiLineEdit::blam
///
void QQPalmiLineEdit::blam()
{
	insertText(QString::fromLatin1("_o/* <b>BLAM</b>! "));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::bold
///
void QQPalmiLineEdit::bold()
{
	insertText(QString::fromLatin1("<b>%s</b>"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::italic
///
void QQPalmiLineEdit::italic()
{
	insertText(QString::fromLatin1("<i>%s</i>"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::paf
///
void QQPalmiLineEdit::paf()
{
	insertText(QString::fromLatin1("_o/* <b>paf!</b> "));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::moment
///
void QQPalmiLineEdit::moment()
{
	insertText(QString::fromLatin1("====> <b>Moment %s</b> <===="));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::strike
///
void QQPalmiLineEdit::strike()
{
	insertText(QString::fromLatin1("<s>%s</s>"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::underline
///
void QQPalmiLineEdit::underline()
{
	insertText(QString::fromLatin1("<u>%s</u>"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::attachFile
///
void QQPalmiLineEdit::attachFile(QString fileName)
{
	if(fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
												"", tr("Files (*.*)"));
	if(QFile::exists(fileName))
		m_fPoster.postFile(fileName);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::pushCurrentToHistory
///
void QQPalmiLineEdit::pushCurrentToHistory()
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
/// \brief QQPalmiLineEdit::dropEvent
/// \param e
///
void QQPalmiLineEdit::dropEvent(QDropEvent *event)
{
	if(event->mimeData()->hasUrls())
	{
		QUrl url = event->mimeData()->urls().at(0);
		if(url.isLocalFile())
		{
			attachFile(url.toLocalFile());
			event->accept();
		}
		else
			QLineEdit::dropEvent(event);
	}
	else
		QLineEdit::dropEvent(event);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::focusInEvent
/// \param e
///
void QQPalmiLineEdit::focusInEvent(QFocusEvent *e)
{
	updateTotozCompleter();
	QLineEdit::focusInEvent(e);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::keyPressEvent
/// \param e
///
void QQPalmiLineEdit::keyPressEvent(QKeyEvent *e)
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
					break;
				}
			}
		}
		else if(key == Qt::Key_Up || key == Qt::Key_Down)
		{
			emit changeBoard(key == Qt::Key_Down);
		}
	}
	else if(e->modifiers() == Qt::NoModifier)
	{
		if(key == Qt::Key_Escape)
		{
			clear();
			eventManaged = true;
		}

		else if(key == Qt::Key_Up || key == Qt::Key_Down)
		{
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
void QQPalmiLineEdit::resizeEvent(QResizeEvent *event)
{
	QSize sz = m_clearButton->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	m_clearButton->move(rect().right() - frameWidth - sz.width(),
						(rect().bottom() + 1 - sz.height())/2);

	QLineEdit::resizeEvent(event);
}
#endif

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::joinFileErr
/// \param errStr
///
void QQPalmiLineEdit::joinFileErr(const QString &errStr)
{
	QMessageBox *msgBox =  new QMessageBox(this);
	msgBox->setText(tr("File upload service error."));
	msgBox->setInformativeText(errStr);
	msgBox->setStandardButtons(QMessageBox::Ok);
	msgBox->setIcon(QMessageBox::Critical);
	msgBox->exec();
}

//////////////////////////////////////////////////////////////
/// \brief LineEdit::updateCloseButton
/// \param text
///
void QQPalmiLineEdit::updateCloseButton(const QString &text)
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
/// \brief QQPalmiLineEdit::updateTotozCompleter
///
void QQPalmiLineEdit::updateTotozCompleter()
{
	m_listTotoz = QQTotozManager::bookmarkedTotozIds();
	m_listTotoz.sort();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::completeTotoz
///
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

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::rotateHistory
///
void QQPalmiLineEdit::rotateHistory(bool forward)
{
	if(forward && m_indexInPostHistory < m_postHistory.size() - 1)
		m_indexInPostHistory ++;
	else if((!forward) && m_indexInPostHistory > 0)
		m_indexInPostHistory --;

	setText(m_postHistory.at(m_indexInPostHistory));
}

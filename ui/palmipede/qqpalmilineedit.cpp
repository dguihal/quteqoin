#include "qqpalmilineedit.h"

#include "qqpalmilineeditint.h"

#include <QBoxLayout>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QRect>
#include <QStyleOption>

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::QQPalmiLineEdit
/// \param parent
///
QQPalmiLineEdit::QQPalmiLineEdit(QWidget *parent) :
    QFrame(parent),
    m_currBoardcolor(Qt::transparent),
    m_pctUp(100),
    m_fPoster(this)
{
	setFrameShape(QFrame::NoFrame);
	m_privLineEdit = new QQPalmiLineEditInt(this);
	setFocusProxy(m_privLineEdit);

	setAcceptDrops(true);

	QBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(m_privLineEdit);

	connect(m_privLineEdit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
	connect(m_privLineEdit, SIGNAL(textChanged(QString)), this, SLOT(update()));
	connect(m_privLineEdit, SIGNAL(changeBoard(bool)), this, SIGNAL(changeBoard(bool)));

	connect(&m_fPoster, SIGNAL(finished(QString)), m_privLineEdit, SLOT(insertText(QString)));
	connect(&m_fPoster, SIGNAL(postErr(QString)), this, SLOT(joinFileErr(QString)));
	connect(&m_fPoster, SIGNAL(uploadProgress(quint32)), this, SLOT(updateUploadProgress(quint32)));

}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::insert
/// \param newText
///
void QQPalmiLineEdit::insert(const QString &newText)
{
	m_privLineEdit->insert(newText);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::clear
///
void QQPalmiLineEdit::clear()
{
	m_privLineEdit->clear();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::setText
/// \param text
///
void QQPalmiLineEdit::setText(const QString &text)
{
	m_privLineEdit->setText(text);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::text
/// \return
///
QString QQPalmiLineEdit::text() const
{
	return m_privLineEdit->text();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::dragEnterEvent
/// \param event
///
void QQPalmiLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::dropEvent
/// \param e
///
void QQPalmiLineEdit::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasUrls())
	{
		QUrl url = event->mimeData()->urls().at(0);
		if (url.isLocalFile())
			attachFileWithName(url.toLocalFile());
		else
			m_privLineEdit->insertText(url.toString());
		event->accept();
	}
	else if (mimeData->hasText())
	{
		m_privLineEdit->insertText(mimeData->text());
		event->accept();
	}
	else
		QWidget::dropEvent(event);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::paintEvent
///
void QQPalmiLineEdit::paintEvent(QPaintEvent *event)
{
	QRect rect = geometry();

	QPainter rectPainter(this);

	// Background
	QBrush bg(palette().window());
	rectPainter.setBrush(bg);
	rectPainter.setPen(Qt::NoPen);
	rectPainter.setOpacity(1);
	rectPainter.drawRoundedRect(rect, 2, 2);

	// Foreground
	QRect rf = rect;
#if(QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	int clearBtnWidth = 0;
	if(palette().window().color().lightness() < 64 && ! m_privLineEdit->text().isEmpty())
	{
		QStyle *s = style();
		clearBtnWidth = s->pixelMetric(QStyle::PM_ButtonIconSize) +
		                    s->pixelMetric(QStyle::PM_LayoutLeftMargin) +
		                    s->pixelMetric(QStyle::PM_LayoutRightMargin);

		rf.setWidth(rf.width() - (clearBtnWidth - 2));
	}
#endif
	rf.setWidth((static_cast<unsigned int>(rf.width()) * m_pctUp) / 100);

	QLinearGradient gradient(0, 0, 0, rect.height());
	gradient.setColorAt(0.0, m_currBoardcolor);
	gradient.setColorAt(0.1, m_currBoardcolor);
	gradient.setColorAt(0.3, m_currBoardcolor.lighter(105));
	gradient.setColorAt(0.7, m_currBoardcolor.lighter(105));
	gradient.setColorAt(0.9, m_currBoardcolor);
	gradient.setColorAt(1.0, m_currBoardcolor);

	// Fill Fg
	QBrush fg(gradient);
	rectPainter.setBrush(fg);
	rectPainter.setPen(Qt::NoPen);
	rectPainter.setOpacity(1);
	rectPainter.drawRoundedRect(rf, 2, 2);

#if(QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
	if(clearBtnWidth > 0)
	{
		QRect rf2 = rect;
		rf2.setWidth(clearBtnWidth);
		rf2.moveRight(rect.width());

		QBrush b(m_currBoardcolor.darker(150));
		rectPainter.setBrush(b);
		rectPainter.setPen(Qt::NoPen);
		rectPainter.setOpacity(1);
		rectPainter.drawRoundedRect(rf2, 2, 2);
	}
#endif

	rectPainter.setBrush(Qt::NoBrush);
	rectPainter.setPen(QPen(m_currBoardcolor.darker(120)));
	rectPainter.drawRoundedRect(geometry().adjusted(0, 0, -1, -1), 2, 2);

	QFrame::paintEvent(event);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::attachFile
///
void QQPalmiLineEdit::attachFile()
{
	auto fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
	                                        "", tr("Files (*.*)"));
	attachFileWithName(fileName);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::attachFile
///
void QQPalmiLineEdit::attachFileWithName(QString fileName)
{
	if(QFile::exists(fileName))
		m_fPoster.postFile(fileName);
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEditInt::joinFileErr
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

	m_pctUp = 100;
	update();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::pushCurrentToHistory
///
void QQPalmiLineEdit::pushCurrentToHistory()
{
	m_privLineEdit->pushCurrentToHistory();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::changeColor
/// \param newColor
///
void QQPalmiLineEdit::changeColor(const QColor &newColor)
{
	m_currBoardcolor = newColor;
	update();
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::insertText
/// \param str
///
void QQPalmiLineEdit::insertText(const QString &str)
{
	m_privLineEdit->insertText(str);
}


//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::blam
///
void QQPalmiLineEdit::blam()
{
	m_privLineEdit->insertText(QString::fromLatin1("_o/* <b>BLAM</b>! "));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::bold
///
void QQPalmiLineEdit::bold()
{
	m_privLineEdit->insertText(QString::fromLatin1("<b>%s</b>"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::italic
///
void QQPalmiLineEdit::italic()
{
	m_privLineEdit->insertText(QString::fromLatin1("<i>%s</i>"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::paf
///
void QQPalmiLineEdit::paf()
{
	m_privLineEdit->insertText(QString::fromLatin1("_o/* <b>paf!</b> "));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::moment
///
void QQPalmiLineEdit::moment()
{
	m_privLineEdit->insertText(QString::fromLatin1("====> <b>Moment %s</b> <===="));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::strike
///
void QQPalmiLineEdit::strike()
{
	m_privLineEdit->insertText(QString::fromLatin1("<s>%s</s>"));
}

//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::underline
///
void QQPalmiLineEdit::underline()
{
	m_privLineEdit->insertText(QString::fromLatin1("<u>%s</u>"));
}


//////////////////////////////////////////////////////////////
/// \brief QQPalmiLineEdit::updateUploadProgress
/// \param pctProgress
///
void QQPalmiLineEdit::updateUploadProgress(quint32 pctProgress)
{
	m_pctUp = qMin(pctProgress, quint32(100));
	update();
}

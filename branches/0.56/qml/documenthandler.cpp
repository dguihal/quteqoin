#include "documenthandler.h"

DocumentHandler::DocumentHandler(QObject *parent) :
	QObject(parent),
	m_target(0),
	m_doc(0),
	m_cursorPosition(-1),
	m_selectionStart(0),
	m_selectionEnd(0)
{
}

void DocumentHandler::setTarget(QQuickItem *target)
{
	m_doc = 0;
	m_target = target;
	if (!m_target)
		return;

	QVariant doc = m_target->property("textDocument");
	if (doc.canConvert<QQuickTextDocument*>()) {
		QQuickTextDocument *qqdoc = doc.value<QQuickTextDocument*>();
		if (qqdoc)
			m_doc = qqdoc->textDocument();
	}
	emit targetChanged();
}

void DocumentHandler::setCursorPosition(int position)
{
	if (position == m_cursorPosition)
		return;

	m_cursorPosition = position;
}

void DocumentHandler::setSelectionStart(int position)
{
	m_selectionStart = position;
}

void DocumentHandler::setSelectionEnd(int position)
{
	m_selectionEnd = position;
}

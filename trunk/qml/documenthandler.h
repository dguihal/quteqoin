#ifndef DOCUMENTHANDLER_H
#define DOCUMENTHANDLER_H

#include <QQuickTextDocument>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class DocumentHandler : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged)
	Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
	Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
	Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

public:
	explicit DocumentHandler(QObject *parent = 0);

	QQuickItem *target() { return m_target; }

	void setTarget(QQuickItem *target);

	void setCursorPosition(int position);
	void setSelectionStart(int position);
	void setSelectionEnd(int position);

	int cursorPosition() const { return m_cursorPosition; }
	int selectionStart() const { return m_selectionStart; }
	int selectionEnd() const { return m_selectionEnd; }

signals:
	void targetChanged();
	void cursorPositionChanged();
	void selectionStartChanged();
	void selectionEndChanged();

public slots:

private:
	QQuickItem *m_target;
	QTextDocument *m_doc;

	int m_cursorPosition;
	int m_selectionStart;
	int m_selectionEnd;
};

#endif // DOCUMENTHANDLER_H

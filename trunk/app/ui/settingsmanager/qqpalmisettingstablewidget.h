#ifndef QQPALMISETTINGSTABLEWIDGET_H
#define QQPALMISETTINGSTABLEWIDGET_H

#include <QLabel>
#include <QMultiMap>
#include <QSignalMapper>
#include <QStyledItemDelegate>
#include <QTableWidget>

class QQKeyboardShortcutInput : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString m_shortcutKey READ shortcutKey WRITE setShortcutKey NOTIFY shortcutKeyChanged USER true)
public:
	QQKeyboardShortcutInput(QWidget *parent = 0);

	QString shortcutKey() const { return m_shortcutKey; }
	void setShortcutKey(const QString &shortcutKey);

signals:
	void shortcutKeyChanged();

protected:
	virtual void focusInEvent(QFocusEvent * event);
	virtual void keyPressEvent(QKeyEvent *);

protected slots:
	void handleEditorTextEdited(const QString &text);

private:
	void setupUi();

	QString m_shortcutKey;

	QLabel *m_altLabel;
	QLineEdit *m_lineEdit;
};

class QQKeyboardShortcutItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	QQKeyboardShortcutItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}

	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class QQKeyboardShortcutDataItem : public QTableWidgetItem
{
public:
	QQKeyboardShortcutDataItem(int type = QTableWidgetItem::UserType)
		: QTableWidgetItem(type) { m_editRoleData = QString(); }
	explicit QQKeyboardShortcutDataItem(const QString & text, int type = QTableWidgetItem::UserType)
		: QTableWidgetItem(text, type) { m_editRoleData = QString(); }
	explicit QQKeyboardShortcutDataItem(const QIcon & icon, const QString & text, int type = QTableWidgetItem::UserType)
		: QTableWidgetItem(icon, text, type) { m_editRoleData = QString(); }

	void setKey(QChar key);
	virtual void setData(int role, const QVariant &value);
	virtual QVariant data(int role) const;

private:
	QString m_editRoleData;
};

class QQPalmiSettingsTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	explicit QQPalmiSettingsTableWidget(QWidget *parent = 0);
	~QQPalmiSettingsTableWidget();

	void appendStaticRow(const QChar &key, const QString &value);
	QList< QPair<QChar, QString> > getUserShotcuts() const;
	void appendUserRow(const QChar &key, const QString &value);

	void initLastLine();
signals:

public slots:
	int appendEmptyUserRow();
	void handleRemoveRowClicked();
	void keyChanged(QChar oldKey, QChar newKey, QTableWidgetItem *item);

protected:

protected slots:

private:
	void createLastLine();

	bool m_hasLastLine;
	QMultiMap<QChar, QQKeyboardShortcutDataItem *> m_mmapKeys;
	QQKeyboardShortcutItemDelegate *m_kbshortcutDelegate;
};

#endif // QQPALMISETTINGSTABLEWIDGET_H

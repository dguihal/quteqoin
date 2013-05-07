#ifndef QQPALMISETTINGSTABLEWIDGET_H
#define QQPALMISETTINGSTABLEWIDGET_H

#include <QMultiMap>
#include <QSignalMapper>
#include <QStyledItemDelegate>
#include <QTableWidget>

class QQKeyboardShortcutItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	QQKeyboardShortcutItemDelegate(QObject *parent = 0);

	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel * model, const QModelIndex & index) const;

protected slots:
	void handleEditorTextEdited(const QString & text);

private:
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

	void appendStaticRow(QChar key, QString value);
	QList< QPair<QChar, QString> > getUserShotcuts() const;
	void appendUserRow(QChar key, QString value);

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

#include "qqpalmisettingstablewidget.h"

#include <QtDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>

/////////////////////////////////////////////////////////////////////////////////////////////
/// QQKeyboardShortcutItemDelegate
/////////////////////////////////////////////////////////////////////////////////////////////

QQKeyboardShortcutItemDelegate::QQKeyboardShortcutItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

QItemEditorFactory *QQKeyboardShortcutItemDelegate::itemEditorFactory() const
{
	//qDebug() << "QQKeyboardShortcutItemDelegate::itemEditorFactory";
	return NULL;
}

QWidget *QQKeyboardShortcutItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);

	//qDebug() << "QQKeyboardShortcutItemDelegate::createEditor :" << parent << ", " << option << ", " << index;

	QLineEdit *lineEdit = new QLineEdit(parent);
	lineEdit->setMaxLength(1);
	lineEdit->setInputMask("<X");
	connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(handleEditorTextEdited(QString)));

	return lineEdit;
}

void QQKeyboardShortcutItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	Q_UNUSED(index);

	qDebug() << "QQKeyboardShortcutItemDelegate::setModelData :" << editor << ", " << model << ", " << index;

	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
	QString key = lineEdit->text();

	if(key.length() > 0)
		model->setData(index, QString(key), Qt::EditRole);
}

void QQKeyboardShortcutItemDelegate::handleEditorTextEdited(const QString &text)
{
	Q_UNUSED(text);

	//qDebug() << "QQKeyboardShortcutItemDelegate::handleEditorTextEdited :" << text;

	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());

	lineEdit->cursorBackward(false, 1);
	lineEdit->cursorForward(true, 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/// QQKeyboardShortcutDataItem
/////////////////////////////////////////////////////////////////////////////////////////////

void QQKeyboardShortcutDataItem::setData(int role, const QVariant &value)
{
	qDebug() << "QQKeyboardShortcutDataItem::setData : role =" << role << ", value =" << value;

	if(role == Qt::EditRole &&
	   value.canConvert<QString>())
	{
		QString oldEditRoleData = m_editRoleData;
		m_editRoleData = value.toString();
		QString longStr = QString("Alt+").append(m_editRoleData);
		QTableWidgetItem::setData(Qt::DisplayRole, longStr);
		QTableWidgetItem::setData(Qt::ToolTipRole, longStr);

		if(oldEditRoleData.isEmpty())
			(qobject_cast<QQPalmiSettingsTableWidget *>(tableWidget()))->keyChanged(QChar(), m_editRoleData.at(0), this);
		else
			(qobject_cast<QQPalmiSettingsTableWidget *>(tableWidget()))->keyChanged(oldEditRoleData.at(0), m_editRoleData.at(0), this);
	}
	else
		QTableWidgetItem::setData(role, value);

}

QVariant QQKeyboardShortcutDataItem::data(int role) const
{
	if(role == Qt::EditRole)
		return QVariant(m_editRoleData);
	else
		return QTableWidgetItem::data(role);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/// QQPalmiSettingsTableWidget
/////////////////////////////////////////////////////////////////////////////////////////////

QQPalmiSettingsTableWidget::QQPalmiSettingsTableWidget(QWidget *parent) :
	QTableWidget(parent)
{
	m_hasLastLine = false;
	m_mmapKeys.clear();

	m_kbshortcutDelegate = new QQKeyboardShortcutItemDelegate(this);
	setItemDelegateForColumn(1, m_kbshortcutDelegate);
}

QQPalmiSettingsTableWidget::~QQPalmiSettingsTableWidget()
{
	delete m_kbshortcutDelegate;
}

void QQPalmiSettingsTableWidget::appendStaticRow(QChar key, QString value)
{
	int currRow = rowCount();
	int colIndex = 0;

	insertRow(currRow);

	// 1st Col
	setCellWidget(currRow, colIndex++, new QWidget(this));

	// 2nd Col
	QQKeyboardShortcutDataItem *kbSItem = new QQKeyboardShortcutDataItem();
	kbSItem->setData(Qt::DisplayRole, QString("Alt+").append(key));
	kbSItem->setData(Qt::ToolTipRole, QString("Alt+").append(key));
	kbSItem->setFlags(
				kbSItem->flags()
				& ~Qt::ItemIsEnabled
				& ~Qt::ItemIsTristate);
	m_mmapKeys.insert(key, kbSItem);
	setItem(currRow, colIndex++, kbSItem);

	// 3nd Col
	QTableWidgetItem *item = new QTableWidgetItem();
	item->setData(Qt::DisplayRole, value);
	item->setData(Qt::ToolTipRole, value);
	item->setFlags(
				item->flags()
				& ~Qt::ItemIsEnabled
				& ~Qt::ItemIsTristate);
	setItem(currRow, colIndex++, item);
}

void QQPalmiSettingsTableWidget::appendUserRow(QChar key, QString value)
{
	int currRow = appendEmptyUserRow();
	int colIndex = 0;

	// 1st Col
	// Nothing to do
	colIndex++;

	// 2nd Col
	QTableWidgetItem *twItem = item(currRow, colIndex++);
	twItem->setData(Qt::EditRole, QString(key));

	// 3nd Col
	twItem = item(currRow, colIndex++);
	twItem->setData(Qt::DisplayRole, value);
	twItem->setData(Qt::ToolTipRole, value);
}

QList< QPair<QChar, QString> > QQPalmiSettingsTableWidget::getUserShotcuts() const
{
	QList< QPair<QChar, QString> > rep;
	QList<QChar> listKeys;
	QTableWidgetItem *twItem = NULL;
	for(int i = 0; i < rowCount(); i++)
	{
		twItem = item(i, 1);
		if(twItem != NULL && twItem->flags() & Qt::ItemIsEnabled)
		{
			QChar key = twItem->data(Qt::EditRole).toString().at(0);
			if(! listKeys.contains(key))
				rep.append(qMakePair(key, item(i, 2)->data(Qt::EditRole).toString()));
		}
	}
	return rep;
}

void QQPalmiSettingsTableWidget::initLastLine()
{
	createLastLine();
}

int QQPalmiSettingsTableWidget::appendEmptyUserRow()
{
	int currRow = m_hasLastLine ? rowCount() - 1 : rowCount();

	if(currRow > 0 && item(currRow - 1, 1)->text().size() == 0)
		return currRow - 1;

	int colIndex = 0;
	QPushButton *rowDelBtn;
	QPixmap delPixmap(":/img/delete-icon.png");
	QPixmap scaledDelPixmap = delPixmap.scaledToWidth(24);

	insertRow(currRow);

	// 1st Col
	rowDelBtn = new QPushButton(QIcon(scaledDelPixmap), "", this);
	rowDelBtn->setMaximumWidth(24);
	connect(rowDelBtn, SIGNAL(clicked()), this, SLOT(handleRemoveRowClicked()));
	setCellWidget(currRow, colIndex++, rowDelBtn);

	// 2nd Col
	QQKeyboardShortcutDataItem *kbSItem = new QQKeyboardShortcutDataItem();
	kbSItem->setFlags(
				kbSItem->flags()
				| Qt::ItemIsDragEnabled
				| Qt::ItemIsEditable
				| Qt::ItemIsSelectable
				| Qt::ItemIsEditable
				| Qt::ItemIsEnabled);
	kbSItem->setFlags(
				kbSItem->flags()
				& ~Qt::ItemIsTristate
				& ~Qt::ItemIsUserCheckable);
	setItem(currRow, colIndex++, kbSItem);

	// 3nd Col
	QTableWidgetItem *twItem = new QTableWidgetItem("");
	twItem->setFlags(
				twItem->flags()
				| Qt::ItemIsDragEnabled
				| Qt::ItemIsSelectable
				| Qt::ItemIsEditable
				| Qt::ItemIsEnabled);
	twItem->setFlags(
				twItem->flags()
				& ~Qt::ItemIsTristate
				& ~Qt::ItemIsUserCheckable);
	setItem(currRow, colIndex++, twItem);

	return currRow;
}


void QQPalmiSettingsTableWidget::handleRemoveRowClicked()
{
	QChar key;
	QTableWidgetItem *twItem = NULL;
	for(int i = 0; i < rowCount(); i++)
	{
		QWidget *widget = cellWidget(i, 0);
		if(widget->underMouse())
		{
			twItem = item(i, 1);
			key = twItem->data(Qt::EditRole).toString().at(0);
			//Needs to hide row before remove, otherwise we have widget refressh issue
			setRowHidden(i, true);
			removeRow(i);
			break;
		}
	}

	if(! key.isNull())
	{
		m_mmapKeys.remove(key, (QQKeyboardShortcutDataItem *) twItem);
		if(m_mmapKeys.count(key) == 1) //It was a multiple occurence, but not anymore
		{
			QTableWidgetItem *tItem = m_mmapKeys.values(key).at(0);
			if(tItem->flags() & Qt::ItemIsEnabled)
				tItem->setForeground(Qt::black);
		}
	}
}

void QQPalmiSettingsTableWidget::keyChanged(QChar oldKey, QChar newKey, QTableWidgetItem *item)
{
	if(! oldKey.isNull())
	{
		m_mmapKeys.remove(oldKey, (QQKeyboardShortcutDataItem *) item);
		if(m_mmapKeys.count(oldKey) == 1) //It was a multiple occurence, but not anymore
		{
			QTableWidgetItem *tItem = m_mmapKeys.values(oldKey).at(0);
			if(tItem->flags() & Qt::ItemIsEnabled)
				tItem->setForeground(Qt::black);
		}
	}


	if(m_mmapKeys.contains(newKey))
	{
		item->setForeground(Qt::red);
		QList<QQKeyboardShortcutDataItem *> items = m_mmapKeys.values(newKey);
		for(int i = 0; i < items.size(); i++)
		{
			QQKeyboardShortcutDataItem *tItem = items.at(i);
			if(tItem->flags() & Qt::ItemIsEnabled)
				tItem->setForeground(Qt::red);
		}
	}
	else
		item->setForeground(Qt::black);

	if(! newKey.isNull())
		m_mmapKeys.insert(newKey, (QQKeyboardShortcutDataItem *) item);

	item->setSelected(false);
}

void QQPalmiSettingsTableWidget::createLastLine()
{
	int currRow = rowCount();
	int colIndex = 0;
	QPixmap addPixmap(":/img/add-icon.png");
	QPixmap scaledAddPixmap = addPixmap.scaledToWidth(24);

	insertRow(currRow);

	QPushButton *rowAddBtn = new QPushButton(QIcon(scaledAddPixmap), "", this);
	connect(rowAddBtn, SIGNAL(clicked()), this, SLOT(appendEmptyUserRow()));
	rowAddBtn->setMaximumWidth(24);
	setCellWidget(currRow, colIndex++, rowAddBtn);
	setCellWidget(currRow, colIndex++, new QWidget(this));
	setCellWidget(currRow, colIndex++, new QWidget(this));

	m_hasLastLine = true;
}

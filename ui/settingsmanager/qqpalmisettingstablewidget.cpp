#include "qqpalmisettingstablewidget.h"

#include <QtDebug>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>

#define BTN_COLUMN 0
#define SHORTCUT_COLUMN 1
#define TEXT_COLUMN 2

#define SHORTCUT_PREFIX "Alt+"

#define BTN_PIXMAP_MAX_WIDTH 24

/////////////////////////////////////////////////////////////////////////////////////////////
/// QQKeyboardShortcutInput
/////////////////////////////////////////////////////////////////////////////////////////////
///
QQKeyboardShortcutInput::QQKeyboardShortcutInput(QWidget *parent) :
    QWidget(parent)
{
	setupUi();
}

void QQKeyboardShortcutInput::setShortcutKey(const QString &shortcutKey)
{
	m_shortcutKey = shortcutKey;
	m_lineEdit->setText(m_shortcutKey);

	m_lineEdit->cursorBackward(false, 1);
	m_lineEdit->cursorForward(true, 1);
}

void QQKeyboardShortcutInput::focusInEvent(QFocusEvent *event)
{
	m_lineEdit->setFocus(event->reason());
}

void QQKeyboardShortcutInput::keyPressEvent(QKeyEvent *event)
{
	QCoreApplication::sendEvent(m_lineEdit, event);
}

void QQKeyboardShortcutInput::setupUi()
{
	setAutoFillBackground(true);

	QLayout *hLayout = new QHBoxLayout(this);
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(0);

	m_altLabel = new QLabel(SHORTCUT_PREFIX);
	m_altLabel->setAlignment(Qt::AlignCenter);
	m_altLabel->setFont(font());
	m_altLabel->setMargin(3);

	hLayout->addWidget(m_altLabel);

	m_lineEdit = new QLineEdit();
	m_lineEdit->setMaxLength(1);
	m_lineEdit->setFont(font());
	auto *reVal = new QRegularExpressionValidator(QRegularExpression("[^A-Z]"), this);
	m_lineEdit->setValidator(reVal);
	connect(m_lineEdit, SIGNAL(textEdited(QString)), this, SLOT(handleEditorTextEdited(QString)));

	hLayout->addWidget(m_lineEdit);
}

void QQKeyboardShortcutInput::handleEditorTextEdited(const QString &text)
{
	m_shortcutKey = text.size() > 0 ? text.at(0) : QChar();

	m_lineEdit->cursorBackward(false, 1);
	m_lineEdit->cursorForward(true, 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/// QQKeyboardShortcutItemDelegate
/////////////////////////////////////////////////////////////////////////////////////////////
///

QWidget *QQKeyboardShortcutItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == SHORTCUT_COLUMN)
	{
		QQKeyboardShortcutInput *shortcutInput = new QQKeyboardShortcutInput(parent);
		shortcutInput->setFont(option.font);
		return shortcutInput;
	}
	else
		return QStyledItemDelegate::createEditor(parent, option, index);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/// QQKeyboardShortcutDataItem
/////////////////////////////////////////////////////////////////////////////////////////////

void QQKeyboardShortcutDataItem::setData(int role, const QVariant &value)
{
	if(role == Qt::EditRole &&
	   value.canConvert<QString>())
	{
		QString oldEditRoleData = m_editRoleData;
		m_editRoleData = value.toString().at(0);
		QString longStr = QString(SHORTCUT_PREFIX);
		if(m_editRoleData == QString(" "))
			longStr.append("Space");
		else
			longStr.append(m_editRoleData);
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
	setItemDelegate(m_kbshortcutDelegate);
}

QQPalmiSettingsTableWidget::~QQPalmiSettingsTableWidget()
{
	delete m_kbshortcutDelegate;
}

void QQPalmiSettingsTableWidget::appendStaticRow(const QChar &key, const QString &value)
{
	int currRow = rowCount();

	insertRow(currRow);

	// 1st Col
	setCellWidget(currRow, BTN_COLUMN, new QWidget(this));

	// 2nd Col
	QQKeyboardShortcutDataItem *kbSItem = new QQKeyboardShortcutDataItem();
	kbSItem->setData(Qt::DisplayRole, QString(SHORTCUT_PREFIX).append(key));
	kbSItem->setData(Qt::ToolTipRole, QString(SHORTCUT_PREFIX).append(key));
	kbSItem->setFlags(
	            kbSItem->flags()
	            & ~Qt::ItemIsEnabled
	            & ~Qt::ItemIsUserTristate);
	m_mmapKeys.insert(key, kbSItem);
	setItem(currRow, SHORTCUT_COLUMN, kbSItem);

	// 3nd Col
	QTableWidgetItem *item = new QTableWidgetItem();
	item->setData(Qt::DisplayRole, value);
	item->setData(Qt::ToolTipRole, value);
	item->setFlags(
	            item->flags()
	            & ~Qt::ItemIsEnabled
	            & ~Qt::ItemIsUserTristate);
	setItem(currRow, TEXT_COLUMN, item);
}

void QQPalmiSettingsTableWidget::appendUserRow(const QChar &key, const QString &value)
{
	int currRow = appendEmptyUserRow();

	// 1st Col
	// Nothing to do

	// 2nd Col
	QTableWidgetItem *twItem = item(currRow, SHORTCUT_COLUMN);
	twItem->setData(Qt::EditRole, QString(key));

	// 3nd Col
	twItem = item(currRow, TEXT_COLUMN);
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
		twItem = item(i, SHORTCUT_COLUMN);
		if(twItem != NULL && twItem->flags() & Qt::ItemIsEnabled)
		{
			QChar key = twItem->data(Qt::EditRole).toString().at(0);
			if(! listKeys.contains(key))
				rep.append(qMakePair(key, item(i, TEXT_COLUMN)->data(Qt::EditRole).toString()));
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

	if(currRow > 0 && item(currRow - 1, SHORTCUT_COLUMN)->text().size() == 0)
		return currRow - 1;

	QPushButton *rowDelBtn;
	QPixmap delPixmap = style()->standardIcon(QStyle::SP_DialogCancelButton).pixmap(BTN_PIXMAP_MAX_WIDTH);

	insertRow(currRow);

	// 1st Col
	rowDelBtn = new QPushButton(QIcon(delPixmap), "", this);
	rowDelBtn->setMaximumWidth(BTN_PIXMAP_MAX_WIDTH);
	connect(rowDelBtn, SIGNAL(clicked()), this, SLOT(handleRemoveRowClicked()));
	setCellWidget(currRow, BTN_COLUMN, rowDelBtn);

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
	            & ~Qt::ItemIsUserTristate
	            & ~Qt::ItemIsUserCheckable);
	setItem(currRow, SHORTCUT_COLUMN, kbSItem);

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
	            & ~Qt::ItemIsUserTristate
	            & ~Qt::ItemIsUserCheckable);
	setItem(currRow, TEXT_COLUMN, twItem);

	return currRow;
}


void QQPalmiSettingsTableWidget::handleRemoveRowClicked()
{
	QChar key;
	QTableWidgetItem *twItem = NULL;
	for(int i = 0; i < rowCount(); i++)
	{
		QWidget *widget = cellWidget(i, BTN_COLUMN);
		if(widget->underMouse())
		{
			twItem = item(i, SHORTCUT_COLUMN);
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
			QTableWidgetItem *tItem = m_mmapKeys.values(key).constFirst();
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
			QTableWidgetItem *tItem = m_mmapKeys.values(oldKey).constFirst();
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
	QPixmap addPixmap(":/img/add-icon.png");
	QPixmap scaledAddPixmap = addPixmap.scaledToWidth(BTN_PIXMAP_MAX_WIDTH);

	insertRow(currRow);

	QPushButton *rowAddBtn = new QPushButton(QIcon(scaledAddPixmap), "", this);
	connect(rowAddBtn, SIGNAL(clicked()), this, SLOT(appendEmptyUserRow()));
	rowAddBtn->setMaximumWidth(BTN_PIXMAP_MAX_WIDTH);
	setCellWidget(currRow, BTN_COLUMN, rowAddBtn);
	setCellWidget(currRow, SHORTCUT_COLUMN, new QWidget(this));
	setCellWidget(currRow, TEXT_COLUMN, new QWidget(this));

	m_hasLastLine = true;
}

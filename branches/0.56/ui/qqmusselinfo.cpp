#include "qqmusselinfo.h"

#include "core/qqbouchot.h"

#include <QtDebug>
#include <QContextMenuEvent>
#include <QMenu>

//////////////////////////////////////////////////////////////
/// \brief QQMusselInfo::QQMusselInfo
/// \param mussel
/// \param parent
///
QQMusselInfo::QQMusselInfo(QQMussel mussel, QWidget *parent) :
	QLabel(parent),
	m_mussel(mussel),
	m_selected(false)
{
	setText(m_mussel.name());
	updateStyleSheet();
}

//////////////////////////////////////////////////////////////
/// \brief QQMusselInfo::toggleBak
/// \param baked
///
void QQMusselInfo::toggleBak(bool baked)
{
	m_mussel.bak(baked);
}

//////////////////////////////////////////////////////////////
/// \brief QQMusselInfo::togglePlopify
/// \param plopified
///
void QQMusselInfo::togglePlopify(bool plopified)
{
	m_mussel.plopify(plopified);
}

//////////////////////////////////////////////////////////////
/// \brief QQMusselInfo::mouseReleaseEvent
/// \param event
///
void QQMusselInfo::mouseReleaseEvent(QMouseEvent *event)
{
	QLabel::mousePressEvent(event);
	if(! selected())
		setSelected();
	else
		setUnselected();

	updateStyleSheet();
}

//////////////////////////////////////////////////////////////
/// \brief QQMusselInfo::contextMenuEvent
/// \param event
///
void QQMusselInfo::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	QMenu assocMenu(tr("Associate"));

	QList<QMenu *> bMenus;
	foreach (QQBouchot *b, QQBouchot::listBouchots())
	{
		QList<QQMussel> lPosters = b->lastPosters();
		if(lPosters.size() > 0)
		{
			QMenu *bMenu = new QMenu(b->name());
			assocMenu.addMenu(bMenu);
			bMenus.append(bMenu);
			foreach (QQMussel m, lPosters)
			{
				if(m != m_mussel)
				{
					QAction *a = bMenu->addAction(m.name());

					QMap<QString, QVariant> actionsData;
					actionsData.insert("Bouchot", b->name());
					a->setData(QVariant(actionsData));

					a->setCheckable(true);
					if(! m.isAuth())
					{
						QFont f = a->font();
						f.setItalic(true);
						a->setFont(f);
					}
				}
			}
		}
	}
	connect(&assocMenu, SIGNAL(triggered(QAction*)), this, SLOT(onMenuSelection(QAction*)));
	menu.addMenu(&assocMenu);

	if(! m_mussel.isMe()) {
		QAction *a = menu.addAction(tr("Bak"));
		a->setCheckable(true);
		a->setChecked(m_mussel.isBaked());
		connect(a, SIGNAL(toggled(bool)), this, SLOT(toggleBak(bool)));

		a = menu.addAction(tr("Plopify"));
		a->setCheckable(true);
		a->setChecked(m_mussel.isPlopified());
		connect(a, SIGNAL(toggled(bool)), this, SLOT(togglePlopify(bool)));
	}

	menu.exec(event->globalPos());

	foreach (QMenu *m, bMenus)
		delete m;
}

//////////////////////////////////////////////////////////////
/// \brief QQMusselInfo::onMenuSelection
/// \param action
///
void QQMusselInfo::onMenuSelection(QAction *action)
{
	qDebug() << Q_FUNC_INFO << action->text() << action->data().toMap().value("Bouchot").toString();
}

//////////////////////////////////////////////////////////////
/// \brief QQMusselInfo::updateStyleSheet
///
void QQMusselInfo::updateStyleSheet()
{
	QString style("QLabel {%1}");

	if(selected())
	{
		QPalette curPalette = palette();
		style = style.arg(
					QString("border: 2px solid; border-color: %1; border-radius: 4px; padding: 0px; %2")
					.arg(curPalette.text().color().name()));
	}
	else
		style = style.arg("padding: 2px; %1");


	if(m_mussel.isAuth())
		style = style.arg("");
	else
		style = style.arg("font-style: italic;");

	setStyleSheet(style);
}

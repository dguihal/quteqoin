#include "qqcmdtoolbuttons.h"
#include "ui_qqcmdtoolbuttons.h"

#include <QPushButton>

//////////////////////////////////////////////////////////////
/// \brief QQCmdToolButtons::QQCmdToolButtons
/// \param parent
///
QQCmdToolButtons::QQCmdToolButtons(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QQCmdToolButtons)
{
	ui->setupUi(this);

	connect(ui->optionsPushButton, &QPushButton::clicked, this, &QQCmdToolButtons::optionsButtonClicked);
}

//////////////////////////////////////////////////////////////
/// \brief QQCmdToolButtons::~QQCmdToolButtons
///
QQCmdToolButtons::~QQCmdToolButtons()
{
	delete ui;
}

//////////////////////////////////////////////////////////////
/// \brief QQCmdToolButtons::addAction
/// \param action
///
void QQCmdToolButtons::addAction(QAction *action)
{
	ui->windowsButton->addAction(action);
}

//////////////////////////////////////////////////////////////
/// \brief QQCmdToolButtons::optionsButtonClicked
///
void QQCmdToolButtons::optionsButtonClicked()
{
	emit showOptions();
}

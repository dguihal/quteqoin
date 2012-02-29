#ifndef QQBOUCHOTSETTINGSDIALOG_H
#define QQBOUCHOTSETTINGSDIALOG_H

#include "core/qqbouchot.h"

#include <QDialog>
#include <QStringList>

class QQBouchot;

namespace Ui {
    class QQBouchotSettingsDialog;
}

class QQBouchotSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QQBouchotSettingsDialog(QWidget *parent = 0);
    QQBouchotSettingsDialog(QString, QQBouchot::QQBouchotSettings, QWidget *parent = 0);
    ~QQBouchotSettingsDialog();

    QQBouchot::QQBouchotSettings bouchotSettings() { return m_bouchotSettings; }
    QString bouchotName() { return m_bouchotName; }

    void setGroups(QStringList listGroups);

public slots:
    void okPressed();
    void cancelPressed();
    void usePresetPressed();

private:
    void setBouchot();

    Ui::QQBouchotSettingsDialog *ui;
    QQBouchot::QQBouchotSettings m_bouchotSettings;
    QString m_bouchotName;
};

#endif // QQBOUCHOTSETTINGSDIALOG_H

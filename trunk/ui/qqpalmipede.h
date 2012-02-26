#ifndef QQPALMIPEDE_H
#define QQPALMIPEDE_H

#include <QGroupBox>

class QColor;
class QShortcut;

namespace Ui {
class QQPalmipede;
}

class QQPalmipede : public QGroupBox
{
    Q_OBJECT

public:
    explicit QQPalmipede(QWidget *parent = 0);
    ~QQPalmipede();

    //QString getCurrentBouchot();

    void insertText(const QString &);

signals:
    void postMessage(const QString&);

public slots:
    void changePalmiColor(const QColor&);
    void insertSurroundText(const QString&, const QString&);
    void insertReplaceText(const QString&);

private:
    Ui::QQPalmipede *ui;
    QShortcut *blamShortcut;
    QShortcut *pafShortcut;

private slots:
    void boldClicked();
    void italicClicked();
    void underlineClicked();
    void strikeClicked();
    void momentClicked();
    void blamPafActivated(int);
    void insertBlam();
    void insertPaf();
};

#endif // QQPALMIPEDE_H

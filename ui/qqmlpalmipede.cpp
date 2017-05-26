#include "qqmlpalmipede.h"

#include <QQmlContext>
/**
 * @brief QQmlPalmipede::QQmlPalmipede
 * @param parent
 */
QQmlPalmipede::QQmlPalmipede(QWidget *parent)
{
	setSource(QUrl("qrc:/qml/QQmlPalmi.qml"));
	rootContext()->setContextProperty("backend", this);
}

/**
 * @brief QQmlPalmipede::changeBoard
 * @param next
 */
void QQmlPalmipede::changeBoard(bool next)
{

}

/**
 * @brief QQmlPalmipede::insertReplaceText
 * @param bouchot
 * @param tag
 */
void QQmlPalmipede::insertReplaceTextBoard(const QString &board, const QString &tag)
{

}

/**
 * @brief QQmlPalmipede::insertReplaceText
 * @param tag
 */
void QQmlPalmipede::insertReplaceText(const QString &tag)
{

}

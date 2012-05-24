#ifndef QQMESSAGEBLOCKUSERDATA_H
#define QQMESSAGEBLOCKUSERDATA_H

#include <core/qqnorlogeref.h>

#include <QList>
#include <QMap>
#include <QPair>
#include <QString>
#include <QTextBlockUserData>
#include <QVariant>

class QQMessageBlockUserData : public QTextBlockUserData
{
public:
    QQMessageBlockUserData();

    ~QQMessageBlockUserData();

    bool constainsData(int key);
    QVariant getData(int key);
    void storeData(int key, const QVariant & data);
    void removeData(int key);

    void addNorlogeRefZone(const QQNorlogeRef & norlogeRef) { m_listNRef.append(norlogeRef); }
    QQNorlogeRef norlogeRefForIndex(int index);
    QList<QQNorlogeRef> norlogeRefs() { return m_listNRef; }

    void addTotozZone(int index, const QString & totoz) { m_mapTotoz.insert(index, totoz); }
    QPair<int, QString> totozForIndex(int index) { return stringForIndex(index, m_mapTotoz); }

    void addDuckZone(int index, const QString & duck) { m_mapDuck.insert(index, duck); }
    QPair<int, QString> duckForIndex(int index) { return stringForIndex(index, m_mapDuck); }

    void addTableVZone(int index, const QString & tableV) { m_mapTableV.insert(index, tableV); }
    QPair<int, QString> tableVForIndex(int index) { return stringForIndex(index, m_mapTableV); }

	void setWasParsed(bool wasParsed) { m_wasParsed = wasParsed; }
	bool wasParsed() { return m_wasParsed; }

	void setHighlighted() { m_isHighlighted = true; }
	void resetHighlighted() { m_isHighlighted = false; }
	bool isHighlighted() { return m_isHighlighted; }


    enum dateKeys
    {
        BOUCHOT_NAME,
        POST_NORLOGE,
        IS_LOGIN_UA_ZONE,
        IS_NORLOGE_ZONE,
        IS_MESSAGE_ZONE
    };

private:
    QPair<int, QString> stringForIndex(int index, const QMap<int, QString> & map);
    bool isIndexInString(int index, int stringIndex, const QString & string);

    QMap<int, QVariant> m_customData;

    QList<QQNorlogeRef> m_listNRef;
    QMap<int, QString> m_mapTotoz;
    QMap<int, QString> m_mapDuck;
    QMap<int, QString> m_mapTableV;

    bool m_wasParsed;
    bool m_isHighlighted;

};

#endif // QQMESSAGEBLOCKUSERDATA_H

#ifndef QQMESSAGEBLOCKUSERDATA_H
#define QQMESSAGEBLOCKUSERDATA_H

#include <QList>
#include <QMap>
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

    void addNorlogeRefZone(int index, const QString & norlogeRef) { m_mapNRef.insert(index, norlogeRef); }
    QString norlogeRefForIndex(int index) { return stringForIndex(index, m_mapNRef); }

    void addTotozZone(int index, const QString & totoz) { m_mapTotoz.insert(index, totoz); }
    QString totozForIndex(int index) { return stringForIndex(index, m_mapTotoz); }

    void addDuckZone(int index, const QString & duck) { m_mapDuck.insert(index, duck); }
    QString duckForIndex(int index) { return stringForIndex(index, m_mapDuck); }

    void addTableVZone(int index, const QString & tableV) { m_mapTableV.insert(index, tableV); }
    QString tableVForIndex(int index) { return stringForIndex(index, m_mapTableV); }

	void setWasParsed(bool wasParsed) { m_wasParsed = wasParsed; }
	bool wasParsed() { return m_wasParsed; }


    enum dateKeys
    {
        BOUCHOT_NAME,
        POST_NORLOGE,
        IS_LOGIN_UA_ZONE,
        IS_NORLOGE_ZONE,
        IS_MESSAGE_ZONE
    };

private:
    QString stringForIndex(int index, const QMap<int, QString> & map);

    QMap<int, QVariant> m_customData;

    QMap<int, QString> m_mapNRef;
    QMap<int, QString> m_mapTotoz;
    QMap<int, QString> m_mapDuck;
    QMap<int, QString> m_mapTableV;

	bool m_wasParsed;

};

#endif // QQMESSAGEBLOCKUSERDATA_H

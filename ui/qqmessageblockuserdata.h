#ifndef QQMESSAGEBLOCKUSERDATA_H
#define QQMESSAGEBLOCKUSERDATA_H

#include <QMap>
#include <QString>
#include <QTextBlockUserData>
#include <QVariant>

class QQMessageBlockUserData : public QTextBlockUserData
{
public:
    QQMessageBlockUserData();

    bool constainsData(int key);
    QVariant getData(int key);
    void storeData(int key, const QVariant & data);
    void removeData(int key);

    enum dateKeys
    {
        BOUCHOT_NAME,
        POST_NORLOGE,
        IS_LOGIN_UA_ZONE,
        IS_NORLOGE_ZONE,
        IS_MESSAGE_ZONE
    };

private:
    QMap<int, QVariant> m_customData;
};

#endif // QQMESSAGEBLOCKUSERDATA_H

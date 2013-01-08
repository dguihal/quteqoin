#ifndef QQMESSAGEBLOCKUSERDATA_H
#define QQMESSAGEBLOCKUSERDATA_H

#include <core/qqbigornoitem.h>
#include <core/qqnorlogeref.h>
#include <core/qqpost.h>
#include <core/qqtotoz.h>

#include <QList>
#include <QPair>
#include <QPointer>
#include <QString>
#include <QTextBlockUserData>
#include <QVariant>

class QQMessageBlockUserData : public QTextBlockUserData
{
public:
	class ZoneRange {
	public:
		bool isInRange( const int val ) { return val >= begin && val <= end; }
		int begin;
		int end;
	};

	enum zoneRangeID { NORLOGE, LOGINUA, MESSAGE };

	QQMessageBlockUserData();
	QQMessageBlockUserData(const QQMessageBlockUserData & userData);
	~QQMessageBlockUserData();

	ZoneRange zRangeForID( const zoneRangeID zrId ) { return m_ranges[zrId]; }
	void setZRange( const zoneRangeID zrId, const ZoneRange range ) { m_ranges.insert(zrId, range); }
	bool isIndexInZRange( const int index, const zoneRangeID zrId );

	QQPost * post() { return m_post; }
	void setPost(QQPost * newPost) { m_post = QPointer<QQPost>(newPost); }

	void addNorlogeRefZone(const QQNorlogeRef & norlogeRef) { m_listNRef.append(norlogeRef); }
	QQNorlogeRef norlogeRefForIndex(int index);
	QList<QQNorlogeRef> norlogeRefs() { return m_listNRef; }

	void addTotozZone(const QQTotoz & totoz) { m_listTotoz.append(totoz); }
	QQTotoz totozForIndex(int index);
	QList<QQTotoz> totozZones() { return m_listTotoz; }

	void addDuckZone(int index, const QString & duck) { m_mapDuck.insert(index, duck); }
	QPair<int, QString> duckForIndex(int index) { return stringForIndex(index, m_mapDuck); }

	void addTableVZone(int index, const QString & tableV) { m_mapTableV.insert(index, tableV); }
	QPair<int, QString> tableVForIndex(int index) { return stringForIndex(index, m_mapTableV); }

	void addBigornoZone(QQBigornoItem item) { m_listBigorno.append(item); }
	QList<QQBigornoItem> bigornoItems() { return m_listBigorno; }

	void setParsed() { m_wasParsed = true; }
	bool wasParsed() { return m_wasParsed; }

	void setHighlighted() { m_isHighlighted = true; }
	void resetHighlighted() { m_isHighlighted = false; }
	bool isHighlighted() { return m_isHighlighted; }

	bool isNew() { return m_isNew; }
	void setAcknowledged() { m_isNew = false; }

	bool isValid() { return m_ranges.size() > 0; }


private:
	QPair<int, QString> stringForIndex(int index, const QMap<int, QString> & map);
	bool isIndexInString(int index, int stringIndex, const QString & string);

	QList<QQNorlogeRef> m_listNRef;
	QList<QQBigornoItem> m_listBigorno;
	QList<QQTotoz> m_listTotoz;
	QMap<int, QString> m_mapDuck;
	QMap<int, QString> m_mapTableV;

	bool m_wasParsed;
	bool m_isHighlighted;
	bool m_isNew;

	QHash<int, ZoneRange> m_ranges;

	QPointer<QQPost> m_post;

};

#endif // QQMESSAGEBLOCKUSERDATA_H

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
		bool isInRange(const int val) const { return val >= begin && val <= end; }
		int begin;
		int end;
	};

	enum zoneRangeID { NORLOGE, LOGINUA, MESSAGE };

	explicit QQMessageBlockUserData();
	QQMessageBlockUserData(const QQMessageBlockUserData &userData);

	ZoneRange zRangeForID(const zoneRangeID zrId) const { return m_ranges[zrId]; }
	void setZRange(const zoneRangeID zrId, const ZoneRange range) { m_ranges.insert(zrId, range); }
	bool isIndexInZRange(const int index, const zoneRangeID zrId) const { return zRangeForID(zrId).isInRange(index); }

	QQPost * post() { return m_post; }
	void setPost(QQPost *newPost) { m_post = QPointer<QQPost>(newPost); }

	void addNorlogeRefZone(const QQNorlogeRef &norlogeRef) { m_listNRef.append(norlogeRef); }
	QQNorlogeRef norlogeRefForIndex(const int index) const;
	QList<QQNorlogeRef> norlogeRefs() const { return m_listNRef; }
	bool hasNRefToSelfPost() const;

	void addTotozZone(int index, const QString &totozId) { m_mapTotozId.insert(index, totozId); }
	QPair<int, QString> totozIdAndIndexForIndex(int index) const { return stringForIndex(index, m_mapTotozId); }
	QString totozIdForIndex(const int index) const { return totozIdAndIndexForIndex(index).second; }
	QList<int> totozIndexes() const { return m_mapTotozId.uniqueKeys(); }

	void addDuckZone(int index, const QString &duck) { m_mapDuck.insert(index, duck); }
	QPair<int, QString> duckForIndex(const int index) const { return stringForIndex(index, m_mapDuck); }
	QList<int> duckIndexes() const { return m_mapDuck.uniqueKeys(); }

	void addTableVZone(int index, const QString &tableV) { m_mapTableV.insert(index, tableV); }
	QPair<int, QString> tableVForIndex(const int index) const { return stringForIndex(index, m_mapTableV); }
	QList<int> tableVIndexes() const { return m_mapTableV.uniqueKeys(); }

	void addBigornoZone(QQBigornoItem item) { m_listBigorno.append(item); }
	QList<QQBigornoItem> bigornoItems() const { return m_listBigorno; }

	void setParsed() { m_wasParsed = true; }
	bool wasParsed() const { return m_wasParsed; }

	bool isValid() const { return m_ranges.size() > 0; }


private:
	QPair<int, QString> stringForIndex(int index, const QMap<int, QString> &map) const;
	bool isIndexInString(int index, int stringIndex, const QString &string) const;

	QList<QQNorlogeRef> m_listNRef;
	QList<QQBigornoItem> m_listBigorno;
	QMap<int, QString> m_mapTotozId;
	QMap<int, QString> m_mapDuck;
	QMap<int, QString> m_mapTableV;

	bool m_wasParsed;

	QHash<int, ZoneRange> m_ranges;

	QPointer<QQPost> m_post;

};

#endif // QQMESSAGEBLOCKUSERDATA_H

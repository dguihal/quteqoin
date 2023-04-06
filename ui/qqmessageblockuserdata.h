#ifndef QQMESSAGEBLOCKUSERDATA_H
#define QQMESSAGEBLOCKUSERDATA_H

#include <core/qqbigornoitem.h>
#include <core/qqnorlogeref.h>
#include <core/qqpost.h>
#include <core/qqtotoz.h>

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

	int appendNorlogeRef(const QQNorlogeRef &norlogeRef) { m_listNRef.append(norlogeRef); return (m_listNRef.size() - 1); }
	QQNorlogeRef norlogeRefForIndex(const int index) const;

	QList<QQNorlogeRef> norlogeRefs() const { return m_listNRef; }
	bool hasNRefToSelfPost() const;

	void setHasDuck() { m_hasDuck = true; }
	bool hasDuck() { return m_hasDuck; }

	void setHasTableV() { m_hasTableV = true; }
	bool hasTableV() { return m_hasTableV; }

	void addBigornoZone(QQBigornoItem item) { m_listBigorno.append(item); }
	QList<QQBigornoItem> bigornoItems() const { return m_listBigorno; }

	void setParsed() { m_wasParsed = true; }
	bool wasParsed() const { return m_wasParsed; }

	bool isValid() const { return m_ranges.size() > 0; }

private:
	QList<QQNorlogeRef> m_listNRef;
	QList<QQBigornoItem> m_listBigorno;

	bool m_wasParsed;
	bool m_hasDuck;
	bool m_hasTableV;

	QHash<int, ZoneRange> m_ranges;

	QPointer<QQPost> m_post;

};

#endif // QQMESSAGEBLOCKUSERDATA_H

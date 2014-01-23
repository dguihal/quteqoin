#ifndef QQNORLOGE_H
#define QQNORLOGE_H

#include <QRegExp>
#include <QString>

class QQNorloge
{
public:
	QQNorloge();
	QQNorloge(QString bouchot, QString dateh);
	QQNorloge(const QQNorloge& norloge);

	QStringList matchingNRefsId() const;

	QString toStringPalmi();
	QString srcBouchot() { return m_srcBouchot; }

	void setNorlogeIndex(int index) { m_norlogeIndex = index;}
	int norlogeIndex() { return m_norlogeIndex; }

protected:
	QString m_srcBouchot;

	QString m_dateYearPart;
	QString m_dateMonthPart;
	QString m_dateDayPart;
	QString m_dateHourPart;
	QString m_dateMinutePart;
	QString m_dateSecondPart;

	int m_norlogeIndex;
};

#endif // QQNORLOGE_H

#include "qqbigornoitem.h"

QQBigornoItem::QQBigornoItem(int postition, const QString & word)
{
	m_position = postition;
	m_word = word;
	m_wasNotified = false;
}


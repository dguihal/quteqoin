#include "qqbackend.h"

QQBackend::QQBackend(const QString &name, QObject *parent) :
	QQNetworkAccessor(parent),
	m_name(name),
	m_parser(nullptr)
{

}

QQBackend::~QQBackend()
{

}

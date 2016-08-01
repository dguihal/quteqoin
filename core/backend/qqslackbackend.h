#ifndef QQSLACKBACKEND_H
#define QQSLACKBACKEND_H

#include "core/backend/qqbackend.h"

class QQSlackBackend : public QQBackend
{
	Q_OBJECT
public:
	explicit QQSlackBackend(const QString &name, QObject *parent = 0);

	virtual QQBackend::BackendType backendType();
signals:

public slots:
};

#endif // QQSLACKBACKEND_H

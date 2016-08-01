#include "qqslackbackend.h"

// curl 'https://slack.com/api/channels.list?token=xoxp-17981889749-17981889893-17980689508-26e22c8e18'
// curl 'https://slack.com/api/chat.postMessage?token=xoxp-17981889749-17981889893-17980689508-26e22c8e18&as_user=true&channel=C0HUZPCMV'
// curl 'https://slack.com/api/channels.history?token=xoxp-17981889749-17981889893-17980689508-26e22c8e18&as_user=true&channel=C0HUZPCMV'

QQSlackBackend::QQSlackBackend(const QString &name, QObject *parent) : QQBackend(name, parent)
{

}


QQBackend::BackendType QQSlackBackend::backendType()
{
	return QQBackend::Slack;
}

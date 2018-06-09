#include "precomp.h"
#include "JSearchWatcher.h"
#include "private/jsearchwatcher_p.h"
#include <QCoreApplication>

using namespace icdmeta;

// class JSearchWatcher

JSearchWatcher::JSearchWatcher(QJSValue callback, QObject *target, QObject *parent)
    : JWatcher(callback, target, parent)
{

}

JSearchWatcher::~JSearchWatcher()
{

}

void JSearchWatcher::cancel()
{
    setTarget(nullptr);
    JWatcher::cancel();
    waitForFinished();
    QCoreApplication::removePostedEvents(this, Event_SetProperty);
}

void JSearchWatcher::customEvent(QEvent *event)
{
    switch (static_cast<int>(event->type())) {
    case Event_SetProperty:
    {
        SetPropertyEvent *spEvent = reinterpret_cast<SetPropertyEvent *>(event);
        if (!spEvent) {
            break;
        }
        spEvent->execute(target());
        break;
    }
    default:
        break;
    }
}

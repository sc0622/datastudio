#ifndef JSEARCHWATCHER_P_H
#define JSEARCHWATCHER_P_H

#include "../../icdmeta_global.h"
#include "../../core/jicditem.h"
#include <QSharedPointer>

namespace icdmeta {

// class SetPropertyEvent

class SetPropertyEvent : public QEvent
{
    Q_GADGET
public:
    explicit SetPropertyEvent(const QString &name, const QVariant &value,
                              const QSharedPointer<QObject> &watcher = QSharedPointer<QObject>())
        : QEvent(static_cast<Type>(Event_SetProperty))
        , name(name)
        , value(value)
        , watcher(watcher)
    {

    }

    void execute(QObject *target)
    {
        if (!target) {
            return;
        }
        target->setProperty(name.toLatin1(), value);
    }

private:
    QString name;
    QVariant value;
    QSharedPointer<QObject> watcher;
};

}

#endif // JSEARCHWATCHER_P_H


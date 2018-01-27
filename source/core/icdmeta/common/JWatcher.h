#ifndef JWATCHER_H
#define JWATCHER_H

#include "../icdmeta_global.h"
#include <QFutureWatcher>

namespace icdmeta {

template <typename T>
class JWatcher : public QFutureWatcher<T>
{
public:
    explicit JWatcher(QObject *parent = nullptr)
        : QFutureWatcher(parent)
        , _callback(QJSValue::UndefinedValue)
        , _target(nullptr)
    {

    }

    explicit JWatcher(QJSValue callback, QObject *target, QObject *parent = nullptr)
        : QFutureWatcher(parent)
        , _callback(callback)
        , _target(target)
    {

    }

    virtual ~JWatcher()
    {
        //qDebug() << "~JWatcher";
    }

    static void registerQmlType();

    QJSValue callback() const
    {
        return _callback;
    }

    void setCallback(QJSValue callback)
    {
        _callback = callback;
    }

    QObject *target() const
    {
        return _target;
    }

    void setTarget(QObject *target)
    {
        _target = target;
    }

    bool isValid() const
    {
        return (_target != nullptr);
    }

private:
    Q_DISABLE_COPY(JWatcher)
    QJSValue _callback;
    QObject *_target;
};

}

#endif // JWATCHER_H

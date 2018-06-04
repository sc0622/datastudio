#ifndef ICDMETA_NOTIFY_H
#define ICDMETA_NOTIFY_H

#include "../icdmeta_global.h"
#include <functional>
#include <QJSValue>

namespace icdmeta {

// class JNEvent

class ICDMETA_EXPORT JNEvent
{
public:
    explicit JNEvent();
    explicit JNEvent(const QString &channel, const QVariant &argument);
    virtual ~JNEvent();

    JNEvent(const JNEvent &other)
    {
        *this = other;
    }

    JNEvent &operator =(const JNEvent &other)
    {
        if (this == &other) {
            return *this;
        }
        _channel = other._channel;
        _argument = other._argument;
        _returnValue = other._returnValue;
        return *this;
    }

    const QString &channel() const { return _channel; }
    const QVariant &argument() const { return _argument; }
    const QVariant &returnValue() const { return _returnValue; }
    void setReturnValue(const QVariant &value) { _returnValue = value; }

    QJSValueList toList() const;

private:
    QString _channel;
    QVariant _argument;
    QVariant _returnValue;
    friend class JNotify;
};

//
typedef std::function<void()> JNotifyCallbackEmpty;
typedef std::function<void(JNEvent &event)> JNotifyCallback;

// class JNotify

class JNotify;
typedef std::shared_ptr<JNotify> JNotifyPtr;
class JNotifyPrivate;

class ICDMETA_EXPORT JNotify : public QObject
{
    Q_OBJECT
public:
    static void registerQmlType();

    enum InternalEvent {
        Evt_PostMsg = QEvent::User + 1
    };

    static JNotify *inst(const QString &name, QObject *parent = nullptr);

    Q_INVOKABLE void on(const QString &channel, JNotifyCallback callback, bool sync = true);
    Q_INVOKABLE void on(const QString &channel, QObject *receiver, JNotifyCallback callback, bool sync = true);
    Q_INVOKABLE QVariant send(const QString &channel, const QVariant &argument = QVariant());
    Q_INVOKABLE QVariant send(const QString &channel, QObject *receiver, const QVariant &data = QVariant());
    Q_INVOKABLE void post(const QString &channel, const QVariant &argument = QVariant());
    Q_INVOKABLE void post(const QString &channel, QObject *receiver, const QVariant &arguments = QVariant());
    Q_INVOKABLE void postJs(const QString &channel, const QString &argument = QString());
    Q_INVOKABLE bool has(const QString &channel) const;
    Q_INVOKABLE void un(const QString &channel);
    Q_INVOKABLE void un(QObject *receiver);
    Q_INVOKABLE void clear();

    // for qml/js
    Q_INVOKABLE void on(const QString &channel, QJSValue callback, bool sync = true);
    Q_INVOKABLE void on(const QString &channel, QObject *receiver, QJSValue callback, bool sync = true);
    Q_INVOKABLE QJSValue jsSend(const QString &channel, const QJSValue &argument = QJSValue());

signals:

public slots:

protected:
    void customEvent(QEvent *event);

private:
    explicit JNotify(QObject *parent = nullptr);
    ~JNotify();

private:
    Q_DISABLE_COPY(JNotify)
    J_DECLARE_PRIVATE(JNotify)
    J_FRIEND_HANDLEPTR()
};

}

#endif // ICDMETA_NOTIFY_H

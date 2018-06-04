#ifndef JCMDOBJECT_H
#define JCMDOBJECT_H

#include "../icdmeta_global.h"
#include "JCmdChannel.h"
#include <functional>

class QJSValue;

namespace icdmeta {

class JCmdObjectPrivate;

class ICDMETA_EXPORT JCmdObject : public QObject, public Json::Serializable
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString arguments READ arguments WRITE setArguments NOTIFY argumentsChanged)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY suffixChanged)
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(QString parsedPattern READ parsedPattern NOTIFY parsedPatternChanged)
    Q_PROPERTY(bool sync READ isSync WRITE setSync NOTIFY syncChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(QString desc READ desc WRITE setDesc NOTIFY descChanged)
    Q_PROPERTY(bool blocking READ blocking WRITE setBlocking NOTIFY blockingChanged)
    Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_PROPERTY(int acceptCount READ acceptCount WRITE setAcceptCount NOTIFY acceptCountChanged)
public:
    explicit JCmdObject(QObject *parent = nullptr);
    ~JCmdObject();

    static void registerQmlType();

    bool isValid() const;

    QString name() const;
    QString arguments() const;
    QString suffix() const;
    QString pattern() const;
    QString parsedPattern() const;
    bool isSync() const;
    int timeout() const;
    QString message() const;
    QString desc() const;
    bool blocking() const;
    int priority() const;
    int acceptCount() const;

    virtual JCmdObject *clone() const;
    JCmdObject &operator =(const JCmdObject &other);

    Q_INVOKABLE QByteArray toByteArray() const;

    Q_INVOKABLE void setCallback(const std::function<void(QString)> &callback,
                                 const QString &guid);
    Q_INVOKABLE void setCallback(QJSValue &callback,
                                 const QString &guid);
    Q_INVOKABLE bool callWithClear();
    Q_INVOKABLE void clearCallback(const QString &guid);

protected:
    JCmdObject(const JCmdObject &other);

signals:
    void nameChanged(const QString &value);
    void argumentsChanged(const QString &value);
    void suffixChanged(const QString &value);
    void patternChanged(const QString &value);
    void parsedPatternChanged();
    void syncChanged(bool value);
    void timeoutChanged(int value);
    void messageChanged(const QString &value);
    void descChanged(const QString &value);
    void blockingChanged(bool value);
    void priorityChanged(int value);
    void acceptCountChanged(int value);

public slots:
    void setArguments(const QString &value);

private slots:
    void setName(const QString &value);
    void setSuffix(const QString &value);
    void setPattern(const QString &value);
    void setSync(bool value);
    void setTimeout(int value);
    void setMessage(const QString &value);
    void setDesc(const QString &value);
    void setBlocking(bool value);
    void setPriority(int value);
    void setAcceptCount(int value);

    // Serializable interface
public:
    Json::Value save() const override;
    bool restore(const Json::Value &json, int /*deep*/ = -1) override;

private:
    void parsePattern();

private:
    //Q_DISABLE_COPY(JCmdObject)
    J_DECLARE_PRIVATE(JCmdObject)
    friend class JCmdModel;
    friend class JCmdModelPrivate;
};

}

#endif // JCMDOBJECT_H

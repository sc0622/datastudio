#ifndef JLOGMODEL_H
#define JLOGMODEL_H

#include "../icdmeta_global.h"

namespace icdmeta {

class JLogModelPrivate;

class ICDMETA_EXPORT JLogModel : public QObject
{
    Q_OBJECT
public:
    static void registerQmlType();

    static QString moduleName();
    static void setModelName(const QString &name);

    bool init();

    Q_INVOKABLE void emerg(const QString &msg);
    Q_INVOKABLE void fatal(const QString &msg);
    Q_INVOKABLE void alert(const QString &msg);
    Q_INVOKABLE void crit(const QString &msg);
    Q_INVOKABLE void error(const QString &msg);
    Q_INVOKABLE void warn(const QString &msg);
    Q_INVOKABLE void notice(const QString &msg);
    Q_INVOKABLE void info(const QString &msg);
    Q_INVOKABLE void debug(const QString &msg);

    Q_INVOKABLE void operate(const QString &msg, QChar channel);

signals:

public slots:

private:
    explicit JLogModel(QObject *parent = nullptr);
    ~JLogModel();

private:
    Q_DISABLE_COPY(JLogModel)
    J_DECLARE_PRIVATE(JLogModel)
    J_DECLARE_SINGLE_INSTANCE(JLogModel)
};

}

#endif // JLOGMODEL_H

#ifndef LOGGING_H
#define LOGGING_H

#include "main_global.h"

class LoggingPrivate;

class Logging : public QObject
{
    Q_OBJECT
public:
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

signals:

public slots:

private:
    explicit Logging(QObject *parent = nullptr);
    ~Logging();

private:
    Q_DISABLE_COPY(Logging)
    J_DECLARE_PRIVATE(Logging)
    J_DECLARE_SINGLE_INSTANCE(Logging)
};

#endif // LOGGING_H

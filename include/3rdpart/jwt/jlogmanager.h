#ifndef JLOGMANAGER_H
#define JLOGMANAGER_H

#include "jwt_global.h"
#if QT_VERSION >= 0x050000
#if !defined(Q_OS_MAC)
#include <QLoggingCategory>
#endif
#endif

// - class JLogManager -

#if QT_VERSION >= 0x050000

class JLogManagerPrivate;

class JWT_EXPORT JLogManager
{
public:
    enum LogType { LogConsole, LogFile };
    static void installMessageHandler();
#if !defined(Q_OS_MAC)
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#endif
    static void setMessagePattern(const QString &pattern);

    static LogType logType();
    static void setLogType(LogType logType);

private:
    JLogManager();
    ~JLogManager();

private:
    J_DECLARE_SINGLE_INSTANCE(JLogManager)
    J_DECLARE_PRIVATE(JLogManager)
};
#else

class JWT_EXPORT JLogManager
{
public:

private:
    JLogManager();
    ~JLogManager();
};

#endif

#endif // JLOGMANAGER_H

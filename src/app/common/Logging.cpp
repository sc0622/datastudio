#include "precomp.h"
#include "Logging.h"
#include "log4cpp/Category.hh"
#include "log4cpp/PropertyConfigurator.hh"
#include "log4cpp/convenience.h"

// class LoggingPrivate

class LoggingPrivate
{
public:
    LoggingPrivate(Logging *q)
        : q_ptr(q)
        , categorySystem(log4cpp::Category::getInstance("datastudio.system"))
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(Logging)
    log4cpp::Category &categorySystem;
};

void LoggingPrivate::init()
{
    const QString filePath(JMain::instance()->configDir().append("/logging.conf"));
    try {
        log4cpp::PropertyConfigurator::configure(filePath.toStdString());
    } catch (log4cpp::ConfigureFailure cf) {
#if defined(__unix__)
        if (QString::fromLatin1(cf.what()).startsWith(
                    "Appender 'console_win32' has unknown type 'Win32DebugAppender'")) {
            //
        } else
#endif
        {
            //
            Q_ASSERT_X(false, "Warning",
                       Logging::tr("Loading log configuration file failure![file: %1] [desc: %2]")
                       .arg(filePath)
                       .arg(cf.what())
                       .toUtf8().data());
            return;
        }
    }
}

// class Logging

J_IMPLEMENT_SINGLE_INSTANCE(Logging, JMain)

Logging::Logging(QObject *parent)
    : QObject(parent)
    , d_ptr(new LoggingPrivate(this))
{
    Q_D(Logging);
    d->init();
}

Logging::~Logging()
{
    Q_D(Logging);
    d->categorySystem.shutdown();
    delete d;
}

bool Logging::init()
{
    return true;
}

void Logging::emerg(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_EMERG(d->categorySystem, msg.toStdString());
}

void Logging::fatal(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_FATAL(d->categorySystem, msg.toStdString());
}

void Logging::alert(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_ALERT(d->categorySystem, msg.toStdString());
}

void Logging::crit(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_CRIT(d->categorySystem, msg.toStdString());
}

void Logging::error(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_ERROR(d->categorySystem, msg.toStdString());
}

void Logging::warn(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_WARN(d->categorySystem, msg.toStdString());
}

void Logging::notice(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_NOTICE(d->categorySystem, msg.toStdString());
}

void Logging::info(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_INFO(d->categorySystem, msg.toStdString());
}

void Logging::debug(const QString &msg)
{
    Q_D(Logging);
    LOG4CPP_DEBUG(d->categorySystem, msg.toStdString());
}

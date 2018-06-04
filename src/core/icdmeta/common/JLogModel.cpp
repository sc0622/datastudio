#include "precomp.h"
#include "JLogModel.h"
#include "log4cpp/Category.hh"
#include "log4cpp/PropertyConfigurator.hh"
#include "log4cpp/convenience.h"

namespace icdmeta {

// class JLogModelPrivate

class JLogModelPrivate
{
public:
    JLogModelPrivate(JLogModel *q)
        : J_QPTR(q)
        , categorySystem(log4cpp::Category::getInstance(moduleName + ".system"))
        , categoryOperateA(log4cpp::Category::getInstance(moduleName + ".operate_a"))
        , categoryOperateB(log4cpp::Category::getInstance(moduleName + ".operate_b"))
        , categoryOperateC(log4cpp::Category::getInstance(moduleName + ".operate_c"))
    {

    }

    void init();

private:
    J_DECLARE_PUBLIC(JLogModel)
    QString configDir;
    static std::string moduleName;
    log4cpp::Category &categorySystem;
    log4cpp::Category &categoryOperateA;
    log4cpp::Category &categoryOperateB;
    log4cpp::Category &categoryOperateC;
};

std::string JLogModelPrivate::moduleName = std::string();

void JLogModelPrivate::init()
{
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
    configDir = settings.value("CONFIG_DIR").toString();
    settings.endGroup();

    const QString filePath(configDir+ "/logging.conf");
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
                       JLogModel::tr("Loading log configuration file failure![file: %1] [desc: %2]")
                       .arg(filePath)
                       .arg(cf.what())
                       .toUtf8().data());
            return;
        }
    }
}

// class JLogModel

J_QML_IMPLEMENT_SINGLE_INSTANCE(JLogModel, QQmlEngine::CppOwnership, IcdCore)

JLogModel::JLogModel(QObject *parent)
    : QObject(parent)
    , J_DPTR(new JLogModelPrivate(this))
{
    Q_D(JLogModel);
    d->init();
}

JLogModel::~JLogModel()
{
    Q_D(JLogModel);
    d->categorySystem.shutdown();
    delete d;
}

void JLogModel::registerQmlType()
{
    //
    IcdMetaRegisterSingletonType2(JLogModel, "Logging");

    //
}

QString JLogModel::moduleName()
{
    return QString::fromStdString(JLogModelPrivate::moduleName);
}

void JLogModel::setModelName(const QString &name)
{
    JLogModelPrivate::moduleName = name.toStdString();
}

bool JLogModel::init()
{
    return true;
}

void JLogModel::emerg(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_EMERG(d->categorySystem, msg.toStdString());
}

void JLogModel::fatal(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_FATAL(d->categorySystem, msg.toStdString());
}

void JLogModel::alert(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_ALERT(d->categorySystem, msg.toStdString());
}

void JLogModel::crit(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_CRIT(d->categorySystem, msg.toStdString());
}

void JLogModel::error(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_ERROR(d->categorySystem, msg.toStdString());
}

void JLogModel::warn(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_WARN(d->categorySystem, msg.toStdString());
}

void JLogModel::notice(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_NOTICE(d->categorySystem, msg.toStdString());
}

void JLogModel::info(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_INFO(d->categorySystem, msg.toStdString());
}

void JLogModel::debug(const QString &msg)
{
    Q_D(JLogModel);
    LOG4CPP_DEBUG(d->categorySystem, msg.toStdString());
}

void JLogModel::operate(const QString &msg, QChar channel)
{
    if (msg.isEmpty()) {
        return;
    }

    const char lowerChannel = channel.toUpper().toLatin1();
    const std::string message = msg.toStdString();

    Q_D(JLogModel);
    switch (lowerChannel) {
    case 'A':
        LOG4CPP_INFO(d->categoryOperateA, message);
        break;
    case 'B':
        LOG4CPP_INFO(d->categoryOperateB, message);
        break;
    case 'C':
        LOG4CPP_INFO(d->categoryOperateC, message);
        break;
    default:
        break;
    }
}

}

#include "precomp.h"
#include "icdmeta_global.h"
#include "core/jicdtable.h"
#include "channel/JChannelPool.h"
#include "protocol/JDataChannelMgr.h"
#include "protocol/JFilterChannel.h"
#include "protocol/JProtocolPool.h"
#include "common/JLangModel.h"
#include "common/JLogModel.h"
#include "common/JEncryptModel.h"
#include "common/JLoginModel.h"
#include "command/JCmdModel.h"
#include "command/simulate/JCmdSysModelSim.h"
#include "common/ftp/JFtpModel.h"
#include <QQuickStyle>
#include <QFontDatabase>
#include <QQmlEngine>

namespace icdmeta {

// class

struct JCallbackEventData
{
    QJSValue callback;
    QJSValueList arguments;
};

// class JCallbackEvent

JCallbackEvent::JCallbackEvent(QJSValue callback, const QJSValueList &arguments)
    : QEvent(static_cast<Type>(Event_Callback))
    , data(new JCallbackEventData)
{
    data->callback = callback;
    data->arguments = arguments;
}

JCallbackEvent::~JCallbackEvent()
{
    delete data;
}

bool JCallbackEvent::isValid() const
{
    return data->callback.isCallable();
}

void JCallbackEvent::execute()
{
    if (!data->callback.isCallable()) {
        return;
    }
    QJSValue _callback = data->callback;
    data->callback = QJSValue();
    QJSValue ret = _callback.call(data->arguments);
    if (ret.isError()) {
        qDebug() << ret.toString();
    }
    data->arguments.clear();
}

}

// class IcdCorePrivate

class IcdCorePrivate
{
public:
    IcdCorePrivate(IcdCore *q)
        : J_QPTR(q)
        , fontSize(10)
        , translator(nullptr)
    {
        const QString dir = QDir(QCoreApplication::applicationDirPath().append("/../config"))
                .canonicalPath();
        if (QDir(dir).exists()) {
            configDir = dir;
        }
    }

    void init();
    QJSValueList parseArgs(const QJSValue &args);

private:
    J_DECLARE_PUBLIC(IcdCore)
    bool simulate;
    QList<SingletonReleaseCallback> callbacks;
    QString configDir;
    int fontSize;
    QTimer *timerDelay;
    QTranslator *translator;
};

void IcdCorePrivate::init()
{
    Q_Q(IcdCore);
    timerDelay = new QTimer(q);
    timerDelay->setSingleShot(true);
    translator = new QTranslator(q);
}

QJSValueList IcdCorePrivate::parseArgs(const QJSValue &args)
{
    QJSValueList arguments;
    if (args.isArray()) {
        QVariantList vArgs = args.toVariant().toList();
        for (auto arg : vArgs) {
            switch (arg.type()) {
            case QVariant::Bool: arguments.append(arg.toBool()); break;
            case QVariant::Int: arguments.append(arg.toInt()); break;
            case QVariant::UInt: arguments.append(arg.toUInt()); break;
            case QVariant::Double: arguments.append(arg.toDouble()); break;
            case QVariant::String: arguments.append(arg.toString()); break;
            default: break;
            }
        }
    }
    return arguments;
}

// class IcdCore

J_QML_IMPLEMENT_SINGLE_INSTANCE(IcdCore, QQmlEngine::CppOwnership, IcdCore)

void IcdCore::registerQmlType()
{
    IcdMetaRegisterSingletonType3(IcdCore);

    icdmeta::JLangModel::registerQmlType();
    icdmeta::JLogModel::registerQmlType();
    icdmeta::JEncryptModel::registerQmlType();
    icdmeta::JLoginModel::registerQmlType();
    icdmeta::JFtpModel::registerQmlType();
    icdmeta::JIcdTable::registerQmlType();
    icdmeta::JChannelPool::registerQmlType();
    icdmeta::JProtocolPool::registerQmlType();
    icdmeta::JDataChannelMgr::registerQmlType();
    icdmeta::JFilterChannel::registerQmlType();
    icdmeta::JCmdModel::registerQmlType();
}

QString IcdCore::stringSection(const QString &source, QChar sep,
                               int start, int end, QString::SectionFlags flags) const
{
    return source.section(sep, start, end, flags);
}

QString IcdCore::stringSection(const QString &source, const QString &in_sep,
                               int start, int end, QString::SectionFlags flags) const
{
    return source.section(in_sep, start, end, flags);
}

QString IcdCore::stringSection(const QString &source, const QRegExp &reg,
                               int start, int end, QString::SectionFlags flags) const
{
    return source.section(reg, start, end, flags);
}

bool IcdCore::matchString(const QString &pattern, const QString &source) const
{
    return QRegExp(pattern).exactMatch(source);
}

bool IcdCore::matchString(const QRegExp &regexp, const QString &source) const
{
    return regexp.exactMatch(source);
}

QStringList IcdCore::splitString(const QString &sep, const QString &source) const
{
    return source.split(sep, QString::SkipEmptyParts);
}

QStringList IcdCore::splitString(const QRegExp &regexp, const QString &source) const
{
    return source.split(regexp, QString::SkipEmptyParts);
}

QString IcdCore::stringArg(const QVariant &value, int fieldwidth, int base, QChar fillChar) const
{
    switch (value.type()) {
    case QVariant::LongLong:
        return QString("%1").arg(value.toLongLong(), fieldwidth, base, fillChar);
    case QVariant::ULongLong:
        return QString("%1").arg(value.toULongLong(), fieldwidth, base, fillChar);
    case QVariant::Int:
        return QString("%1").arg(value.toInt(), fieldwidth, base, fillChar);
    case QVariant::UInt:
        return QString("%1").arg(value.toUInt(), fieldwidth, base, fillChar);
    case QVariant::Char:
        return QString("%1").arg(value.toChar(), fieldwidth, fillChar);
    default:
        return QString();
    }
}

quint8 IcdCore::sumOfString(const QString &str)
{
    quint8 sum = 0;
    for (int i = 0; i < str.size(); ++i) {
        char c = str.at(i).toLatin1();
        if (c == ' ') {
            continue;
        }
        uchar uc = uchar(c);
        sum += uc;
    }
    return sum;
}

QString IcdCore::generateGuid() const
{
    return QUuid::createUuid().toString();
}

bool IcdCore::jsValueIsString(const QJSValue &value) const
{
    return value.isString();
}

void IcdCore::destroyObject(QObject *object)
{
    if (object) {
        object->deleteLater();
    }
}

qreal IcdCore::currentSecsSinceEpoch() const
{
    return QDateTime::currentMSecsSinceEpoch() / 1000.0;
}

void IcdCore::startAwait()
{
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void IcdCore::stopAwait()
{
    QCoreApplication::processEvents();
}

void IcdCore::asyncCall(QJSValue callback, const QJSValue &args)
{
    Q_D(IcdCore);
    QCoreApplication::postEvent(this, new icdmeta::JCallbackEvent(callback, d->parseArgs(args)));
}

void IcdCore::delayCall(QJSValue callback, const QJSValue &args, qint64 msecs)
{
    Q_D(IcdCore);
    if (msecs > 0) {
        d->timerDelay->disconnect(this);
        QObject::connect(d->timerDelay, &QTimer::timeout, this, [=](){
            d->timerDelay->disconnect(this);
            QCoreApplication::postEvent(this, new icdmeta::JCallbackEvent(callback, d->parseArgs(args)));
        });
        d->timerDelay->start(int(msecs));
    } else {
        QCoreApplication::postEvent(this, new icdmeta::JCallbackEvent(callback, d->parseArgs(args)));
    }
}

void IcdCore::cancelDelayCall()
{
    Q_D(IcdCore);
    if (d->timerDelay->isActive()) {
        d->timerDelay->stop();
        QCoreApplication::removePostedEvents(this, QEvent::Timer);
    }
}

void IcdCore::registerSingletonRelease(SingletonReleaseCallback callback)
{
    if (!callback) {
        return;
    }

    Q_D(IcdCore);
    if (d->callbacks.contains(callback)) {
        return;
    }

    return d->callbacks.append(callback);
}

void IcdCore::initQuickEnv(const QString &configPath)
{
    // quick controls 1 style
    if (qgetenv("QT_QUICK_CONTROLS_1_STYLE").isEmpty()) {
        qputenv("QT_QUICK_CONTROLS_1_STYLE", "Flat");
    }

    // quick controls configurations
    if (qgetenv("QT_QUICK_CONTROLS_CONF").isEmpty()) {
        const QString _configPath = QFileInfo(((configPath.isEmpty()
                                                ? QCoreApplication::applicationDirPath()
                                                  + "/../config" : configPath))).canonicalFilePath()
                + "/qtquickcontrols2.conf";
        qputenv("QT_QUICK_CONTROLS_CONF", _configPath.toLocal8Bit());
    }

    // application settings
    QSettings settings;
    QString style = QQuickStyle::name();
    if (style.isEmpty()) {
        const QString key = QCoreApplication::applicationVersion() + "/style";
        style = settings.value(key).toString();
        if (style.isEmpty()) {
            style = "Material";
            settings.setValue(key, style);
        }
    }
    QQuickStyle::setStyle(style);
}

bool IcdCore::initFontDatabase(const QString &fontPath)
{
    // fonts
    const QString _fontPath = QFileInfo((fontPath.isEmpty()
                                         ? QCoreApplication::applicationDirPath()
                                           + "/../config/fonts" : fontPath)).canonicalFilePath();
    QDir fontDir(_fontPath);
    if (fontDir.exists()) {
        QStringList fontFiles = fontDir.entryList(QStringList() << "*.ttf" << "*.otf",
                                                  QDir::NoDotAndDotDot | QDir::Files
                                                  | QDir::NoSymLinks | QDir::Readable);
        foreach (auto &font, fontFiles) {
            int result = QFontDatabase::addApplicationFont(fontDir.path() + "/" + font);
            if (result <= 0) {
                //qDebug() << "addFont:" << result;
            }
        }
    }

    return true;
}

bool IcdCore::initTranslators(const QString &language)
{
    Q_D(IcdCore);

    QCoreApplication::removeTranslator(d->translator);

    if (language.isEmpty()) {
        return false;
    }

    QLocale locale(language);

    // translator - icdmeta
    if (!d->translator->load(locale, ":/icdmeta/lang/icdmeta_")) {
        return false;
    }

    QCoreApplication::installTranslator(d->translator);

    return true;
}

QString IcdCore::configDir() const
{
    Q_D(const IcdCore);
    return d->configDir;
}

int IcdCore::fontSize() const
{
    Q_D(const IcdCore);
    return d->fontSize;
}

QString IcdCore::parsePath(const QString &path, bool canonical) const
{
    QString _path = path;
    const QString appDir = QCoreApplication::applicationDirPath();
    _path = _path.replace("@AppDir@", appDir)
            .replace("@ConfigDir@", configDir())
            .replace("@ThisDir@", QDir(appDir + "/../").canonicalPath());
    if (canonical) {
        _path = QDir(_path).canonicalPath();
    }
    return _path;
}

QString IcdCore::restorePath(const QString &path) const
{
    QString _path = path;
    const QString appDir = QCoreApplication::applicationDirPath();
    _path = _path.replace(appDir, "@AppDir@")
            .replace(configDir(), "@ConfigDir@")
            .arg(QDir(appDir + "/../").canonicalPath(), "@ThisDir@");
    return _path;
}

void IcdCore::setConfigDir(const QString &dir)
{
    Q_D(IcdCore);
    if (dir != d->configDir) {
        d->configDir = dir;
        emit configDirChanged(dir);
    }
}

void IcdCore::setFontSize(int size)
{
    Q_D(IcdCore);
    if (size != d->fontSize) {
        d->fontSize = size;
        emit fontSizeChanged(size);
    }
}

void IcdCore::reset()
{
    icdmeta::JCmdModel::instance()->reset();
    icdmeta::JDataChannelMgr::instance()->reset();
    icdmeta::JFilterChannel::instance()->reset();
    icdmeta::JProtocolPool::instance()->reset();
    icdmeta::JChannelPool::instance()->reset();
}

void IcdCore::customEvent(QEvent *event)
{
    switch (static_cast<int>(event->type())) {
    case icdmeta::Event_Callback:
    {
        auto cbEvent = reinterpret_cast<icdmeta::JCallbackEvent *>(event);
        if (!cbEvent) {
            break;
        }
        cbEvent->accept();
        cbEvent->execute();
        break;
    }
    default:
        break;
    }
}

IcdCore::IcdCore(QObject *parent)
    : QObject(parent)
    , J_DPTR(new IcdCorePrivate(this))
{
    Q_INIT_RESOURCE(resource);
    Q_D(IcdCore);
    d->init();
}

IcdCore::~IcdCore()
{
    Q_D(IcdCore);
    foreach (auto &callback, d->callbacks) {
        if (callback) {
            callback();
        }
    }

    delete d;

    QCoreApplication::removeTranslator(d->translator);

    Q_CLEANUP_RESOURCE(resource);
}

#include "precomp.h"
#include "main_global.h"
#include "common/LanguageMgr.h"
#include "SplashWidget.h"
#include "mainview/MainWindow.h"
#include <QFileSystemWatcher>
#include <QMainWindow>
#include <QDockWidget>
#include <QTabWidget>
#include "icdcore/3rdpart/jsoncpp/json/json.h"

// class JMainPrivate

class JMainPrivate
{
public:
    JMainPrivate(JMain *q)
        : q_ptr(q)
        , notify(nullptr)
        , neeedToRestart(false)
    {
        const QString fileName = "main.json";
        configDir = QDir(QCoreApplication::applicationDirPath().append("/../config"))
                .canonicalPath();
        configFile = configDir + "/~" + fileName;
        QFile::remove(configFile);
        QFile::copy(configDir + "/" + fileName, configFile);
        // replace config
        QFile file(configFile);
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QByteArray content = file.readAll();
            if (!content.isEmpty()) {
                content = replaceConfig(content, false);
                file.resize(0);
                file.write(content);
            }
            file.close();
        } else {
            qDebug() << QString("File \"%1\" open failure!").arg(configFile);
        }
    }

    ~JMainPrivate()
    {
        const QString oldConfigFile = configDir + "/main.json";
        if (QFile::exists(configFile)) {
            QFile file(configFile);
            if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                QByteArray content = file.readAll();
                if (!content.isEmpty()) {
                    content = replaceConfig(content, true);
                    file.resize(0);
                    file.write(content);
                }
                file.close();
                QFile::remove(oldConfigFile);
                QFile::rename(configFile, oldConfigFile);
            }
        } else {
            QFile::remove(configFile);
        }
    }

    void init();
    QByteArray &replaceConfig(QByteArray &content, bool reverse) const;
    QString &replaceConfig(QString &content, bool reverse) const;
    static bool setStyleSheet(const QString &filePath);

private:
    J_DECLARE_PUBLIC(JMain)
    Icd::JNotifyPtr notify;
    QList<SingletonReleaseCallback> callbacks;
    QString configDir;
    QString configFile;
    bool neeedToRestart;
};

void JMainPrivate::init()
{
    Q_Q(JMain);
    notify = Icd::JNotifyPtr(Icd::JNotify::inst(QCoreApplication::applicationName(), q));
    Q_ASSERT(notify != nullptr);
}

QByteArray &JMainPrivate::replaceConfig(QByteArray &content, bool reverse) const
{
    const QByteArray appDir = QCoreApplication::applicationDirPath().toLocal8Bit();
    const QByteArray thisDir = QDir(appDir + "/../").canonicalPath().toLocal8Bit();
    if (reverse) {
        return content.replace(appDir, QByteArray("@AppDir@"))
                .replace(configDir.toLocal8Bit(), QByteArray("@ConfigDir@"))
                .replace(thisDir, QByteArray("@ThisDir@"));
    } else {
        return content.replace(QByteArray("@AppDir@"), appDir)
                .replace(QByteArray("@ConfigDir@"), configDir.toLocal8Bit())
                .replace(QByteArray("@ThisDir@"), thisDir);
    }
}

QString &JMainPrivate::replaceConfig(QString &content, bool reverse) const
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString thisDir = QDir(appDir + "/../").canonicalPath();
    if (reverse) {
        return content.replace(appDir, "@AppDir@")
                .replace(configDir, "@ConfigDir@")
                .replace(thisDir, "@ThisDir@");
    } else {
        return content.replace("@AppDir@", appDir)
                .replace("@ConfigDir@", configDir)
                .replace("@ThisDir@", thisDir);
    }
}

bool JMainPrivate::setStyleSheet(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << "File" << filePath << "is not exists!";
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    qApp->setStyleSheet(file.readAll());

    return true;
}

// class JMain

J_IMPLEMENT_SINGLE_INSTANCE(JMain, JMain)

JMain::JMain(QObject *parent)
    : QObject(parent)
    , d_ptr(new JMainPrivate(this))
{
    Q_D(JMain);
    d->init();
}

JMain::~JMain()
{
    Q_D(JMain);
    foreach (auto &callback, d->callbacks) {
        if (callback) {
            callback();
        }
    }
    delete d;
}

QString JMain::configFile() const
{
    Q_D(const JMain);
    return d->configFile;
}

QString &JMain::replaceConfig(QString &content, bool reverse) const
{
    Q_D(const JMain);
    return d->replaceConfig(content, reverse);
}

QString JMain::replaceConfig(const QString &content, bool reverse) const
{
    Q_D(const JMain);
    QString _content = content;
    return d->replaceConfig(_content, reverse);
}

QString JMain::theme() const
{
    QJsonObject themeJson = JJson::object(configFile(), "global/theme", false);
    if (themeJson.isEmpty()) {
        return QString();
    }

    return themeJson["name"].toString();
}

bool JMain::initTranslators()
{
    return LanguageMgr::instance()->installTranslator();
}

bool JMain::initFontDatabase()
{
    // fonts
    QDir fontDir(instance()->configDir().append("/fonts"));
    QStringList fontFiles = fontDir.entryList(QStringList() << "*.ttf" << "*.otf",
                                              QDir::NoDotAndDotDot | QDir::Files
                                              | QDir::NoSymLinks | QDir::Readable);
    foreach (auto &font, fontFiles) {
        int result = QFontDatabase::addApplicationFont(fontDir.path() + "/" + font);
        if (result <= 0) {
            //qDebug() << "addFont:" << result;
        }
    }

    return true;
}

QWidget *JMain::createMainWindow()
{
    MainWindow *mainWindow = new MainWindow();

    QScreen *screen = QApplication::primaryScreen();
    mainWindow->resize(screen->geometry().width() * 0.7,
                       screen->geometry().height() * 0.75);
    mainWindow->init();

    return mainWindow;
}

int JMain::execApp(QApplication *app)
{
    if (!app) {
        return -1;
    }

    auto splashWidget = QSharedPointer<SplashWidget>(new SplashWidget, j_delete_qobject);
    splashWidget->show();

    initFontDatabase();

    // initialize models
    bool result = instance()->init();
    if (!result) {
        QMessageBox::critical(nullptr, QStringLiteral("Error"),
                              QStringLiteral("global config initialize failure!"));
        releaseInstance();
        return -1;
    }

    // mainWindow
    auto mainWindow = createMainWindow();
    if (!mainWindow) {
        delete mainWindow;
        mainWindow = nullptr;
        releaseInstance();
        return -1;
    }

    mainWindow->show();
    const QString theme = JMain::instance()->theme();
#if 1//defined(TEST_STYLESHEET)
    const QString prefix(QApplication::applicationDirPath()
                         .append("/../src/app/resource/qss/stylesheet-"));
    const QString filePath(prefix + theme + ".qss");
    if (QFile(filePath).exists()) {
        JMainPrivate::setStyleSheet(filePath);
        QFileSystemWatcher *fileSysWatcher = new QFileSystemWatcher(mainWindow);
        fileSysWatcher->addPath(filePath);
        QObject::connect(fileSysWatcher, &QFileSystemWatcher::fileChanged,
                         [=](const QString &filePath){
            JMainPrivate::setStyleSheet(filePath);
        });
        QObject::connect(JMain::instance(), &JMain::themeChanged,
                         [=](const QString &currnt, const QString &previous){
            QString filePath(prefix + previous + ".qss");
            fileSysWatcher->removePath(filePath);
            filePath = prefix + currnt + ".qss";
            fileSysWatcher->addPath(filePath);
            JMainPrivate::setStyleSheet(filePath);
        });
    } else {
        JMain::instance()->setTheme(theme);
    }
#else
    JMain::instance()->setTheme(theme);
#endif
    splashWidget.clear();

    jnotify->on("main.mainwindow.inst", JMain::instance(), [=](JNEvent &event){
        event.setReturnValue(qVariantFromValue((void*)mainWindow));
    });

    int exitCode = app->exec();

    Icd::WorkerPool::releaseInstance();

    delete mainWindow;
    mainWindow = nullptr;

    //
    if (instance()->isNeedToRestart()) {
        QProcess::startDetached(QCoreApplication::applicationFilePath(),
                                QStringList(), QCoreApplication::applicationDirPath());
    }

    releaseInstance();

    return exitCode;
}

int JMain::appVerMajor()
{
    QString version = appVersion();
    if (version.isEmpty()) {
        return -1;
    }

    return version.section('.', 0, 0).toInt();
}

int JMain::appVerMinor()
{
    QString version = appVersion();
    if (version.isEmpty()) {
        return -1;
    }

    return version.section('.', 1, 1).toInt();
}

int JMain::appVerPatch()
{
    QString version = appVersion();
    if (version.isEmpty()) {
        return -1;
    }

    return version.section('.', 2, 2).toInt();
}

int JMain::appVerBuild()
{
    QString version = appVersion();
    if (version.isEmpty()) {
        return -1;
    }

    return version.section('.', 3, 3).toInt();
}

QString JMain::appVersion()
{
    QFile file(QApplication::applicationDirPath().append("/../VERSION"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    return file.readLine(20).trimmed();
}

void JMain::registerSingletonRelease(SingletonReleaseCallback callback)
{
    if (!callback) {
        return;
    }

    Q_D(JMain);
    if (d->callbacks.contains(callback)) {
        return;
    }

    return d->callbacks.append(callback);
}

void JMain::setRestartFlag(bool neeedToRestart)
{
    Q_D(JMain);
    d->neeedToRestart = neeedToRestart;
}

bool JMain::isNeedToRestart() const
{
    Q_D(const JMain);
    return d->neeedToRestart;
}

QWidget *JMain::mainWindow() const
{
    QWidget *widget = jVariantFromVoid<QWidget>(jnotify->send("main.mainwindow.inst", this));
    return widget;
}

Icd::JNotifyPtr JMain::notify()
{
    Q_D(JMain);
    return d->notify;
}

const Icd::JNotifyPtr JMain::notify() const
{
    Q_D(const JMain);
    return d->notify;
}

void JMain::saveWidgetState(QWidget *widget, const QString prefix)
{
    if (!widget) {
        return;
    }

    QString _prefix = prefix;
    if (prefix.isEmpty()) {
        _prefix = widget->objectName();
        if (_prefix.isEmpty()) {
            _prefix = widget->metaObject()->className();
        }
    }

    QSettings settings;
    settings.beginGroup(settingsGroupPrefix(_prefix));
    if (widget->inherits("QMainWindow")) {
        QMainWindow *mainWindow = qobject_cast<QMainWindow *>(widget);
        if (!mainWindow) {
            return;
        }
        if (mainWindow->isFullScreen()) {
            mainWindow->setWindowState(mainWindow->windowState() & ~Qt::WindowFullScreen);
        }
        settings.setValue("geometry", mainWindow->saveGeometry());
        settings.setValue("state", mainWindow->saveState());
    } else if (widget->inherits("QDockWidget")) {
        QDockWidget *dockWidget = qobject_cast<QDockWidget *>(widget);
        if (!dockWidget) {
            return;
        }
        settings.setValue("geometry", dockWidget->saveGeometry());
    } else if (widget->inherits("JSplitter")) {
        JSplitter *splitter = qobject_cast<JSplitter *>(widget);
        if (!splitter) {
            return;
        }
        const QList<double> scales = splitter->scales();
        QVariantList varScales;
        QListIterator<double> citerScales(scales);
        while (citerScales.hasNext()) {
            varScales.append(citerScales.next());
        }
        settings.setValue("scales", varScales);
    } else if (widget->inherits("QTabWidget")) {
        QTabWidget *tabWidget = qobject_cast<QTabWidget *>(widget);
        if (!tabWidget) {
            return;
        }
        settings.setValue("currentIndex", tabWidget->currentIndex());
    }
    settings.endGroup();
}

void JMain::restoreWidgetState(QWidget *widget, const QString prefix)
{
    if (!widget) {
        return;
    }

    QString _prefix = prefix;
    if (prefix.isEmpty()) {
        _prefix = widget->objectName();
        if (_prefix.isEmpty()) {
            _prefix = widget->metaObject()->className();
        }
    }

    QSettings settings;
    settings.beginGroup(settingsGroupPrefix(_prefix));
    if (widget->inherits("QMainWindow")) {
        QMainWindow *mainWindow = qobject_cast<QMainWindow *>(widget);
        if (!mainWindow) {
            return;
        }
        if (settings.contains("geometry")) {
            mainWindow->restoreGeometry(settings.value("geometry").toByteArray());
        }
        if (settings.contains("sate")) {
            mainWindow->restoreState(settings.value("state").toByteArray());
        }
    } else if (widget->inherits("QDockWidget")) {
        QDockWidget *dockWidget = qobject_cast<QDockWidget *>(widget);
        if (!dockWidget) {
            return;
        }
        if (settings.contains("geometry")) {
            dockWidget->restoreGeometry(settings.value("geometry").toByteArray());
        }
    } else if (widget->inherits("JSplitter")) {
        JSplitter *splitter = qobject_cast<JSplitter *>(widget);
        if (!splitter) {
            return;
        }
        if (settings.contains("scales")) {
            const QVariantList varScales = settings.value("scales").toList();
            QList<double> scales;
            QListIterator<QVariant> citerVarScales(varScales);
            while (citerVarScales.hasNext()) {
                scales.append(citerVarScales.next().toDouble());
            }
            splitter->setScales(scales);
        }
    } else if (widget->inherits("QTabWidget")) {
        QTabWidget *tabWidget = qobject_cast<QTabWidget *>(widget);
        if (!tabWidget) {
            return;
        }
        if (settings.contains("currentIndex")) {
            tabWidget->setCurrentIndex(settings.value("currentIndex").toInt());
        }
    }
    settings.endGroup();
}

QString JMain::settingsGroupPrefix(const QString prefix)
{
    if (prefix.isEmpty()) {
        return QCoreApplication::applicationVersion();
    } else {
        return QCoreApplication::applicationVersion().append('/').append(prefix);
    }
}

Json::Value JMain::config(const QString &path) const
{
    return Json::resolve(configFile().toStdString(), path.toStdString());
}

bool JMain::setConfig(const QString &path, const Json::Value &config)
{
    return Json::make(configFile().toStdString(), path.toStdString(), config,
                      true, false);
}

Json::Value JMain::option(const QString &module) const
{
    return config("module." + module);
}

Json::Value JMain::option(const QString &module, const QString &key) const
{
    return config("module." + module + '.' + key);
}

bool JMain::setOption(const QString &module, const Json::Value &option)
{
    return setConfig("module." + module, option);
}

bool JMain::setOption(const QString &module, const QString &key, const Json::Value &option)
{
    return setConfig("module." + module + '.' + key, option);
}

Icd::ParserPtr JMain::parser(const QString &module) const
{
    if (module.isEmpty()) {
        return Icd::ParserPtr();
    }

    Icd::JParserPtrHandle handle;
    if (!jnotify->send(module + ".parser.inst", qVariantFromValue((void*)&handle)).toBool()
            || !handle.parser) {
        return Icd::ParserPtr();
    }

    return handle.parser;
}

QString JMain::typeString(const Icd::ItemPtr &item)
{
    if (!item) {
        return QString();
    }

    switch (item->type()) {
    case Icd::ItemHead: return tr("Head");
    case Icd::ItemCounter:
    {
        const Icd::CounterItemPtr counterItem =
                JHandlePtrCast<Icd::CounterItem, Icd::Item>(item);
        if (!counterItem) {
            break;
        }
        return counterTypeString(counterItem->counterType());
    }
    case Icd::ItemCheck:
    {
        const Icd::CheckItemPtr checkItem =
                JHandlePtrCast<Icd::CheckItem, Icd::Item>(item);
        if (!checkItem) {
            break;
        }
        return checkTypeString(checkItem->checkType());
    }
    case Icd::ItemFrameCode:
    {
        const Icd::FrameCodeItemPtr frameCodeItem =
                JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item);
        if (!frameCodeItem) {
            break;
        }
        return frameCodeTypeString(frameCodeItem->frameCodeType());
    }
    case Icd::ItemNumeric:
    {
        const Icd::NumericItemPtr numericItem =
                JHandlePtrCast<Icd::NumericItem, Icd::Item>(item);
        if (!numericItem) {
            break;
        }
        return numericTypeString(numericItem->numericType());
    }
    case Icd::ItemBitMap: return tr("BitMap");
    case Icd::ItemBitValue: return tr("BitValue");
    case Icd::ItemComplex: return tr("Complex");
    case Icd::ItemFrame: return tr("Frame");
    default:
        break;
    }

    return tr("Unknown");
}

QString JMain::numericTypeString(int numericType)
{
    switch (numericType) {
    case Icd::NumericInt8: return tr("Int8");
    case Icd::NumericUint8: return tr("UInt8");
    case Icd::NumericInt16: return tr("Int16");
    case Icd::NumericUint16: return tr("UInt16");
    case Icd::NumericInt32: return tr("Int32");
    case Icd::NumericUint32: return tr("UInt32");
    case Icd::NumericInt64: return tr("Int64");
    case Icd::NumericUint64: return tr("UInt64");
    case Icd::NumericFloat32: return tr("Float32");
    case Icd::NumericFloat64: return tr("Float64");
    default:
        break;
    }

    return tr("Unknown");
}

QString JMain::counterTypeString(int counterType)
{
    switch (counterType) {
    case Icd::CounterU8: return tr("Counter U8");
    case Icd::CounterU16: return tr("Counter U16");
    case Icd::CounterU32: return tr("Counter U32");
    case Icd::CounterU64: return tr("Counter U64");
    default:
        break;
    }

    return tr("Unknown");
}

QString JMain::checkTypeString(int checkType)
{
    switch (checkType) {
    case Icd::CheckNone: return tr("No check");
    case Icd::CheckSum8: return tr("Check sum8");
    case Icd::CheckSum16: return tr("Check sum16");
    case Icd::CheckCrc8: return tr("Check crc8");
    case Icd::CheckCrc16: return tr("Check crc16");
    case Icd::CheckXor8: return tr("Check xor8");
    case Icd::CheckXor16: return tr("Check xor16");
    default:
        break;
    }

    return tr("Unknown");
}

QString JMain::frameCodeTypeString(int frameCodeType)
{
    switch (frameCodeType) {
    case Icd::FrameCodeU8: return tr("FrameCode U8");
    case Icd::FrameCodeU16: return tr("FrameCode U16");
    case Icd::FrameCodeU32: return tr("FrameCode U32");
    case Icd::FrameCodeU64: return tr("FrameCode U64");
    default:
        break;
    }

    return tr("Unknown");
}

void JMain::setTheme(const QString &value)
{
    const QString previous = theme();
    if (!setConfig("global.theme.name", Json::Value(value.toStdString()))) {
        return;
    }

    if (value == "none") {
        qApp->setStyleSheet(QString());
    } else {
        const QString filePath(":/datastudio/qss/stylesheet-" + value + ".qss");
        if (!QFile::exists(filePath)) {
            qWarning() << "Theme" <<value << "is invalid!";
            return;
        }
        JMainPrivate::setStyleSheet(filePath);
    }

    emit themeChanged(value, previous);

    jnotify->post("global.theme.changed", value);
}

bool JMain::init()
{
    return true;
}

QString JMain::configDir() const
{
    Q_D(const JMain);
    return d->configDir;
}

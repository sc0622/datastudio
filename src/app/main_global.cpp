﻿#include "precomp.h"
#include "common/LanguageMgr.h"
#include "SplashWidget.h"
#include "mainview/MainWindow.h"
#include <QFileSystemWatcher>
#include <QMainWindow>
#include <QDockWidget>
#include <QTabWidget>
#include "common/Logging.h"

// class JMainPrivate

class JMainPrivate
{
public:
    JMainPrivate(JMain *q)
        : J_QPTR(q)
        , notify(nullptr)
        , neeedToRestart(false)
    {
        const QString fileName = "main.json";
        QSettings settings;
        settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
        configDir = settings.value("CONFIG_DIR").toString();
        configFile = configDir + "/~" + fileName;
        settings.setValue("CONFIG_FILE", configFile);
        settings.endGroup();
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
            qDebug().noquote() << QString("File \"%1\" open failure!").arg(configFile);
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
    //
    JUtralEditCore::instance()->setLanguage("zh_CN");
    JChartCore::instance()->setLanguage("zh_CN");
    //
    notify->on("app.theme", q, [=](JNEvent &event){
        q->setTheme(event.argument().toString());
    });
}

QByteArray &JMainPrivate::replaceConfig(QByteArray &content, bool reverse) const
{
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
    const QByteArray appDir = settings.value("APP_DIR").toString().toLocal8Bit();
    const QByteArray thisDir = settings.value("THIS_DIR").toString().toLocal8Bit();
    settings.endGroup();
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
    QSettings settings;
    settings.beginGroup(QCoreApplication::applicationVersion().append("/Temp/Paths"));
    const QByteArray appDir = settings.value("APP_DIR").toString().toLocal8Bit();
    const QByteArray thisDir = settings.value("THIS_DIR").toString().toLocal8Bit();
    settings.endGroup();
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
    , J_DPTR(new JMainPrivate(this))
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
    mainWindow->resize(int(screen->geometry().width() * 0.7),
                       int(screen->geometry().height() * 0.75));
    mainWindow->init();

    return mainWindow;
}

int JMain::execApp(QApplication *app)
{
    if (!app) {
        return -1;
    }

    auto splashWidget = QSharedPointer<SplashWidget>(new SplashWidget, jdelete_qobject);
    splashWidget->show();

    initFontDatabase();
    IcdParser::instance()->init();
    IcdWidget::instance()->init();
    Logging::instance()->init();

    auto releaseInstances = [=](){
      IcdParser::releaseInstance();
      IcdWidget::releaseInstance();
      Logging::releaseInstance();
      JMain::releaseInstance();
    };

    // initialize models
    bool result = instance()->init();
    if (!result) {
        QMessageBox::critical(nullptr, QStringLiteral("Error"),
                              QStringLiteral("global config initialize failure!"));
        releaseInstances();
        return -1;
    }
    //
    const QString theme = JMain::instance()->theme();
#if defined(TEST_STYLESHEET)
    const QString prefix(PROJECT_DIR + QLatin1String("/src/app/resource/qss/stylesheet-"));
    const QString filePath(prefix + theme + ".qss");
    if (QFile(filePath).exists()) {
        JMainPrivate::setStyleSheet(filePath);
        QFileSystemWatcher *fileSysWatcher = new QFileSystemWatcher(JMain::instance());
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

    // mainWindow
    auto mainWindow = createMainWindow();
    if (!mainWindow) {
        delete mainWindow;
        mainWindow = nullptr;
        releaseInstances();
        return -1;
    }

    mainWindow->show();

    jnotify->on("main.mainwindow.inst", JMain::instance(), [=](JNEvent &event){
        event.setReturnValue(qVariantFromValue(static_cast<void*>(mainWindow)));
    });

    splashWidget.clear();

    int exitCode = app->exec();

    Icd::WorkerPool::releaseInstance();

    delete mainWindow;
    mainWindow = nullptr;

    //
    if (instance()->isNeedToRestart()) {
        QProcess::startDetached(QCoreApplication::applicationFilePath(),
                                QStringList(), QCoreApplication::applicationDirPath());
    }

    releaseInstances();

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
    QWidget *widget = jVariantFromVoid<QWidget>(jnotify->send("main.mainwindow.inst",
                                                              static_cast<const QObject*>(this)));
    return widget;
}

Icd::JNotifyPtr JMain::notify()
{
    Q_D(JMain);
    return d->notify;
}

const Icd::JNotifyPtr &JMain::notify() const
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
    } else if (widget->inherits("QListWidget")) {
        QListWidget *listWidget = qobject_cast<QListWidget *>(widget);
        if (!listWidget) {
            return;
        }
        settings.setValue("currentRow", listWidget->currentRow());
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
    } else if (widget->inherits("QListWidget")) {
        QListWidget *listWidget = qobject_cast<QListWidget *>(widget);
        if (!listWidget) {
            return;
        }
        if (settings.contains("currentRow")) {
            listWidget->setCurrentRow(settings.value("currentRow").toInt());
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
    if (!jnotify->send(module + ".parser.inst", qVariantFromValue(static_cast<void*>(&handle))).toBool()
            || !handle.parser) {
        return Icd::ParserPtr();
    }

    return handle.parser;
}

QStringList JMain::modules()
{
    QStringList _modules;
    _modules << "edit" << "monitor" << "simulate" << "analyse";
    return _modules;
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
            qWarning() << "Theme" << value << "is invalid!";
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

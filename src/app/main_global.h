#ifndef MAIN_GLOBAL_H
#define MAIN_GLOBAL_H

#ifdef ICDWIDGET_LIB
#include "icdwidget/icdwidget_global.h"
#endif
#include <QObject>
#include <QStandardItem>
#include <QEvent>

namespace Icd {

class Item;
typedef std::shared_ptr<Item> ItemPtr;

class Parser;
typedef std::shared_ptr<Parser> ParserPtr;

class JNotify;
typedef std::shared_ptr<JNotify> JNotifyPtr;

}

using Icd::JNEvent;

// class JMain

class QWidget;
class JMainPrivate;

class JMain : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
public:
    enum ViewType {
        PreferenceView,
        EditView,
        MonitorView,
        SimulateView,
        AnalyseView
    };
    Q_ENUM(ViewType)

    bool init();

    QString configDir() const;
    QString configFile() const;
    QString &replaceConfig(QString &content, bool reverse = false) const;
    QString replaceConfig(const QString &content, bool reverse = false) const;

    QString theme() const;

    static bool initTranslators();
    static bool initFontDatabase();
    static QWidget *createMainWindow();
    static int execApp(QApplication *app);

    static int appVerMajor();
    static int appVerMinor();
    static int appVerPatch();
    static int appVerBuild();
    static QString appVersion();

    void registerSingletonRelease(SingletonReleaseCallback callback);

    void setRestartFlag(bool neeedToRestart);
    bool isNeedToRestart() const;

    QWidget *mainWindow() const;

    Icd::JNotifyPtr notify();
    const Icd::JNotifyPtr &notify() const;

    static void saveWidgetState(QWidget *widget, const QString prefix = QString());
    static void restoreWidgetState(QWidget *widget, const QString prefix = QString());
    static QString settingsGroupPrefix(const QString prefix = QString());

    Json::Value config(const QString &path) const;
    bool setConfig(const QString &path, const Json::Value &config);
    Json::Value option(const QString &module) const;
    Json::Value option(const QString &module, const QString &key) const;
    bool setOption(const QString &module, const Json::Value &option);
    bool setOption(const QString &module, const QString &key, const Json::Value &option);

    Icd::ParserPtr parser(const QString &module) const;

    static QStringList modules();

signals:
    void themeChanged(const QString &currnt, const QString &previous);

public slots:
    void setTheme(const QString &value);

private:
    explicit JMain(QObject *parent = nullptr);
    ~JMain();

private:
    Q_DISABLE_COPY(JMain)
    J_DECLARE_PRIVATE(JMain)
    J_DECLARE_SINGLE_INSTANCE(JMain)
};

// class Serializable

class Serializable
{
public:
    virtual ~Serializable() {}
    virtual QJsonObject save() const = 0;
    virtual void restore(const QJsonObject &/*json*/) = 0;
};

//
#ifdef jnotify
#undef jnotify
#endif
#define jnotify JMain::instance()->notify()

#endif // MAIN_GLOBAL_H

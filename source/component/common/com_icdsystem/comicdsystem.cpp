#include "precomp.h"
#include "comicdsystem.h"
#include "icdparser/icdparser_sql.h"
#include "comicdsystemui.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComIcdSystem(reinterpret_cast<IJAttempter *>(attemper)));
}

ComIcdSystem::ComIcdSystem(IJAttempter *attemper)
    : d_attempter(attemper)
    , fileWatcher(nullptr)
    , d_ui(nullptr)
{

}

ComIcdSystem::~ComIcdSystem()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = nullptr;
    }

    if (fileWatcher) {
        fileWatcher->deleteLater();
        fileWatcher = nullptr;
    }
}

bool ComIcdSystem::loadInterface()
{
    bool result = true;

    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("j_module_changed", &ComIcdSystem::onModuleChanged)
            .append("query_icd_parser", &ComIcdSystem::queryIcdParser)
            .endGroup();

    return result;
}

bool ComIcdSystem::updateInterface()
{
    bool result = true;

    //

    return result;
}

void ComIcdSystem::releaseInterface()
{

    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("j_module_changed")
            .remove("query_icd_parser")
            .endGroup();
}

void* ComIcdSystem::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);
    Q_UNUSED(iid);
    Q_UNUSED(ver);

    return 0;
}

std::string ComIcdSystem::componentName() const
{
    return QStringLiteral("com_icdsystem").toStdString();
}

std::string ComIcdSystem::componentDesc() const
{
    return QStringLiteral("ICD系统管理器").toStdString();
}

void ComIcdSystem::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .endGroup();

    //
}

void ComIcdSystem::detach()
{
    //

    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            //.remove(...)
            .endGroup();
}

void *ComIcdSystem::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (objectName == "page_about") {
        //
        if (!fileWatcher) {
            fileWatcher = new QFileSystemWatcher();
            QString filePath = QString::fromStdString(jframeFacade()->configDirPath())
                    .append("/stylesheet_light.qss");
            fileWatcher->addPath(filePath);
            QObject::connect(fileWatcher, &QFileSystemWatcher::fileChanged,
                             [=](const QString &path){
                QFile file(path);
                if (file.open(QIODevice::ReadOnly)) {
                    IJMainWindow *mainWindow = jframeMainWindow();
                    if (mainWindow) {
                        QWidget *mainWidget = (QWidget *)mainWindow->mainWidget();
                        if (mainWidget) {
                            mainWidget->setStyleSheet(file.readAll());
                        }
                    }
                }
            });
        }
        //
        if (d_ui) {
            Q_ASSERT(false);
            return 0;
        }
        //
        d_ui = new ComIcdSystemUi(*d_attempter);
        return qobject_cast<QWidget *>(d_ui);
    } else {
        return 0;
    }

    return 0;
}

bool ComIcdSystem::commandSink(void *sender, const std::string &domain, const std::string &objectName,
                               const std::string &eventType, void *data)
{
    Q_UNUSED(domain);
    Q_UNUSED(objectName);
    Q_UNUSED(eventType);
    Q_UNUSED(data);

    QObject *objectSender = reinterpret_cast<QObject *>(sender);
    if (!objectSender) {
        return false;
    }

    //
    if (eventType == "currentPageChanged") {
        //
        std::string moduleName;
        if (objectName == "page_editor") {
            moduleName = QStringLiteral("编辑系统").toStdString();
        } else if (objectName == "page_monitor") {
            moduleName = QStringLiteral("监控系统").toStdString();
        } else if (objectName == "page_simulator") {
            moduleName = QStringLiteral("仿真系统").toStdString();
        } else if (objectName == "page_analyse") {
            moduleName = QStringLiteral("数据分析").toStdString();
        }
        //
        if (!moduleName.empty()) {
            d_attempter->notifier().sendMessage("j_switch_module@jlayout.module_manager",
                                                (JWPARAM)&moduleName);
        }
    }

    return false;
}

bool ComIcdSystem::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //

    return false;
}

std::string ComIcdSystem::observerId() const
{
    return componentName();
}

JLRESULT ComIcdSystem::onModuleChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);

    //
    std::string *pModule = (std::string *)(wParam);
    if (pModule == 0) {
        return -1;
    }

    //
    QString module = QString::fromStdString(*pModule);
    if (module == QStringLiteral("编辑系统")) {
        setRibbonCurrentPage(module);
    } else if (module == QStringLiteral("监控系统")) {
        setRibbonCurrentPage(module);
    } else if (module == QStringLiteral("仿真系统")) {
        setRibbonCurrentPage(module);
    } else if (module == QStringLiteral("数据分析")) {
        setRibbonCurrentPage(module);
    }

    return 0;
}

JLRESULT ComIcdSystem::queryIcdParser(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    return d_attempter->notifier().sendMessage("query_icd_parser@com_databasemgr",
                                               wParam, lParam);
}

bool ComIcdSystem::setRibbonCurrentPage(const QString &pageName)
{
    //
    QtRibbon::RibbonBar *ribbonBar =
            (QtRibbon::RibbonBar *)d_attempter->mainWindow()->ribbonBar();
    if (ribbonBar == 0) {
        return false;
    }

    //
    QWidget *mainWidget = (QWidget *)d_attempter->mainWindow()->mainWidget();
    if (mainWidget) {
        mainWidget->setWindowTitle(QStringLiteral("ICD系统 —— ") + pageName);
    }

    //
    QtRibbon::RibbonPage *currentPage = ribbonBar->getPage(ribbonBar->currentIndexPage());
    if (currentPage) {
        if (currentPage->title() == pageName) {
            return true;    //
        }
    }

    //
    for (int i = 0; i < ribbonBar->getPageCount(); ++i) {
        QtRibbon::RibbonPage *page = ribbonBar->getPage(i);
        if (page == 0) {
            continue;
        }
        const QString title = page->title();
        if (title == pageName) {
            ribbonBar->setCurrentPage(i);
            break;
        }
    }

    return true;
}

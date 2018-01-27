#include "precomp.h"
#include "comdatabasemgr.h"
#include "comdatabasemgrui.h"
#include "icdparser/icdparser_inc.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComDatabaseMgr(reinterpret_cast<IJAttempter *>(attemper)));
}

ComDatabaseMgr::ComDatabaseMgr(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
    , d_icdParser(0)
{

}

ComDatabaseMgr::~ComDatabaseMgr()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComDatabaseMgr::loadInterface()
{
    bool result = true;

    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("dataSourceChanged", &ComDatabaseMgr::onDataSourceChanged)
            .append("query_icd_parser", &ComDatabaseMgr::queryIcdParser)
            .endGroup();

    return result;
}

bool ComDatabaseMgr::updateInterface()
{
    bool result = true;

    //
    if (d_ui) {
        //
        if (!d_icdParser) {
            if (!updateParser(d_ui->sourceType(), d_ui->filePath())) {
                return false;
            }
        }
    }

    return result;
}

void ComDatabaseMgr::releaseInterface()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("dataSourceChanged")
            .remove("query_icd_parser")
            .endGroup();
}

void* ComDatabaseMgr::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComDatabaseMgr::componentName() const
{
    return QStringLiteral("com_databasemgr").toStdString();
}

std::string ComDatabaseMgr::componentDesc() const
{
    return QStringLiteral("数据库管理组件").toStdString();
}

void ComDatabaseMgr::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .endGroup();

    //
}

void ComDatabaseMgr::detach()
{
    // 取消订阅消息
    d_attempter->notifier().remove(this);
}

void *ComDatabaseMgr::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (objectName == "page_database_mgr") {
        //
        if (d_ui) {
            Q_ASSERT(false);
            return 0;
        }
        //
        d_ui = new ComDatabaseMgrUi(*d_attempter);
        return qobject_cast<QWidget *>(d_ui);
    } else {
        return 0;
    }

    return 0;
}

bool ComDatabaseMgr::commandSink(void *sender, const std::string &domain, const std::string &objectName,
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

    return false;
}

bool ComDatabaseMgr::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //

    return false;
}

std::string ComDatabaseMgr::observerId() const
{
    return componentName();
}

JLRESULT ComDatabaseMgr::onDataSourceChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    const QString *const type = (QString *)wParam;
    if (!type) {
        return JFrame::ResultError;
    }

    if (*type == "database") {
        if (!updateParser(*type)) {
            return JFrame::ResultError;
        }
    } else if (*type == QStringLiteral("文件")) {
        const QString *const filePath = (QString *)lParam;
        if (!filePath) {
            return JFrame::ResultError;
        }
        if (!updateParser(*type, *filePath)) {
            return JFrame::ResultError;
        }
    } else {
        Q_ASSERT(false);    // not support
        return JFrame::ResultSuccess;
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComDatabaseMgr::queryIcdParser(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);

    //
    Icd::Parser **parser = (Icd::Parser **)wParam;
    if (!parser) {
        return JFrame::ResultError;
    }

    //
    *parser = d_icdParser.get();

    return JFrame::ResultSuccess;
}

bool ComDatabaseMgr::updateParser(const QString &sourceType, const QString &filePath)
{
    //
    d_attempter->notifier().sendMessage("parserChanged", 0);

    //
    if (sourceType == "database") {
        d_icdParser = Icd::SqlParserPtr(new Icd::SqlParser(jframeFacade()->configDirPath()
                                                           .append("/jframe_global.xml")));
    } else if (sourceType == QStringLiteral("文件")) {
        if (filePath.isEmpty()) {
            return false;
        }
        d_icdParser = Icd::FileParserPtr(new Icd::FileParser(filePath.toStdString()));
    } else {
        Q_ASSERT(false);    // not support
        return false;
    }

    //
    d_attempter->notifier().sendMessage("parserChanged", (JWPARAM)d_icdParser.get());

    return true;
}

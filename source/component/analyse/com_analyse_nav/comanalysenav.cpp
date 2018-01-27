#include "precomp.h"
#include "comanalysenav.h"
#include "comanalysenavui.h"
#include "private/convertdatadlg.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComAnalyseNav(reinterpret_cast<IJAttempter *>(attemper)));
}

ComAnalyseNav::ComAnalyseNav(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
{

}

ComAnalyseNav::~ComAnalyseNav()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComAnalyseNav::loadInterface()
{
    bool result = true;

    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("parserChanged", &ComAnalyseNav::onParserChanged)
            .endGroup();

    return result;
}

bool ComAnalyseNav::updateInterface()
{
    bool result = true;

    //
    if (d_ui) {
        QObject *object = 0;
        // offset
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_offset@group_showitem@page_analyse@*");
        if (object) {
            d_ui->showItemOffset(object->property("checked").toBool());
        }
        // type
        object= (QObject *)jframeMainWindow()->queryObject(
                    "check_show_type@group_showitem@page_analyse@*");
        if (object) {
            d_ui->showItemType(object->property("checked").toBool());
        }
        // update-show-navigation
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_nav@group_window@page_analyse@*");
        if (object) {
            d_ui->setVisible(object->property("checked").toBool());
        }
    }

    return result;
}

void ComAnalyseNav::releaseInterface()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("parserChanged")
            .endGroup();

    //
    if (d_ui) {
        d_ui->releaseWork();
    }
}

void* ComAnalyseNav::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComAnalyseNav::componentName() const
{
    return QStringLiteral("com_analyse_nav").toStdString();
}

std::string ComAnalyseNav::componentDesc() const
{
    return QStringLiteral("数据分析组件").toStdString();
}

void ComAnalyseNav::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .endGroup();

    //TODO: Add your statements.
}

void ComAnalyseNav::detach()
{
    //TODO: Add your statements.

    // 取消订阅消息
    d_attempter->notifier().remove(this);
}

void *ComAnalyseNav::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (d_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    d_ui = new ComAnalyseNavUi(*d_attempter);

    //
    d_ui->init();

    return qobject_cast<QWidget *>(d_ui);
}

bool ComAnalyseNav::commandSink(void *sender, const std::string &domain, const std::string &objectName,
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
    QString _domain = QString::fromStdString(domain);
    if (_domain.endsWith(QString::fromStdString(componentName()))) {
        //
        if (objectName == "check_show_offset") {
            if (d_ui) {
                d_ui->showItemOffset(*(bool *)data);
            }
            return true;
        } else if (objectName == "check_show_type") {
            if (d_ui) {
                d_ui->showItemType(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_load") {
            if (d_ui) {
                d_ui->loadIcdFile();
            }
            return true;
        } else if (objectName == "action_unload") {
            if (d_ui) {
                d_ui->unloadDataFile();
            }
            return true;
        } else if (objectName == "action_convert") {
            if (d_ui) {
                convertData();
            }
            return true;
        } else if (objectName == "check_show_nav") {
            if (d_ui) {
                d_ui->setVisible(*(bool *)data);
            }
            return true;
        }
    }

    return false;
}

bool ComAnalyseNav::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //TODO: Add your statements.

    return false;
}

std::string ComAnalyseNav::observerId() const
{
    return componentName();
}

JLRESULT ComAnalyseNav::onParserChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //
    if (d_ui) {
        d_ui->updateUi();
    }

    return JFrame::ResultSuccess;
}

void ComAnalyseNav::convertData()
{
    ConvertDataDlg *convertDlg = new ConvertDataDlg(d_ui);

    // init

    convertDlg->exec();
    convertDlg->deleteLater();
}

#include "precomp.h"
#include "commonitornav.h"
#include "commonitornavui.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComMonitorNav(reinterpret_cast<IJAttempter *>(attemper)));
}

ComMonitorNav::ComMonitorNav(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
{

}

ComMonitorNav::~ComMonitorNav()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComMonitorNav::loadInterface()
{
    bool result = true;

    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("parserChanged", &ComMonitorNav::onParserChanged)
            .append("query_loading_deep", &ComMonitorNav::queryLoadingDeep)
            .endGroup();

    return result;
}

bool ComMonitorNav::updateInterface()
{
    bool result = true;

    //
    if (d_ui) {
        QObject *object = 0;
        // load-deep
        object = (QObject *)jframeMainWindow()->queryObject(
                    "combo_loading_deep@group_property@page_monitor@*");
        if (object) {
            d_ui->setLoadingDeep(object->property("currentData").toInt());
        }
        // offset
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_offset@group_showitem@page_monitor@*");
        if (object) {
            d_ui->showItemOffset(object->property("checked").toBool());
        }
        // type
        object= (QObject *)jframeMainWindow()->queryObject(
                    "check_show_type@group_showitem@page_monitor@*");
        if (object) {
            d_ui->showItemType(object->property("checked").toBool());
        }
        // data
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_data@group_showitem@page_monitor@*");
        if (object) {
            d_ui->showItemData(object->property("checked").toBool());
        }
        // data-format
        object = (QObject *)jframeMainWindow()->queryObject(
                    "combo_show_data_format@group_showitem@page_monitor@*");
        if (object) {
            d_ui->setDataFormat(object->property("currentData").toInt());
        }
        // value
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_value@group_showitem@page_monitor@*");
        if (object) {
            d_ui->showItemValue(object->property("checked").toBool());
        }
        // spec
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_spec@group_showitem@page_monitor@*");
        if (object) {
            d_ui->showItemSpec(object->property("checked").toBool());
        }
        // play
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_play@group_showitem@page_monitor@*");
        if (object) {
            d_ui->setPlay(object->property("checked").toBool());
        }
        // update-interval
        object = (QObject *)jframeMainWindow()->queryObject(
                    "spin_flush_period@group_showitem@page_monitor@*");
        if (object) {
            d_ui->setIntervalUpdate(object->property("value").toInt());
        }
        // update-show-navigation
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_nav@group_window@page_monitor@*");
        if (object) {
            d_ui->setVisible(object->property("checked").toBool());
        }
    }

    return result;
}

void ComMonitorNav::releaseInterface()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("parserChanged")
            .remove("query_loading_deep")
            .endGroup();

    //
    if (d_ui) {
        d_ui->releaseWork();
    }
}

void* ComMonitorNav::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComMonitorNav::componentName() const
{
    return QStringLiteral("com_monitor_nav").toStdString();
}

std::string ComMonitorNav::componentDesc() const
{
    return QStringLiteral("监控系统ICD结构树管理").toStdString();
}

void ComMonitorNav::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .endGroup();

    //
}

void ComMonitorNav::detach()
{
    //

    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .endGroup();
}

void *ComMonitorNav::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (d_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    d_ui = new ComMonitorNavUi(*d_attempter);

    //
    d_ui->init();

    return qobject_cast<QWidget *>(d_ui);
}

bool ComMonitorNav::commandSink(void *sender, const std::string &domain, const std::string &objectName,
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
        if (objectName == "combo_loading_deep") {
            if (d_ui) {
                d_ui->setLoadingDeep(objectSender->property("currentData").toInt());
            }
            return true;
        } else if (objectName == "check_show_offset") {
            if (d_ui) {
                d_ui->showItemOffset(*(bool *)data);
            }
            return true;
        } else if (objectName == "check_show_type") {
            if (d_ui) {
                d_ui->showItemType(*(bool *)data);
            }
            return true;
        } else if (objectName == "check_show_data") {
            if (d_ui) {
                d_ui->showItemData(*(bool *)data);
            }
            return true;
        } else if (objectName == "combo_show_data_format") {
            if (d_ui) {
                d_ui->setDataFormat(objectSender->property("currentData").toInt());
            }
            return true;
        } else if (objectName == "check_show_value") {
            if (d_ui) {
                d_ui->showItemValue(*(bool *)data);
            }
            return true;
        } else if (objectName == "check_show_spec") {
            if (d_ui) {
                d_ui->showItemSpec(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_play") {
            if (d_ui) {
                bool running = *(bool *)data;
                QPixmap pixmap = QPixmap(QString(":/icd/common/images/")
                                         + (running ? "pause" : "play") + ".png")
                        .scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                objectSender->setProperty("icon", QIcon(pixmap));
                objectSender->setProperty("iconText", running
                                          ? QStringLiteral("停止刷新") : QStringLiteral("启动刷新"));
                d_ui->setPlay(*(bool *)data);
            }
            return true;
        } else if (objectName == "spin_flush_period") {
            if (d_ui) {
                d_ui->setIntervalUpdate(*(int *)data);
            }
            return true;
        }
        // batch process
        else if (objectName == "action_bindingch") {
            if (d_ui) {
                d_ui->bindingChannels();
            }
            return true;
        } else if (objectName == "action_unbinding") {
            if (d_ui) {
                d_ui->unbindingChannels();
            }
            return true;
        } else if (objectName == "action_export") {
            if (d_ui) {
                d_ui->exportBindingStatus();
            }
            return true;
        } else if (objectName == "action_run_all") {
            if (d_ui) {
                d_ui->runAllChannels();
            }
            return true;
        } else if (objectName == "action_stop_all") {
            if (d_ui) {
                d_ui->stopAllChannels();
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

bool ComMonitorNav::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //

    return false;
}

std::string ComMonitorNav::observerId() const
{
    return componentName();
}

JLRESULT ComMonitorNav::onParserChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

JLRESULT ComMonitorNav::queryLoadingDeep(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);

    int *pDeep = (int *)wParam;
    if (!pDeep) {
        return JFrame::ResultError;
    }

    //
    QObject *comboBoxLoadingDeep = (QObject *)d_attempter->mainWindow()->queryObject(
                "combo_loading_deep@group_property@page_monitor@" + componentName());
    if (!comboBoxLoadingDeep) {
        return JFrame::ResultError;
    }

    *pDeep = comboBoxLoadingDeep->property("currentData").toInt();

    return JFrame::ResultSuccess;
}

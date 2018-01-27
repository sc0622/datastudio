#include "precomp.h"
#include "commonitor.h"
#include "commonitorui.h"
#include "icdworker/icdworker_pool.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComMonitor(reinterpret_cast<IJAttempter *>(attemper)));
}

ComMonitor::ComMonitor(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
{

}

ComMonitor::~ComMonitor()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComMonitor::loadInterface()
{
    bool result = true;

    // 订阅共享消息
    d_attempter->notifier().beginGroup(this)
            .append("treeItemUnloaded", &ComMonitor::onTreeItemUnloaded)
            .append("channelUnbound", &ComMonitor::onChannelUnbound)
            .append("channelChanged", &ComMonitor::onChannelChanged)
            .append("unbindItem", &ComMonitor::onUnbindItem)
            .append("setPlay", &ComMonitor::onSetPlay)
            .endGroup();

    return result;
}

bool ComMonitor::updateInterface()
{
    bool result = true;

    //
    if (d_ui) {
        QObject *object = 0;
        // y-label
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_chart_y_label@group_chart_settings@page_monitor@*");
        if (object) {
            d_ui->showChartYLabel(object->property("checked").toBool());
        }
        // y-align
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_chart_y_align@group_chart_settings@page_monitor@*");
        if (object) {
            d_ui->showChartYAlign(object->property("checked").toBool());
        }
        // sync-track
        object = (QObject *)jframeMainWindow()->queryObject(
                    "action_chart_sync_track@group_chart_settings@page_monitor@*");
        if (object) {
            d_ui->setSyncTrack(object->property("checked").toBool());
        }
        // chart column count
        object = (QObject *)jframeMainWindow()->queryObject(
                    "spin_chart_colcnt@group_chart_settings@page_monitor@*");
        if (object) {
            d_ui->setColumnCount(object->property("value").toInt());
        }
        // y-width
        object = (QObject *)jframeMainWindow()->queryObject(
                    "spin_chart_y_width@group_chart_settings@page_monitor@*");
        if (object) {
            d_ui->setYLabelLength(object->property("value").toInt());
        }
        // update-show-chartview
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_chartview@group_window@page_monitor@*");
        if (object) {
            d_ui->setVisible(object->property("checked").toBool());
            //
            QObject *objectChecked = (QObject *)jframeMainWindow()->queryObject(
                        "check_show_chartview@group_window@page_simulator@com_simulator");
            if (objectChecked) {
                object->disconnect(objectChecked);
                QObject::connect(object, SIGNAL(toggled(bool)),
                                 objectChecked, SLOT(setChecked(bool)));
            }
        }
    }

    return result;
}

void ComMonitor::releaseInterface()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("treeItemUnloaded")
            .remove("channelUnbound")
            .remove("channelChanged")
            .remove("unbindItem")
            .remove("setPlay")
            .endGroup();

    //
    if (d_ui) {
        d_ui->releaseWork();
    }
}

void* ComMonitor::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComMonitor::componentName() const
{
    return "com_monitor";
}

std::string ComMonitor::componentDesc() const
{
    return "com_monitor";
}

void ComMonitor::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("treeItemClick", &ComMonitor::onTreeItemClicked)
            .append("currentItemChanged", &ComMonitor::onCurrentItemChanged)
            .append("channelBound", &ComMonitor::onChannelBound)
            .endGroup();

    //
}

void ComMonitor::detach()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("treeItemClick")
            .remove("currentItemChanged")
            .remove("channelBound")
            .endGroup();
}

void *ComMonitor::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (d_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    d_ui = new ComMonitorUi(*d_attempter);

    //
    d_ui->init();

    return qobject_cast<QWidget *>(d_ui);
}

bool ComMonitor::commandSink(void *sender, const std::string &domain, const std::string &objectName,
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
        if (objectName == "action_chart_clear") {
            if (d_ui) {
                d_ui->clearCharts();
            }
            return true;
        } else if (objectName == "action_chart_colwidth") {
            if (d_ui) {
                d_ui->setColumnWidthSame();
            }
            return true;
        } else if (objectName == "action_chart_y_label") {
            if (d_ui) {
                d_ui->showChartYLabel(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_chart_y_align") {
            if (d_ui) {
                d_ui->showChartYAlign(*(bool *)data);
            }
            return true;
        } else if (objectName == "action_chart_sync_track") {
            if (d_ui) {
                d_ui->setSyncTrack(*(bool *)data);
            }
            return true;
        } else if (objectName == "spin_chart_colcnt") {
            if (d_ui) {
                d_ui->setColumnCount(*(int *)data);
            }
            return true;
        } else if (objectName == "spin_chart_y_width") {
            if (d_ui) {
                d_ui->setYLabelLength(*(int *)data);
            }
            return true;
        }
    }

    //
    if (objectName == "check_show_chartview") {
        if (d_ui) {
            d_ui->setVisible(*(bool *)data);
        }
        return true;
    }

    return false;
}

bool ComMonitor::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //

    return false;
}

std::string ComMonitor::observerId() const
{
    return componentName();
}

JLRESULT ComMonitor::onTreeItemClicked(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    if (!item) {
        return JFrame::ResultError;
    }

    //
    if (d_ui) {
        d_ui->onTreeItemClicked(item);
    }

    // 转发消息
    jframeNotifier().sendMessage("treeItemClick@com_utraledit", wParam, lParam);

    return JFrame::ResultSuccess;
}

JLRESULT ComMonitor::onCurrentItemChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    //
    QStandardItem *current = (QStandardItem *)wParam;
    //QStandardItem *previous = (QStandardItem *)lParam;
    if (!current) {
        return JFrame::ResultError;
    }

    //
    if (d_ui) {
        //
    }

    // 转发消息
    jframeNotifier().sendMessage("currentItemChanged@com_utraledit", wParam, lParam);

    return JFrame::ResultSuccess;
}

JLRESULT ComMonitor::onTreeItemUnloaded(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    QStandardItem *itemTable = (QStandardItem *)lParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    if (d_ui) {
        d_ui->onTreeItemUnloaded(item, itemTable);
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComMonitor::onChannelBound(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    const QString *const channelId = (QString *)lParam;
    if (!item || !channelId) {
        return JFrame::ResultError;
    }

    //

    return JFrame::ResultSuccess;
}

JLRESULT ComMonitor::onChannelUnbound(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    const QString *const channelId = (QString *)lParam;
    if (!item || !channelId) {
        return JFrame::ResultError;
    }

    //
    if (d_ui) {
        d_ui->onChannelUnbound(item, *channelId);
    }

    // 转发消息
    jframeNotifier().sendMessage("channelUnbound@com_utraledit", wParam, lParam);

    return JFrame::ResultSuccess;
}

JLRESULT ComMonitor::onChannelChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    const QString *const channelId = (QString *)lParam;
    if (!item || !channelId) {
        return JFrame::ResultError;
    }

    //
    if (d_ui) {
        d_ui->onChannelChanged(item, *channelId);
    }

    // 转发消息
    jframeNotifier().sendMessage("channelChanged@com_utraledit", wParam, lParam);

    return JFrame::ResultSuccess;
}

JLRESULT ComMonitor::onUnbindItem(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    //
    QStandardItem *item = (QStandardItem *)wParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    QStandardItem *itemTable = (QStandardItem *)lParam;
    if (!item) {
        return JFrame::ResultError;
    }
    //
    if (d_ui) {
        d_ui->onUnbindItem(item, itemTable);
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComMonitor::onSetPlay(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    //
    bool *running = (bool *)wParam;
    if (!running) {
        return JFrame::ResultError;
    }

    if (d_ui) {
        d_ui->setPlay(*running);
    }

    return JFrame::ResultSuccess;
}

#include "precomp.h"
#include "comsimulator.h"
#include "comsimulatorui.h"
#include "icdworker/icdworker_pool.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComSimulator(reinterpret_cast<IJAttempter *>(attemper)));
}

ComSimulator::ComSimulator(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
{

}

ComSimulator::~ComSimulator()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComSimulator::loadInterface()
{
    bool result = true;

    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("treeItemUnloaded", &ComSimulator::onTreeItemUnloaded)
            .append("channelUnbound", &ComSimulator::onChannelUnbound)
            .append("channelChanged", &ComSimulator::onChannelChanged)
            .append("unbindItem", &ComSimulator::onUnbindItem)
            .endGroup();

    return result;
}

bool ComSimulator::updateInterface()
{
    bool result = true;

    //
    if (d_ui) {
        QObject *object = 0;
        // update-show-editview
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_editview@group_window@page_simulator@*");
        if (object) {
            d_ui->setVisible(object->property("checked").toBool());
        }
        // update-show-chartview
        object = (QObject *)jframeMainWindow()->queryObject(
                    "check_show_chartview@group_window@page_simulator@*");
        if (object) {
            QObject *objectChecked = (QObject *)jframeMainWindow()->queryObject(
                        "check_show_chartview@group_window@page_monitor@com_monitor");
            if (objectChecked) {
                object->disconnect(objectChecked);
                QObject::connect(object, SIGNAL(toggled(bool)),
                                 objectChecked, SLOT(setChecked(bool)));
            }
        }
    }

    return result;
}

void ComSimulator::releaseInterface()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("treeItemUnloaded")
            .remove("channelUnbound")
            .remove("channelChanged")
            .remove("unbindItem")
            .endGroup();

    //
    if (d_ui) {
        d_ui->releaseWork();
    }
}

void* ComSimulator::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComSimulator::componentName() const
{
    return QStringLiteral("com_simulator").toStdString();
}

std::string ComSimulator::componentDesc() const
{
    return QStringLiteral("ICD仿真系统组件").toStdString();
}

void ComSimulator::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("treeItemClick", &ComSimulator::onTreeItemClicked)
            .append("channelBound", &ComSimulator::onChannelBound)
            .endGroup();

    //
}

void ComSimulator::detach()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("treeItemClick")
            .remove("channelBound")
            .endGroup();
}

void *ComSimulator::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (d_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    d_ui = new ComSimulatorUi(*d_attempter);

    return qobject_cast<QWidget *>(d_ui);
}

bool ComSimulator::commandSink(void *sender, const std::string &domain, const std::string &objectName,
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
        if (objectName == "check_show_editview") {
            if (d_ui) {
                d_ui->setVisible(*(bool *)data);
            }
            return true;
        }
    }

    return false;
}

bool ComSimulator::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //

    return false;
}

std::string ComSimulator::observerId() const
{
    return componentName();
}

JLRESULT ComSimulator::onTreeItemClicked(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

    return JFrame::ResultSuccess;
}

JLRESULT ComSimulator::onTreeItemUnloaded(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

JLRESULT ComSimulator::onChannelBound(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

JLRESULT ComSimulator::onChannelUnbound(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

    return JFrame::ResultSuccess;
}

JLRESULT ComSimulator::onChannelChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

    return JFrame::ResultSuccess;
}

JLRESULT ComSimulator::onUnbindItem(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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
        d_ui->onUnbindItem(item);
    }

    return JFrame::ResultSuccess;
}

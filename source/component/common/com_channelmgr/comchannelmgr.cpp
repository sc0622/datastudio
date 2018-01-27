#include "precomp.h"
#include "comchannelmgr.h"
#include "comchannelmgrui.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComChannelMgr(reinterpret_cast<IJAttempter *>(attemper)));
}

ComChannelMgr::ComChannelMgr(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
{

}

ComChannelMgr::~ComChannelMgr()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComChannelMgr::loadInterface()
{
    bool result = true;

    //

    return result;
}

bool ComChannelMgr::updateInterface()
{
    bool result = true;

    //

    return result;
}

void ComChannelMgr::releaseInterface()
{
    //

    if (d_ui) {
        d_ui->releaseWork();
    }
}

void* ComChannelMgr::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComChannelMgr::componentName() const
{
    return QStringLiteral("com_channelmgr").toStdString();
}

std::string ComChannelMgr::componentDesc() const
{
    return QStringLiteral("com_channelmgr").toStdString();
}

void ComChannelMgr::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .endGroup();

    //
}

void ComChannelMgr::detach()
{
    //

    // 取消订阅消息
    d_attempter->notifier().remove(this);
}

void *ComChannelMgr::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (objectName == "page_channel_mgr") {
        //
        if (d_ui) {
            Q_ASSERT(false);
            return 0;
        }
        //
        d_ui = new ComChannelMgrUi(*d_attempter);
        return qobject_cast<QWidget *>(d_ui);
    } else {
        return 0;
    }

    return 0;
}

bool ComChannelMgr::commandSink(void *sender, const std::string &domain, const std::string &objectName,
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

bool ComChannelMgr::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //

    return false;
}

std::string ComChannelMgr::observerId() const
{
    return componentName();
}

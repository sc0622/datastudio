#include "precomp.h"
#include "comutraledit.h"
#include "comutraleditui.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    // 非管理员用户才能使用此组件
    if (jframeLogin()->loginManager()->isAdminUser()) {
        return 0;
    }

    return static_cast<IJComponent *>
            (new ComUtralEdit(reinterpret_cast<IJAttempter *>(attemper)));
}

ComUtralEdit::ComUtralEdit(IJAttempter *attemper)
    : d_attempter(attemper)
    , d_ui(0)
{

}

ComUtralEdit::~ComUtralEdit()
{
    if (d_ui) {
        d_ui->deleteLater();
        d_ui = 0;
    }
}

bool ComUtralEdit::loadInterface()
{
    bool result = true;

    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("query_show_utraledit", &ComUtralEdit::queryShowUtralEdit)
            .append("channelUnbound", &ComUtralEdit::onChannelUnbound)
            .append("channelChanged", &ComUtralEdit::onChannelChanged)
            .endGroup();

    return result;
}

bool ComUtralEdit::updateInterface()
{
    bool result = true;

    //
    if (d_ui) {
        bool visible = false;
        if (queryShowUtralEdit("query_show_utraledit@com_utraledit", (JWPARAM)&visible, 0)
                == JFrame::ResultSuccess) {
            d_ui->setVisible(visible);
        }
    }

    return result;
}

void ComUtralEdit::releaseInterface()
{
    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("query_show_utraledit")
            .remove("channelUnbound")
            .remove("channelChanged")
            .endGroup();
}

void* ComUtralEdit::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);
    J_QUERY_INTERFACE(IJMessageSink, iid, ver);

    return 0;
}

std::string ComUtralEdit::componentName() const
{
    return QStringLiteral("com_utraledit").toStdString();
}

std::string ComUtralEdit::componentDesc() const
{
    return QStringLiteral("十六进制数据编辑管理组件").toStdString();
}

void ComUtralEdit::attach()
{
    // 订阅消息
    d_attempter->notifier().beginGroup(this)
            .append("treeItemClick", &ComUtralEdit::onTreeItemClicked)
            .endGroup();

    //
}

void ComUtralEdit::detach()
{
    //

    // 取消订阅消息
    d_attempter->notifier().beginGroup(this)
            .remove("treeItemClick")
            .endGroup();
}

void *ComUtralEdit::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (d_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    d_ui = new ComUtralEditUi(*d_attempter);

    return qobject_cast<QWidget *>(d_ui);
}

bool ComUtralEdit::commandSink(void *sender, const std::string &domain, const std::string &objectName,
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
    if (_domain.startsWith("group_window")) {
        //
        if (objectName == "check_show_utraledit") {
            if (d_ui) {
                d_ui->setVisible(*(bool *)data);
            }
            return true;
        }
    }

    return false;
}

bool ComUtralEdit::messageSink(IJComponent *sender, const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(sender);
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    //

    return false;
}

std::string ComUtralEdit::observerId() const
{
    return componentName();
}

JLRESULT ComUtralEdit::queryShowUtralEdit(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);

    bool *pRet = (bool *)wParam;
    if (!pRet) {
        return JFrame::ResultError;
    }

    QObject *object = (QObject *)jframeMainWindow()
            ->queryObject("check_show_utraledit@group_window@page_monitor@*");
    if (!object) {
        return JFrame::ResultError;
    }

    const QVariant variant = object->property("checked");
    if (variant.isNull()) {
        return JFrame::ResultError;
    }

    *(pRet) = variant.toBool();

    return JFrame::ResultSuccess;
}

JLRESULT ComUtralEdit::onTreeItemClicked(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

JLRESULT ComUtralEdit::onCurrentItemChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    //
    QStandardItem *current = (QStandardItem *)wParam;
    QStandardItem *previous = (QStandardItem *)lParam;
    if (!current) {
        return JFrame::ResultError;
    }

    //
    if (d_ui) {
        d_ui->onCurrentItemChanged(current, previous);
    }

    return JFrame::ResultSuccess;
}

JLRESULT ComUtralEdit::onChannelUnbound(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

JLRESULT ComUtralEdit::onChannelChanged(const std::string &id, JWPARAM wParam, JLPARAM lParam)
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

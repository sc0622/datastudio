#include "icdmainview.h"
#include "icdmainviewui.h"
#include "planenode.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    return static_cast<IJComponent *>
            (new ICDMainView(reinterpret_cast<IJAttempter *>(attemper)));
}

ICDMainView::ICDMainView(IJAttempter *attemper)
    : q_attempter(attemper)
    , q_ui(0)
{

}

ICDMainView::~ICDMainView()
{
    if (q_ui) {
        q_ui->deleteLater();
        q_ui = 0;
    }
}

bool ICDMainView::loadInterface()
{
    bool result = true;

    // 订阅消息
    q_attempter->notifier().beginGroup(this)
            .append("showInfrastructure", &ICDMainView::showInfrastructure)
            .endGroup();

    return result;
}

bool ICDMainView::updateInterface()
{
    bool result = true;
    //
    if (q_ui) {
        QObject *object = 0;
        QStringList lst;
        lst << "action_add" << "action_insert" << "action_up"
            << "action_down"  << "action_delete" << "action_clear"
            << "action_database"  << "action_file";
        QStringListIterator it = lst;
        while (it.hasNext()) {
            std::string name = it.next().toStdString();
            object = (QObject *)jframeMainWindow()->queryObject(
                        name + "@*@page_editor@*");

            q_ui->setMenuPtr(name, qobject_cast<QAction *>(object));
        }
    }

    return result;
}

void ICDMainView::releaseInterface()
{
    // 取消订阅消息
    q_attempter->notifier().beginGroup(this)
            .remove("showInfrastructure")
            .endGroup();
}

void* ICDMainView::queryInterface(const std::string &iid, unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    J_QUERY_INTERFACE(IJCommandSink, iid, ver);

    return 0;
}

std::string ICDMainView::componentName() const
{
    return "ICDMainView";
}

std::string ICDMainView::componentDesc() const
{
    return "ICDMainView";
}

void ICDMainView::attach()
{
    // 订阅消息
    q_attempter->notifier().beginGroup(this)
        .append("queryTableInformation", &ICDMainView::queryTableInformation)
        .append("queryLengthOffset", &ICDMainView::queryLengthOffset)
        .append("queryExistedData", &ICDMainView::queryExistedData)
        .append("queryFrameData", &ICDMainView::queryFrameData)
        .append("fillBitSerial", &ICDMainView::fillBitSerial)
        .append("queryMainViewState", &ICDMainView::queryMainViewState)
        .append("dealNavigationCommand", &ICDMainView::dealNavigationCommand)
        .endGroup();

    //TODO: Add your statements.
}

void ICDMainView::detach()
{
    //TODO: Add your statements.

    // 取消订阅消息
    q_attempter->notifier().beginGroup(this)
            .remove("queryTableInformation")
            .remove("queryLengthOffset")
            .remove("queryExistedData")
            .remove("queryFrameData")
            .remove("fillBitSerial")
            .remove("queryMainViewState")
            .remove("dealNavigationCommand")
            .endGroup();
}

void *ICDMainView::createWindow(void *parent, const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (q_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    q_ui = new ICDMainViewUi(*q_attempter);

    return qobject_cast<QWidget *>(q_ui);
}

bool ICDMainView::commandSink(void *sender, const std::string &domain,
                              const std::string &objectName,
                              const std::string &eventType, void *data)
{
    Q_UNUSED(eventType);
    Q_UNUSED(data);

    QObject *objectSender = reinterpret_cast<QObject *>(sender);
    if (!objectSender) {
        return false;
    }
    QString _domain = QString::fromStdString(domain);
    if (_domain.endsWith(QString::fromStdString(componentName()))) {
        if (q_ui->dealMenuCmd(objectName)) {
            return true;
        }
    }

    return false;
}

std::string ICDMainView::observerId() const
{
    return componentName();
}

// 加载显示架构信息
JLRESULT ICDMainView::showInfrastructure(const std::string &id,
                                         JWPARAM wParam,
                                         JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui) {
        return JFrame::ResultError;
    }
    q_ui->initData(lParam, (void*)wParam);

    return JFrame::ResultSuccess;
}

// 查询主视图表数据信息
JLRESULT ICDMainView::queryTableInformation(const std::string &id,
                                            JWPARAM wParam,
                                            JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }

    //
    int &result = *reinterpret_cast<int *>(wParam);
    QString &type = *reinterpret_cast<QString *>(lParam);

    result = q_ui->queryTableInformation(type);

    return JFrame::ResultSuccess;
}

// 查询数据长度偏移
JLRESULT ICDMainView::queryLengthOffset(const std::string &id,
                                        JWPARAM wParam,
                                        JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    ICDMetaData::smtMeta &meta
        = *reinterpret_cast<ICDMetaData::smtMeta *>(wParam);
    int &result = *reinterpret_cast<int *>(lParam);

    result = q_ui->queryLengthOffset(meta);

    return JFrame::ResultSuccess;
}

// 查询当前表中已有数据（不包括当前行数据）
JLRESULT ICDMainView::queryExistedData(const std::string &id,
                                       JWPARAM wParam,
                                       JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    QMap<QString, QString> &result
        = *reinterpret_cast<QMap<QString, QString>*>(wParam);

    const QString &section = *reinterpret_cast<QString *>(lParam);

    result = q_ui->queryExistedData(section);

    return JFrame::ResultSuccess;
}

// 查询当前表中的帧数据
JLRESULT ICDMainView::queryFrameData(const std::string &id,
                                     JWPARAM wParam,
                                     JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    QMap<int, QString> &frame
        = *reinterpret_cast<QMap<int, QString> *>(wParam);
    QString &type = *reinterpret_cast<QString *>(lParam);

    frame = q_ui->queryFrameData(type);

    return JFrame::ResultSuccess;
}

// 填充规则序号和字节序号
JLRESULT ICDMainView::fillBitSerial(const std::string &id,
                                         JWPARAM wParam,
                                         JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    if (!q_ui || !wParam) {
        return JFrame::ResultError;
    }
    ICDMetaData::smtMeta &meta
        = *reinterpret_cast<ICDMetaData::smtMeta *>(wParam);

    q_ui->fillBitSerial(meta);

    return JFrame::ResultSuccess;
}

// 查询主视图控件状态
JLRESULT ICDMainView::queryMainViewState(const std::string &id,
                                         JWPARAM wParam,
                                         JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    const QString &name = *reinterpret_cast<QString *>(wParam);
    bool &result = *reinterpret_cast<bool *>(lParam);

    result = q_ui->queryWidgetState(name);

    return JFrame::ResultSuccess;
}

// 处理来自Navigation模块的命令
JLRESULT ICDMainView::dealNavigationCommand(const std::string &id,
                                            JWPARAM wParam,
                                            JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !lParam) {
        return JFrame::ResultError;
    }
    const QVariant &var = *reinterpret_cast<QVariant *>(lParam);
    q_ui->dealCommand(wParam, var);

    return JFrame::ResultSuccess;
}

#include "icdnavigation.h"
#include "icdnavigationui.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    return static_cast<IJComponent *>
            (new ICDNavigation(reinterpret_cast<IJAttempter *>(attemper)));
}

ICDNavigation::ICDNavigation(IJAttempter *attemper)
    : q_attempter(attemper)
    , q_ui(0)
{

}

ICDNavigation::~ICDNavigation()
{
    if (q_ui) {
        q_ui->deleteLater();
        q_ui = 0;
    }
}

bool ICDNavigation::loadInterface()
{
    bool result = true;

    // 订阅消息
    q_attempter->notifier().beginGroup(this)
            .append("j_frame_try_exit", &ICDNavigation::onSystemQuit)
            .append("reloadUiData", &ICDNavigation::reloadUiData)
            .endGroup();

    return result;
}

bool ICDNavigation::updateInterface()
{
    bool result = true;

    return result;
}

void ICDNavigation::releaseInterface()
{
    // 取消订阅消息
    q_attempter->notifier().beginGroup(this)
            .remove("j_frame_try_exit")
            .remove("reloadUiData")
            .endGroup();
}

void* ICDNavigation::queryInterface(const std::string &iid,
                                    unsigned int ver)
{
    J_QUERY_INTERFACE(IJComponentUi, iid, ver);
    Q_UNUSED(iid);
    Q_UNUSED(ver);

    return 0;
}

std::string ICDNavigation::componentName() const
{
    return "ICDNavigation";
}

std::string ICDNavigation::componentDesc() const
{
    return "ICDNavigation";
}

void ICDNavigation::attach()
{
    // 订阅消息
    q_attempter->notifier().beginGroup(this)
            .append("queryNodeData", &ICDNavigation::queryNodeData)
            .append("queryNodeKeys", &ICDNavigation::queryNodeKeys)
            .append("queryNodeFlag", &ICDNavigation::queryNodeFlag)
            .append("queryTopTable", &ICDNavigation::queryTopTable)
            .append("queryID", &ICDNavigation::queryID)
            .append("updateNodeData", &ICDNavigation::updateNodeData)
            .append("insertNodeData", &ICDNavigation::insertNodeData)
            .append("deleteNode", &ICDNavigation::deleteNode)
            .append("reorderNodeData", &ICDNavigation::reorderNodeData)
            .append("savePastedData", &ICDNavigation::savePastedData)
            .append("saveMemoryData", &ICDNavigation::saveMemoryData)
            .endGroup();
}

void ICDNavigation::detach()
{
    // 取消订阅消息
    q_attempter->notifier().beginGroup(this)
            .remove("queryNodeData")
            .remove("queryNodeKeys")
            .remove("queryNodeFlag")
            .remove("queryTopTable")
            .remove("queryID")
            .remove("updateNodeData")
            .remove("insertNodeData")
            .remove("deleteNode")
            .remove("reorderNodeData")
            .remove("savePastedData")
            .remove("saveMemoryData")
            .endGroup();
}

void *ICDNavigation::createWindow(void *parent, 
                                  const std::string &objectName)
{
    Q_UNUSED(parent);
    Q_UNUSED(objectName);

    //
    if (q_ui) {
        Q_ASSERT(false);
        return 0;
    }

    //
    q_ui = new ICDNavigationUi(*q_attempter);

    return qobject_cast<QWidget *>(q_ui);
}

std::string ICDNavigation::observerId() const
{
    return componentName();
}

// 查询节点数据
JLRESULT ICDNavigation::queryNodeData(const std::string &id,
                                      JWPARAM wParam,
                                      JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam) {
        return JFrame::ResultError;
    }
    QString &condition = *reinterpret_cast<QString*>(lParam);
    ICDElement::smtElement& element
        = *reinterpret_cast<ICDElement::smtElement *>(wParam);

    q_ui->queryNodeData(condition, element);

    return JFrame::ResultSuccess;
}

// 查询当前节点及直属父节点的关键码集
JLRESULT ICDNavigation::queryNodeKeys(const std::string &id,
                                      JWPARAM wParam,
                                      JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    if (!q_ui || !wParam) {
        return JFrame::ResultError;
    }
    QString &result = *reinterpret_cast<QString *>(wParam);

    result = q_ui->queryCurrentKeys();

    return JFrame::ResultSuccess;
}

// 查询节点加载状态
JLRESULT ICDNavigation::queryNodeFlag(const std::string &id,
                                      JWPARAM wParam,
                                      JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    int &result = *reinterpret_cast<int *>(wParam);
    QString &type = *reinterpret_cast<QString *>(lParam);

    result = q_ui->queryNodeFlag(type);

    return JFrame::ResultSuccess;
}

// 查询节点顶层表
JLRESULT ICDNavigation::queryTopTable(const std::string &id,
                                      JWPARAM wParam,
                                      JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(lParam);
    if (!q_ui || !wParam) {
        return JFrame::ResultError;
    }
    ICDElement::smtElement& element
        = *reinterpret_cast<ICDElement::smtElement *>(wParam);

    q_ui->queryTopTable(element);

    return JFrame::ResultSuccess;
}

// 查询分组ID
JLRESULT ICDNavigation::queryID(const std::string &id,
                                     JWPARAM wParam,
                                     JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam) {
        return JFrame::ResultError;
    }
    QString &result = *reinterpret_cast<QString *>(wParam);

    result = q_ui->nextID(lParam);

    return JFrame::ResultSuccess;
}

// 更新节点数据
JLRESULT ICDNavigation::updateNodeData(const std::string &id,
                                       JWPARAM wParam,
                                       JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    bool &result = *reinterpret_cast<bool *>(lParam);

    result = q_ui->updateNodeData(wParam);

    return JFrame::ResultSuccess;
}

// 插入节点数据
JLRESULT ICDNavigation::insertNodeData(const std::string &id,
                                       JWPARAM wParam,
                                       JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    std::vector<int> &params
        = *reinterpret_cast<std::vector<int> *>(wParam);
    bool &result = *reinterpret_cast<bool *>(lParam);

    result = q_ui->insertNodeData(params);

    return JFrame::ResultSuccess;
}

// 删除节点
JLRESULT ICDNavigation::deleteNode(const std::string &id,
                                   JWPARAM wParam,
                                   JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    std::vector<QString> keys
        = *reinterpret_cast<std::vector<QString> *>(wParam);
    bool &result = *reinterpret_cast<bool *>(lParam);

    result = q_ui->deleteNode(keys);

    return JFrame::ResultSuccess;
}

// 重新调整节点顺序
JLRESULT ICDNavigation::reorderNodeData(const std::string &id, 
                                        JWPARAM wParam, 
                                        JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam || !lParam) {
        return JFrame::ResultError;
    }
    std::vector<int> &params
        = *reinterpret_cast<std::vector<int> *>(wParam);
    bool &result = *reinterpret_cast<bool *>(lParam);

    result = q_ui->reorderNodeData(params);

    return JFrame::ResultSuccess;
}

// 保存粘贴数据
JLRESULT ICDNavigation::savePastedData(const std::string &id,
                                       JWPARAM wParam,
                                       JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam) {
        return JFrame::ResultError;
    }
    const QVector<int> &params
        = *reinterpret_cast<QVector<int> *>(wParam);
    ICDElement::smtElement &result
        = *reinterpret_cast<ICDElement::smtElement *>(lParam);

    result = q_ui->savePastedData(params);

    return JFrame::ResultSuccess;
}

// 保存数据到数据源
JLRESULT ICDNavigation::saveMemoryData(const std::string &id,
                                       JWPARAM wParam,
                                       JLPARAM lParam)
{
    Q_UNUSED(id);
    if (!q_ui || !wParam) {
        return JFrame::ResultError;
    }
    QVector<int> &params = *reinterpret_cast<QVector<int> *>(wParam);
    if (params.size() != 2) {
        return JFrame::ResultError;
    }
    QString &err = *reinterpret_cast<QString *>(params.at(0));
    QString &file = *reinterpret_cast<QString *>(params.at(1));

    q_ui->saveMemoryData(lParam, file, err);

    return JFrame::ResultSuccess;
}

// 重新加载界面数据
JLRESULT ICDNavigation::reloadUiData(const std::string &id,
                                     JWPARAM wParam, JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    if (!q_ui) {
        return JFrame::ResultError;
    }
    PlaneNode::planeVector data;
    q_attempter->notifier().sendMessage("queryAllPlane@DataManegement",
                                        (JWPARAM)&data);
    q_ui->loadData(data, q_ui->queryDataSource());

    return JFrame::ResultSuccess;
}

// 模式切换
JLRESULT ICDNavigation::onModuleChanged(const std::string &id,
                                        JWPARAM wParam,
                                        JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    return JFrame::ResultSuccess;
}

// 退出系统
JLRESULT ICDNavigation::onSystemQuit(const std::string &id,
                                   JWPARAM wParam,
                                   JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    if (!q_ui) {
        return JFrame::ResultIgnore;
    }

    if (GlobalDefine::noneState == q_ui->queryNodeFlag(QString("dataState"))) {
        return JFrame::ResultIgnore;
    }

    // 数据变更尚未保存
    int option = QMessageBox::warning(
        jframeLayout()->mainWindow(),
        QStringLiteral("退出软件"),
        QStringLiteral("有数据尚未保存，继续操作将丢失这部分数据！"),
        QStringLiteral("注销（重启）"),
        QStringLiteral("退出"),
        QStringLiteral("取消"), 1);
    int result = JFrame::ResultCancelExit;
    switch (option) {
    case 0:     // 注销（重启）
        result = JFrame::ResultAcceptRestart;
        break;
    case 1:     // 退出
        result = JFrame::ResultAcceptExit;
        break;
    case 2:     // 取消
    default:    // 忽略
        result = JFrame::ResultCancelExit;
        break;
    }

    return result;
}

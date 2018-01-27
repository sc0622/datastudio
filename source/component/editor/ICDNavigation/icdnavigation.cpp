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

    // ������Ϣ
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
    // ȡ��������Ϣ
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
    // ������Ϣ
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
    // ȡ��������Ϣ
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

// ��ѯ�ڵ�����
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

// ��ѯ��ǰ�ڵ㼰ֱ�����ڵ�Ĺؼ��뼯
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

// ��ѯ�ڵ����״̬
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

// ��ѯ�ڵ㶥���
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

// ��ѯ����ID
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

// ���½ڵ�����
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

// ����ڵ�����
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

// ɾ���ڵ�
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

// ���µ����ڵ�˳��
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

// ����ճ������
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

// �������ݵ�����Դ
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

// ���¼��ؽ�������
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

// ģʽ�л�
JLRESULT ICDNavigation::onModuleChanged(const std::string &id,
                                        JWPARAM wParam,
                                        JLPARAM lParam)
{
    Q_UNUSED(id);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    return JFrame::ResultSuccess;
}

// �˳�ϵͳ
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

    // ���ݱ����δ����
    int option = QMessageBox::warning(
        jframeLayout()->mainWindow(),
        QStringLiteral("�˳����"),
        QStringLiteral("��������δ���棬������������ʧ�ⲿ�����ݣ�"),
        QStringLiteral("ע����������"),
        QStringLiteral("�˳�"),
        QStringLiteral("ȡ��"), 1);
    int result = JFrame::ResultCancelExit;
    switch (option) {
    case 0:     // ע����������
        result = JFrame::ResultAcceptRestart;
        break;
    case 1:     // �˳�
        result = JFrame::ResultAcceptExit;
        break;
    case 2:     // ȡ��
    default:    // ����
        result = JFrame::ResultCancelExit;
        break;
    }

    return result;
}

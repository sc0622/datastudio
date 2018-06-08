#include "precomp.h"
#include "icdnavigation.h"
#include "icdnavigationui.h"

ICDNavigation::ICDNavigation(QWidget *parent)
    : QWidget(parent)
    , dataMgr_(nullptr)
{
    dataMgr_ = new DataManegement(this);

    QHBoxLayout *layoutMain = new QHBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    q_ui = new ICDNavigationUi(this);
    layoutMain->addWidget(q_ui);

    jnotify->on("edit.toolbar.database.config", this, [=](JNEvent &){
        QVariantList args;
        args << "edit" << qVariantFromValue((void*)this);
        jnotify->send("database.config", args);
    });
    jnotify->on("edit.toolbar.window.tree", this, [=](JNEvent &event){
        setVisible(event.argument().toBool());
    });
    jnotify->on("edit.parser.changed", this, [=](JNEvent &){
        //TODO
    });
    jnotify->on("edit.parser.inst", this, [=](JNEvent &event){
        Icd::JParserPtrHandle *handle =
                jVariantFromVoid<Icd::JParserPtrHandle>(event.argument());
        if (!handle) {
            return;
        }
        //handle->parser = treeView_->parser(); //TODO
        event.setReturnValue(true);
    });

    jnotify->on("edit.tryQuit", this, [=](Icd::JNEvent &event){
        onSystemQuit(event);
    });
    jnotify->on("edit.reloadUiData", this, [=](Icd::JNEvent &event){
        reloadUiData(event);
    });
    jnotify->on("edit.queryNodeData", this, [=](Icd::JNEvent &event){
        queryNodeData(event);
    });
    jnotify->on("edit.queryNodeKeys", this, [=](Icd::JNEvent &event){
        queryNodeKeys(event);
    });
    jnotify->on("edit.queryNodeFlag", this, [=](Icd::JNEvent &event){
        queryNodeFlag(event);
    });
    jnotify->on("edit.queryTopTable", this, [=](Icd::JNEvent &event){
        queryTopTable(event);
    });
    jnotify->on("edit.queryId", this, [=](Icd::JNEvent &event){
        queryID(event);
    });
    jnotify->on("edit.updateNodeData", this, [=](Icd::JNEvent &event){
        updateNodeData(event);
    });
    jnotify->on("edit.insertNodeData", this, [=](Icd::JNEvent &event){
        insertNodeData(event);
    });
    jnotify->on("edit.deleteNode", this, [=](Icd::JNEvent &event){
        deleteNode(event);
    });
    jnotify->on("edit.reorderNodeData", this, [=](Icd::JNEvent &event){
        reorderNodeData(event);
    });
    jnotify->on("edit.savePastedData", this, [=](Icd::JNEvent &event){
        savePastedData(event);
    });
    jnotify->on("edit.saveMemoryData", this, [=](Icd::JNEvent &event){
        saveMemoryData(event);
    });
}

ICDNavigation::~ICDNavigation()
{
    if (dataMgr_) {
        delete dataMgr_;
        dataMgr_ = nullptr;
    }
}

bool ICDNavigation::init()
{
    dataMgr_->init();
    q_ui->init();

    return true;
}

// ��ѯ�ڵ�����
JLRESULT ICDNavigation::queryNodeData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    QString *condition = jVariantFromVoid<QString>(args[1]);
    if (!condition) {
        return -1;
    }
    ICDElement::smtElement *element = jVariantFromVoid<ICDElement::smtElement>(args[0]);
    if (!element) {
        return -1;
    }

    q_ui->queryNodeData(*condition, *element);

    return 0;
}

// ��ѯ��ǰ�ڵ㼰ֱ�����ڵ�Ĺؼ��뼯
JLRESULT ICDNavigation::queryNodeKeys(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 1) {
        return -1;
    }

    QString *result = jVariantFromVoid<QString>(args[0]);
    if (!result) {
        return -1;
    }

    *result = q_ui->queryCurrentKeys();

    return 0;
}

// ��ѯ�ڵ����״̬
JLRESULT ICDNavigation::queryNodeFlag(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    int *result = jVariantFromVoid<int>(args[0]);
    if (!result) {
        return -1;
    }
    QString *type = jVariantFromVoid<QString>(args[1]);
    if (!type) {
        return -1;
    }

    *result = q_ui->queryNodeFlag(*type);

    return 0;
}

// ��ѯ�ڵ㶥���
JLRESULT ICDNavigation::queryTopTable(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 1) {
        return -1;
    }

    ICDElement::smtElement *element = jVariantFromVoid<ICDElement::smtElement>(args[0]);
    if (!element) {
        return -1;
    }

    q_ui->queryTopTable(*element);

    return 0;
}

// ��ѯ����ID
JLRESULT ICDNavigation::queryID(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 1) {
        return -1;
    }

    QString *result = jVariantFromVoid<QString>(args[0]);
    if (!result) {
        return -1;
    }

    *result = q_ui->nextID(args[1].toInt());

    return 0;
}

// ���½ڵ�����
JLRESULT ICDNavigation::updateNodeData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    bool *result = jVariantFromVoid<bool>(args[1]);
    if (!result) {
        return -1;
    }

    *result = q_ui->updateNodeData(args[0].toInt());

    return 0;
}

// ����ڵ�����
JLRESULT ICDNavigation::insertNodeData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }
    std::vector<int> *params = jVariantFromVoid<std::vector<int> >(args[0]);
    if (!params) {
        return -1;
    }

    bool *result = jVariantFromVoid<bool>(args[1]);
    if (!result) {
        return -1;
    }

    *result = q_ui->insertNodeData(*params);

    return 0;
}

// ɾ���ڵ�
JLRESULT ICDNavigation::deleteNode(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }
    std::vector<QString> *keys = jVariantFromVoid<std::vector<QString> >(args[0]);
    if (!keys) {
        return -1;
    }

    bool *result = jVariantFromVoid<bool>(args[1]);
    if (!result) {
        return -1;
    }

    *result = q_ui->deleteNode(*keys);

    return 0;
}

// ���µ����ڵ�˳��
JLRESULT ICDNavigation::reorderNodeData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    std::vector<int> *params = jVariantFromVoid<std::vector<int> >(args[0]);
    if (!params) {
        return -1;
    }

    bool *result = jVariantFromVoid<bool>(args[1]);
    if (!result) {
        return -1;
    }

    *result = q_ui->reorderNodeData(*params);

    return 0;
}

// ����ճ������
JLRESULT ICDNavigation::savePastedData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    const QVector<int> *params = jVariantFromVoid<QVector<int> >(args[0]);
    if (!params) {
        return -1;
    }

    ICDElement::smtElement *result = jVariantFromVoid<ICDElement::smtElement>(args[1]);
    if (!result) {
        return -1;
    }

    *result = q_ui->savePastedData(*params);

    return 0;
}

// �������ݵ�����Դ
JLRESULT ICDNavigation::saveMemoryData(const Icd::JNEvent &event)
{
    const QVariantList args = event.argument().toList();
    if (args.count() < 2) {
        return -1;
    }

    QVector<int> *params = jVariantFromVoid<QVector<int> >(args[0]);
    if (!params || params->size() != 2) {
        return -1;
    }

    QString *err = reinterpret_cast<QString *>(params->at(0));
    if (!err) {
        return -1;
    }

    QString *file = reinterpret_cast<QString *>(params->at(1));
    if (!file) {
        return -1;
    }

    q_ui->saveMemoryData(args[1].toInt(), *file, *err);

    return 0;
}

// ���¼��ؽ�������
JLRESULT ICDNavigation::reloadUiData(const Icd::JNEvent &event)
{
    Q_UNUSED(event);

    PlaneNode::planeVector data;

    QVariantList args;
    args.append(qVariantFromValue((void*)&data));
    jnotify->send("edit.queryAllPlane", args);
    q_ui->loadData(data, q_ui->queryDataSource());

    return 0;
}

// ģʽ�л�
JLRESULT ICDNavigation::onModuleChanged(const Icd::JNEvent &event)
{
    Q_UNUSED(event);
    return 0;
}

// �˳�ϵͳ
JLRESULT ICDNavigation::onSystemQuit(const Icd::JNEvent &event)
{
    Q_UNUSED(event);

    if (!q_ui) {
        return -1;
    }

    if (GlobalDefine::noneState == q_ui->queryNodeFlag(QString("dataState"))) {
        return -1;
    }

    // ���ݱ����δ����
    int option = QMessageBox::warning(
        this,
        QStringLiteral("�˳����"),
        QStringLiteral("��������δ���棬������������ʧ�ⲿ�����ݣ�"),
        QStringLiteral("ע����������"),
        QStringLiteral("�˳�"),
        QStringLiteral("ȡ��"), 1);
    int result = -1;
    switch (option) {
    case 0:     // ע����������
        result = -1;
        break;
    case 1:     // �˳�
        result = -1;
        break;
    case 2:     // ȡ��
    default:    // ����
        result = -1;
        break;
    }

    return result;
}

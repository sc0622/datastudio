#ifndef ICDNAVIGATION_H
#define ICDNAVIGATION_H

#include <QWidget>
#include "../datamgr/datamanagement.h"

class ICDNavigationUi;

class ICDNavigation : public QWidget
{
    Q_OBJECT
public:
    explicit ICDNavigation(QWidget *parent = nullptr);
    virtual ~ICDNavigation();

signals:

public slots:

protected:
    // 查询节点数据
    JLRESULT queryNodeData(const Icd::JNEvent &event);
    // 查询当前节点及直属父节点的关键码集
    JLRESULT queryNodeKeys(const Icd::JNEvent &event);
    // 查询节点加载状态
    JLRESULT queryNodeFlag(const Icd::JNEvent &event);
    // 查询节点顶层表
    JLRESULT queryTopTable(const Icd::JNEvent &event);
    // 查询分组ID
    JLRESULT queryID(const Icd::JNEvent &event);
    // 更新节点数据
    JLRESULT updateNodeData(const Icd::JNEvent &event);
    // 插入节点数据
    JLRESULT insertNodeData(const Icd::JNEvent &event);
    // 删除节点
    JLRESULT deleteNode(const Icd::JNEvent &event);
    // 重新调整节点顺序
    JLRESULT reorderNodeData(const Icd::JNEvent &event);
    // 保存粘贴数据
    JLRESULT savePastedData(const Icd::JNEvent &event);
    // 保存数据到数据源
    JLRESULT saveMemoryData(const Icd::JNEvent &event);
    // 重新加载界面数据
    JLRESULT reloadUiData(const Icd::JNEvent &event);
    // 模式切换
    JLRESULT onModuleChanged(const Icd::JNEvent &event);
    // 退出系统
    JLRESULT onSystemQuit(const Icd::JNEvent &event);

private:
    DataManegement *dataMgr_;
    ICDNavigationUi *q_ui;
};

#endif // ICDNAVIGATION_H

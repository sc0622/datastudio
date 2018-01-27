#ifndef ICDNAVIGATION_H
#define ICDNAVIGATION_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ICDNavigationUi;

class ICDNavigation
        : public IJComponent
        , public IJComponentUi
        , public JObserver
{
public:
    explicit ICDNavigation(IJAttempter* attemper);
    virtual ~ICDNavigation();

    // IJUnknown interface
public:
    bool loadInterface();
    bool updateInterface();
    void releaseInterface();
    void *queryInterface(const std::string &iid, unsigned int ver);

    // IJComponent interface
public:
    std::string componentName() const;
    std::string componentDesc() const;
    void attach();
    void detach();

    // IJComponentUi interface
public:
    void *createWindow(void *parent, const std::string &objectName);

    // JObserver interface
public:
    std::string observerId() const;

protected:
    // 查询节点数据
    JLRESULT queryNodeData(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // 查询当前节点及直属父节点的关键码集
    JLRESULT queryNodeKeys(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // 查询节点加载状态
    JLRESULT queryNodeFlag(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // 查询节点顶层表
    JLRESULT queryTopTable(const std::string &id,
                           JWPARAM wParam,
                           JLPARAM lParam);
    // 查询分组ID
    JLRESULT queryID(const std::string &id,
                     JWPARAM wParam,
                     JLPARAM lParam);
    // 更新节点数据
    JLRESULT updateNodeData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // 插入节点数据
    JLRESULT insertNodeData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // 删除节点
    JLRESULT deleteNode(const std::string &id,
                        JWPARAM wParam,
                        JLPARAM lParam);
    // 重新调整节点顺序
    JLRESULT reorderNodeData(const std::string &id,
                             JWPARAM wParam,
                             JLPARAM lParam);
    // 保存粘贴数据
    JLRESULT savePastedData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // 保存数据到数据源
    JLRESULT saveMemoryData(const std::string &id,
                            JWPARAM wParam,
                            JLPARAM lParam);
    // 重新加载界面数据
    JLRESULT reloadUiData(const std::string &id,
                          JWPARAM wParam,
                          JLPARAM lParam);
    // 模式切换
    JLRESULT onModuleChanged(const std::string &id,
                             JWPARAM wParam,
                             JLPARAM lParam);
    // 退出系统
    JLRESULT onSystemQuit(const std::string &id,
                          JWPARAM wParam,
                          JLPARAM lParam);

private:
    IJAttempter *q_attempter;
    ICDNavigationUi *q_ui;
};

#endif // ICDNAVIGATION_H

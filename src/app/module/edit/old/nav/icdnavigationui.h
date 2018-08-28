#ifndef ICDNAVIGATIONUI_H
#define ICDNAVIGATIONUI_H

#include <QWidget>
#include "KernelClass/planenode.h"
#include "KernelClass/icdcomplexdata.h"

namespace Icd { class JSearchEdit; }
class JTreeView;
class QMenu;
class QStandardItem;

class ICDNavigationUi : public QWidget
{
    // 存储数据类型定义
    enum {
        UserKey     = Qt::UserRole + 50,// 标记数据
        ComplexFlag,                    // 复合规则标识
        LevelIndex,                     // 节点层次
        ItemLoaded,                     // 规则表加载状态
        SubTable,                       // 子表名（复合数据）
        RuleDefine,                     // 规则类型定义
        DataChanged,                    // 数据变更标识（未保存到数据源）
        DataSource,                     // 数据源
    };
    enum {
        optUnload = 0,  // 卸载
        optLoad         // 加载
    };
    enum
    {
        StateReset,     // 重置
        StateChanged,   // 变更
    };
    Q_OBJECT
public:
    explicit ICDNavigationUi(QWidget *parent = nullptr);
    ~ICDNavigationUi();

    bool init();

    // 加载显示机型系统结构数据
    void loadData(const PlaneNode::planeVector &data, int source);

    // 更新树
    void updateRuleItems(const PlaneNode::smtPlane &plane);

    // 查询节点数据
    void queryNodeData(const QString &condition,
                       ICDElement::smtElement &element);

    // 查询节点状态
    int queryNodeFlag(const QString &type) const;

    // 查询当前节点顶层表
    void queryTopTable(ICDElement::smtElement &element);

    // 查询当前节点及直属父节点的关键码集
    QString queryCurrentKeys() const;

    // 查询数据源
    int queryDataSource() const;

    // 更新节点数据
    bool updateNodeData(int dataAddr);

    // 获取不重复的ID
    QString nextID(int type);

    // 重新调整节点顺序
    bool insertNodeData(const std::vector<int> &params);

    // 删除节点
    bool deleteNode(const std::vector<QString> &nodeID);

    // 插入节点数据
    bool reorderNodeData(const std::vector<int> &params);

    // 保存粘贴数据
    ICDElement::smtElement savePastedData(const QVector<int> &params);

    // 保存内存数据到数据源
    bool saveMemoryData(int type, const QString &file, QString &err, bool saveAll = false);

Q_SIGNALS:

protected Q_SLOTS:
    // 点击树节点
    void slotItemPressed(QStandardItem *item);

    // 切换选中节点
    void slotCurrentChanged(QStandardItem *current, QStandardItem *previous);

    // 加载规则表数据
    void slotLoadRule();

    // 卸载规则表数据
    void slotUnloadRule();

    // 更新规则表数据
    void slotUpdate();

    // 新增数据
    void slotNew();

    // 清除表数据
    void slotClear();

    // 删除数据
    void slotDelete();

    // 加载数据
    void slotLoadData();
    void loadData(const QString &name);

    //
    bool loadVehicle();

private:
    // 构造单个机型节点（包括下挂分系统）
    QStandardItem* formPlaneNode(QStandardItem *parent,
                                 const PlaneNode::smtPlane &plane);
    // 重新调整机型节点顺序
    void reorderPlaneNode(const std::vector<ICDElement::smtElement> &planes, int pos);

    // 构造单个分系统节点
    QStandardItem* formSystemNode(QStandardItem *parent,
                                  const SystemNode::smtSystem& system);
    // 重新调整系统节点顺序
    void reorderSystemNode(const PlaneNode::smtPlane &plane, int pos);

    // 构造单个规则表节点
    QStandardItem* formTableNode(QStandardItem *parent,
                                 const TableNode::smtTable& table);
    // 重新调整规则表节点顺序
    void reorderTableNode(const SystemNode::smtSystem &system, int pos);

    // 构造单个规则子表节点
    QStandardItem* formSubTableNode(QStandardItem *parent,
                                    const TableNode::smtTable& table, int index,
                                    int offset);
    // 重新调整规则子表节点顺序
    void reorderSubTableNode(const ICDMetaData::smtMeta &meta, int pos);

    // 构造规则节点
    QStandardItem* formRuleNode(QStandardItem *parent,
                                const ICDMetaData::smtMeta& meta,
                                int offset = -1);
    // 重新调整规则节点顺序
    void reorderRuleNode(const TableNode::smtTable& table, int pos);

    // 加载规则节点
    void loadRuleNode(QStandardItem *parent,
                      const TableNode::smtTable &table,
                      int offset = -1);

    // 卸载规则数据
    void unLoadRuleNode(QStandardItem *item);

    // 重新加载规则节点
    void reLoadRuleNode(QStandardItem *item);

    // 显示右键菜单
    void showMenu(QStandardItem* item);

    // 查询指定节点数据加载状态
    int dataLoaded(QStandardItem* item) const;

    // 获取当前节点的直属子节点数据
    QVector<QString> directChidData(QStandardItem* item) const;

    // 获取指定节点的子表
    std::vector<std::string> subTables(QStandardItem *item) const;

    // 查询节点是否有下挂规则节点
    bool hasRuleTable(QStandardItem *item) const;

    // 查询节点下挂顶层规则表数量
    int topRuleTable(QStandardItem *item) const;

    // 获取当前选中节点的直属父节点关键码（机型码，系统码，表名）
    // level : 目标节点等级
    QString parentKey(int level);
    /**获取节点数据**/
    PlaneNode::planeVector planeNodeMap();
    SystemNode::systemMap systemNodeMap(QStandardItem *item,
                                        PlaneNode::smtPlane plane);
    TableNode::tableMap tableNodeMap(QStandardItem *item,
                                     PlaneNode::smtPlane plane);
    TableNode::smtTable ruleNodeMap(QStandardItem *item,
                                    PlaneNode::smtPlane plane);
    TableNode::smtTable subRuleNodeMap(QStandardItem *item,
                                       PlaneNode::smtPlane plane);
    ICDMetaData::smtMeta ruleNodeData(QStandardItem *item,
                                      PlaneNode::smtPlane plane);

    // 根据指定数据查询节点
    QStandardItem *findItem(int role, const QVariant &value,
                            QStandardItem *parent, bool direct = true) const;

    // 更新已加载规则表状态
    void updateLoadedFlag(QStandardItem *item, int state);

    // 更新父节点已加载规则状态
    void updateParentFlag(QStandardItem *item);

    // 更新子节点已加载规则状态
    void updateChildFlag(QStandardItem *item, int state);

    // 更新节点数据更新状态
    void updateDataState(QStandardItem *item, int state);
    // 更新父节点数据更新状态
    void updateParentState(QStandardItem *item, int state);
    // 更新子节点数据更新状态
    void updateChildState(QStandardItem *item, int state);

    // 更新主视图数据
    void updateMainView(QStandardItem *current);

    // 保存机型数据
    bool updatePalneData(stPlane &data);

    // 保存系统数据
    bool updateSystemData(stSystem &data);

    // 保存ICD表数据
    bool updateICDData(stICDBase &data);

    // 保存ICD规则数据
    bool updateRuleData(ICDMetaData::smtMeta &data, bool top = true);

    // 保存ICD规则数据
    bool updateDetailRuleData(ICDMetaData::smtMeta &data);

    // 保存子表数据
    bool updateSubTableData(stICDBase &data);

    // 构造数据偏移量字符串
    QString offsetString(const ICDElement::smtElement &element, int offset = -1) const;

    // 查询树中指定节点类型的最大值
    void maxID(QStandardItem *parent, int level, QString &id);

    // 查询节点及直属父节点的关键码集
    QString queryNodeKeys(QStandardItem *item) const;

    // 收起节点及其子节点
    void collapseItem(QStandardItem *item);

    // 创建新增规则菜单
    void createRuleMenu();

    // 更新当前节点顶层表下的所有规则节点提示信息
    void updateTopTableTip();

    // 更新规则节点提示信息
    void updateRuleTip(const TableNode::smtTable& table, QStandardItem *parent, int offset);

private:
    Icd::JSearchEdit *searchEdit_;
    JTreeView   *q_treeView;
    QMenu       *q_ruleMenu;
};

#endif // ICDNAVIGATIONUI_H

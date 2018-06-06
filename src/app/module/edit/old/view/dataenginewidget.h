#ifndef BASICTABLEWIDGET_H
#define BASICTABLEWIDGET_H

#include "KernelClass/planenode.h"

#include <QtWidgets>

class LoggingWidget;
class JXmlTable;
struct _UIData;
/**
 * @brief 数据编辑主窗口，根据不同的数据类型，加载、编辑对应信息
 */
class DataEngineWidget
        : public QWidget
{
    enum optType{
        optNew  = 0,    // 编辑
        optEdit,        // 修改
        optDelete,      // 删除
        optCopy,        // 拷贝
    };
    enum {
        ComplexTable = Qt::UserRole + 5,
    };
    Q_OBJECT
public:
    DataEngineWidget(QWidget *parent = nullptr);
    ~DataEngineWidget();

    // 初始化界面
    void initUI(int type, void *data);
    // 主动初始化
    void reInit();
    // 查询当前显示表数据信息
    int queryTableInformation(const QString &type) const;
    // 查询已存在表中的数据
    QMap<QString, QString> queryExistedData(const QString &section) const;
    // 查询帧数据
    QMap<int, QString> queryFrameData(const QString &type) const;
    // 填充bit类型数据的起始字节号
    void fillBitIndex(ICDMetaData::smtMeta &meta);
    // 查询数据长度偏移量
    int queryOffset(const ICDMetaData::smtMeta &meta) const;
    // 查询界面控件状态
    bool queryWidgetState(const QString &name) const;
    // 处理来自其他模块的命令
    void dealCommand(int command, const QVariant &param);
    //
    void setMenuPtr(const std::string &name, QAction *action);
    //
    bool dealMenuCmd(const std::string &name);

protected:
    /**加载显示数据**/
    // 机型
    void showData(const PlaneNode::planeVector &planes);
    void updateOne(int index, const PlaneNode::smtPlane &data, optType type);
    // 系统
    void showData(const SystemNode::systemVector &systems);
    void updateOne(int index, const SystemNode::smtSystem &data, optType type);
    // ICD表
    void showData(const TableNode::tableVector &tables);
    void updateOne(int index, const TableNode::smtTable &data,
                   optType type);
    // 规则
    void showData(const ICDMetaData::ruleMap &rules);
    void updateOne(int index, const ICDMetaData::smtMeta &data,
                   optType type);
    // 规则数据项
    void showData(const ICDMetaData::smtMeta &data);
    // 子表
    void updateSubOne(int index, const stICDBase &data, optType type);
    // 规则属性
    void updateMetaOne(const ICDMetaData::smtMeta &data);
    /**加载显示数据**/

    // 更新编辑数据窗口
    void updateLoggingUI(GlobalDefine::OptionType option);

    // 获取当前选中项数据
    ICDElement::smtElement currentData() const;
    // 启/停用信号槽
    void enableConnection(bool enable);
    // 新增机型
    void newPlane();
    // 更新机型编辑数据界面
    void updatePlaneUI(GlobalDefine::OptionType option);
    // 新增系统
    void newSystem();
    // 更新系统编辑数据界面
    void updateSystemUI(GlobalDefine::OptionType option);
    // 新增ICD表
    void newICDTable();
    // 更新ICD表编辑数据界面
    void updateICDTableUI(GlobalDefine::OptionType option);
    // 新增规则
    void newICDRule(int type);
    // 更新规则编辑数据界面
    void updateICDRuleUI(GlobalDefine::OptionType option);
    // 更新数据录入界面
    void updateDetailUI(const _UIData &data);
    // 新增子表
    void newSubTable();
    // 更新子表编辑数据界面
    void updateSubTableUI(GlobalDefine::OptionType option);
    // 显/隐状态提示
    void setTipsVisible(bool visible);
    // 查询当前选中行所处bit位的第一项数据序号
    int queryFirstBitSerial(int bitIndex);
    // 查询能否将当前数据合并到前一项或者后一项数据
    bool canMerged(const ICDMetaData::smtMeta &meta) const;
    // 显示右键菜单
    void showMenu();
    // 当前表是否允许粘贴复制的数据
    bool canPasted() const;
    // 删除数据
    void deleteRule(int pos);
    // 插入数据
    void insertRule(const ICDMetaData::smtMeta &meta, int pos);
    // 变更机型位置
    void movePlane(int from, int to);
    // 变更系统位置
    void moveSystem(int from, int to);
    // 变更表位置
    void moveTable(int from, int to);
    // 变更规则位置
    void moveRule(int from, int to);
    // 变更子表位置
    void moveSubTable(int from, int to);
    // 保存数据到副本
    void saveEditData(void *data);
    // 重新计算规则数据字节序号
    void reorderTable(TableNode::smtTable &table, int from);
    // 查询当前规则数据父表
    TableNode::smtTable parentTable() const;
    //
    void setActionState(QAction *act, bool enabled);

protected slots:
    // 表当前选中项变更
    void slotCurrentItemChanged(QStandardItem *current,
                                QStandardItem *previous);
    // 点击表数据项
    void slotItemPressed(QStandardItem *item);
    // 表自定义菜单
    void slotCustomContextMenu(const QPoint &pos);
    // 表数据变更
    void slotContentChanged();
    // 变更表数据位置
    void slotDataPosChanged(int logical, int oldIndex, int newIndex);
    // 保存界面数据到内存
    void slotSave2Memory(void *data, bool &result);
    // 取消
    void slotCanceled();
    // 新增
    void slotNew(const QVariant &param);
    // 插入
    void slotInsert();
    // 上移
    void slotMoveUp();
    // 下移
    void slotMoveDown();
    // 删除
    void slotDelete();
    // 清空
    void slotClear();
    // 保存数据到数据源
    void slotSave2Source(GlobalDefine::DataSource type);
    // 复制
    void slotCopy();
    // 粘贴
    void slotPaste();

private:
    LoggingWidget   *q_loggingWidget;   // 信息录入界面

    JXmlTable       *q_table;       // 基本信息表
    QGroupBox       *q_tableBox;    // 基本信息表框
    QWidget         *q_tipWidget;   // 提示窗口
    QLabel          *q_edtStatus;   // 状态提示

    QAction         *q_actNew;      // 新增按钮
    QAction         *q_actInsert;   // 插入按钮
    QAction         *q_actUp;       // 上移按钮
    QAction         *q_actDown;     // 下移按钮
    QAction         *q_actDelete;   // 删除按钮
    QAction         *q_actClear;    // 清空按钮
    QAction         *q_actSaveDB;   // 保存到数据库
    QAction         *q_actSaveFile; // 保存到文件
    QAction         *q_paste;       // 右键菜单粘贴

    QAction         *q_genTableGuid;// 生成表GUID

    const QString   q_planeName;    // 机型表
    const QString   q_systemName;   // 分系统表
    const QString   q_tableName;    // icd表
    const QString   q_ruleName;     // 规则表
    const QString   q_discertName;  // 帧识别码
    const QString   q_metaName;     // 规则定义
    QString         q_defaultPath;  // 保存文件默认路径

    int             q_dataType;     // 数据类型
    int             q_subType;      // 子类型
    int             q_newIndex;     // 记录新增数据行号
    int             q_changePos;    // 数据变更起始值，从0开始
    ICDElement::elementVector   q_vData;    // 当前显示信息
    ICDElement::smtElement      q_data;     // 当前显示数据信息
};

#endif // BASICTABLEWIDGET_H

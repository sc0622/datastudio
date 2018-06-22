#ifndef EDITMAINVIEW_H
#define EDITMAINVIEW_H

#include <QWidget>
#include "KernelClass/planenode.h"
#include "../datamgr/datamanagement.h"

class DataEditWidget;

class EditMainView : public QWidget
{
    Q_OBJECT
public:
    explicit EditMainView(QWidget *parent = nullptr);
    ~EditMainView();

    bool init();

    /**
     * @brief 初始化数据
     * @param [in] type : 数据类型(GLOBALDEF::NODETYPE)
     * @param [in] data : 数据
     */
    void initData(int type, void* data);

    // 查询当前显示表数据信息
    int queryTableInformation(const QString &type) const;

    // 查询数据长度偏移量
    int queryLengthOffset(const ICDMetaData::smtMeta &meta) const;

    // 查询已存在表中的数据
    QMap<QString, QString> queryExistedData(const QString &section) const;

    // 查询帧数据
    QMap<int, QString> queryFrameData(const QString &type) const;

    // 填充bit类型数据的起始字节号和序号
    void fillBitSerial(ICDMetaData::smtMeta &meta);

    // 查询界面控件状态
    bool queryWidgetState(const QString &name) const;

    // 处理来自其他模块的命令
    void dealCommand(int command, const QVariant &param);

signals:

public slots:

private:
    // 加载显示架构信息
    JLRESULT showInfrastructure(const Icd::JNEvent &event);
    // 查询主视图表数据信息
    JLRESULT queryTableInformation(const Icd::JNEvent &event);
    // 查询数据长度偏移
    JLRESULT queryLengthOffset(const Icd::JNEvent &event);
    // 查询当前表中已有数据（不包括当前行数据）
    JLRESULT queryExistedData(const Icd::JNEvent &event);
    // 查询当前表中的帧数据
    JLRESULT queryFrameData(const Icd::JNEvent &event);
    // 填充规则序号和字节序号
    JLRESULT fillBitSerial(const Icd::JNEvent &event);
    // 查询主视图控件状态
    JLRESULT queryMainViewState(const Icd::JNEvent &event);
    // 处理来自Navigation模块的命令
    JLRESULT dealNavigationCommand(const Icd::JNEvent &event);

private:
    DataEditWidget *q_dataWidget;      // 信息编辑窗口
};

#endif // EDITMAINVIEW_H

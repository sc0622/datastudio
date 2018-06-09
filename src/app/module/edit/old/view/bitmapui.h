#ifndef BITMAPUI_H
#define BITMAPUI_H

#include<QtWidgets>

#include "metaui.h"
#include "KernelClass/icdbitdata.h"

class JXmlTable;
class LimitTextEdit;
class LimitLineEdit;
// bitMap类
class BitMapUI : public MetaUI
{
    Q_OBJECT
public:
    explicit BitMapUI(QWidget *parent = nullptr);

    int uiType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // 切换数据类型
    void changeDataType(int type) override;
    // 原始数据类型
    int originalType() const override;

signals:

public slots :
    // 数据录入完成
    void slotEditFinished();
    // 编辑文本变更
    void slotTextChanged(const QString& text);

private:
    // 确认
    void confirm() override;
    // 取消
    void cancel() override;

    // 初始化界面数据
    void init();
    // 启/停用信号槽
    void enableConnection(bool enable);
    // 校验界面数据
    bool dataValid();
    // 更新特征值表数据
    void updateTable(int newCount, bool behind = true);
    // 向表插入一条数据
    void updateMapOne(int index, const _Eigenvalue &data,
                      int option = GlobalDefine::optNew);
    // 获取特征值表数据
    std::unordered_map<double, std::string> tableValues() const;

private:
    ICDBitData::smtBit  q_data; // 界面数据
    ICDBitData::smtBit  q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;     // 名称
    QLineEdit       *q_edtCode;     // 编码
    QSpinBox        *q_spinStart;   // 起始位
    QSpinBox        *q_spinLength;  // 长度
    QLineEdit       *q_edtDefault;  // 默认值
    LimitTextEdit   *q_edtDescribe; // 描述

    JXmlTable       *q_table;       // 特征值表
};

#endif // BITMAPUI_H

#ifndef BITMAPUI_H
#define BITMAPUI_H

#include<QtWidgets>

#include "metaui.h"
#include "icdbitdata.h"

class JXmlTable;
class LimitTextEdit;
class LimitLineEdit;
// bitMap类
class BitMapUI : public MetaUI
{
    Q_OBJECT
public:
    explicit BitMapUI(IJAttempter &attempter, QWidget *parent = 0);
    // MetaUI interface
    void setUIData(const _UIData &data);
    void*uiData() const;
    // 切换数据类型
    void changeDataType(int type);
    // 原始数据类型
    int originalType() const;
    // 启/停用界面控制按钮
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots :
    // 数据录入完成
    void slotEditFinished();
    // 编辑文本变更
    void slotTextChanged(const QString& text);
    // 确认
    void slotConfirm();
    // 取消
    void slotCanceled();

protected:
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
    std::map<double, std::string> tableValues() const;

private:
    IJAttempter     &q_attempter;

    ICDBitData::smtBit  q_data; // 界面数据
    ICDBitData::smtBit  q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;     // 名称
    QLineEdit       *q_edtCode;     // 编码
    QSpinBox        *q_spinStart;   // 起始位
    QSpinBox        *q_spinLength;  // 长度
    QLineEdit       *q_edtDefault;  // 默认值
    LimitTextEdit   *q_edtDescribe; // 描述

    QLineEdit       *q_edtStatus;   // 状态提示
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;

    JXmlTable       *q_table;       // 特征值表
};

#endif // BITMAPUI_H

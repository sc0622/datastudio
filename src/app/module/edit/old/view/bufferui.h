#ifndef BUFFERUI_H
#define BUFFERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcustomizeddata.h"

class LimitTextEdit;
class LimitLineEdit;
// 数据预留区界面
class BufferUI : public MetaUI
{
    Q_OBJECT

public:
    BufferUI(QWidget *parent = nullptr);
    ~BufferUI();

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

private:
    ICDCustomizedData::smtCustom    q_data; // 界面数据
    ICDCustomizedData::smtCustom    q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;     // 名称
    QLineEdit       *q_edtCode;     // 编码
    QSpinBox        *q_spinLength;  // 长度
    LimitTextEdit   *q_edtDescribe; // 描述

    QLineEdit       *q_edtStatus;   // 状态提示
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
};

#endif // BUFFERUI_H

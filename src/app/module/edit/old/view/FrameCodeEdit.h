#ifndef FRAMECODE_EDIT_H
#define FRAMECODE_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdframecodedata.h"

class FrameCodeEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit FrameCodeEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // 切换数据类型
    void changeDataType(int type) override;
    // 原始数据类型
    int originalType() const override;

signals:

private slots:
    // 编辑框数据录入完成
    void slotEditFinished();
    // 编辑框文本变更
    void slotTextChanged(const QString& text);

private:
    // 确认
    void confirm() override;
    // 取消
    void cancel() override;

    // 初始化界面数据
    void init();
    // 初始化帧码长度下拉框
    void initBoxLenght();
    // 初始化帧数据下拉框
    void initBoxData();
    // 启/停用信号槽
    void enableConnection(bool enable);
    // 校验界面数据
    bool dataValid();

private:
    ICDFrameCodeData::smtFrameCode  q_data; // 界面数据
    ICDFrameCodeData::smtFrameCode  q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxLength;   // 帧码长度
    QComboBox       *q_boxData;     // 帧数据
    LimitTextEdit   *q_edtRemak;
};

#endif // FRAMECODE_EDIT_H

#ifndef ARRAY_EDIT_H
#define ARRAY_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdarraydata.h"

class QSpinBox;

class ArrayEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ArrayEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // ObjectEdit interface
    void setUIData(const _UIData &data);
    void*uiData() const;
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
    // 初始化校验类型下拉框
    void initBoxType();
    // 启/停用信号槽
    void enableConnection(bool enable);
    // 校验界面数据
    bool dataValid();

private:
    ICDArrayData::smtArray  q_data;     // 界面数据
    ICDArrayData::smtArray  q_old;      // 原始数据

    QColor          q_color;            // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxArrayType;    // 数组类型
    QSpinBox        *q_spinCount;       // 数组长度
    LimitTextEdit   *q_edtRemak;
};

#endif // ARRAY_EDIT_H

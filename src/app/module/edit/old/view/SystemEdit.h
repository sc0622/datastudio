#ifndef SYSTEM_EDIT_H
#define SYSTEM_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/globalstruct.h"

class QGridLayout;

class SystemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit SystemEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
public:
    void setUIData(const _UIData &data) override;
    void *uiData() const override;

signals:

private slots:
    // 编辑框数据录入完成
    void slotEditFinished();
    // 编辑框文本变更
    void slotTextChanged(const QString &text);

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

private:
    stSystem    q_data;     // 基本信息
    stSystem    q_old;      // 原始数据
    QColor      q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;

    QGridLayout     *q_gridLayout;
};

#endif // SYSTEM_EDIT_H

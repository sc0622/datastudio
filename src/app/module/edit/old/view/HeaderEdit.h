#ifndef HEADER_EDIT_H
#define HEADER_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdcommondata.h"

class HexSpinBox;

class HeaderEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit HeaderEdit(QWidget *parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void *uiData() const override;
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
    // 启/停用信号槽
    void enableConnection(bool enable);
    // 校验界面数据
    bool dataValid();

private:
    ICDCommonData::smtCommon    q_data; // 界面数据
    ICDCommonData::smtCommon    q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    HexSpinBox      *q_spinDefault;
    LimitTextEdit   *q_edtRemak;
};

#endif // HEADER_EDIT_H

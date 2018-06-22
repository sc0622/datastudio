#ifndef TABLE_EDIT_H
#define TABLE_EDIT_H

#include "ItemEdit.h"

class LimitTextEdit;
class LimitLineEdit;
class TableEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit TableEdit(QWidget* parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;

signals:

private slots:
    // 编辑框文本变更
    void slotTextChanged(const QString& text);
    // 数据录入完成
    void slotEditFinished() ;
    //
    void slotCheckStateChanged(bool checked);

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
    stICDBase       q_data;     // 基本信息
    stICDBase       q_old;      // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QSpinBox        *q_spinLength;
    QCheckBox       *q_check;
    LimitTextEdit   *q_edtDescribe;
};

#endif // TABLE_EDIT_H

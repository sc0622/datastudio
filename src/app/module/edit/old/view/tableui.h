#ifndef TABLEUI_H
#define TABLEUI_H

#include <QtWidgets>

#include "metaui.h"

class LimitTextEdit;
class LimitLineEdit;
class TableUI : public MetaUI
{
    Q_OBJECT
public:
    explicit TableUI(QWidget* parent = nullptr);
    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;

signals:

protected slots:
    // 编辑框文本变更
    void slotTextChanged(const QString& text);
    // 数据录入完成
    void slotEditFinished() ;
    //
    void slotCheckStateChanged(bool checked);

protected:
    // 确认
    void confirm() override;
    // 取消
    void cancel() override;

    // 初始化界面数据
    void init();
    // 启/停用信号槽
    void enableConnection(bool enable) override;
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

#endif // TABLEUI_H

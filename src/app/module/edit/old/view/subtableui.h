#ifndef SUBTABLEUI_H
#define SUBTABLEUI_H

#include <QtWidgets>

#include "metaui.h"

class LimitTextEdit;
class LimitLineEdit;
class JLargeSpinBox;
class SubTableUI : public MetaUI
{
    Q_OBJECT
public:
    explicit SubTableUI(QWidget* parent = nullptr);
    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;

signals:

protected slots:
    // 编辑框文本变更
    void slotTextChanged(const QString& text);
    // 帧码数据变更
    void slotCodeChanged(const qulonglong &value);
    // 数据录入完成
    void slotEditFinished();

protected:
    // 确认
    void confirm() override;
    // 取消
    void cancel() override;

    // 初始化界面数据
    void init();
    // 初始化帧码长度下拉框
    void initBoxLenght();
    // 启/停用信号槽
    void enableConnection(bool enable) override;
    // 校验界面数据
    bool dataValid();

private:
    stICDBase       q_data;     // 基本信息
    stICDBase       q_old;      // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QComboBox       *q_boxType;   // 帧码长度
    JLargeSpinBox   *q_spinCode;
    QSpinBox        *q_spinSequence;
    LimitTextEdit   *q_edtDescribe;
};

#endif // SUBTABLEUI_H

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
    void setUIData(const _UIData &data);
    void* uiData() const;
    // 启/停用界面控制按钮
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots:
    // 编辑框文本变更
    void slotTextChanged(const QString& text);
    // 帧码数据变更
    void slotCodeChanged(const qulonglong &value);
    // 数据录入完成
    void slotEditFinished();
    // 确认
    void slotConfirm();
    // 取消
    void slotCanceled();

protected:
    // 初始化界面数据
    void init();
    // 初始化帧码长度下拉框
    void initBoxLenght();
    // 启/停用信号槽
    void enableConnection(bool enable);
    // 校验界面数据
    bool dataValid();

private:
    stICDBase       q_data;     // 基本信息
    stICDBase       q_old;      // 原始数据

    QColor          q_color;    // 默认背景色

    QLineEdit       *q_edtStatus;   // 状态提示
    LimitLineEdit   *q_edtName;
    QComboBox       *q_boxType;   // 帧码长度
    JLargeSpinBox   *q_spinCode;
    QSpinBox        *q_spinSequence;
    LimitTextEdit   *q_edtDescribe;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
};

#endif // SUBTABLEUI_H

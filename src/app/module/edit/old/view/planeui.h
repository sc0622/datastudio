#ifndef PLANEUI_H
#define PLANEUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/globalstruct.h"

class LimitTextEdit;
class LimitLineEdit;
/**
* @brief 机型界面类
*/
class PlaneUI : public MetaUI
{
    Q_OBJECT

public:
    PlaneUI(QWidget *parent = nullptr);
    ~PlaneUI();

    // MetaUI interface
public:
    void setUIData(const _UIData &data);
    void *uiData() const;
    // 启/停用界面控制按钮
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots:
    // 编辑框文本变更
    void slotTextChanged(const QString &text);
    // 数据录入完成
    void slotEditFinished();
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
    stPlane     q_data;     // 基本信息
    stPlane     q_old;      // 原始数据
    QColor      q_color;    // 默认背景色

    QLineEdit       *q_edtStatus; // 状态提示
    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
    
};

#endif // PLANEUI_H

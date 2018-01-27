#ifndef SYSTEMUI_H
#define SYSTEMUI_H

#include <QtWidgets>

#include "metaui.h"
#include "globalstruct.h"

class LimitTextEdit;
class LimitLineEdit;
/**
* @brief 系统结构界面类
*/
class SystemUI : public MetaUI
{
    Q_OBJECT

public:
    explicit SystemUI(IJAttempter &attempter, QWidget *parent = 0);
    ~SystemUI();

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
    // 编辑框数据录入完成
    void slotEditFinished();
    // 编辑框文本变更
    void slotTextChanged(const QString &text);
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
    IJAttempter &q_attempter;

    stSystem    q_data;     // 基本信息
    stSystem    q_old;      // 原始数据
    QColor      q_color;    // 默认背景色

    QLineEdit       *q_edtStatus; // 状态提示
    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtDescribe;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;

    QGridLayout     *q_gridLayout;
};

#endif // SYSTEMUI_H

#ifndef HEADERUI_H
#define HEADERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "icdcommondata.h"

class LimitTextEdit;
class LimitLineEdit;
class hexSpinBox;
class HeaderUI : public MetaUI
{
    Q_OBJECT
public:
    explicit HeaderUI(IJAttempter &attempter, QWidget *parent = 0);
    // MetaUI interface
    void setUIData(const _UIData &data);
    void *uiData() const;
    // 切换数据类型
    void changeDataType(int type);
    // 原始数据类型
    int originalType() const;
    // 启/停用界面控制按钮
    void enableOptionButton(bool enable);

signals:
    void confirm(bool &);
    void canceled();

protected slots:
    // 编辑框数据录入完成
    void slotEditFinished();
    // 编辑框文本变更
    void slotTextChanged(const QString& text);
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
    IJAttempter                 &q_attempter;

    ICDCommonData::smtCommon    q_data; // 界面数据
    ICDCommonData::smtCommon    q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    QLineEdit       *q_edtStatus; // 状态提示
    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    hexSpinBox      *q_spinDefault;
    LimitTextEdit   *q_edtRemak;
    QPushButton     *q_btnConfirm;
    QPushButton     *q_btnCancel;
};

#endif // HEADERUI_H

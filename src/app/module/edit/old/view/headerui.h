#ifndef HEADERUI_H
#define HEADERUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcommondata.h"

class LimitTextEdit;
class LimitLineEdit;
class hexSpinBox;
class HeaderUI : public MetaUI
{
    Q_OBJECT
public:
    explicit HeaderUI(QWidget *parent = nullptr);

    int uiType() const override;

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
    hexSpinBox      *q_spinDefault;
    LimitTextEdit   *q_edtRemak;
};

#endif // HEADERUI_H

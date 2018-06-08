#ifndef COMPLEXUI_H
#define COMPLEXUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcomplexdata.h"

class LimitTextEdit;
class LimitLineEdit;
// 复合数据界面
class ComplexUI : public MetaUI
{
    Q_OBJECT

public:
    ComplexUI(QWidget *parent = nullptr);

    int uiType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // 切换数据类型
    void changeDataType(int type) override;
    // 原始数据类型
    int originalType() const override;

signals:

private slots :
    // 数据录入完成
    void slotEditFinished();
    // 编辑文本变更
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
    ICDComplexData::smtComplex  q_data; // 界面数据
    ICDComplexData::smtComplex  q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;     // 名称
    QLineEdit       *q_edtCode;     // 编码
    QSpinBox        *q_spinLength;  // 长度
    LimitTextEdit   *q_edtDescribe; // 描述
};

#endif // COMPLEXUI_H

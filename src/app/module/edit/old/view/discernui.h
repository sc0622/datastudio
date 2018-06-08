#ifndef DISCERNUI_H
#define DISCERNUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcomplexdata.h"

class LimitTextEdit;
class LimitLineEdit;
// 帧数据
class DiscernUI : public MetaUI
{
    Q_OBJECT

public:
    explicit DiscernUI(QWidget *parent = nullptr);

    int uiType() const override;

    ~DiscernUI();
    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
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
    ICDComplexData::smtComplex  q_data; // 界面数据
    ICDComplexData::smtComplex  q_old;  // 原始数据

    QColor      q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
//    QSpinBox        *q_spinLength;      // 数据区长度
    QSpinBox        *q_spinSequence;    // 时序数
    LimitTextEdit   *q_edtRemak;
};

#endif // DISCERNUI_H

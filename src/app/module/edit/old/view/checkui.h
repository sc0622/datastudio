#ifndef CHECKUI_H
#define CHECKUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdcheckdata.h"

class LimitTextEdit;
class LimitLineEdit;
class CheckUI : public MetaUI
{
    Q_OBJECT
public:
    explicit CheckUI(QWidget *parent = nullptr);
    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void*uiData() const override;
    // 切换数据类型
    void changeDataType(int type) override;
    // 原始数据类型
    int originalType() const override;

signals:

protected slots:
    // 编辑框数据录入完成
    void slotEditFinished();
    // 编辑框文本变更
    void slotTextChanged(const QString& text);

protected:
    // 确认
    void confirm() override;
    // 取消
    void cancel() override;

    // 初始化界面数据
    void init();
    // 初始化校验类型下拉框
    void initBoxType();
    // 启/停用信号槽
    void enableConnection(bool enable);
    // 校验界面数据
    bool dataValid();

private:
    ICDCheckData::smtCheck      q_data; // 界面数据
    ICDCheckData::smtCheck      q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxCheckType;    // 校验类型
    QSpinBox        *q_spinStart;       // 校验起始位
    QSpinBox        *q_spinEnd;         // 校验终止位
    LimitTextEdit   *q_edtRemak;
};

#endif // CHECKUI_H

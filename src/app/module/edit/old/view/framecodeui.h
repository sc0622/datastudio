#ifndef FRAMECODEUI_H
#define FRAMECODEUI_H

#include <QtWidgets>

#include "metaui.h"
#include "KernelClass/icdframecodedata.h"

class LimitTextEdit;
class LimitLineEdit;
// 帧识别码
class FrameCodeUI : public MetaUI
{
    Q_OBJECT
public:
    explicit FrameCodeUI(QWidget *parent = nullptr);
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
    // 初始化帧码长度下拉框
    void initBoxLenght();
    // 初始化帧数据下拉框
    void initBoxData();
    // 启/停用信号槽
    void enableConnection(bool enable) override;
    // 校验界面数据
    bool dataValid();

private:
    ICDFrameCodeData::smtFrameCode  q_data; // 界面数据
    ICDFrameCodeData::smtFrameCode  q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    QComboBox       *q_boxLength;   // 帧码长度
    QComboBox       *q_boxData;     // 帧数据
    LimitTextEdit   *q_edtRemak;
};

#endif // FRAMECODEUI_H

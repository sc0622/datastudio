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
    void setUIData(const _UIData &data);
    void*uiData() const;
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
    // 初始化帧码长度下拉框
    void initBoxLenght();
    // 初始化帧数据下拉框
    void initBoxData();
    // 启/停用信号槽
    void enableConnection(bool enable);
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

    QLineEdit   *q_edtStatus;       // 状态提示
    QPushButton *q_btnConfirm;
    QPushButton *q_btnCancel;
};

#endif // FRAMECODEUI_H

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
    ~DiscernUI();
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

    QLineEdit   *q_edtStatus;       // 状态提示
    QPushButton *q_btnConfirm;
    QPushButton *q_btnCancel;
};

#endif // DISCERNUI_H

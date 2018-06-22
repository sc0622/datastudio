#ifndef BITVALUE_EDIT_H
#define BITVALUE_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdbitdata.h"

class QSpinBox;

class BitValueEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit BitValueEdit(QWidget* parent = nullptr);

    int windowType() const override;

    // MetaUI interface
    void setUIData(const _UIData &data) override;
    void* uiData() const override;
    // 切换数据类型
    void changeDataType(int type) override;
    // 原始数据类型
    int originalType() const override;

signals:

public slots:
    // 数据录入完成
    void slotEditFinished();
    // 编辑文本变更
    void slotTextChanged(const QString& text);
    // 范围限制状态变更
    void slotRangeStateChanged(bool enable);
    // 表点击
    void slotTableItemPressed(QStandardItem *item);
    // 新增
    void slotNew();
    // 删除
    void slotDelete();
    // 清除
    void slotClear();

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
    // 显示特征表右键菜单
    void showMenu();
    // 特征表菜单处理
    void dealAction(QAction *action);
    // 向特征值表插入一条数据
    void insertValueOne(int index, const _Eigenvalue &data);

private:
    ICDBitData::smtBit  q_data; // 界面数据
    ICDBitData::smtBit  q_old;  // 原始数据

    QColor          q_color;    // 默认背景色

    LimitLineEdit   *q_edtName;
    QLineEdit       *q_edtCode;
    LimitTextEdit   *q_edtRemak;

    QSpinBox    *q_spinStart;   // 起始位
    QSpinBox    *q_spinLength;  // 长度
    // 范围
    DoubleSpinbox *q_spinMin;
    QCheckBox   *q_checkLower;
    DoubleSpinbox *q_spinMax;
    QCheckBox   *q_checkUpper;
    // 默认值
    DoubleSpinbox *q_spinDefault;
    // 偏置
    DoubleSpinbox *q_spinOffset;
    // 单位
    LimitLineEdit   *q_edtUnit;
    // 比例尺
    DoubleSpinbox *q_spinLSB;

    JXmlTable   *q_table;       // 特征值表
    QAction     *q_actionNew;   // 右键菜单新增
    QAction     *q_actionClear; // 右键菜单清空
};

#endif // BITVALUE_EDIT_H

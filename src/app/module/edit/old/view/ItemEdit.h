#ifndef ITEM_EDIT_H
#define ITEM_EDIT_H

#include "ObjectEdit.h"
#include "KernelClass/icdcommondata.h"

class QComboBox;
class QCheckBox;
class QSpinBox;
class DoubleSpinBox;
class JXmlTable;

class ItemEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit ItemEdit(QWidget* parent = nullptr);
    explicit ItemEdit(int windowType, QWidget* parent = nullptr);
    virtual ~ItemEdit();

signals:

public slots:
    void onRangeStateChanged(bool enabled);
    void onTableItemPressed(QStandardItem *item);
    void onNew();
    void onDelete();
    void onClear();

    // ObjectEdit interface
protected:
    int windowType() const override;
    bool onEditFinished() override;
    bool onTextChanged(const QString &text) override;
    bool init() override;
    void enableConnect(bool enabled) override;
    bool confirm() override;
    bool validate();
    ICDCommonData *data() override;
    ICDCommonData *oldData() override;

private:
    void setupUI(int windowType);
    // 初始化数值类型下拉框
    void initBoxType();
    // 显示特征表右键菜单
    void showMenu();
    // 特征表菜单处理
    void dealAction(QAction *action);
    // 向特征值表插入一条数据
    void insertValueOne(int index, const _Eigenvalue &data);

private:
    QComboBox *comboNumericType_;
    DoubleSpinBox *spinMinimum_;
    QCheckBox *checkLeftInf_;
    DoubleSpinBox *spinMaximum_;
    QCheckBox *checkRightInf_;
    DoubleSpinBox *spinDefault_;
    DoubleSpinBox *spinOffset_;
    DoubleSpinBox *spinScale_;
    LimitLineEdit *editUnit_;
    JXmlTable *tableView_;
    QAction *actionNew_;
    QAction *actionClear_;
};

#endif // ITEM_EDIT_H

#ifndef BITVALUE_EDIT_H
#define BITVALUE_EDIT_H

#include "ItemEdit.h"
#include "KernelClass/icdbitdata.h"

class QSpinBox;
class QStandardItem;

class BitValueEdit : public ObjectEdit
{
    Q_OBJECT
public:
    explicit BitValueEdit(QWidget* parent = nullptr);

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
    bool validate();
    ICDBitData *data() override;
    ICDBitData *oldData() override;

private:
    void showMenu();
    void dealAction(QAction *action);
    void insertValueOne(int index, const _Eigenvalue &data);

private:
    QSpinBox *spinBitStart_;
    QSpinBox *spinBitCount_;
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
    QAction *actionClear;
};

#endif // BITVALUE_EDIT_H

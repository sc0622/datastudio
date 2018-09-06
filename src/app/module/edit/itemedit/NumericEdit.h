#ifndef NUMERIC_EDIT_H
#define NUMERIC_EDIT_H

#include "ItemEdit.h"

namespace Icd {
class NumericItem;
typedef std::shared_ptr<NumericItem> NumericItemPtr;
}

class JDoubleSpinBox;
class QCheckBox;

namespace Edit {

class SpecsTable;

// class NumericEdit

class NumericEdit : public ItemEdit
{
    Q_OBJECT
public:
    explicit NumericEdit(const Icd::NumericItemPtr &numeric, QWidget *parent = nullptr);
    ~NumericEdit();

    Icd::NumericItemPtr numeric() const;

    bool init() override;

signals:

public slots:

private:
    JDoubleSpinBox *spinOffset_;
    JDoubleSpinBox *spinScale_;
    QLineEdit *editUnit_;
    JDoubleSpinBox *spinMinimum_;
    QCheckBox *checkMinimumInf_;
    JDoubleSpinBox *spinMaximum_;
    QCheckBox *checkMaximumInf_;
    JDoubleSpinBox *spinDefaultValue_;
    SpecsTable *tableSpecs_;
};

}

#endif // NUMERIC_EDIT_H

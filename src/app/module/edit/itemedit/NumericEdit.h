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

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() override;
    void saveContent() override;

private:
    QComboBox *comboNumericType_;
    JDoubleSpinBox *spinOffset_;
    JDoubleSpinBox *spinScale_;
    QLineEdit *editUnit_;
    JDoubleSpinBox *spinMinimum_;
    QCheckBox *checkMinimum_;
    JDoubleSpinBox *spinMaximum_;
    QCheckBox *checkMaximum_;
    JDoubleSpinBox *spinDefaultValue_;
    SpecsTable *tableSpecs_;
};

}

#endif // NUMERIC_EDIT_H

#ifndef BITVALUE_EDIT_H
#define BITVALUE_EDIT_H

#include "BitEdit.h"

class JDoubleSpinBox;
class QCheckBox;

namespace Edit {

// class BitValueEdit

class BitValueEdit : public BitEdit
{
    Q_OBJECT
public:
    explicit BitValueEdit(const Icd::BitItemPtr &bit, QWidget *parent = nullptr);
    ~BitValueEdit();

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
};

}

#endif // BITVALUE_EDIT_H

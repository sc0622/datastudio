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

protected:
    void restoreContent(bool recursive = true) override;
    bool validate() override;
    void saveContent() override;

private:
    JDoubleSpinBox *spinOffset_;
    JDoubleSpinBox *spinScale_;
    QLineEdit *editUnit_;
    JDoubleSpinBox *spinMinimum_;
    QCheckBox *checkMinimum_;
    JDoubleSpinBox *spinMaximum_;
    QCheckBox *checkMaximum_;
    JDoubleSpinBox *spinDefaultValue_;
};

}

#endif // BITVALUE_EDIT_H

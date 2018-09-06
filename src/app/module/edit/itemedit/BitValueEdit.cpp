#include "precomp.h"
#include "BitValueEdit.h"

namespace Edit {

// class BitValueEdit

BitValueEdit::BitValueEdit(const Icd::BitItemPtr &bit, QWidget *parent)
    : BitEdit(bit, parent)
{
    spinOffset_ = new JDoubleSpinBox(this);
    addRow(tr("Offset:"), spinOffset_);

    spinScale_ = new JDoubleSpinBox(this);
    addRow(tr("Scale:"), spinScale_);

    editUnit_ = new QLineEdit(this);
    editUnit_->setMaxLength(30);
    addRow(tr("Unit:"), editUnit_);

    QHBoxLayout *layoutMinimum = new QHBoxLayout();
    spinMinimum_ = new JDoubleSpinBox(this);
    layoutMinimum->addWidget(spinMinimum_);
    checkMinimumInf_ = new QCheckBox(this);
    checkMinimumInf_->setMaximumWidth(checkMinimumInf_->sizeHint().height());
    layoutMinimum->addWidget(checkMinimumInf_);
    addRow(tr("Minimum:"), layoutMinimum);

    QHBoxLayout *layoutMaximum = new QHBoxLayout();
    spinMaximum_ = new JDoubleSpinBox(this);
    layoutMaximum->addWidget(spinMaximum_);
    checkMaximumInf_ = new QCheckBox(this);
    checkMaximumInf_->setMaximumWidth(checkMaximumInf_->sizeHint().height());
    layoutMaximum->addWidget(checkMaximumInf_);
    addRow(tr("Maximum:"), layoutMaximum);

    spinDefaultValue_ = new JDoubleSpinBox(this);
    addRow(tr("Default value:"), spinDefaultValue_);

    // init

    //
}

BitValueEdit::~BitValueEdit()
{

}

bool BitValueEdit::init()
{
    if (!BitEdit::init()) {
        return false;
    }

    lock();

    const Icd::BitItemPtr bit = this->bit();
    if (bit) {
        // offset
        spinOffset_->setValue(bit->offset());
        // scale
        spinScale_->setValue(bit->scale());
        // unit
        editUnit_->setText(QString::fromStdString(bit->unit()));
        // minimum
        spinMinimum_->setValue(bit->limit()->minimum());
        // mnimum-inf
        checkMinimumInf_->setChecked(!bit->limit()->minimumInf());
        // maximum
        spinMaximum_->setValue(bit->limit()->maximum());
        // maximum-inf
        checkMaximumInf_->setChecked(!bit->limit()->maximumInf());
        // default
        spinDefaultValue_->setValue(bit->defaultValue());
    }

    unlock();

    return true;
}

}

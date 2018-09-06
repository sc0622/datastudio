#include "precomp.h"
#include "NumericEdit.h"
#include "../details/SpecsTable.h"

namespace Edit {

// class NumericEdit

NumericEdit::NumericEdit(const Icd::NumericItemPtr &numeric, QWidget *parent)
    : ItemEdit(numeric, parent)
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

    tableSpecs_ = new SpecsTable(numeric, this);
    addWidget(tableSpecs_);

    // init

    //
}

NumericEdit::~NumericEdit()
{

}

Icd::NumericItemPtr NumericEdit::numeric() const
{
    return JHandlePtrCast<Icd::NumericItem>(item());
}

bool NumericEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    lock();

    const Icd::NumericItemPtr numeric = this->numeric();
    if (numeric) {
        // offset
        spinOffset_->setValue(numeric->offset());
        // scale
        spinScale_->setValue(numeric->scale());
        // unit
        editUnit_->setText(QString::fromStdString(numeric->unit()));
        // minimum
        spinMinimum_->setValue(numeric->limit()->minimum());
        // mnimum-inf
        checkMinimumInf_->setChecked(!numeric->limit()->minimumInf());
        // maximum
        spinMaximum_->setValue(numeric->limit()->maximum());
        // maximum-inf
        checkMaximumInf_->setChecked(!numeric->limit()->maximumInf());
        // default value
        spinDefaultValue_->setValue(numeric->defaultValue());
        // specs
        if (!tableSpecs_->init()) {
            unlock();
            return false;
        }
    }

    unlock();

    return true;
}

}

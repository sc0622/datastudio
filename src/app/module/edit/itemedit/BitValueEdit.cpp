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
    spinMinimum_->setReadOnly(true);
    layoutMinimum->addWidget(spinMinimum_);
    checkMinimumInf_ = new QCheckBox(this);
    checkMinimumInf_->setMaximumWidth(checkMinimumInf_->sizeHint().height());
    layoutMinimum->addWidget(checkMinimumInf_);
    layoutMinimum->addSpacing(3);
    addRow(tr("Minimum:"), layoutMinimum);

    QHBoxLayout *layoutMaximum = new QHBoxLayout();
    spinMaximum_ = new JDoubleSpinBox(this);
    spinMaximum_->setReadOnly(true);
    layoutMaximum->addWidget(spinMaximum_);
    checkMaximumInf_ = new QCheckBox(this);
    checkMaximumInf_->setMaximumWidth(checkMaximumInf_->sizeHint().height());
    layoutMaximum->addWidget(checkMaximumInf_);
    layoutMaximum->addSpacing(3);
    addRow(tr("Maximum:"), layoutMaximum);

    spinDefaultValue_ = new JDoubleSpinBox(this);
    addRow(tr("Default value:"), spinDefaultValue_);

    connect(spinOffset_, static_cast<void(JDoubleSpinBox::*)(double)>(&JDoubleSpinBox::valueChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinScale_, static_cast<void(JDoubleSpinBox::*)(double)>(&JDoubleSpinBox::valueChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(editUnit_, &QLineEdit::textChanged, this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinMinimum_, static_cast<void(JDoubleSpinBox::*)(double)>(&JDoubleSpinBox::valueChanged),
            this, [=](double value){
        if (checkMaximumInf_->isChecked()) {
            const double maximum = spinMaximum_->value();
            if (value > maximum) {
                spinMaximum_->setValue(value);
            }
        }
        if (spinDefaultValue_->value() < value) {
            spinDefaultValue_->setValue(value);
        }
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(checkMinimumInf_, &QCheckBox::toggled, this, [=](bool checked){
        spinMinimum_->setReadOnly(!checked);
        if (checked) {
            spinMinimum_->setValue(qMin(spinMaximum_->value(), spinDefaultValue_->value()));
        } else {
            spinMinimum_->clear();
        }
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinMaximum_, static_cast<void(JDoubleSpinBox::*)(double)>(&JDoubleSpinBox::valueChanged),
            this, [=](double value){
        if (checkMinimumInf_->isChecked()) {
            const double minimum = spinMinimum_->value();
            if (value < minimum) {
                spinMinimum_->setValue(value);
            }
        }
        if (spinDefaultValue_->value() > value) {
            spinDefaultValue_->setValue(value);
        }
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(checkMaximumInf_, &QCheckBox::toggled, this, [=](bool checked){
        spinMaximum_->setReadOnly(!checked);
        if (checked) {
            spinMaximum_->setValue(qMax(spinMinimum_->value(), spinDefaultValue_->value()));
        } else {
            spinMaximum_->clear();
        }
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinDefaultValue_, static_cast<void(JDoubleSpinBox::*)(double)>(&JDoubleSpinBox::valueChanged),
            this, [=](double value){
        if (checkMinimumInf_->isChecked()) {
            if (value < spinMinimum_->value()) {
                spinMinimum_->setValue(value);
            }
        }
        if (checkMaximumInf_->isChecked()) {
            if (value > spinMaximum_->value()) {
                spinMaximum_->setValue(value);
            }
        }
        if (!blocking()) {
            emit contentChanged();
        }
    });
}

BitValueEdit::~BitValueEdit()
{

}

bool BitValueEdit::init()
{
    if (!BitEdit::init()) {
        return false;
    }

    BitValueEdit::restoreContent(false);

    return true;
}

void BitValueEdit::restoreContent(bool recursive)
{
    if (recursive) {
        BitEdit::restoreContent(recursive);
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
        // mnimum-inf
        checkMinimumInf_->setChecked(!bit->limit()->minimumInf());
        // minimum
        if (checkMinimumInf_->isChecked()) {
            spinMinimum_->setValue(bit->limit()->minimum());
        } else {
            spinMinimum_->clear();
        }
        // maximum-inf
        checkMaximumInf_->setChecked(!bit->limit()->maximumInf());
        // maximum
        if (checkMaximumInf_->isChecked()) {
            spinMaximum_->setValue(bit->limit()->maximum());
        } else {
            spinMaximum_->clear();
        }
        // default
        spinDefaultValue_->setValue(bit->defaultValue());
    }

    unlock();
}

bool BitValueEdit::validate()
{
    if (!BitEdit::validate()) {
        return false;
    }

    //TODO

    return true;
}

void BitValueEdit::saveContent()
{
    BitEdit::saveContent();

    const Icd::BitItemPtr bit = this->bit();
    if (!bit) {
        return;
    }

    // offset
    bit->setOffset(spinOffset_->value());
    // scale
    bit->setScale(spinScale_->value());
    // unit
    bit->setUnit(editUnit_->text().trimmed().toStdString());
    // minimumInf
    bit->limit()->setMinimumInf(checkMinimumInf_->isChecked());
    // minimum
    if (checkMinimumInf_->isChecked()) {
        bit->limit()->setMinimum(spinMinimum_->value());
    }
    // maximumInf
    bit->limit()->setMaximumInf(checkMaximumInf_->isChecked());
    // maximum
    if (checkMaximumInf_->isChecked()) {
        bit->limit()->setMaximum(spinMaximum_->value());
    }
    // default value
    bit->setDefaultValue(spinDefaultValue_->value());
}

}

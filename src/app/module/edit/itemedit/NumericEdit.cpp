#include "precomp.h"
#include "NumericEdit.h"
#include "../details/SpecsTable.h"

namespace Edit {

// class NumericEdit

NumericEdit::NumericEdit(const Icd::NumericItemPtr &numeric, QWidget *parent)
    : ItemEdit(numeric, parent)
{
    comboNumericType_ = new QComboBox(this);
    addRow("<font color=red>*</font>" + tr("Numeric type:"), comboNumericType_);

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
    checkMinimum_ = new QCheckBox(this);
    checkMinimum_->setMaximumWidth(checkMinimum_->sizeHint().height());
    layoutMinimum->addWidget(checkMinimum_);
    layoutMinimum->addSpacing(3);
    addRow(tr("Minimum:"), layoutMinimum);

    QHBoxLayout *layoutMaximum = new QHBoxLayout();
    spinMaximum_ = new JDoubleSpinBox(this);
    spinMaximum_->setReadOnly(true);
    layoutMaximum->addWidget(spinMaximum_);
    checkMaximum_ = new QCheckBox(this);
    checkMaximum_->setMaximumWidth(checkMaximum_->sizeHint().height());
    layoutMaximum->addWidget(checkMaximum_);
    layoutMaximum->addSpacing(3);
    addRow(tr("Maximum:"), layoutMaximum);

    spinDefaultValue_ = new JDoubleSpinBox(this);
    addRow(tr("Default value:"), spinDefaultValue_);

    tableSpecs_ = new SpecsTable(numeric, this);
    addWidget(tableSpecs_);

    // numeric-type
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericI8), Icd::NumericI8);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericU8), Icd::NumericU8);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericI16), Icd::NumericI16);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericU16), Icd::NumericU16);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericI32), Icd::NumericI32);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericU32), Icd::NumericU32);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericI64), Icd::NumericI64);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericU64), Icd::NumericU64);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericF32), Icd::NumericF32);
    comboNumericType_->addItem(IcdWidget::arrayTypeString(Icd::NumericF64), Icd::NumericF64);

    connect(comboNumericType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
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
        if (checkMaximum_->isChecked()) {
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
    connect(checkMinimum_, &QCheckBox::toggled, this, [=](bool checked){
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
        if (checkMinimum_->isChecked()) {
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
    connect(checkMaximum_, &QCheckBox::toggled, this, [=](bool checked){
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
        if (checkMinimum_->isChecked()) {
            if (value < spinMinimum_->value()) {
                spinMinimum_->setValue(value);
            }
        }
        if (checkMaximum_->isChecked()) {
            if (value > spinMaximum_->value()) {
                spinMaximum_->setValue(value);
            }
        }
        if (!blocking()) {
            emit contentChanged();
        }
    });
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

    //
    if (!tableSpecs_->init()) {
        return false;
    }

    //
    NumericEdit::restoreContent(false);

    return true;
}

void NumericEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::NumericItemPtr numeric = this->numeric();
    if (numeric) {
        // numeric-type
        comboNumericType_->setCurrentIndex(comboNumericType_->findData(numeric->numericType()));
        // offset
        spinOffset_->setValue(numeric->offset());
        // scale
        spinScale_->setValue(numeric->scale());
        // unit
        editUnit_->setText(QString::fromStdString(numeric->unit()));
        // mnimum-inf
        checkMinimum_->setChecked(!numeric->limit()->minimumInf());
        // minimum
        if (checkMinimum_->isChecked()) {
            spinMinimum_->setValue(numeric->limit()->minimum());
        } else {
            spinMinimum_->clear();
        }
        // maximum-inf
        checkMaximum_->setChecked(!numeric->limit()->maximumInf());
        // maximum
        if (checkMaximum_->isChecked()) {
            spinMaximum_->setValue(numeric->limit()->maximum());
        } else {
            spinMaximum_->clear();
        }
        // default value
        spinDefaultValue_->setValue(numeric->defaultValue());
        // specs
        tableSpecs_->restoreContent();
    }

    unlock();
}

bool NumericEdit::validate() const
{
    if (!ItemEdit::validate()) {
        return false;
    }

    if (!tableSpecs_->validate()) {
        return false;
    }

    // numeric-type
    if (comboNumericType_->currentIndex() == -1) {
        comboNumericType_->setFocus();
        return false;
    }

    return true;
}

void NumericEdit::saveContent()
{
    ItemEdit::saveContent();

    tableSpecs_->saveContent();

    const Icd::NumericItemPtr numeric = this->numeric();
    if (!numeric) {
        return;
    }

    // numeric-type
    numeric->setNumericType(Icd::NumericType(comboNumericType_->currentData().toInt()));
    // offset
    numeric->setOffset(spinOffset_->value());
    // scale
    numeric->setScale(spinScale_->value());
    // unit
    numeric->setUnit(editUnit_->text().trimmed().toStdString());
    // minimumInf
    numeric->limit()->setMinimumInf(!checkMinimum_->isChecked());
    // minimum
    if (checkMinimum_->isChecked()) {
        numeric->limit()->setMinimum(spinMinimum_->value());
    }
    // maximumInf
    numeric->limit()->setMaximumInf(!checkMaximum_->isChecked());
    // maximum
    if (checkMaximum_->isChecked()) {
        numeric->limit()->setMaximum(spinMaximum_->value());
    }
    // default value
    numeric->setDefaultValue(spinDefaultValue_->value());
    // specs
    tableSpecs_->saveContent();
}

}

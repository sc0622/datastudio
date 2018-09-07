#include "precomp.h"
#include "ArrayEdit.h"

namespace Edit {

// class ArrayEdit

ArrayEdit::ArrayEdit(const Icd::ArrayItemPtr &array, QWidget *parent)
    : ItemEdit(array, parent)
{
    comboArrayType_ = new QComboBox(this);
    addRow("<font color=red>*</font>" + tr("Array type:"), comboArrayType_);

    spinCount_ = new QSpinBox(this);
    spinCount_->setRange(1, INT_MAX);
    addRow(tr("Count:"), spinCount_);

    // countertypes
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayI8), Icd::ArrayI8);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayU8), Icd::ArrayU8);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayI16), Icd::ArrayI16);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayU16), Icd::ArrayU16);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayI32), Icd::ArrayI32);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayU32), Icd::ArrayU32);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayI64), Icd::ArrayI64);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayU64), Icd::ArrayU64);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayF32), Icd::ArrayF32);
    comboArrayType_->addItem(IcdWidget::arrayTypeString(Icd::ArrayF64), Icd::ArrayF64);

    connect(comboArrayType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinCount_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
}

ArrayEdit::~ArrayEdit()
{

}

Icd::ArrayItemPtr ArrayEdit::array() const
{
    return JHandlePtrCast<Icd::ArrayItem>(item());
}

bool ArrayEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    //
    ArrayEdit::restoreContent(false);

    return true;
}

void ArrayEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::ArrayItemPtr array = this->array();
    if (array) {
        // array-type
        comboArrayType_->setCurrentIndex(comboArrayType_->findData(array->arrayType()));
        // count
        spinCount_->setValue(array->count());
    }

    unlock();
}

bool ArrayEdit::validate()
{
    if (!ItemEdit::validate()) {
        return false;
    }

    // array-type
    if (comboArrayType_->currentIndex() == -1) {
        comboArrayType_->setFocus();
        return false;
    }

    return true;
}

void ArrayEdit::saveContent()
{
    ItemEdit::saveContent();

    const Icd::ArrayItemPtr array = this->array();
    if (!array) {
        return;
    }

    // array-type
    array->setArrayType(Icd::ArrayType(comboArrayType_->currentData().toInt()));
    // count
    array->setCount(spinCount_->value());
}

}

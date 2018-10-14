#include "precomp.h"
#include "BitEdit.h"
#include "BitMapEdit.h"
#include "BitValueEdit.h"
#include "../details/SpecsTable.h"

namespace Edit {

// class BitEdit

BitEdit::BitEdit(const Icd::BitItemPtr &bit, QWidget *parent)
    : ItemEdit(bit, parent)
{
    spinBitStart_ = new QSpinBox(this);
    spinBitStart_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    spinBitStart_->setRange(0, 63);
    addRow("<font color=red>*</font>" + tr("Start of bit:"), spinBitStart_);

    spinBitCount_ = new QSpinBox(this);
    spinBitCount_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    spinBitCount_->setRange(1, 64);
    addRow("<font color=red>*</font>" + tr("Count of bit:"), spinBitCount_);

    tableSpecs_ = new SpecsTable(bit, this);
    addWidget(tableSpecs_);

    connect(spinBitStart_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](int value){
        if (value + spinBitCount_->value() > 63) {
            spinBitCount_->setValue(64 - value);
        }
        updateTableSpecs();
        emit bitStartChanged(value);
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinBitCount_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](int value){
        if (spinBitStart_->value() + value > 63) {
            spinBitStart_->setValue(64 - value);
        }
        updateTableSpecs();
        emit bitCountChanged(value);
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(tableSpecs_, &SpecsTable::contentChanged, this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
}

BitEdit::~BitEdit()
{

}

Icd::BitItemPtr BitEdit::bit() const
{
    return JHandlePtrCast<Icd::BitItem>(item());
}

BitEdit *BitEdit::create(const Icd::BitItemPtr &bit)
{
    if (!bit) {
        return nullptr;
    }

    switch (bit->type()) {
    case Icd::ItemBitMap: return new BitMapEdit(bit);
    case Icd::ItemBitValue: return new BitValueEdit(bit);
    default:
        break;
    }

    return nullptr;
}

bool BitEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    if (!tableSpecs_->init()) {
        return false;
    }

    BitEdit::restoreContent(false);

    return true;
}

void BitEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::BitItemPtr bit = this->bit();
    if (bit) {
        // bit-start
        spinBitStart_->setValue(bit->bitStart());
        // bit-count
        spinBitCount_->setValue(bit->bitCount());
        // specs
        tableSpecs_->restoreContent();
    }

    unlock();
}

bool BitEdit::validate() const
{
    if (!ItemEdit::validate()) {
        return false;
    }

    if (!tableSpecs_->validate()) {
        return false;
    }

    return true;
}

void BitEdit::saveContent()
{
    ItemEdit::saveContent();

    const Icd::BitItemPtr bit = this->bit();
    if (!bit) {
        return;
    }

    // bit-start
    bit->setBitStart(spinBitStart_->value());
    // bit-count
    bit->setBitCount(spinBitCount_->value());
    // specs
    tableSpecs_->saveContent();
}

int BitEdit::bitStart() const
{
    return spinBitStart_->value();
}

int BitEdit::bitCount() const
{
    return spinBitCount_->value();
}

void BitEdit::updateTableSpecs()
{
    const Icd::BitItemPtr bit = this->bit();
    if (!bit) {
        return;
    }

    if (bit->type() != Icd::ItemBitMap) {
        return;
    }

    tableSpecs_->updateContent(spinBitStart_->value(), spinBitCount_->value());
}

}

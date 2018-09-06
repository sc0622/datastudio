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
    spinBitStart_->setRange(0, 127);
    addRow("<font color=red>*</font>" + tr("Start of bit:"), spinBitStart_);

    spinBitCount_ = new QSpinBox(this);
    spinBitCount_->setRange(0, 128);
    addRow("<font color=red>*</font>" + tr("Count of bit:"), spinBitCount_);

    tableSpecs_ = new SpecsTable(bit, this);
    addWidget(tableSpecs_);

    // init

    //
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

    lock();

    const Icd::BitItemPtr bit = this->bit();
    if (bit) {
        // bit-start
        spinBitStart_->setValue(bit->bitStart());
        // bit-count
        spinBitCount_->setValue(bit->bitCount());
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

#include "precomp.h"
#include "BitMapEdit.h"

namespace Edit {

// class BitMapEdit

BitMapEdit::BitMapEdit(const Icd::BitItemPtr &bit, QWidget *parent)
    : BitEdit(bit, parent)
{
    spinDefaultValue_ = new JLargeSpinBox(this);
    spinDefaultValue_->setPrefix("0x");
    spinDefaultValue_->setRadix(16);
    spinDefaultValue_->setFillChar(QChar('0'));
    addRow(tr("Default value:"), spinDefaultValue_);
}

BitMapEdit::~BitMapEdit()
{

}

bool BitMapEdit::init()
{
    if (!BitEdit::init()) {
        return false;
    }

    lock();

    const Icd::BitItemPtr bit = this->bit();
    if (bit) {
        // default value
        spinDefaultValue_->setValue(qulonglong(bit->defaultValue()));
    }

    unlock();

    return true;
}

}

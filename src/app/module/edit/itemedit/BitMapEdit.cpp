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

    spinDefaultValue_->setRange(0, (1ULL << bit->bitCount()) - 1);

    connect(this, &BitMapEdit::bitStartChanged, this, [=](){
        spinDefaultValue_->setRange(0, (1ULL << bitCount()) - 1);
    });
    connect(this, &BitMapEdit::bitCountChanged, this, [=](int value){
        spinDefaultValue_->setRange(0, (1ULL << value) - 1);
    });
    connect(spinDefaultValue_, &JLargeSpinBox::valueChanged, this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
}

BitMapEdit::~BitMapEdit()
{

}

bool BitMapEdit::init()
{
    if (!BitEdit::init()) {
        return false;
    }

    BitMapEdit::restoreContent(false);

    return true;
}

void BitMapEdit::restoreContent(bool recursive)
{
    if (recursive) {
        BitEdit::restoreContent(recursive);
    }

    lock();

    const Icd::BitItemPtr bit = this->bit();
    if (bit) {
        // default value
        spinDefaultValue_->setValue(qulonglong(bit->defaultValue()));
    }

    unlock();
}

bool BitMapEdit::validate() const
{
    if (!BitEdit::validate()) {
        return false;
    }

    return true;
}

void BitMapEdit::saveContent()
{
    BitEdit::saveContent();

    const Icd::BitItemPtr bit = this->bit();
    if (!bit) {
        return;
    }

    // default value
    bit->setDefaultValue(spinDefaultValue_->value());
}

}

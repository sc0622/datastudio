#include "precomp.h"
#include "BitEdit.h"
#include "BitMapEdit.h"
#include "BitValueEdit.h"

namespace Edit {

// class BitEdit

BitEdit::BitEdit(const Icd::BitItemPtr &bit, QWidget *parent)
    : ItemEdit(bit, parent)
{

}

BitEdit::~BitEdit()
{

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

Icd::BitItemPtr BitEdit::bit() const
{
    return JHandlePtrCast<Icd::BitItem>(item());
}

}

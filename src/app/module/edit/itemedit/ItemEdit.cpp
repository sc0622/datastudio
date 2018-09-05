#include "precomp.h"
#include "ItemEdit.h"
#include "HeaderEdit.h"
#include "CounterEdit.h"
#include "CheckEdit.h"
#include "FrameCodeEdit.h"
#include "NumericEdit.h"
#include "ArrayEdit.h"
#include "BitEdit.h"
#include "ComplexEdit.h"
#include "FrameEdit.h"

namespace Edit {

// class ItemEdit

ItemEdit::ItemEdit(const Icd::ItemPtr &item, QWidget *parent)
    : ObjectEdit(item, parent)
{

}

ItemEdit::~ItemEdit()
{

}

ItemEdit *ItemEdit::create(const Icd::ItemPtr &item)
{
    if (!item) {
        return nullptr;
    }

    switch (item->type()) {
    case Icd::ItemHead:
        return new HeaderEdit(JHandlePtrCast<Icd::HeaderItem>(item));
    case Icd::ItemCounter:
        return new CounterEdit(JHandlePtrCast<Icd::CounterItem>(item));
    case Icd::ItemCheck:
        return new CheckEdit(JHandlePtrCast<Icd::CheckItem>(item));
    case Icd::ItemFrameCode:
        return new FrameCodeEdit(JHandlePtrCast<Icd::FrameCodeItem>(item));
    case Icd::ItemNumeric:
        return new NumericEdit(JHandlePtrCast<Icd::NumericItem>(item));
    case Icd::ItemArray:
        return new ArrayEdit(JHandlePtrCast<Icd::ArrayItem>(item));
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
        return BitEdit::create(JHandlePtrCast<Icd::BitItem>(item));
    case Icd::ItemComplex:
        return new ComplexEdit(JHandlePtrCast<Icd::ComplexItem>(item));
    case Icd::ItemFrame:
        return new FrameEdit(JHandlePtrCast<Icd::FrameItem>(item));
    default:
        break;
    }

    return nullptr;
}

Icd::ItemPtr ItemEdit::item() const
{
    return JHandlePtrCast<Icd::Item>(object());
}

}

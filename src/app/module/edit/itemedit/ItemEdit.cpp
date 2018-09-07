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
    comboType_ = new QComboBox(this);
    insertRow(0, "<font color=red>*</font>" + tr("Type:"), comboType_);

    // types
    const auto mapTypes = IcdWidget::protoItemMapTypes();
    for (auto citer = mapTypes.constBegin(); citer != mapTypes.constEnd(); ++citer) {
        comboType_->addItem(citer.value(), citer.key());
    }

    connect(comboType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        if (!blocking()) {
            emit itemTypeChanged(comboType_->currentData().toInt());
        }
    });
}

ItemEdit::~ItemEdit()
{

}

Icd::ItemPtr ItemEdit::item() const
{
    return JHandlePtrCast<Icd::Item>(object());
}

int ItemEdit::itemType() const
{
    const Icd::ItemPtr item = this->item();
    if (item) {
        return item->type();
    } else {
        return ObjectEdit::itemType();
    }
}

ItemEdit *ItemEdit::create(const Icd::ItemPtr &item)
{
    if (!item) {
        return nullptr;
    }

    switch (item->type()) {
    case Icd::ItemHeader:
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

bool ItemEdit::init()
{
    if (!ObjectEdit::init()) {
        return false;
    }

    ItemEdit::restoreContent(false);

    return true;
}

void ItemEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ObjectEdit::restoreContent(recursive);
    }

    lock();

    const Icd::ItemPtr item = this->item();
    if (item) {
        // type
        comboType_->setCurrentIndex(comboType_->findData(item->type()));
    }

    unlock();
}

bool ItemEdit::validate()
{
    if (!ObjectEdit::validate()) {
        return false;
    }

    //TODO

    return true;
}

void ItemEdit::saveContent()
{
    ObjectEdit::saveContent();

    //TODO
}

}

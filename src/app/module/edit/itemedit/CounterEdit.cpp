#include "precomp.h"
#include "CounterEdit.h"

namespace Edit {

// class CounterEdit

CounterEdit::CounterEdit(const Icd::CounterItemPtr &counter, QWidget *parent)
    : ItemEdit(counter, parent)
{
    comboCounterType_ = new QComboBox(this);
    addRow("<font color=red>*</font>" + tr("Counter type:"), comboCounterType_);

    connect(comboCounterType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        //
    });

    // init

    // countertypes
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU8), Icd::CounterU8);
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU16), Icd::CounterU16);
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU32), Icd::CounterU32);
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU64), Icd::CounterU64);
}

CounterEdit::~CounterEdit()
{

}

Icd::CounterItemPtr CounterEdit::counter() const
{
    return JHandlePtrCast<Icd::CounterItem>(item());
}

bool CounterEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    lock();

    const Icd::CounterItemPtr counter = this->counter();
    if (counter) {
        // countertype
        comboCounterType_->setCurrentIndex(comboCounterType_->findData(counter->counterType()));
    }

    unlock();

    return true;
}

}

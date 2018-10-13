#include "precomp.h"
#include "CounterEdit.h"

namespace Edit {

// class CounterEdit

CounterEdit::CounterEdit(const Icd::CounterItemPtr &counter, QWidget *parent)
    : ItemEdit(counter, parent)
{
    comboCounterType_ = new QComboBox(this);
    addRow("<font color=red>*</font>" + tr("Counter type:"), comboCounterType_);

    // countertypes
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU8), Icd::CounterU8);
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU16), Icd::CounterU16);
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU32), Icd::CounterU32);
    comboCounterType_->addItem(IcdWidget::counterTypeString(Icd::CounterU64), Icd::CounterU64);

    connect(comboCounterType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
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

    //
    CounterEdit::restoreContent(false);

    return true;
}

void CounterEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::CounterItemPtr counter = this->counter();
    if (counter) {
        // countertype
        comboCounterType_->setCurrentIndex(comboCounterType_->findData(counter->counterType()));
    }

    unlock();
}

bool CounterEdit::validate() const
{
    if (!ItemEdit::validate()) {
        return false;
    }

    //

    return true;
}

void CounterEdit::saveContent()
{
    ItemEdit::saveContent();

    const Icd::CounterItemPtr counter = this->counter();
    if (!counter) {
        return;
    }

    // counter-type
    counter->setCounterType(Icd::CounterType(comboCounterType_->currentData().toInt()));
}

}

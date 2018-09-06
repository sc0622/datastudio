#include "precomp.h"
#include "HeaderEdit.h"

namespace Edit {

// class HeaderEdit

HeaderEdit::HeaderEdit(const Icd::HeaderItemPtr &header, QWidget *parent)
    : ItemEdit(header, parent)
{
    spinDefault_ = new JSpinBox(this);
    spinDefault_->setPrefix("0x");
    spinDefault_->setRadix(16);
    spinDefault_->setFillChar(QChar('0'));
    spinDefault_->setRange(0, 0xFF);
    addRow(tr("Default value:"), spinDefault_);

    connect(spinDefault_, static_cast<void(JSpinBox::*)(int)>(&JSpinBox::valueChanged),
            this, [=](int){
        //
    });
}

HeaderEdit::~HeaderEdit()
{

}

Icd::HeaderItemPtr HeaderEdit::header() const
{
    return JHandlePtrCast<Icd::HeaderItem>(item());
}

bool HeaderEdit::init()
{
    if (!ObjectEdit::init()) {
        return false;
    }

    lock();

    const Icd::HeaderItemPtr header = this->header();
    if (header) {
        // default value
        spinDefault_->setValue(int(header->defaultValue()));
    }

    unlock();

    return true;
}

}

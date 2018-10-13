#include "precomp.h"
#include "HeaderEdit.h"

namespace Edit {

// class HeaderEdit

HeaderEdit::HeaderEdit(const Icd::HeaderItemPtr &header, QWidget *parent)
    : ItemEdit(header, parent)
{
    spinDefaultValue_ = new JSpinBox(this);
    spinDefaultValue_->setPrefix("0x");
    spinDefaultValue_->setRadix(16);
    spinDefaultValue_->setFillChar(QChar('0'));
    spinDefaultValue_->setRange(0, 0xFF);
    addRow(tr("Default value:"), spinDefaultValue_);

    connect(spinDefaultValue_, static_cast<void(JSpinBox::*)(int)>(&JSpinBox::valueChanged),
            this, [=](int){
        if (!blocking()) {
            emit contentChanged();
        }
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
    if (!ItemEdit::init()) {
        return false;
    }

    HeaderEdit::restoreContent(false);

    return true;
}

void HeaderEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::HeaderItemPtr header = this->header();
    if (header) {
        // default value
        spinDefaultValue_->setValue(int(header->defaultValue()));
    }

    unlock();
}

bool HeaderEdit::validate() const
{
    if (!ItemEdit::validate()) {
        return false;
    }

    //

    return true;
}

void HeaderEdit::saveContent()
{
    ItemEdit::saveContent();

    const Icd::HeaderItemPtr header = this->header();
    if (!header) {
        return;
    }

    // default type
    header->setDefaultValue(spinDefaultValue_->value());
}

}

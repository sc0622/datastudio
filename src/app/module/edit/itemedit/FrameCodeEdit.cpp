#include "precomp.h"
#include "FrameCodeEdit.h"

namespace Edit {

// class FrameCodeEdit

FrameCodeEdit::FrameCodeEdit(const Icd::FrameCodeItemPtr &frameCode, QWidget *parent)
    : ItemEdit(frameCode, parent)
{
    comboFrameCodeType_ = new QComboBox(this);
    addRow("<font color=red>*</font>" + tr("FrameCode type:"), comboFrameCodeType_);

    connect(comboFrameCodeType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        //
    });

    // init

    // framecode-typs
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU8), Icd::FrameCodeU8);
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU16), Icd::FrameCodeU16);
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU32), Icd::FrameCodeU32);
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU64), Icd::FrameCodeU64);
}

FrameCodeEdit::~FrameCodeEdit()
{

}

Icd::FrameCodeItemPtr FrameCodeEdit::frameCode() const
{
    return JHandlePtrCast<Icd::FrameCodeItem>(item());
}

bool FrameCodeEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    lock();

    const Icd::FrameCodeItemPtr frameCode = this->frameCode();
    if (frameCode) {
        // framecode-type
        comboFrameCodeType_->setCurrentIndex(comboFrameCodeType_->findData(frameCode->frameCodeType()));
    }

    unlock();

    return true;
}

}

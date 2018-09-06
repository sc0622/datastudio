#include "precomp.h"
#include "FrameEdit.h"

namespace Edit {

// class FrameEdit

FrameEdit::FrameEdit(const Icd::FrameItemPtr &frame, QWidget *parent)
    : ItemEdit(frame, parent)
{
    // sequence
    spinBoxSequence_ = new QSpinBox(this);
    spinBoxSequence_->setRange(0, INT_MAX);
    addRow(tr("Sequence:"), spinBoxSequence_);
}

FrameEdit::~FrameEdit()
{

}

Icd::FrameItemPtr FrameEdit::frame() const
{
    return JHandlePtrCast<Icd::FrameItem>(item());
}

bool FrameEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    lock();

    const Icd::FrameItemPtr frame = this->frame();
    if (frame) {
        //sequence
        spinBoxSequence_->setValue(frame->sequenceCount());
    }

    unlock();

    return true;
}

}

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

    connect(spinBoxSequence_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
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

    FrameEdit::restoreContent(false);

    return true;
}

void FrameEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::FrameItemPtr frame = this->frame();
    if (frame) {
        //sequence
        spinBoxSequence_->setValue(frame->sequenceCount());
    }

    unlock();
}

bool FrameEdit::validate()
{
    if (!ItemEdit::validate()) {
        return false;
    }

    return true;
}

void FrameEdit::saveContent()
{
    ItemEdit::saveContent();

    const Icd::FrameItemPtr frame = this->frame();
    if (!frame) {
        return;
    }

    // sequence
    frame->setSequenceCount(spinBoxSequence_->value());
}

}

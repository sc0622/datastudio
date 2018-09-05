#include "precomp.h"
#include "FrameEdit.h"

namespace Edit {

// class FrameEdit

FrameEdit::FrameEdit(const Icd::FrameItemPtr &frame, QWidget *parent)
    : ItemEdit(frame, parent)
{

}

FrameEdit::~FrameEdit()
{

}

Icd::FrameItemPtr FrameEdit::frame() const
{
    return JHandlePtrCast<Icd::FrameItem>(item());
}

}

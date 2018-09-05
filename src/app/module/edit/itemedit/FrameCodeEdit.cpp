#include "precomp.h"
#include "FrameCodeEdit.h"

namespace Edit {

// class FrameCodeEdit

FrameCodeEdit::FrameCodeEdit(const Icd::FrameCodeItemPtr &frameCode, QWidget *parent)
    : ItemEdit(frameCode, parent)
{

}

FrameCodeEdit::~FrameCodeEdit()
{

}

Icd::FrameCodeItemPtr FrameCodeEdit::frameCode() const
{
    return JHandlePtrCast<Icd::FrameCodeItem>(item());
}

}

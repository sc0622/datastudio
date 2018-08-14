#include "precomp.h"
#include "FrameEdit.h"
#include "limitlineedit.h"
#include "limittextedit.h"

FrameEdit::FrameEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    spinSequence_ = new QSpinBox(this);
    spinSequence_->setRange(1, 1e6);
    addFormRow(tr("Sequence:"), spinSequence_);

    enableConnect(true);
}

int FrameEdit::windowType() const
{
    return wdFrame;
}

bool FrameEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool FrameEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    if (sender() == spinSequence_) {
        data()->setOffset(spinSequence_->value());
        enableCommit(true);
        return true;
    }

    return false;
}

bool FrameEdit::init()
{
    if (int(data()->offset()) < 1) {
        data()->setOffset(1.0);
    }
    spinSequence_->setValue(int(data()->offset()));

    return true;
}

void FrameEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(spinSequence_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    if (enabled) {
        connect(spinSequence_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
    }
}

ICDComplexData *FrameEdit::data()
{
    return static_cast<ICDComplexData*>(ObjectEdit::data());
}

ICDComplexData *FrameEdit::oldData()
{
    return static_cast<ICDComplexData*>(ObjectEdit::oldData());
}

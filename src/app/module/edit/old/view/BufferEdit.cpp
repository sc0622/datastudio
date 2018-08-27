#include "precomp.h"
#include "BufferEdit.h"
#include "limitlineedit.h"
#include "limittextedit.h"

BufferEdit::BufferEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    spinLength_ = new QSpinBox(this);
    spinLength_->setRange(1, 1e6);
    spinLength_->setSuffix(tr(" B"));
    addFormRow(tr("Length:"), spinLength_);

    enableConnect(true);
}

int BufferEdit::windowType() const
{
    return wdBuffer;
}

bool BufferEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool BufferEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    const QObject *sender = this->sender();
    if (sender == spinLength_) {
        data()->setOffset(spinLength_->value());
        enableCommit(true);
        return true;
    }

    return false;
}

bool BufferEdit::init()
{
    spinLength_->setValue(data()->length());

    return true;
}

void BufferEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(spinLength_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    if (enabled) {
        connect(spinLength_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
    }
}

ICDCustomizedData *BufferEdit::data()
{
    return static_cast<ICDCustomizedData*>(ObjectEdit::data());
}

ICDCustomizedData *BufferEdit::oldData()
{
    return static_cast<ICDCustomizedData*>(ObjectEdit::oldData());
}

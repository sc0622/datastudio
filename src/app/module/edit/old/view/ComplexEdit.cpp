#include "precomp.h"
#include "ComplexEdit.h"
#include "limitlineedit.h"
#include "limittextedit.h"

ComplexEdit::ComplexEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    spinLength_ = new QSpinBox(this);
    spinLength_->setRange(0, 1e6);
    spinLength_->setSuffix(tr(" B"));
    spinLength_->setEnabled(false);
    addFormRow(tr("Length:"), spinLength_);

    enableConnect(true);
}

int ComplexEdit::windowType() const
{
    return wdComplex;
}

bool ComplexEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool ComplexEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    if (sender() == spinLength_) {
        //data()->setLength(spinLength_->value());
    }

    return false;
}

bool ComplexEdit::init()
{
    spinLength_->setValue(data()->length());

    return true;
}

void ComplexEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(spinLength_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    if (enabled) {
        connect(spinLength_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
    }
}

bool ComplexEdit::confirm()
{
    return true;
}

bool ComplexEdit::validate()
{
    return true;
}

ICDComplexData *ComplexEdit::data()
{
    return static_cast<ICDComplexData*>(ObjectEdit::data());
}

ICDComplexData *ComplexEdit::oldData()
{
    return static_cast<ICDComplexData*>(ObjectEdit::oldData());
}

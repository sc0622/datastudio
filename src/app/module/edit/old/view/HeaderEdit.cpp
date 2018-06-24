#include "precomp.h"
#include "HeaderEdit.h"
#include "KernelClass/icdfactory.h"
#include "LimitLineEdit.h"
#include "LimitTextEdit.h"
#include "HexSpinBox.h"

HeaderEdit::HeaderEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    spinDefault_ = new HexSpinBox(this);
    spinDefault_->setRange(0, 0xff);
    addFormRow(QStringLiteral("Ä¬ÈÏÖµ£º"), spinDefault_);

    enableConnect(true);
}

int HeaderEdit::windowType() const
{
    return wdHeader;
}

bool HeaderEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool HeaderEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    if (sender() == spinDefault_) {
        data()->setDefaultStr(QString::number(spinDefault_->value(), 16).toUpper().toStdString());
        enableCommit(true);
        return true;
    }

    return false;
}

bool HeaderEdit::init()
{
    spinDefault_->setValue(QString(data()->defaultStr().c_str()).toInt(0, 16));
    spinDefault_->clearFocus();

    return true;
}

void HeaderEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(spinDefault_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    if (enabled) {
        connect(spinDefault_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
    }
}

bool HeaderEdit::confirm()
{
    return true;
}

bool HeaderEdit::validate()
{
    return true;
}

ICDCommonData *HeaderEdit::data()
{
    return static_cast<ICDCommonData*>(ObjectEdit::data());
}

ICDCommonData *HeaderEdit::oldData()
{
    return static_cast<ICDCommonData*>(ObjectEdit::oldData());
}

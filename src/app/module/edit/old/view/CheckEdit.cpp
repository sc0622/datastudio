#include "precomp.h"
#include "CheckEdit.h"
#include "limitlineedit.h"
#include "limittextedit.h"

CheckEdit::CheckEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    comboCheckType_ = new QComboBox(this);
    addFormRow(tr("Parity type:"), comboCheckType_);

    spinStart_ = new QSpinBox(this);
    spinStart_->setRange(0, 1e4);
    addFormRow(tr("Start bit:"), spinStart_);

    spinEnd_ = new QSpinBox(this);
    spinEnd_->setRange(0, 1e4);
    addFormRow(tr("Stop bit:"), spinEnd_);

    enableConnect(true);
}

int CheckEdit::windowType() const
{
    return wdCheck;
}

bool CheckEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    bool result = false;
    const QObject *sender = this->sender();
    if (sender == spinStart_) {
        const int value = spinStart_->value();
        if (value != data()->start()) {
            spinEnd_->setMinimum(spinStart_->value());
            result = true;
        }
    } else if (sender == spinEnd_) {
        const int value = spinEnd_->value();
        if (value != data()->end()) {
            spinStart_->setMaximum(spinEnd_->value());
            result = true;
        }
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool CheckEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    bool result = false;
    ICDCheckData *checkData = data();
    const QObject *sender = this->sender();
    if (sender == comboCheckType_) {
        checkData->setCheckType(comboCheckType_->itemData(comboCheckType_->currentIndex()).toInt());
        spinStart_->setDisabled(GlobalDefine::ctInvalid == checkData->checkType());
        spinEnd_->setDisabled(GlobalDefine::ctInvalid == checkData->checkType());
        const int bound = spinEnd_->property("bound").toInt();
        if (bound > 0) {
            spinEnd_->setMaximum(bound - 1 - checkData->length());
        }
        result = true;
    } else if (sender == spinStart_) {
        checkData->setStart(spinStart_->value());
        result = true;
    } else if (sender == spinEnd_) {
        checkData->setEnd(spinEnd_->value());
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool CheckEdit::init()
{
    comboCheckType_->clear();
    std::vector<stDictionary> names;
    QVariantList args;
    args.append(qVariantFromValue(static_cast<void*>(&names)));
    args.append(int(GlobalDefine::dicCheckType));
    jnotify->send("edit.queryDictionaryTable", args);

    const size_t count = names.size();
    for (size_t i = 0; i < count; ++i) {
        const stDictionary &dic = names.at(i);
        comboCheckType_->addItem(dic.sDec.c_str(), dic.nCode);
    }
    const int index = comboCheckType_->findData(data()->checkType());
    comboCheckType_->setCurrentIndex((index < 0) ? 0 : index);

    int bound = 0;
    args.clear();
    args.append(qVariantFromValue(static_cast<void*>(&bound)));
    QString command("total");
    args.append(qVariantFromValue(static_cast<void*>(&command)));
    jnotify->send("edit.queryTableInformation", args);

    if (bound > 0) {
        spinStart_->setMaximum(bound - 1);
    }
    spinStart_->setValue(data()->start());
    data()->setStart(spinStart_->value());

    if (bound > 0) {
        // 起始字号从0开始，除去自身长度
        spinEnd_->setMaximum(bound - 1 - data()->length());
        spinEnd_->setProperty("bound", bound);
    }
    spinEnd_->setValue(data()->end());
    data()->setEnd(spinEnd_->value());

    return true;
}

void CheckEdit::enableConnect(bool enabled)
{
    disconnect(comboCheckType_, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinStart_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinStart_, SIGNAL(editingFinished()),
               this, SLOT(onEditFinished()));
    disconnect(spinEnd_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinEnd_, SIGNAL(editingFinished()),
               this, SLOT(onEditFinished()));
    if (enabled) {
        connect(comboCheckType_, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinStart_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinStart_, SIGNAL(editingFinished()),
                this, SLOT(onEditFinished()));
        connect(spinEnd_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinEnd_, SIGNAL(editingFinished()),
                this, SLOT(onEditFinished()));
    }
}

bool CheckEdit::confirm()
{
    return true;
}

bool CheckEdit::validate()
{
    return true;
}

ICDCheckData *CheckEdit::data()
{
    return static_cast<ICDCheckData*>(ObjectEdit::data());
}

ICDCheckData *CheckEdit::oldData()
{
    return static_cast<ICDCheckData*>(ObjectEdit::oldData());
}

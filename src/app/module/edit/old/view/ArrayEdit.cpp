#include "precomp.h"
#include "ArrayEdit.h"
#include "limitlineedit.h"
#include "limittextedit.h"

ArrayEdit::ArrayEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    comboArrayType_ = new QComboBox(this);
    addFormRow(tr("Array type:"), comboArrayType_);

    spinCount_ = new QSpinBox(this);
    spinCount_->setRange(1, INT_MAX);
    addFormRow(tr("Length:"), spinCount_);

    enableConnect(true);
}

int ArrayEdit::windowType() const
{
    return wdArray;
}

bool ArrayEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool ArrayEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    bool result = false;
    ICDArrayData *arrayData = data();
    const QObject *sender = this->sender();
    if (sender == comboArrayType_) {
        arrayData->setArrayType(comboArrayType_->itemData(comboArrayType_->currentIndex()).toInt());
        result = true;
    } else if (sender == spinCount_) {
        arrayData->setCount(spinCount_->value());
        result = true;
    }

    if (result) {
        enableCommit(true);
    }

    return result;
}

bool ArrayEdit::init()
{
    // array-type
    comboArrayType_->clear();
    std::vector<stDictionary> names;
    QVariantList args;
    args.append(qVariantFromValue((void*)&names));
    args.append(int(GlobalDefine::dicArrayType));
    jnotify->send("edit.queryDictionaryTable", args);
    const int count = names.size();
    for (int i = 0; i < count; ++i) {
        const stDictionary &dic = names.at(i);
        comboArrayType_->addItem(QString::fromStdString(dic.sDec), dic.nCode);
    }
    const int index = comboArrayType_->findData(data()->arrayType());
    comboArrayType_->setCurrentIndex((index < 0) ? 0 : index);

    // count
    spinCount_->setValue(data()->count());

    return true;
}

void ArrayEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(comboArrayType_, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinCount_, SIGNAL(valueChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    disconnect(spinCount_, SIGNAL(editingFinished()),
               this, SLOT(onEditFinished()));
    if (enabled) {
        connect(comboArrayType_, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinCount_, SIGNAL(valueChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
        connect(spinCount_, SIGNAL(editingFinished()),
                this, SLOT(onEditFinished()));
    }
}

bool ArrayEdit::confirm()
{
    return true;
}

bool ArrayEdit::validate()
{
    return true;
}

ICDArrayData *ArrayEdit::data()
{
    return static_cast<ICDArrayData*>(ObjectEdit::data());
}

ICDArrayData *ArrayEdit::oldData()
{
    return static_cast<ICDArrayData*>(ObjectEdit::oldData());
}

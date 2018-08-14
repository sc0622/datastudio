#include "precomp.h"
#include "CounterEdit.h"
#include "limitlineedit.h"
#include "limittextedit.h"

CounterEdit::CounterEdit(QWidget *parent)
    : ObjectEdit(parent)
{
    comboCounterType_ = new QComboBox(this);
    addFormRow(tr("Counter type:"), comboCounterType_);

    enableConnect(true);
}

int CounterEdit::windowType() const
{
    return wdCounter;
}

bool CounterEdit::onEditFinished()
{
    if (ObjectEdit::onEditFinished()) {
        return true;
    }

    return false;
}

bool CounterEdit::onTextChanged(const QString& text)
{
    if (ObjectEdit::onTextChanged(text)) {
        return true;
    }

    if (sender() == comboCounterType_) {
        data()->setCounterType(comboCounterType_->itemData(comboCounterType_->currentIndex()).toInt());
        enableCommit(true);
        return true;
    }

    return false;
}

bool CounterEdit::init()
{
    comboCounterType_->clear();
    std::vector<stDictionary> names;
    QVariantList args;
    args.append(qVariantFromValue(static_cast<void*>(&names)));
    args.append(int(GlobalDefine::dicCounterType));
    jnotify->send("edit.queryDictionaryTable", args);

    const size_t count = names.size();
    for (size_t i = 0; i < count; ++i) {
        const stDictionary &dic = names.at(i);
        comboCounterType_->addItem(dic.sDec.c_str(), dic.nCode);
    }

    const int index = comboCounterType_->findData(data()->counterType());
    comboCounterType_->setCurrentIndex((index < 0) ? 0 : index);

    return true;
}

void CounterEdit::enableConnect(bool enabled)
{
    ObjectEdit::enableConnect(enabled);

    disconnect(comboCounterType_, SIGNAL(currentTextChanged(const QString &)),
               this, SLOT(onTextChanged(const QString &)));
    if (enabled) {
        connect(comboCounterType_, SIGNAL(currentTextChanged(const QString &)),
                this, SLOT(onTextChanged(const QString &)));
    }
}

bool CounterEdit::confirm()
{
    return true;
}

bool CounterEdit::validate()
{
    return true;
}

ICDCounterData *CounterEdit::data()
{
    return static_cast<ICDCounterData*>(ObjectEdit::data());
}

ICDCounterData *CounterEdit::oldData()
{
    return static_cast<ICDCounterData*>(ObjectEdit::oldData());
}

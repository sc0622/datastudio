#include "precomp.h"
#include "FrameCodeEdit.h"

namespace Edit {

// class FrameCodeEdit

FrameCodeEdit::FrameCodeEdit(const Icd::FrameCodeItemPtr &frameCode, QWidget *parent)
    : ItemEdit(frameCode, parent)
{
    comboFrameCodeType_ = new QComboBox(this);
    addRow("<font color=red>*</font>" + tr("FrameCode type:"), comboFrameCodeType_);

    comboBinding_ = new QComboBox(this);
    addRow(tr("Binding:"), comboBinding_);

    // framecode-typs
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU8), Icd::FrameCodeU8);
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU16), Icd::FrameCodeU16);
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU32), Icd::FrameCodeU32);
    comboFrameCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU64), Icd::FrameCodeU64);

    // bindings
    initBinding();

    connect(comboFrameCodeType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(comboBinding_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
}

FrameCodeEdit::~FrameCodeEdit()
{

}

Icd::FrameCodeItemPtr FrameCodeEdit::frameCode() const
{
    return JHandlePtrCast<Icd::FrameCodeItem>(item());
}

bool FrameCodeEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    FrameCodeEdit::restoreContent(false);

    return true;
}

void FrameCodeEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
    }

    lock();

    const Icd::FrameCodeItemPtr frameCode = this->frameCode();
    if (frameCode) {
        // framecode-type
        comboFrameCodeType_->setCurrentIndex(comboFrameCodeType_->findData(frameCode->frameCodeType()));
        // binding
        restoreBinding();
    }

    unlock();
}

bool FrameCodeEdit::validate()
{
    if (!ItemEdit::validate()) {
        return false;
    }

    return true;
}

void FrameCodeEdit::saveContent()
{
    ItemEdit::saveContent();

    const Icd::FrameCodeItemPtr frameCode = this->frameCode();
    if (!frameCode) {
        return;
    }

    // framecode-type
    frameCode->setFrameCodeType(Icd::FrameCodeType(comboFrameCodeType_->currentData().toInt()));
    // binding
    frameCode->setFrameId(comboBinding_->currentData().toString().toStdString());
}

bool FrameCodeEdit::initBinding()
{
    comboBinding_->clear();
    comboBinding_->addItem(tr("No binding"));

    Icd::FrameCodeItemPtr frameCode = this->frameCode();
    if (!frameCode) {
        return false;
    }

    // frames
    Icd::Object *parentObject = frameCode->parent();
    if (!parentObject || parentObject->objectType() != Icd::ObjectTable) {
        return false;
    }

    const Icd::Table *table = dynamic_cast<Icd::Table*>(parentObject);
    if (!table) {
        return false;
    }

    const Icd::ItemPtrArray &items = table->allItem();
    for (auto &citer = items.cbegin(); citer != items.cend(); ++citer) {
        const Icd::ItemPtr item = *citer;
        if (item && item->type() == Icd::ItemFrame) {
            comboBinding_->addItem(tr("Binding<") + QString::fromStdString(item->name()) + ">",
                                   QString::fromStdString(item->id()));
        }
    }

    return true;
}

bool FrameCodeEdit::restoreBinding()
{
    Icd::FrameCodeItemPtr frameCode = this->frameCode();
    if (!frameCode) {
        return false;
    }

    int index = comboBinding_->findData(QString::fromStdString(frameCode->frameId()));
    if (index == -1) {
        index = 0;
    }

    comboBinding_->setCurrentIndex(index);

    return true;
}

}

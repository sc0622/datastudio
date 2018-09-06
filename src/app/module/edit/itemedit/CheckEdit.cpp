#include "precomp.h"
#include "CheckEdit.h"

namespace Edit {

// class CheckEdit

CheckEdit::CheckEdit(const Icd::CheckItemPtr &check, QWidget *parent)
    : ItemEdit(check, parent)
{
    comboCheckType_ = new QComboBox(this);
    addRow("<font color=red>*</font>" + tr("Check type:"), comboCheckType_);

    spinStartPos_ = new QSpinBox(this);
    addRow(tr("Start pos:"), spinStartPos_);

    spinEndPos_ = new QSpinBox(this);
    addRow(tr("End pos:"), spinEndPos_);

    connect(comboCheckType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        //
    });

    // init

    // check-typs
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckNone), Icd::CheckNone);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckSum8), Icd::CheckSum8);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckSum16), Icd::CheckSum16);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckCrc8), Icd::CheckCrc8);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckCrc16), Icd::CheckCrc16);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckXor8), Icd::CheckXor8);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckXor16), Icd::CheckXor16);
}

CheckEdit::~CheckEdit()
{

}

Icd::CheckItemPtr CheckEdit::check() const
{
    return JHandlePtrCast<Icd::CheckItem>(item());
}

bool CheckEdit::init()
{
    if (!ItemEdit::init()) {
        return false;
    }

    lock();

    const Icd::CheckItemPtr check = this->check();
    if (check) {
        // check-type
        comboCheckType_->setCurrentIndex(comboCheckType_->findData(check->checkType()));
        // startPos
        spinStartPos_->setValue(check->startPos());
        // endPos
        spinEndPos_->setValue(check->endPos());
    }

    unlock();

    return true;
}

}

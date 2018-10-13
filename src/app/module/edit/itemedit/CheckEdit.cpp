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

    // check-typs
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckNone), Icd::CheckNone);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckSum8), Icd::CheckSum8);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckSum16), Icd::CheckSum16);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckCrc8), Icd::CheckCrc8);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckCrc16), Icd::CheckCrc16);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckXor8), Icd::CheckXor8);
    comboCheckType_->addItem(IcdWidget::checkTypeString(Icd::CheckXor16), Icd::CheckXor16);

    connect(comboCheckType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinStartPos_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
    connect(spinEndPos_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](){
        if (!blocking()) {
            emit contentChanged();
        }
    });
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

    //
    CheckEdit::restoreContent(false);

    return true;
}

void CheckEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ItemEdit::restoreContent(recursive);
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
}

bool CheckEdit::validate() const
{
    if (!ItemEdit::validate()) {
        return false;
    }

    //

    return true;
}

void CheckEdit::saveContent()
{
    ItemEdit::saveContent();

    const Icd::CheckItemPtr check = this->check();
    if (!check) {
        return;
    }

    // check-type
    check->setCheckType(Icd::CheckType(comboCheckType_->currentData().toInt()));
    // start-pos
    check->setStartPos(spinStartPos_->value());
    // end-pos
    check->setEndPos(spinEndPos_->value());
}

}

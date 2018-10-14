#include "precomp.h"
#include "TableEdit.h"

namespace Edit {

// class TableEdit

TableEdit::TableEdit(const Icd::TablePtr &table, QWidget *parent)
    : ObjectEdit(table, parent)
    , comboCodeType_(nullptr)
    , spinCode_(nullptr)
    , spinSequence_(nullptr)
{
    if (table->isSubFrameTable()) {
        // mark
        setMarkReadOnly(true);
        setMarkValidator(new QRegExpValidator(QRegExp("^[0-9]{0,16}$")));
        // codeType
        comboCodeType_ = new QComboBox(this);
        comboCodeType_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        addRow(tr("Code Type:"), comboCodeType_);
        // code
        spinCode_ = new JLargeSpinBox(this);
        spinCode_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        spinCode_->setPrefix("0x");
        spinCode_->setRadix(16);
        spinCode_->setFillChar(QChar('0'));
        addRow(tr("Code:"), spinCode_);
        // sequence
        spinSequence_ = new QSpinBox(this);
        spinSequence_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        spinSequence_->setRange(0, INT_MAX);
        addRow(tr("Sequence:"), spinSequence_);

        connect(comboCodeType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, [=](){
            const int currentIndex = comboCodeType_->currentIndex();
            const qulonglong maximum = (1ULL << ((1ULL << currentIndex) * 8)) - 1;
            spinCode_->setRange(0, maximum);
            setMark(spinCode_->textFromValue(spinCode_->value()));
            //
            if (blocking()) {
                return;
            }
            emit contentChanged();
        });
        connect(spinCode_, &JLargeSpinBox::valueChanged, this, [=](double value){
            setMark(spinCode_->textFromValue(value));
            if (blocking()) {
                return;
            }
            emit contentChanged();
        });
        connect(spinSequence_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, [=](){
            if (blocking()) {
                return;
            }
            emit contentChanged();
        });
    }
}

TableEdit::~TableEdit()
{

}

Icd::TablePtr TableEdit::table() const
{
    return JHandlePtrCast<Icd::Table>(object());
}

bool TableEdit::init()
{
    if (!ObjectEdit::init()) {
        return false;
    }

    //
    const Icd::TablePtr table = this->table();
    if (table) {
        if (table->isSubFrameTable()) {
            // codeType
            comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU8), Icd::FrameCodeU8);
            comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU16), Icd::FrameCodeU16);
            comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU32), Icd::FrameCodeU32);
            comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU64), Icd::FrameCodeU64);
        }
    }

    //
    TableEdit::restoreContent(false);

    return true;
}

void TableEdit::restoreContent(bool recursive)
{
    if (recursive) {
        ObjectEdit::restoreContent(recursive);
    }

    lock();

    const Icd::TablePtr table = this->table();
    if (table) {
        if (table->isSubFrameTable()) {
            // codeType
            int frameCodeType = table->frameCodeType();
            if (frameCodeType == Icd::FrameCodeInvalid) {
                comboCodeType_->setCurrentIndex(0);
            } else {
                comboCodeType_->setCurrentIndex(comboCodeType_->findData(frameCodeType));
            }
            // code
            spinCode_->setValue(QString::fromStdString(table->mark()).toULongLong(nullptr, 16));
            //sequence
            spinSequence_->setValue(table->sequence());
        }
    }

    unlock();
}

bool TableEdit::validate() const
{
    if (!ObjectEdit::validate()) {
        return false;
    }

    const Icd::TablePtr table = this->table();
    if (table) {
        if (table->isSubFrameTable()) {
            // code-type
            if (comboCodeType_->currentIndex() == -1) {
                comboCodeType_->setFocus();
                return false;
            }
            // code [unique check]
            auto parentObject = table->parent();
            if (parentObject) {
                const Icd::ObjectPtr childObject = parentObject->childAt(spinCode_->value());
                if (childObject && table->id() != childObject->id()) {
                    spinCode_->setFocus();
                    QToolTip::showText(spinCode_->mapToGlobal(QPoint(0, 8)),
                                       tr("There is already a code with %1").arg(spinCode_->text()));
                    return false;
                }
            }
        }
    }

    return true;
}

void TableEdit::saveContent()
{
    ObjectEdit::saveContent();

    const Icd::TablePtr table = this->table();
    if (!table) {
        return;
    }

    if (table->isSubFrameTable()) {
        // code
        //table->setMark(spinCode_->cleanText().toStdString());
        // sequence
        table->setSequence(spinSequence_->value());
    }
}

}

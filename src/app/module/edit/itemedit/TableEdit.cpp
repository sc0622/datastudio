#include "precomp.h"
#include "TableEdit.h"

namespace Edit {

// class TableEdit

TableEdit::TableEdit(const Icd::TablePtr &table, QWidget *parent)
    : ObjectEdit(table, parent)
    , comboCodeType_(nullptr)
    , spinCode_(nullptr)
    , spinBoxSequence_(nullptr)
{
    if (table->isSubFrameTable()) {
        // mark
        setMarkReadOnly(true);
        // codeType
        comboCodeType_ = new QComboBox(this);
        addRow(tr("Code Type:"), comboCodeType_);
        // code
        spinCode_ = new JLargeSpinBox(this);
        spinCode_->setPrefix("0x");
        spinCode_->setRadix(16);
        spinCode_->setFillChar(QChar('0'));
        addRow(tr("Code:"), spinCode_);
        // sequence
        spinBoxSequence_ = new QSpinBox(this);
        spinBoxSequence_->setRange(0, INT_MAX);
        addRow(tr("Sequence:"), spinBoxSequence_);

        connect(comboCodeType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, [=](){
            const int currentIndex = comboCodeType_->currentIndex();
            const qulonglong maximum = (1ULL << ((1ULL << currentIndex) * 8)) - 1;
            spinCode_->setRange(0, maximum);
            const double value = spinCode_->value();
            spinCode_->clear();
            spinCode_->setValue(value);
            setMark(spinCode_->textFromValue(value));
        });
        connect(spinCode_, &JLargeSpinBox::valueChanged, this, [=](double value){
            setMark(spinCode_->textFromValue(value));
        });

        // init

        // codeType
        comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU8), Icd::FrameCodeU8);
        comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU16), Icd::FrameCodeU16);
        comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU32), Icd::FrameCodeU32);
        comboCodeType_->addItem(IcdWidget::frameCodeTypeString(Icd::FrameCodeU64), Icd::FrameCodeU64);
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

    lock();

    const Icd::TablePtr table = this->table();
    if (table) {
        if (table->isSubFrameTable()) {
            // codeType
            comboCodeType_->setCurrentIndex(comboCodeType_->findData(table->frameCodeType()));
            // code
            spinCode_->setValue(QString::fromStdString(table->mark()).toULongLong(nullptr, 16));
            //sequence
            spinBoxSequence_->setValue(table->sequence());
        }
    }

    unlock();

    return true;
}

}

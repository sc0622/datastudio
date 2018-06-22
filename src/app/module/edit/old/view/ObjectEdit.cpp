#include "precomp.h"
#include "ObjectEdit.h"
#include "HeaderEdit.h"
#include "CounterEdit.h"
#include "CheckEdit.h"
#include "FrameCodeEdit.h"
#include "ItemEdit.h"
#include "BitMapEdit.h"
#include "BitValueEdit.h"
#include "FrameEdit.h"
#include "ComplexEdit.h"
#include "BufferEdit.h"
#include "ArrayEdit.h"
#include "NumericEdit.h"
#include "SubTableEdit.h"
#include "TableEdit.h"
#include "SystemEdit.h"
#include "VehicleEdit.h"

ObjectEdit::ObjectEdit(QWidget *parent)
    : QWidget(parent)
{
    layoutMain_ = new QVBoxLayout(this);
    layoutMain_->setContentsMargins(0, 0, 0, 0);
    layoutMain_->setSpacing(1);

    QHBoxLayout *layoutButton = new QHBoxLayout();
    layoutMain_->addLayout(layoutButton);

    layoutButton->addStretch();
    // apply
    buttonConfirm_ = new QPushButton(QIcon(":/datastudio/image/global/apply.png"),
                                     QStringLiteral("提交"), this);
    buttonConfirm_->setFixedWidth(100);
    layoutButton->addWidget(buttonConfirm_);
    // cancel
    buttonCancel_ = new QPushButton(QIcon(":/datastudio/image/global/cancel.png"),
                                    QStringLiteral("取消"), this);
    buttonCancel_->setFixedWidth(100);
    layoutButton->addSpacing(30);
    layoutButton->addWidget(buttonCancel_);
    layoutButton->addStretch();

    editStatus_ = new QLineEdit(this);
    editStatus_->setObjectName("__status__");
    editStatus_->setFixedHeight(22);
    editStatus_->setReadOnly(true);
    editStatus_->setFrame(false);
    layoutMain_->addWidget(editStatus_);

    connect(buttonConfirm_, &QPushButton::clicked, this, &ObjectEdit::confirm);
    connect(buttonCancel_, &QPushButton::clicked, this, &ObjectEdit::cancel);
}

ObjectEdit::~ObjectEdit()
{

}

int ObjectEdit::windowType() const
{
    return wdUnknown;
}

void ObjectEdit::setUIData(const _UIData &data)
{
    Q_UNUSED(data);
    show();
}

void* ObjectEdit::uiData() const
{
    return NULL;
}

void ObjectEdit::changeDataType(int type)
{
    Q_UNUSED(type);
}

int ObjectEdit::originalType() const
{
    return GlobalDefine::dtInvalid;
}

void ObjectEdit::enableOptionButton(bool enable)
{
    buttonConfirm_->setEnabled(enable);
    buttonCancel_->setEnabled(enable);
}

QVBoxLayout *ObjectEdit::layoutMain() const
{
    return layoutMain_;
}

QPushButton *ObjectEdit::buttonConfirm() const
{
    return buttonConfirm_;
}

QPushButton *ObjectEdit::buttonCancel() const
{
    return buttonCancel_;
}

QLineEdit *ObjectEdit::editStatus() const
{
    return editStatus_;
}

ObjectEdit *ObjectEdit::create(int windowType)
{
    switch (windowType) {
    case wdHeader: return new HeaderEdit();
    case wdCounter: return new CounterEdit();
    case wdCheck: return new CheckEdit();
    case wdFrameCode: return new FrameCodeEdit();
    case wdItem: return new ItemEdit();
    case wdArray: return new ArrayEdit();
    case wdNumeric: return new NumericEdit();
    case wdBitMap: return new BitMapEdit();
    case wdBitValue: return new BitValueEdit();
    case wdFrame: return new FrameEdit();
    case wdComplex: return new ComplexEdit();
    case wdBuffer: return new BufferEdit();
    case wdSubTable: return new SubTableEdit();
    case wdTable: return new TableEdit();
    case wdSystem: return new SystemEdit();
    case wdVehicle: return new VehicleEdit();
    default: return new ObjectEdit();
    }
}

void ObjectEdit::confirm()
{
    buttonConfirm_->setEnabled(false);
    buttonCancel_->setEnabled(false);
}

void ObjectEdit::cancel()
{
    buttonConfirm_->setEnabled(false);
    buttonCancel_->setEnabled(false);
}


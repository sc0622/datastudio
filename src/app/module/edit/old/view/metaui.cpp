#include "precomp.h"
#include "metaui.h"
#include "headerui.h"
#include "counterui.h"
#include "checkui.h"
#include "framecodeui.h"
#include "commonui.h"
#include "bitmapui.h"
#include "bitvalueui.h"
#include "discernui.h"
#include "complexui.h"
#include "bufferui.h"
#include "subtableui.h"
#include "tableui.h"
#include "systemui.h"
#include "planeui.h"

MetaUI::MetaUI(QWidget *parent)
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
                                     QStringLiteral("�ύ"), this);
    buttonConfirm_->setFixedWidth(100);
    layoutButton->addWidget(buttonConfirm_);
    // cancel
    buttonCancel_ = new QPushButton(QIcon(":/datastudio/image/global/cancel.png"),
                                    QStringLiteral("ȡ��"), this);
    buttonCancel_->setFixedWidth(100);
    layoutButton->addSpacing(30);
    layoutButton->addWidget(buttonCancel_);
    layoutButton->addStretch();

    editStatus_ = new QLineEdit(this);
    editStatus_->setObjectName("__status__");
    editStatus_->setMaximumHeight(22);
    editStatus_->setReadOnly(true);
    editStatus_->setFrame(false);
    layoutMain_->addWidget(editStatus_);

    connect(buttonConfirm_, &QPushButton::clicked, this, [=](){
        confirm();
    });
    connect(buttonCancel_, &QPushButton::clicked, this, [=](){
        cancel();
    });
}

int MetaUI::uiType() const
{
    return wdUnknown;
}

void MetaUI::setUIData(const _UIData &data)
{
    Q_UNUSED(data);
    show();
}

void* MetaUI::uiData() const
{
    return NULL;
}

void MetaUI::changeDataType(int type)
{
    Q_UNUSED(type);
}

int MetaUI::originalType() const
{
    return GlobalDefine::dtInvalid;
}

void MetaUI::enableOptionButton(bool enable)
{
    buttonConfirm_->setEnabled(enable);
    buttonCancel_->setEnabled(enable);
}

QVBoxLayout *MetaUI::layoutMain() const
{
    return layoutMain_;
}

QPushButton *MetaUI::buttonConfirm() const
{
    return buttonConfirm_;
}

QPushButton *MetaUI::buttonCancel() const
{
    return buttonCancel_;
}

QLineEdit *MetaUI::editStatus() const
{
    return editStatus_;
}

MetaUI *MetaUI::create(int metaUiType)
{
    switch (metaUiType) {
    case wdHeader: return new HeaderUI();
    case wdCounter: return new CounterUI();
    case wdCheck: return new CheckUI();
    case wdFrameCode: return new FrameCodeUI();
    case wdCommon: return new CommonUI();
    case wdBitMap: return new BitMapUI();
    case wdBitValue: return new BitValueUI();
    case wdDiscern: return new DiscernUI();
    case wdComplex: return new ComplexUI();
    case wdBuffer: return new BufferUI();
    case wdSubTable: return new SubTableUI();
    case wdTable: return new TableUI();
    case wdSystem: return new SystemUI();
    case wdPlane: return new PlaneUI();
    default: return new MetaUI();
    }
}

void MetaUI::confirm()
{

}

void MetaUI::cancel()
{

}


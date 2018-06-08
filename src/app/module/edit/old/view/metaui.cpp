#include "precomp.h"
#include "metaui.h"

#include <QtWidgets>

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

void MetaUI::enableConnection(bool enable)
{
    Q_UNUSED(enable);
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

void MetaUI::confirm()
{

}

void MetaUI::cancel()
{

}


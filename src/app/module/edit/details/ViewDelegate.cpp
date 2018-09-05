#include "precomp.h"
#include "ViewDelegate.h"

namespace Edit {

ViewDelegate::ViewDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , delegateType_(InvalidDelegate)
{

}

ViewDelegate::~ViewDelegate()
{

}

int ViewDelegate::delegateType() const
{
    return delegateType_;
}

void ViewDelegate::setDelegateType(int delegateType)
{
    if (delegateType != delegateType_) {
        delegateType_ = delegateType;
        // update
        setupUi();
    }
}

QWidget *ViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    Q_UNUSED(parent);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return nullptr;
}

void ViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(index);
}

void ViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(model);
    Q_UNUSED(index);
}

void ViewDelegate::setupUi()
{

}

}

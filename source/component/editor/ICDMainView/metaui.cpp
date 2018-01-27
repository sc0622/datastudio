#include "metaui.h"

#include <QtWidgets>

MetaUI::MetaUI(QWidget *parent)
    : QDialog(parent)
{
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
    Q_UNUSED(enable);
}


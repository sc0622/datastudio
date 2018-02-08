#include "precomp.h"
#include "DetailItemEdit.h"

namespace Edit {

DetailItemEdit::DetailItemEdit(QWidget *parent)
    : QWidget(parent)
    , d_object(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_groupBox = new QGroupBox("test", this);
    vertLayoutMain->addWidget(d_groupBox);
}

void DetailItemEdit::resetView()
{
    d_object = nullptr;
    hide();
}

void DetailItemEdit::updateView(const Icd::ObjectPtr &object)
{
    // clear

    d_object = object;

    if (!object) {
        hide();
        return;
    }

    switch (object->objectType()) {
    case Icd::ObjectRoot:
        hide();
        break;
    case Icd::ObjectVehicle:
        hide();
        break;
    case Icd::ObjectSystem:
        hide();
        break;
    case Icd::ObjectTable:
        hide();
        break;
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(object);
        if (!item) {
            hide();
            break;
        }
        switch (item->type()) {
        case Icd::ItemFrame:
            hide();
            break;
        default:
            show();
            break;
        }
        break;
    }
    default:
        hide();
        break;
    }
}

}

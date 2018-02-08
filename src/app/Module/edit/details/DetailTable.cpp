#include "precomp.h"
#include "DetailTable.h"

namespace Edit {

DetailTable::DetailTable(QWidget *parent)
    : QWidget(parent)
    , d_object(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_tableView = new JTableView(this);
    vertLayoutMain->addWidget(d_tableView);
}

void DetailTable::resetView()
{
    d_tableView->clear();
    d_object = nullptr;
}

void DetailTable::updateView(const Icd::ObjectPtr &object)
{
    d_tableView->clear();

    d_object = object;

    if (!object) {
        return;
    }

    switch (object->objectType()) {
    case Icd::ObjectRoot:
        updateRoot();
        break;
    case Icd::ObjectVehicle:
        updateVehicle();
        break;
    case Icd::ObjectSystem:
        updateSystem();
        break;
    case Icd::ObjectTable:
        updateTable();
        break;
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(d_object);
        if (!item) {
            break;
        }

        switch (item->type()) {
        case Icd::ItemFrame:
            updateFrame();
            break;
        default:
            updateItem();
            break;
        }
        break;
    }
    default:
        break;
    }
}

void DetailTable::updateRoot()
{

}

void DetailTable::updateVehicle()
{
    d_tableView->setHorizontalHeaderLabels({});
}

void DetailTable::updateSystem()
{

}

void DetailTable::updateTable()
{

}

void DetailTable::updateItem()
{

}

void DetailTable::updateHead()
{

}

void DetailTable::updateCounter()
{

}

void DetailTable::updateCheck()
{

}

void DetailTable::updateFrameCode()
{

}

void DetailTable::updateNumeric()
{

}

void DetailTable::updateBitMap()
{

}

void DetailTable::updateBitValue()
{

}

void DetailTable::updateComplex()
{

}

void DetailTable::updateFrame()
{

}

}

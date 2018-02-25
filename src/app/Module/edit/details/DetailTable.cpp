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

    connect(d_tableView, &JTableView::currentItemChanged, this,
            [=](QStandardItem *current, QStandardItem *previous){
        qDebug() << "selection";
    });
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
    d_tableView->setHorizontalHeaderLabels({tr("Name"), tr("Mark"), tr("Describe")});

    const Icd::RootPtr root = JHandlePtrCast<Icd::Root, Icd::Object>(d_object);
    if (!root) {
        return;
    }

    const Icd::VehiclePtrArray &vehicles = root->allVehicle();

    d_tableView->setRowCount(vehicles.size());
    const int rowCount = d_tableView->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::VehiclePtr vehicle = vehicles.at(row);
        if (vehicle) {
            d_tableView->setItemData(row, 0, QString::fromStdString(vehicle->name()));
            d_tableView->setItemData(row, 1, QString::fromStdString(vehicle->mark()));
            d_tableView->setItemData(row, 2, QString::fromStdString(vehicle->desc()));
        }
    }
}

void DetailTable::updateVehicle()
{
    d_tableView->setHorizontalHeaderLabels({tr("Name"), tr("Mark"), tr("Describe")});

    const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle, Icd::Object>(d_object);
    if (!vehicle) {
        return;
    }

    const Icd::SystemPtrArray &systems = vehicle->allSystem();

    d_tableView->setRowCount(systems.size());
    const int rowCount = d_tableView->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::SystemPtr system = systems.at(row);
        if (system) {
            d_tableView->setItemData(row, 0, QString::fromStdString(system->name()));
            d_tableView->setItemData(row, 1, QString::fromStdString(system->mark()));
            d_tableView->setItemData(row, 2, QString::fromStdString(system->desc()));
        }
    }
}

void DetailTable::updateSystem()
{
    d_tableView->setHorizontalHeaderLabels({tr("Name"), tr("Mark"), tr("Length"),
                                            tr("Describe")});

    const Icd::SystemPtr system = JHandlePtrCast<Icd::System, Icd::Object>(d_object);
    if (!system) {
        return;
    }

    const Icd::TablePtrArray &tables = system->allTable();

    d_tableView->setRowCount(tables.size());
    const int rowCount = d_tableView->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::TablePtr table = tables.at(row);
        if (table) {
            d_tableView->setItemData(row, 0, QString::fromStdString(table->name()));
            d_tableView->setItemData(row, 1, QString::fromStdString(table->mark()));
            d_tableView->setItemData(row, 2, QString::number(table->bufferSize()));
            d_tableView->setItemData(row, 3, QString::fromStdString(table->desc()));
        }
    }
}

void DetailTable::updateTable()
{
    d_tableView->setHorizontalHeaderLabels({tr("Name"), tr("Mark"), tr("Byte index"),
                                            tr("Length"), tr("Type"), tr("Describe")});

    const Icd::TablePtr table = JHandlePtrCast<Icd::Table, Icd::Object>(d_object);
    if (!table) {
        return;
    }

    const Icd::ItemPtrArray &items = table->allItem();

    d_tableView->setRowCount(items.size());
    const int rowCount = d_tableView->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::ItemPtr item = items.at(row);
        if (item) {
            d_tableView->setItemData(row, 0, QString::fromStdString(item->name()));
            d_tableView->setItemData(row, 1, QString::fromStdString(item->mark()));
            d_tableView->setItemData(row, 2, QString::number(item->bufferOffset()));
            d_tableView->setItemData(row, 3, QString::number(item->bufferSize()));
            d_tableView->setItemData(row, 4, JMain::typeString(item));
            d_tableView->setItemData(row, 5, QString::fromStdString(item->desc()));
        }
    }
}

void DetailTable::updateItem()
{
    // header
    d_tableView->setHorizontalHeaderLabels({tr("Name"), tr("Value")});

    const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(d_object);
    if (!item) {
        return;
    }

    int rowIndex = d_tableView->rowCount() - 1;

    // type of data
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Type of data"));
    d_tableView->setItemData(rowIndex, 1, JMain::typeString(item));
    // name of data
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Name of data"));
    d_tableView->setItemData(rowIndex, 1, QString::fromStdString(item->name()));
    // Mark of data
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Mark of data"));
    d_tableView->setItemData(rowIndex, 1, QString::fromStdString(item->mark()));
    // Offset of data
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Offset of data"));
    d_tableView->setItemData(rowIndex, 1, QString::number(item->bufferOffset()));
    // Length of data
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Length of data"));
    d_tableView->setItemData(rowIndex, 1, QString::number(item->bufferSize()));
    // Describe
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Describe"));
    d_tableView->setItemData(rowIndex, 1, QString::fromStdString(item->desc()));

    switch (item->type()) {
    case Icd::ItemHead: updateHead(); break;
    case Icd::ItemCounter: updateCounter(); break;
    case Icd::ItemCheck: updateCheck(); break;
    case Icd::ItemFrameCode: updateFrameCode(); break;
    case Icd::ItemNumeric: updateNumeric(); break;
    case Icd::ItemBitMap: updateBitMap(); break;
    case Icd::ItemBitValue: updateBitValue(); break;
    default:
        break;
    }
}

void DetailTable::updateHead()
{
    const Icd::HeaderItemPtr headerItem =
            JHandlePtrCast<Icd::HeaderItem, Icd::Object>(d_object);
    if (!headerItem) {
        return;
    }

    int rowIndex = d_tableView->rowCount() - 1;

    // Default value
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Default value"));
    d_tableView->setItemData(rowIndex, 1, "0x" + QString("%1")
                             .arg(uint(headerItem->defaultValue()),
                                  2, 16, QChar('0')).toUpper());
}

void DetailTable::updateCounter()
{

}

void DetailTable::updateCheck()
{
    const Icd::CheckItemPtr checkItem =
            JHandlePtrCast<Icd::CheckItem, Icd::Object>(d_object);
    if (!checkItem) {
        return;
    }

    int rowIndex = d_tableView->rowCount() - 1;

    // Begin at
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Begin at"));
    d_tableView->setItemData(rowIndex, 1, QString::number(checkItem->startPos()));
    // End at
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("End at"));
    d_tableView->setItemData(rowIndex, 1, QString::number(checkItem->endPos()));
}

void DetailTable::updateFrameCode()
{
    const Icd::FrameCodeItemPtr frameCodeItem =
            JHandlePtrCast<Icd::FrameCodeItem, Icd::Object>(d_object);
    if (!frameCodeItem) {
        return;
    }

    int rowIndex = d_tableView->rowCount() - 1;

    // Default value
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Default value"));
    d_tableView->setItemData(rowIndex, 1, "0x" + QString("%1")
                             .arg(qulonglong(frameCodeItem->defaultValue()),
                                  Icd::asciiCountOfSize(16, int(frameCodeItem->bufferSize())),
                                  16, QChar('0')).toUpper());
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
    // header
    d_tableView->setHorizontalHeaderLabels({tr("Name"), tr("Frame code"), tr("Sequence"),
                                            tr("Describe")});
}

}

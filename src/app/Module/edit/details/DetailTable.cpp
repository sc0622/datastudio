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
    d_tableView->setEditTriggers(QAbstractItemView::CurrentChanged
                                 | QAbstractItemView::DoubleClicked
                                 | QAbstractItemView::SelectedClicked);
    vertLayoutMain->addWidget(d_tableView);

    connect(d_tableView, &JTableView::currentCellChanged, this,
            [=](int currentRow, int currentColumn, int previousRow, int previuosColumn){
        Q_UNUSED(currentColumn);
        Q_UNUSED(previuosColumn);
        if (currentRow == previousRow) {
            return;
        }
        if (!d_object) {
            emit currentItemChanged(QVariant::Invalid);
            return;
        }
        switch (d_object->objectType()) {
        case Icd::ObjectItem:
        {
            const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(d_object);
            if (!item) {
                emit currentItemChanged(QVariant::Invalid);
                return;
            }
            switch (item->type()) {
            case Icd::ItemFrame:
                emit currentItemChanged(d_tableView->itemData(currentRow, 0, Qt::UserRole + 1));
                break;
            default:
                break;
            }
            break;
        }
        default:
            emit currentItemChanged(currentRow);
            break;
        }
    });
}

void DetailTable::resetView()
{
    enableChangedNotify(false);
    d_tableView->clear();
    d_object = nullptr;
}

void DetailTable::updateView(const Icd::ObjectPtr &object)
{
    resetView();

    if (!object) {
        return;
    }

    d_object = object;

    bool result = false;

    switch (object->objectType()) {
    case Icd::ObjectRoot:
        result = updateRoot();
        break;
    case Icd::ObjectVehicle:
        result = updateVehicle();
        break;
    case Icd::ObjectSystem:
        result = updateSystem();
        break;
    case Icd::ObjectTable:
        result = updateTable();
        break;
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(d_object);
        if (!item) {
            break;
        }

        switch (item->type()) {
        case Icd::ItemFrame:
            result = updateFrame();
            break;
        default:
            result = updateItem();
            break;
        }
        break;
    }
    default:
        break;
    }

    if (result) {
        enableChangedNotify(true);
    }
}

Icd::ObjectPtr DetailTable::object() const
{
    return d_object;
}

void DetailTable::onContentChanged(QStandardItem *item)
{
    if (!item) {
        return;
    }

    const int row = item->row();
    const int column = item->column();
    QStandardItem *headerItem = d_tableView->horizontalHeaderItem(column);
    if (!headerItem) {
        return;
    }

    const QString name = headerItem->data().toString();

    switch (d_object->objectType()) {
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr icdItem = JHandlePtrCast<Icd::Item, Icd::Object>(d_object);
        if (!icdItem) {
            return;
        }
        switch (icdItem->type()) {
        case Icd::ItemFrame:
            if (saveFrame(item)) {
                emit contentChanged(d_tableView->itemData(row, 0, Qt::UserRole + 1), name);
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
    {
        QStandardItem *headerItem = d_tableView->horizontalHeaderItem(column);
        if (headerItem) {
            bool changed = false;
            switch (d_object->objectType()) {
            case Icd::ObjectRoot: changed = saveRoot(item); break;
            case Icd::ObjectVehicle: changed = saveVehicle(item); break;
            case Icd::ObjectSystem: changed = saveSystem(item); break;
            case Icd::ObjectTable: changed = saveTable(item); break;
            default:
                break;
            }
            if (changed) {
                emit contentChanged(row, name);
            }
        }
        break;
    }}
}

bool DetailTable::updateRoot()
{
    QStringList labels;
    labels << tr("Name") << tr("Mark") << tr("Describe");
    d_tableView->setHorizontalHeaderLabels(labels);
    d_tableView->horizontalHeaderItem(0)->setData("name");
    d_tableView->horizontalHeaderItem(1)->setData("mark");
    d_tableView->horizontalHeaderItem(2)->setData("desc");

    const Icd::RootPtr root = JHandlePtrCast<Icd::Root, Icd::Object>(d_object);
    if (!root) {
        return false;
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

    //
    //d_tableView->setItemDelegateForColumn(0, new NameItemDelegate());

    return true;
}

bool DetailTable::updateVehicle()
{
    QStringList labels;
    labels << tr("Name") << tr("Mark") << tr("Describe");
    d_tableView->setHorizontalHeaderLabels(labels);
    d_tableView->horizontalHeaderItem(0)->setData("name");
    d_tableView->horizontalHeaderItem(1)->setData("mark");
    d_tableView->horizontalHeaderItem(2)->setData("desc");

    const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle, Icd::Object>(d_object);
    if (!vehicle) {
        return false;
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

    return true;
}

bool DetailTable::updateSystem()
{
    QStringList labels;
    labels << tr("Name") << tr("Mark") << tr("Length") << tr("Describe");
    d_tableView->setHorizontalHeaderLabels(labels);
    d_tableView->horizontalHeaderItem(0)->setData("name");
    d_tableView->horizontalHeaderItem(1)->setData("mark");
    d_tableView->horizontalHeaderItem(2)->setData("length");
    d_tableView->horizontalHeaderItem(3)->setData("desc");

    const Icd::SystemPtr system = JHandlePtrCast<Icd::System, Icd::Object>(d_object);
    if (!system) {
        return false;
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

    return true;
}

bool DetailTable::updateTable()
{
    QStringList labels;
    labels << tr("Name") << tr("Mark") << tr("Byte index") << tr("Length")
           << tr("Type") << tr("Describe");
    d_tableView->setHorizontalHeaderLabels(labels);
    d_tableView->horizontalHeaderItem(0)->setData("name");
    d_tableView->horizontalHeaderItem(1)->setData("mark");
    d_tableView->horizontalHeaderItem(2)->setData("byteIndex");
    d_tableView->horizontalHeaderItem(3)->setData("length");
    d_tableView->horizontalHeaderItem(4)->setData("type");
    d_tableView->horizontalHeaderItem(5)->setData("desc");

    const Icd::TablePtr table = JHandlePtrCast<Icd::Table, Icd::Object>(d_object);
    if (!table) {
        return false;
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

    return true;
}

bool DetailTable::updateItem()
{
    // header
    QStringList labels;
    labels << tr("Name") << tr("Value");
    d_tableView->setHorizontalHeaderLabels(labels);
    d_tableView->horizontalHeaderItem(0)->setData("name");
    d_tableView->horizontalHeaderItem(1)->setData("value");

    const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(d_object);
    if (!item) {
        return false;
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

    return true;
}

bool DetailTable::updateHead()
{
    const Icd::HeaderItemPtr headerItem =
            JHandlePtrCast<Icd::HeaderItem, Icd::Object>(d_object);
    if (!headerItem) {
        return false;
    }

    int rowIndex = d_tableView->rowCount() - 1;

    // Default value
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Default value"));
    d_tableView->setItemData(rowIndex, 1, "0x" + QString("%1")
                             .arg(uint(headerItem->defaultValue()),
                                  2, 16, QChar('0')).toUpper());

    return true;
}

bool DetailTable::updateCounter()
{
    return true;
}

bool DetailTable::updateCheck()
{
    const Icd::CheckItemPtr checkItem =
            JHandlePtrCast<Icd::CheckItem, Icd::Object>(d_object);
    if (!checkItem) {
        return false;
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

    return true;
}

bool DetailTable::updateFrameCode()
{
    const Icd::FrameCodeItemPtr frameCodeItem =
            JHandlePtrCast<Icd::FrameCodeItem, Icd::Object>(d_object);
    if (!frameCodeItem) {
        return false;
    }

    int rowIndex = d_tableView->rowCount() - 1;

    // Default value
    d_tableView->insertRow(++rowIndex);
    d_tableView->setItemData(rowIndex, 0, tr("Default value"));
    d_tableView->setItemData(rowIndex, 1, "0x" + QString("%1")
                             .arg(qulonglong(frameCodeItem->defaultValue()),
                                  Icd::asciiCountOfSize(16, int(frameCodeItem->bufferSize())),
                                  16, QChar('0')).toUpper());

    return true;
}

bool DetailTable::updateNumeric()
{
    return true;
}

bool DetailTable::updateBitMap()
{
    return true;
}

bool DetailTable::updateBitValue()
{
    return true;
}

bool DetailTable::updateComplex()
{
    return true;
}

bool DetailTable::updateFrame()
{
    // header
    QStringList labels;
    labels << tr("Name") << tr("Frame code") << tr("Sequence") << tr("Describe");
    d_tableView->setHorizontalHeaderLabels(labels);
    d_tableView->horizontalHeaderItem(0)->setData("name");
    d_tableView->horizontalHeaderItem(1)->setData("code");
    d_tableView->horizontalHeaderItem(2)->setData("sequence");
    d_tableView->horizontalHeaderItem(3)->setData("desc");

    const Icd::FrameItemPtr frameItem = JHandlePtrCast<Icd::FrameItem, Icd::Object>(d_object);
    if (!frameItem) {
        return false;
    }

    const Icd::TablePtrMap &tables = frameItem->allTable();

    d_tableView->setRowCount(tables.size());
    const int rowCount = d_tableView->rowCount();
    int row = 0;
    foreach (auto &item, tables) {
        const Icd::TablePtr table = item.second;
        if (table) {
            d_tableView->setItemData(row, 0, QString::fromStdString(table->name()));
            d_tableView->setItemData(row, 0, item.first, Qt::UserRole + 1);
            d_tableView->setItemData(row, 1, "0x" + QString("%1").arg(item.first, 0, 16).toUpper());
            d_tableView->setItemData(row, 2, QString::number(table->sequence()));
            d_tableView->setItemData(row, 3, QString::fromStdString(table->desc()));
        }
        if (++row >= rowCount) {
            break;
        }
    }

    return true;
}

bool DetailTable::saveRoot(QStandardItem *item)
{
    if (!item) {
        return false;
    }

    const Icd::RootPtr root = JHandlePtrCast<Icd::Root, Icd::Object>(d_object);
    if (!root) {
        return false;
    }

    const Icd::VehiclePtr vehicle = root->vehicleAt(item->row());
    if (!vehicle) {
        return false;
    }

    switch (item->column()) {
    case 0:
        vehicle->setName(item->text().toStdString());
        break;
    case 1:
        vehicle->setMark(item->text().toStdString());
        break;
    case 2:
        vehicle->setDesc(item->text().toStdString());
        break;
    default:
        return false;
    }

    return true;
}

bool DetailTable::saveVehicle(QStandardItem *item)
{
    if (!item) {
        return false;
    }

    const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle, Icd::Object>(d_object);
    if (!vehicle) {
        return false;
    }

    const Icd::SystemPtr system = vehicle->systemAt(item->row());
    if (!system) {
        return false;
    }

    switch (item->column()) {
    case 0:
        system->setName(item->text().toStdString());
        break;
    case 1:
        system->setMark(item->text().toStdString());
        break;
    case 2:
        system->setDesc(item->text().toStdString());
        break;
    default:
        return false;
    }

    return true;
}

bool DetailTable::saveSystem(QStandardItem *item)
{
    if (!item) {
        return false;
    }

    const Icd::SystemPtr system = JHandlePtrCast<Icd::System, Icd::Object>(d_object);
    if (!system) {
        return false;
    }

    const Icd::TablePtr table = system->tableAt(item->row());
    if (!table) {
        return false;
    }

    switch (item->column()) {
    case 0:
        table->setName(item->text().toStdString());
        break;
    case 1:
        table->setMark(item->text().toStdString());
        break;
    case 3:
        table->setDesc(item->text().toStdString());
        break;
    default:
        return false;
    }

    return true;
}

bool DetailTable::saveTable(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveItem(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveHead(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveCounter(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveCheck(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveFrameCode(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveNumeric(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveBitMap(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveBitValue(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveComplex(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveFrame(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

void DetailTable::enableChangedNotify(bool enabled)
{
    disconnect(d_tableView, &JTableView::itemChanged, this, &DetailTable::onContentChanged);

    if (enabled) {
        connect(d_tableView, &JTableView::itemChanged, this, &DetailTable::onContentChanged);
    }
}

}

#include "precomp.h"
#include "DetailTable.h"
#include "ViewDelegate.h"

namespace Edit {

DetailTable::DetailTable(QWidget *parent)
    : QWidget(parent)
    , object_(nullptr)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    tableView_ = new JTableView(this);
    delegate_ = new ViewDelegate(tableView_);
    tableView_->setItemDelegate(delegate_);
    vertLayoutMain->addWidget(tableView_);

    connect(tableView_, &JTableView::currentCellChanged, this,
            [=](int currentRow, int currentColumn, int previousRow, int previuosColumn){
        Q_UNUSED(currentColumn);
        Q_UNUSED(previuosColumn);
        if (currentRow == previousRow) {
            return;
        }
        if (!object_) {
            emit currentItemChanged(QVariant::Invalid);
            return;
        }
        switch (object_->objectType()) {
        case Icd::ObjectItem:
        {
            const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(object_);
            if (!item) {
                emit currentItemChanged(QVariant::Invalid);
                return;
            }
            switch (item->type()) {
            case Icd::ItemFrame:
                emit currentItemChanged(tableView_->itemData(currentRow, 0, Qt::UserRole + 1));
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
    tableView_->clear();
    object_ = nullptr;
}

void DetailTable::updateView(const Icd::ObjectPtr &object)
{
    resetView();

    if (!object) {
        return;
    }

    object_ = object;

    bool result = false;

    switch (object->objectType()) {
    case Icd::ObjectRoot: result = updateRoot(); break;
    case Icd::ObjectVehicle: result = updateVehicle(); break;
    case Icd::ObjectSystem: result = updateSystem(); break;
    case Icd::ObjectTable: result = updateTable(); break;
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(object_);
        if (!item) {
            break;
        }

        switch (item->type()) {
        case Icd::ItemFrame: result = updateFrame(); break;
        default: result = updateItem(); break;
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
    return object_;
}

void DetailTable::onContentChanged(QStandardItem *item)
{
    if (!item) {
        return;
    }

    const int row = item->row();
    const int column = item->column();
    QStandardItem *headerItem = tableView_->horizontalHeaderItem(column);
    if (!headerItem) {
        return;
    }

    const QString name = headerItem->data().toString();

    switch (object_->objectType()) {
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr icdItem = JHandlePtrCast<Icd::Item>(object_);
        if (!icdItem) {
            return;
        }
        switch (icdItem->type()) {
        case Icd::ItemFrame:
            if (saveFrame(item)) {
                emit contentChanged(tableView_->itemData(row, 0, Qt::UserRole + 1), name);
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
    {
        QStandardItem *headerItem = tableView_->horizontalHeaderItem(column);
        if (headerItem) {
            bool changed = false;
            switch (object_->objectType()) {
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
    tableView_->setHorizontalHeaderLabels(labels);
    tableView_->horizontalHeaderItem(0)->setData("name");
    tableView_->horizontalHeaderItem(1)->setData("mark");
    tableView_->horizontalHeaderItem(2)->setData("desc");

    const Icd::RootPtr root = JHandlePtrCast<Icd::Root>(object_);
    if (!root) {
        return false;
    }

    const Icd::VehiclePtrArray &vehicles = root->allVehicle();

    tableView_->setRowCount(vehicles.size());
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::VehiclePtr vehicle = vehicles.at(row);
        if (vehicle) {
            tableView_->setItemData(row, 0, QString::fromStdString(vehicle->name()));
            tableView_->setItemData(row, 1, QString::fromStdString(vehicle->mark()));
            tableView_->setItemData(row, 2, QString::fromStdString(vehicle->desc()));
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
    tableView_->setHorizontalHeaderLabels(labels);
    tableView_->horizontalHeaderItem(0)->setData("name");
    tableView_->horizontalHeaderItem(1)->setData("mark");
    tableView_->horizontalHeaderItem(2)->setData("desc");

    const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle>(object_);
    if (!vehicle) {
        return false;
    }

    const Icd::SystemPtrArray &systems = vehicle->allSystem();

    tableView_->setRowCount(systems.size());
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::SystemPtr system = systems.at(row);
        if (system) {
            tableView_->setItemData(row, 0, QString::fromStdString(system->name()));
            tableView_->setItemData(row, 1, QString::fromStdString(system->mark()));
            tableView_->setItemData(row, 2, QString::fromStdString(system->desc()));
        }
    }

    return true;
}

bool DetailTable::updateSystem()
{
    QStringList labels;
    labels << tr("Name") << tr("Mark") << tr("Length") << tr("Describe");
    tableView_->setHorizontalHeaderLabels(labels);
    tableView_->setColumnWidth(2, 80);
    tableView_->horizontalHeaderItem(0)->setData("name");
    tableView_->horizontalHeaderItem(1)->setData("mark");
    tableView_->horizontalHeaderItem(2)->setData("length");
    tableView_->horizontalHeaderItem(3)->setData("desc");

    const Icd::SystemPtr system = JHandlePtrCast<Icd::System>(object_);
    if (!system) {
        return false;
    }

    const Icd::TablePtrArray &tables = system->allTable();

    tableView_->setRowCount(tables.size());
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::TablePtr table = tables.at(row);
        if (table) {
            tableView_->setItemData(row, 0, QString::fromStdString(table->name()));
            tableView_->setItemData(row, 1, QString::fromStdString(table->mark()));
            tableView_->setItemData(row, 2, QString::number(table->bufferSize()));
            tableView_->setItemData(row, 3, QString::fromStdString(table->desc()));
        }
    }

    return true;
}

bool DetailTable::updateTable()
{
    QStringList labels;
    labels << tr("Name") << tr("Mark") << tr("Byte index") << tr("Length")
           << tr("Type") << tr("Describe");
    tableView_->setHorizontalHeaderLabels(labels);
    tableView_->setColumnWidth(2, 80);
    tableView_->setColumnWidth(3, 80);
    tableView_->setColumnWidth(4, 100);
    tableView_->horizontalHeaderItem(0)->setData("name");
    tableView_->horizontalHeaderItem(1)->setData("mark");
    tableView_->horizontalHeaderItem(2)->setData("byteIndex");
    tableView_->horizontalHeaderItem(3)->setData("length");
    tableView_->horizontalHeaderItem(4)->setData("type");
    tableView_->horizontalHeaderItem(5)->setData("desc");

    const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(object_);
    if (!table) {
        return false;
    }

    const Icd::ItemPtrArray &items = table->allItem();

    tableView_->setRowCount(items.size());
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const Icd::ItemPtr item = items.at(row);
        if (item) {
            const double bufferOffset = item->bufferOffset() - table->bufferOffset();
            //
            tableView_->setItemData(row, 0, QString::fromStdString(item->name()));
            tableView_->setItemData(row, 1, QString::fromStdString(item->mark()));
            // size
            switch (item->type()) {
            case Icd::ItemBitMap:
            case Icd::ItemBitValue:
            {
                const Icd::BitItemPtr bit = JHandlePtrCast<Icd::BitItem>(item);
                if (!bit) {
                    break;
                }
                tableView_->setItemData(row, 2, QString("%1 (%2)").arg(bufferOffset)
                                        .arg(bit->bitStart(), 2, 10, QChar('0')));
                tableView_->setItemData(row, 3, QString("%1/%2").arg(bit->bitCount())
                                        .arg(bit->typeSize() * 8));
                break;
            }
            default:
                tableView_->setItemData(row, 2, QString::number(bufferOffset));
                tableView_->setItemData(row, 3, QString::number(item->bufferSize()));
                break;
            }
            tableView_->setItemData(row, 4, IcdWidget::typeString(item));
            tableView_->setItemData(row, 5, QString::fromStdString(item->desc()));
        }
    }

    return true;
}

bool DetailTable::updateItem()
{
    // header
    QStringList labels;
    labels << tr("Name") << tr("Value");
    tableView_->setHorizontalHeaderLabels(labels);
    tableView_->setColumnWidth(0, 80);
    tableView_->horizontalHeaderItem(0)->setData("name");
    tableView_->horizontalHeaderItem(1)->setData("value");

    const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(object_);
    if (!item) {
        return false;
    }

    double bufferOffset = item->bufferOffset();
    Icd::Table *table = dynamic_cast<Icd::Table*>(item->parent());
    if (table) {
        bufferOffset -= table->bufferOffset();
    }

    int rowIndex = tableView_->rowCount() - 1;

    // type of data
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Type of data"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::typeString(item));
    // name of data
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Name of data"));
    tableView_->setItemData(rowIndex, 1, QString::fromStdString(item->name()));
    // Mark of data
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Mark of data"));
    tableView_->setItemData(rowIndex, 1, QString::fromStdString(item->mark()));
    // Offset of data
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Offset of data"));
    tableView_->setItemData(rowIndex, 1, QString::number(bufferOffset));
    // Length of data
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Length of data"));
    tableView_->setItemData(rowIndex, 1, QString::number(item->bufferSize()));
    // details
    switch (item->type()) {
    case Icd::ItemHeader: updateHeader(); break;
    case Icd::ItemCounter: updateCounter(); break;
    case Icd::ItemCheck: updateCheck(); break;
    case Icd::ItemFrameCode: updateFrameCode(); break;
    case Icd::ItemNumeric: updateNumeric(); break;
    case Icd::ItemArray: updateArray(); break;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue: updateBit(); break;
    default:
        break;
    }
    // Describe
    rowIndex = tableView_->rowCount() - 1;
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Describe"));
    tableView_->setItemData(rowIndex, 1, QString::fromStdString(item->desc()));
    tableView_->resizeRowToContents(rowIndex);

    return true;
}

bool DetailTable::updateHeader()
{
    const Icd::HeaderItemPtr headerItem = JHandlePtrCast<Icd::HeaderItem>(object_);
    if (!headerItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, "0x" + QString("%1 (%2)")
                            .arg(uint(headerItem->defaultValue()),
                                 2, 16, QChar('0')).toUpper()
                            .arg(uint(headerItem->defaultValue())));

    return true;
}

bool DetailTable::updateCounter()
{
    const Icd::CounterItemPtr counterItem = JHandlePtrCast<Icd::CounterItem>(object_);
    if (!counterItem) {
        return false;
    }

    //int rowIndex = tableView_->rowCount() - 1;

    return true;
}

bool DetailTable::updateCheck()
{
    const Icd::CheckItemPtr checkItem = JHandlePtrCast<Icd::CheckItem>(object_);
    if (!checkItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // Begin at
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Begin at"));
    tableView_->setItemData(rowIndex, 1, QString::number(checkItem->startPos()));
    // End at
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("End at"));
    tableView_->setItemData(rowIndex, 1, QString::number(checkItem->endPos()));

    return true;
}

bool DetailTable::updateFrameCode()
{
    const Icd::FrameCodeItemPtr frameCodeItem = JHandlePtrCast<Icd::FrameCodeItem>(object_);
    if (!frameCodeItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // binding
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Binding frame"));
    const Icd::FrameItemPtr frame = frameCodeItem->frame();
    if (frame) {
        tableView_->setItemData(rowIndex, 1, tr("Binding <%1>")
                                .arg(QString::fromStdString(frame->name())));
    } else {
        tableView_->setItemData(rowIndex, 1, tr("Not binding"));
    }

    return true;
}

bool DetailTable::updateNumeric()
{
    const Icd::NumericItemPtr numericItem = JHandlePtrCast<Icd::NumericItem>(object_);
    if (!numericItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // offset
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Offset"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(numericItem->offset()));
    // scale
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Scale"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(numericItem->scale()));
    // unit
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Unit"));
    tableView_->setItemData(rowIndex, 1, QString::fromStdString(numericItem->unit()));
    // range
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Range"));
    auto range = numericItem->valueRange();
    tableView_->setItemData(rowIndex, 1, QString("[%1, %2]")
                            .arg(IcdWidget::prettyValue(range.first))
                            .arg(IcdWidget::prettyValue(range.second)));
    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(numericItem->defaultValue()));
    // specs
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Specs"));
    const auto &specs = numericItem->specs();
    QStringList sections;
    for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer) {
        sections.append(tr("%1: %2").arg(IcdWidget::prettyValue(citer->first))
                        .arg(QString::fromStdString(citer->second)));
    }
    tableView_->setItemData(rowIndex, 1, sections.join("\n"));
    tableView_->resizeRowToContents(rowIndex);

    return true;
}

bool DetailTable::updateArray()
{
    const Icd::ArrayItemPtr arrayItem = JHandlePtrCast<Icd::ArrayItem>(object_);
    if (!arrayItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    return true;
}

bool DetailTable::updateBit()
{
    const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem>(object_);
    if (!bitItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // Length of data
    tableView_->setItemData(rowIndex, 1, QString("%1/%2")
                            .arg(bitItem->bitCount()).arg(bitItem->typeSize() * 8));
    // range of bit
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Range of bit"));
    tableView_->setItemData(rowIndex, 1, QString("%1 ~ %2")
                            .arg(bitItem->bitStart())
                            .arg(bitItem->bitStart() + bitItem->bitCount() - 1));
    // details
    switch (bitItem->type()) {
    case Icd::ItemBitMap: updateBitMap(); break;
    case Icd::ItemBitValue: updateBitValue(); break;
    default:
        break;
    }
    // specs
    rowIndex = tableView_->rowCount() - 1;
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Specs"));
    const auto &specs = bitItem->specs();
    QStringList sections;
    for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer) {
        if (bitItem->type() == Icd::ItemBitMap) {
            sections.append(tr("%1: %2").arg(QString("%1")
                                             .arg(uint(citer->first), bitItem->calcSize() * 2,
                                                  16, QChar('0')).toUpper())
                            .arg(QString::fromStdString(citer->second)));
        } else {
            sections.append(tr("%1: %2").arg(QString::number(citer->first))
                            .arg(QString::fromStdString(citer->second)));
        }
    }
    tableView_->setItemData(rowIndex, 1, sections.join("\n"));
    tableView_->resizeRowToContents(rowIndex);

    return true;
}

bool DetailTable::updateBitMap()
{
    const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem>(object_);
    if (!bitItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, "0x" + QString("%1 (%2)")
                            .arg(uint(bitItem->defaultValue()),
                                 bitItem->calcSize() * 2, 16, QChar('0')).toUpper()
                            .arg(uint(bitItem->defaultValue())));

    return true;
}

bool DetailTable::updateBitValue()
{
    const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem>(object_);
    if (!bitItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // offset
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Offset"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(bitItem->offset()));
    // scale
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Scale"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(bitItem->scale()));
    // unit
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Unit"));
    tableView_->setItemData(rowIndex, 1, QString::fromStdString(bitItem->unit()));
    // range
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Range"));
    auto range = bitItem->valueRange();
    tableView_->setItemData(rowIndex, 1, QString("[%1, %2]")
                            .arg(IcdWidget::prettyValue(range.first))
                            .arg(IcdWidget::prettyValue(range.second)));
    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(bitItem->defaultValue()));

    return true;
}

bool DetailTable::updateComplex()
{
    const Icd::ComplexItemPtr complexItem = JHandlePtrCast<Icd::ComplexItem>(object_);
    if (!complexItem) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    return true;
}

bool DetailTable::updateFrame()
{
    // header
    QStringList labels;
    labels << tr("Name") << tr("Frame code") << tr("Sequence") << tr("Describe");
    tableView_->setHorizontalHeaderLabels(labels);
    tableView_->horizontalHeaderItem(0)->setData("name");
    tableView_->horizontalHeaderItem(1)->setData("code");
    tableView_->horizontalHeaderItem(2)->setData("sequence");
    tableView_->horizontalHeaderItem(3)->setData("desc");

    const Icd::FrameItemPtr frameItem = JHandlePtrCast<Icd::FrameItem>(object_);
    if (!frameItem) {
        return false;
    }

    const Icd::TablePtrMap &tables = frameItem->allTable();

    tableView_->setRowCount(tables.size());
    const int rowCount = tableView_->rowCount();
    int row = 0;
    foreach (auto &item, tables) {
        const Icd::TablePtr table = item.second;
        if (table) {
            tableView_->setItemData(row, 0, QString::fromStdString(table->name()));
            tableView_->setItemData(row, 0, item.first, Qt::UserRole + 1);
            tableView_->setItemData(row, 1, "0x" + QString::fromStdString(table->mark()).toUpper());
            tableView_->setItemData(row, 2, QString::number(table->sequence()));
            tableView_->setItemData(row, 3, QString::fromStdString(table->desc()));
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

    const Icd::RootPtr root = JHandlePtrCast<Icd::Root>(object_);
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

    const Icd::VehiclePtr vehicle = JHandlePtrCast<Icd::Vehicle>(object_);
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

    const Icd::SystemPtr system = JHandlePtrCast<Icd::System>(object_);
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

bool DetailTable::saveArray(QStandardItem *item)
{
    Q_UNUSED(item);
    return true;
}

bool DetailTable::saveBit(QStandardItem *item)
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
    disconnect(tableView_, &JTableView::itemChanged, this, &DetailTable::onContentChanged);

    if (enabled) {
        connect(tableView_, &JTableView::itemChanged, this, &DetailTable::onContentChanged);
    }
}

}

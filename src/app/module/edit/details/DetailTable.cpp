#include "precomp.h"
#include "DetailTable.h"
#include "ViewDelegate.h"

namespace Edit {

DetailTable::DetailTable(QWidget *parent)
    : QWidget(parent)
    , object_(nullptr)
    , editing_(false)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    tableView_ = new JTableView(this);
    tableView_->setContextMenuPolicy(Qt::ActionsContextMenu);
    delegate_ = new ViewDelegate(tableView_);
    tableView_->setItemDelegate(delegate_);
    vertLayoutMain->addWidget(tableView_);

    labelTip_ = new QLabel(this);
    labelTip_->setProperty("_tip_", true);
    labelTip_->setFixedHeight(22);
    labelTip_->setAlignment(Qt::AlignCenter);
    labelTip_->hide();
    vertLayoutMain->addWidget(labelTip_);

    connect(tableView_, &JTableView::customContextMenuRequested,
            this, &DetailTable::showContextMenu);
    connect(tableView_, &JTableView::currentCellChanged, this,
            [=](int currentRow, int currentColumn, int previousRow, int previuosColumn){
        Q_UNUSED(currentColumn);
        Q_UNUSED(previuosColumn);
        if (currentRow == previousRow) {
            return;
        }
        if (!object_) {
            emit currentItemChanged(QVariant::Invalid, newObject_);
            return;
        }
        if (object_->rtti() == Icd::ObjectFrame) {
            emit currentItemChanged(tableView_->itemData(currentRow, 0, Qt::UserRole + 1), newObject_);
        } else {
            emit currentItemChanged(currentRow, newObject_);
        }
    });
}

void DetailTable::resetView()
{
    cancelInsert();
    tableView_->clear();
    object_.reset();
    newObject_.reset();
    setTip(QString());
}

void DetailTable::updateView(const Icd::ObjectPtr &object)
{
    resetView();

    if (!object) {
        return;
    }

    object_ = object;

    bool result = false;

    switch (object->rtti()) {
    case Icd::ObjectRoot: result = updateRoot(); break;
    case Icd::ObjectVehicle: result = updateVehicle(); break;
    case Icd::ObjectSystem: result = updateSystem(); break;
    case Icd::ObjectTable: result = updateTable(); break;
    case Icd::ObjectFrame: result = updateFrame(); break;
    default: result = updateItem(); break;
    }
}

Icd::ObjectPtr DetailTable::object() const
{
    return object_;
}

void DetailTable::setTip(const QString &text) const
{
    labelTip_->setText(text);
    labelTip_->setVisible(!text.isEmpty());
}

bool DetailTable::isEditing() const
{
    return editing_;
}

void DetailTable::setEditing(bool editing)
{
    editing_ = editing;
}

int DetailTable::rowCount() const
{
    return tableView_->rowCount();
}

int DetailTable::currentRow() const
{
    return tableView_->currentRow();
}

Icd::icd_uint64 DetailTable::currentIndex() const
{
    const int currentRow = tableView_->currentRow();

    if (object_ && object_->rtti() == Icd::ObjectFrame) {
        return tableView_->itemData(currentRow, 0, Qt::UserRole + 1).toULongLong();
    }

    return Icd::icd_uint64(currentRow);
}

void DetailTable::insertRow(int row, const Icd::VehiclePtr &vehicle)
{
    if (!vehicle) {
        return;
    }

    row = insertRow(row, JHandlePtrCast<Icd::Object>(vehicle));

    setRowData(row, vehicle);

    tableView_->selectRow(row);
}

void DetailTable::insertRow(int row, const Icd::SystemPtr &system)
{
    if (!system) {
        return;
    }

    row = insertRow(row, JHandlePtrCast<Icd::Object>(system));

    setRowData(row, system);

    tableView_->selectRow(row);
}

void DetailTable::insertRow(int row, const Icd::TablePtr &table)
{
    if (!table) {
        return;
    }

    row = insertRow(row, JHandlePtrCast<Icd::Object>(table));

    setRowData(row, table);

    tableView_->selectRow(row);
}

void DetailTable::updateRow(int row)
{
    if (!object_ || row < 0) {
        return;
    }

    switch (object_->rtti()) {
    case Icd::ObjectRoot:
    {
        auto root = JHandlePtrCast<Icd::Root>(object_);
        if (!root) {
            break;
        }
        auto vehicle = root->vehicleAt(row);
        if (!vehicle) {
            break;
        }
        setRowData(row, vehicle);
        break;
    }
    case Icd::ObjectVehicle:
    {
        auto vehicle = JHandlePtrCast<Icd::Vehicle>(object_);
        if (!vehicle) {
            break;
        }
        auto system = vehicle->systemAt(row);
        if (!system) {
            break;
        }
        setRowData(row, system);
        break;
    }
    case Icd::ObjectSystem:
    {
        auto system = JHandlePtrCast<Icd::System>(object_);
        if (!system) {
            break;
        }
        auto table = system->tableAt(row);
        if (!table) {
            break;
        }
        setRowData(row, table);
        break;
    }
    case Icd::ObjectTable:
    {
        auto table = JHandlePtrCast<Icd::Table>(object_);
        if (!table) {
            break;
        }
        auto item = table->itemAt(row);
        if (!item) {
            break;
        }
        setRowData(row, item, table->bufferOffset());
        break;
    }
    case Icd::ObjectFrame:
    {
        auto frame = JHandlePtrCast<Icd::FrameItem>(object_);
        if (!frame) {
            break;
        }
#if 0 //TODO
        auto item = frame->tableAt(row);
        if (!item) {
            break;
        }
        setRowData(row, item, table->bufferOffset());
#endif
        break;
    }
    case Icd::ObjectComplex:
    {
        auto complex = JHandlePtrCast<Icd::ComplexItem>(object_);
        if (!complex) {
            break;
        }
        //TODO
        setRowData(row, complex->table());
        break;
    }
    default:
        break;
    }
}

void DetailTable::applyInsert()
{
    if (newObject_) {
        newObject_.reset();
    }

    updateRow(tableView_->currentRow());
}

void DetailTable::cancelInsert()
{
    if (newObject_) {
        tableView_->clearSelection();
        tableView_->removeRow(tableView_->currentRow());
        newObject_.reset();
        //TODO restore offset and size
    }
}

void DetailTable::showContextMenu(const QPoint &pos)
{
    if (editing_) {
        return;
    }

    QStandardItem *selectedItem = tableView_->itemAt(pos);

    QMenu menu(this);

    // append
    // remove/copy
    if (selectedItem) {
        // remove
        QAction *actionRemove = menu.addAction(QIcon(":icdwidget/tree/remove.png"),
                                               tr("Remove"));
        connect(actionRemove, &QAction::triggered, this, [=](){
            //
        });
        // copy
    }
    // past
    if (false) {

    }
    // insert above
    // insert below
    // clean
    if (tableView_->rowCount() > 0) {
        QAction *actionClean = menu.addAction(QIcon(":/icdwidget/tree/clean.png"),
                                              tr("Clean"));
        connect(actionClean, &QAction::triggered, this, [=](){
            //
        });
    }

    if (menu.isEmpty()) {
        return;
    }

    menu.exec(pos);
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

    tableView_->setRowCount(static_cast<int>(vehicles.size()));
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        setRowData(row, vehicles.at(size_t(row)));
    }

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

    tableView_->setRowCount(static_cast<int>(systems.size()));
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        setRowData(row, systems.at(size_t(row)));
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

    tableView_->setRowCount(static_cast<int>(tables.size()));
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        setRowData(row, tables.at(size_t(row)));
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

    tableView_->setRowCount(static_cast<int>(items.size()));
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        setRowData(row, items.at(size_t(row)), table->bufferOffset());
    }

    setTip(tr("Size: %1").arg(table->bufferSize()));

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

    //int rowIndex = tableView_->rowCount() - 1;

    // count
    //tableView_->insertRow(++rowIndex);
    //tableView_->setItem();

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

    //int rowIndex = tableView_->rowCount() - 1;

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

    tableView_->setRowCount(static_cast<int>(tables.size()));
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

int DetailTable::insertRow(int row, const Icd::ObjectPtr &object)
{
    newObject_ = object;

    const int _row = tableView_->rowCount();

    tableView_->insertRow(row);

    if (row < 0) {
        row = 0;
    }

    if (row > _row) {
        row = _row;
    }

    return row;
}

void DetailTable::setRowData(int row, const Icd::VehiclePtr &vehicle)
{
    if (!vehicle) {
        return;
    }

    tableView_->setItemData(row, 0, QString::fromStdString(vehicle->name()));
    tableView_->setItemData(row, 1, QString::fromStdString(vehicle->mark()));
    tableView_->setItemData(row, 2, QString::fromStdString(vehicle->desc()));
}

void DetailTable::setRowData(int row, const Icd::SystemPtr &system)
{
    if (!system) {
        return;
    }

    tableView_->setItemData(row, 0, QString::fromStdString(system->name()));
    tableView_->setItemData(row, 1, QString::fromStdString(system->mark()));
    tableView_->setItemData(row, 2, QString::fromStdString(system->desc()));
}

void DetailTable::setRowData(int row, const Icd::TablePtr &table)
{
    if (!table) {
        return;
    }

    tableView_->setItemData(row, 0, QString::fromStdString(table->name()));
    tableView_->setItemData(row, 1, QString::fromStdString(table->mark()));
    tableView_->setItemData(row, 2, QString::number(table->bufferSize()));
    tableView_->setItemData(row, 3, QString::fromStdString(table->desc()));
}

void DetailTable::setRowData(int row, const Icd::ItemPtr &item, double offset)
{
    if (!item) {
        return;
    }

    const double bufferOffset = item->bufferOffset() - offset;
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

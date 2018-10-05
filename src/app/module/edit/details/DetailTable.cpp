#include "precomp.h"
#include "DetailTable.h"
#include "ViewDelegate.h"

namespace Edit {

// class DetaiLTableView

DetailTableView::DetailTableView(QWidget *parent)
    : JTableView(parent)
    , selectionMode_(QAbstractItemView::NoSelection)
{
    setFocusPolicy(Qt::StrongFocus);
}

void DetailTableView::enableSelect(bool enabled)
{
    if (enabled) {
        const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
        if ((modifiers & Qt::ControlModifier) || (modifiers & Qt::ShiftModifier)) {
            selectionMode_ = QAbstractItemView::ExtendedSelection;
        } else {
            selectionMode_ = QAbstractItemView::SingleSelection;
        }
    } else {
        selectionMode_ = QAbstractItemView::NoSelection;
    }

    setSelectionMode(selectionMode_);
}

void DetailTableView::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if ((modifiers & Qt::ControlModifier) || (modifiers & Qt::ShiftModifier)) {
        setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    JTableView::keyPressEvent(event);
}

void DetailTableView::keyReleaseEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if (!((modifiers & Qt::ControlModifier) && (modifiers & Qt::ShiftModifier))) {
        setSelectionMode(selectionMode_);
        //clearSelection();
    }

    JTableView::keyReleaseEvent(event);
}

// class DetailTable

DetailTable::DetailTable(QWidget *parent)
    : QWidget(parent)
    , object_(nullptr)
    , editing_(false)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    tableView_ = new DetailTableView(this);
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
    connect(tableView_, &JTableView::itemSelectionChanged, this, [=](){
        if (!object_) {
            emit currentItemChanged(QVariant::Invalid, newObject_);
            return;
        }
        if (object_->rtti() == Icd::ObjectFrame) {
            emit currentItemChanged(tableView_->itemData(currentRow(), 0, Qt::UserRole + 1), newObject_);
        } else {
            emit currentItemChanged(currentRow(), newObject_);
        }
    });
}

void DetailTable::resetView()
{
    cancelInsert();
    tableView_->clearSelection();
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

    tableView_->enableSelect(true);

    switch (object->rtti()) {
    case Icd::ObjectRoot: result = updateRoot(); break;
    case Icd::ObjectVehicle: result = updateVehicle(); break;
    case Icd::ObjectSystem: result = updateSystem(); break;
    case Icd::ObjectTable: result = updateTable(); break;
    case Icd::ObjectFrame: result = updateFrame(JHandlePtrCast<Icd::FrameItem>(object)); break;
    default:
        tableView_->enableSelect(false);
        result = updateItem();
        break;
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
    const QList<JTableViewSelectionRange> selected = tableView_->selectRanges();
    if (selected.isEmpty()) {
        return -1;
    } else if (selected.size() > 1) {
        return -1;
    } else {
        const JTableViewSelectionRange &first = selected.first();
        if (first.rowCount() > 1) {
            return -1;
        } else {
            return first.topRow();
        }
    }
}

Icd::icd_uint64 DetailTable::currentIndex() const
{
    const int currentRow = this->currentRow();

    if (object_ && object_->rtti() == Icd::ObjectFrame) {
        return tableView_->itemData(currentRow, 0, Qt::UserRole + 1).toULongLong();
    }

    return Icd::icd_uint64(currentRow);
}

bool DetailTable::isMultiRowSelected() const
{
    const QList<JTableViewSelectionRange> selected = tableView_->selectRanges();
    if (selected.isEmpty()) {
        return false;
    } else if (selected.size() > 1) {
        return true;
    } else {
        return (selected.first().rowCount() > 1);
    }
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

void DetailTable::insertRow(int row, const Icd::ItemPtr &item)
{
    if (!item) {
        return;
    }

    row = insertRow(row, JHandlePtrCast<Icd::Object>(item));

    double offset = 0;
    Icd::Object *parent = item->parent();
    if (parent && parent->objectType() == Icd::ObjectTable) {
        Icd::Table *table = dynamic_cast<Icd::Table*>(parent);
        if (table) {
            offset = table->bufferOffset();
        }
    }

    setRowData(row, item, offset);

    tableView_->selectRow(row);
}

void DetailTable::updateRow(int row)
{
    if (!object_ || row < 0) {
        return;
    }

    tableView_->enableSelect(true);

    switch (object_->objectType()) {
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
    case Icd::ObjectItem:
    {
        auto item = JHandlePtrCast<Icd::Item>(object_);
        switch (item->type()) {
        case Icd::ItemFrame:
        {
            auto frame = JHandlePtrCast<Icd::FrameItem>(item);
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
        case Icd::ItemComplex:
        {
            auto complex = JHandlePtrCast<Icd::ComplexItem>(item);
            if (!complex) {
                break;
            }
            //TODO
            setRowData(row, complex->table());
            break;
        }
        default:
            tableView_->enableSelect(false);
            updateItem(item);
            break;
        }
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

    updateRow(currentRow());
}

void DetailTable::cancelInsert()
{
    if (newObject_) {
        tableView_->clearSelection();
        tableView_->removeRow(currentRow());
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

    updateItem(item);

    return true;
}

bool DetailTable::updateItem(const Icd::ItemPtr &item)
{
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
    case Icd::ItemHeader: updateHeader(JHandlePtrCast<Icd::HeaderItem>(item)); break;
    case Icd::ItemCounter: updateCounter(JHandlePtrCast<Icd::CounterItem>(item)); break;
    case Icd::ItemCheck: updateCheck(JHandlePtrCast<Icd::CheckItem>(item)); break;
    case Icd::ItemFrameCode: updateFrameCode(JHandlePtrCast<Icd::FrameCodeItem>(item)); break;
    case Icd::ItemNumeric: updateNumeric(JHandlePtrCast<Icd::NumericItem>(item)); break;
    case Icd::ItemArray: updateArray(JHandlePtrCast<Icd::ArrayItem>(item)); break;
    case Icd::ItemBitMap:
    case Icd::ItemBitValue: updateBit(JHandlePtrCast<Icd::BitItem>(item)); break;
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

bool DetailTable::updateHeader(const Icd::HeaderItemPtr &header)
{
    if (!header) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, "0x" + QString("%1 (%2)")
                            .arg(uint(header->defaultValue()),
                                 2, 16, QChar('0')).toUpper()
                            .arg(uint(header->defaultValue())));

    return true;
}

bool DetailTable::updateCounter(const Icd::CounterItemPtr &counter)
{
    if (!counter) {
        return false;
    }

    //int rowIndex = tableView_->rowCount() - 1;

    return true;
}

bool DetailTable::updateCheck(const Icd::CheckItemPtr &check)
{
    if (!check) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // Begin at
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Begin at"));
    tableView_->setItemData(rowIndex, 1, QString::number(check->startPos()));
    // End at
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("End at"));
    tableView_->setItemData(rowIndex, 1, QString::number(check->endPos()));

    return true;
}

bool DetailTable::updateFrameCode(const Icd::FrameCodeItemPtr &frameCode)
{
    if (!frameCode) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // binding
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Binding frame"));
    const Icd::FrameItemPtr frame = frameCode->frame();
    if (frame) {
        tableView_->setItemData(rowIndex, 1, tr("Binding <%1>")
                                .arg(QString::fromStdString(frame->name())));
    } else {
        tableView_->setItemData(rowIndex, 1, tr("Not binding"));
    }

    return true;
}

bool DetailTable::updateNumeric(const Icd::NumericItemPtr &numeric)
{
    if (!numeric) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // offset
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Offset"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(numeric->offset()));
    // scale
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Scale"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(numeric->scale()));
    // unit
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Unit"));
    tableView_->setItemData(rowIndex, 1, QString::fromStdString(numeric->unit()));
    // range
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Range"));
    auto range = numeric->valueRange();
    tableView_->setItemData(rowIndex, 1, QString("[%1, %2]")
                            .arg(IcdWidget::prettyValue(range.first))
                            .arg(IcdWidget::prettyValue(range.second)));
    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(numeric->defaultValue()));
    // specs
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Specs"));
    const auto &specs = numeric->specs();
    QStringList sections;
    for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer) {
        sections.append(tr("%1: %2").arg(IcdWidget::prettyValue(citer->first))
                        .arg(QString::fromStdString(citer->second)));
    }
    tableView_->setItemData(rowIndex, 1, sections.join("\n"));
    tableView_->resizeRowToContents(rowIndex);

    return true;
}

bool DetailTable::updateArray(const Icd::ArrayItemPtr &array)
{
    if (!array) {
        return false;
    }

    //int rowIndex = tableView_->rowCount() - 1;

    // count
    //tableView_->insertRow(++rowIndex);
    //tableView_->setItem();

    return true;
}

bool DetailTable::updateBit(const Icd::BitItemPtr &bit)
{
    if (!bit) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // Length of data
    tableView_->setItemData(rowIndex, 1, QString("%1/%2")
                            .arg(bit->bitCount()).arg(bit->typeSize() * 8));
    // range of bit
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Range of bit"));
    tableView_->setItemData(rowIndex, 1, QString("%1 ~ %2")
                            .arg(bit->bitStart())
                            .arg(bit->bitStart() + bit->bitCount() - 1));
    // details
    switch (bit->type()) {
    case Icd::ItemBitMap: updateBitMap(bit); break;
    case Icd::ItemBitValue: updateBitValue(bit); break;
    default:
        break;
    }
    // specs
    rowIndex = tableView_->rowCount() - 1;
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Specs"));
    const auto &specs = bit->specs();
    QStringList sections;
    for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer) {
        if (bit->type() == Icd::ItemBitMap) {
            sections.append(tr("%1: %2").arg(QString("%1")
                                             .arg(uint(citer->first), bit->calcSize() * 2,
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

bool DetailTable::updateBitMap(const Icd::BitItemPtr &bit)
{
    if (!bit) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, "0x" + QString("%1 (%2)")
                            .arg(uint(bit->defaultValue()),
                                 bit->calcSize() * 2, 16, QChar('0')).toUpper()
                            .arg(uint(bit->defaultValue())));

    return true;
}

bool DetailTable::updateBitValue(const Icd::BitItemPtr &bit)
{
    if (!bit) {
        return false;
    }

    int rowIndex = tableView_->rowCount() - 1;

    // offset
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Offset"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(bit->offset()));
    // scale
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Scale"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(bit->scale()));
    // unit
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Unit"));
    tableView_->setItemData(rowIndex, 1, QString::fromStdString(bit->unit()));
    // range
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Range"));
    auto range = bit->valueRange();
    tableView_->setItemData(rowIndex, 1, QString("[%1, %2]")
                            .arg(IcdWidget::prettyValue(range.first))
                            .arg(IcdWidget::prettyValue(range.second)));
    // default value
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Default value"));
    tableView_->setItemData(rowIndex, 1, IcdWidget::prettyValue(bit->defaultValue()));

    return true;
}

bool DetailTable::updateComplex(const Icd::ComplexItemPtr &complex)
{
    if (!complex) {
        return false;
    }

    //int rowIndex = tableView_->rowCount() - 1;

    return true;
}

bool DetailTable::updateFrame(const Icd::FrameItemPtr &frame)
{
    if (!frame) {
        return false;
    }

    // header
    QStringList labels;
    labels << tr("Name") << tr("Frame code") << tr("Sequence") << tr("Describe");
    tableView_->setHorizontalHeaderLabels(labels);
    tableView_->horizontalHeaderItem(0)->setData("name");
    tableView_->horizontalHeaderItem(1)->setData("code");
    tableView_->horizontalHeaderItem(2)->setData("sequence");
    tableView_->horizontalHeaderItem(3)->setData("desc");

    const Icd::TablePtrMap &tables = frame->allTable();

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

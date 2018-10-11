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
    setDragDropMode(QAbstractItemView::DragDrop);
}

void DetailTableView::enableSelection(bool enabled)
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

void DetailTableView::enableDragAndDrop(bool enabled)
{
    setDropIndicatorShown(enabled);
    setDragEnabled(enabled);
    setAcceptDrops(enabled);
}

bool DetailTableView::isMultiRowSelected() const
{
    const QList<JTableViewSelectionRange> selected = selectRanges();
    if (selected.isEmpty()) {
        return false;
    } else if (selected.size() > 1) {
        return true;
    } else {
        return (selected.first().rowCount() > 1);
    }
}

void DetailTableView::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if ((modifiers & Qt::ControlModifier) || (modifiers & Qt::ShiftModifier)) {
        if (selectionMode_ != QAbstractItemView::NoSelection) {
            setSelectionMode(QAbstractItemView::ExtendedSelection);
        }
    }

    JTableView::keyPressEvent(event);
}

void DetailTableView::keyReleaseEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if (!((modifiers & Qt::ControlModifier) || (modifiers & Qt::ShiftModifier))) {
        if (selectionMode_ != QAbstractItemView::NoSelection) {
            setSelectionMode(selectionMode_);
        }
    }

    JTableView::keyReleaseEvent(event);
}

void DetailTableView::mousePressEvent(QMouseEvent *event)
{
    if (!indexAt(event->pos()).isValid()) {
        clearSelection();
    }

    JTableView::mousePressEvent(event);
}

void DetailTableView::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    QByteArray mimeType = mimeData->data("application/x-qabstractitemmodeldatalist");
    if (mimeType.isEmpty()) {
        return;
    }

    QDataStream stream(&mimeType, QIODevice::ReadOnly);
    if (stream.atEnd()) {
        return;
    }

    QStandardItem* dropItem = itemAt(event->pos());
    if(!dropItem) {
        return;
    }

    int row, column;
    QMap<int, QVariant> roleDataMap;
    stream >> row >> column >> roleDataMap;

    const int targetRow = dropItem->row();
    if (targetRow == row) {
        return;
    }

    emit movingDropped(row, targetRow);
}

// class DetailTable

DetailTable::DetailTable(QWidget *parent)
    : QWidget(parent)
    , object_(nullptr)
    , moving_(false)
    , originalRow_(-1)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    tableView_ = new DetailTableView(this);
    delegate_ = new ViewDelegate(tableView_);
    tableView_->setItemDelegate(delegate_);
    vertLayoutMain->addWidget(tableView_);

    labelTip_ = new QLabel(this);
    labelTip_->setProperty("_tip_", true);
    labelTip_->setFixedHeight(22);
    labelTip_->setAlignment(Qt::AlignCenter);
    labelTip_->hide();
    vertLayoutMain->addWidget(labelTip_);

    connect(tableView_, &DetailTableView::customContextMenuRequested,
            this, &DetailTable::showContextMenu);
    connect(tableView_, &JTableView::itemSelectionChanged, this, [=](){
        if (isMoving()) {
            return;
        }
        //
        tableView_->enableDragAndDrop(!tableView_->isMultiRowSelected());
        //
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
    connect(tableView_, &DetailTableView::movingDropped, this, &DetailTable::moveRow);
}

void DetailTable::resetView()
{
    cancel();
    tableView_->clearSelection();
    tableView_->clear();
    object_.reset();
    newObject_.reset();
    originalRow_ = -1;
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

    setContextMenuEnabled(true);
    tableView_->enableSelection(true);
    tableView_->enableDragAndDrop(true);

    switch (object->rtti()) {
    case Icd::ObjectRoot: result = updateRoot(); break;
    case Icd::ObjectVehicle: result = updateVehicle(); break;
    case Icd::ObjectSystem: result = updateSystem(); break;
    case Icd::ObjectTable: result = updateTable(); break;
    case Icd::ObjectComplex: result = updateComplex(JHandlePtrCast<Icd::ComplexItem>(object)); break;
    case Icd::ObjectFrame: result = updateFrame(JHandlePtrCast<Icd::FrameItem>(object)); break;
    default:
        setContextMenuEnabled(false);
        tableView_->enableSelection(false);
        tableView_->enableDragAndDrop(false);
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

bool DetailTable::isModified() const
{
    if (parent()) {
        return parent()->property("modified").toBool();
    } else {
        return false;
    }
}


bool DetailTable::isMoving() const
{
    return moving_;
}

bool DetailTable::isMoved() const
{
    return (originalRow_ != -1);
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

int DetailTable::originalRow() const
{
    return originalRow_;
}

bool DetailTable::isMultiRowSelected() const
{
    return tableView_->isMultiRowSelected();
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

void DetailTable::moveCurrentRow(bool up)
{
    int currentRow = tableView_->currentRow();
    if (currentRow == -1) {
        return;
    }

    moveRow(currentRow, currentRow + (up ? -1 : 1));
}

void DetailTable::updateRow(int row, const QVariant &data)
{
    if (!object_ || row < 0) {
        return;
    }

    tableView_->enableSelection(true);
    tableView_->enableDragAndDrop(true);

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

            auto table = frame->tableAt(data.toULongLong());
            if (!table) {
                break;
            }
            setRowData(row, table);

            break;
        }
        case Icd::ItemComplex:
        {
            auto complex = JHandlePtrCast<Icd::ComplexItem>(item);
            if (!complex) {
                break;
            }

            auto table = complex->table();
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
        default:
            tableView_->enableSelection(false);
            tableView_->enableDragAndDrop(false);
            updateItem(item);
            break;
        }
        break;
    }
    default:
        break;
    }
}

bool DetailTable::apply(const Icd::ObjectPtr &target, int row)
{
    if (!object_ || !target) {
        return false;
    }

    if (newObject_) {
        newObject_.reset();
    }

    if (object_ == target) {
        return true;
    }

    switch (object_->objectType()) {
    case Icd::ObjectItem:
    {
        if (row == -1 && object_->rtti() != target->rtti()) {    // type changed
            updateView(target);
            return true;
        } else {
            switch (object_->rtti()) {
            case Icd::ObjectComplex:
            {
                updateRow(row);
                break;
            }
            case Icd::ObjectFrame:
            {
                if (target->objectType() != Icd::ObjectTable) {
                    return false;
                }
                auto table = JHandlePtrCast<Icd::Table>(target);
                if (!table) {
                    return false;
                }
                updateRow(row, table->frameCode());
                break;
            }
            default:
                object_ = target;
                tableView_->clearContents();
                updateItem(JHandlePtrCast<Icd::Item>(object_));
                break;
            }
        }
        break;
    }
    default:
        updateRow(row);
        break;
    }

    // update offset and size
    updateOffsetAndSize();

    //
    originalRow_ = -1;

    return true;
}

void DetailTable::cancel()
{
    if (newObject_) {               // just remove addded row
        tableView_->clearSelection();
        tableView_->removeRow(currentRow());
        newObject_.reset();
    } else if (originalRow_ >= 0) { // restore original row of moved row
        moveRow(currentRow(), originalRow_);
    }

    originalRow_ = -1;
}

void DetailTable::showContextMenu(const QPoint &pos)
{
    Q_UNUSED(pos);
    if (isModified() || !object_) {
        return;
    }

    if (object_->objectType() == Icd::ObjectItem
            && (object_->rtti() != Icd::ObjectFrame
                && object_->rtti() != Icd::ObjectComplex)) {
        return;
    }

    const QList<JTableViewSelectionRange> selected = tableView_->selectRanges();

    QMenu menu(this);

    // add
    QAction *actionAdd = menu.addAction(QIcon(":icdwidget/image/tree/add.png"),
                                        tr("Add"));
    connect(actionAdd, &QAction::triggered, this, [=](){
        //
    });
    if (!selected.isEmpty()) {
        // insert above
        QAction *actionInsertAbove = menu.addAction(QIcon(":icdwidget/image/tree/insert-above.png"),
                                                    tr("Insert above"));
        connect(actionInsertAbove, &QAction::triggered, this, [=](){
            //
        });
        // insert below
        QAction *actionInsertBelow = menu.addAction(QIcon(":icdwidget/image/tree/insert-below.png"),
                                                    tr("Insert below"));
        connect(actionInsertBelow, &QAction::triggered, this, [=](){
            //
        });
    }
    //
    bool hasPast = false;
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard && clipboard->ownsClipboard()) {
        const QMimeData *mimeData = clipboard->mimeData();
        if (mimeData) {
            if (mimeData->hasFormat("icdwidget/domain")
                    || mimeData->hasFormat("detailtable/domain")) {
                hasPast = true;
                // past - append
                QAction *actionPast = menu.addAction(QIcon(":icdwidget/image/tree/past.png"),
                                                     tr("Past"));
                connect(actionPast, &QAction::triggered, this, [=](){
                    //
                });
                // past - above
                QAction *actionPastAbove = menu.addAction(QIcon(":icdwidget/image/tree/past-above.png"),
                                                          tr("Past above"));
                connect(actionPastAbove, &QAction::triggered, this, [=](){
                    //
                });
                // past - below
                QAction *actionPastBelow = menu.addAction(QIcon(":icdwidget/image/tree/past-below.png"),
                                                          tr("Past below"));
                connect(actionPastBelow, &QAction::triggered, this, [=](){
                    //
                });
            }
        }
    }
    // copy
    if (!hasPast && selected.size() == 1) {
        // copy
        QAction *actionCopy = menu.addAction(QIcon(":icdwidget/image/tree/copy.png"),
                                             tr("Copy"));
        connect(actionCopy, &QAction::triggered, this, [=](){
            //
        });
        // clone
        QAction *actionClone = menu.addAction(QIcon(":icdwidget/image/tree/clone.png"),
                                              tr("Clone"));
        connect(actionClone, &QAction::triggered, this, [=](){
            //
        });
    }
    // remove
    if (!selected.isEmpty()) {
        QAction *actionRemove = menu.addAction(QIcon(":icdwidget/image/tree/remove.png"),
                                               tr("Remove"));
        connect(actionRemove, &QAction::triggered, this, [=](){
            //
        });
    }
    // clean
    if (tableView_->rowCount() > 0) {
        QAction *actionClean = menu.addAction(QIcon(":/icdwidget/image/tree/clean.png"),
                                              tr("Clean"));
        connect(actionClean, &QAction::triggered, this, [=](){
            //
        });
    }

    if (menu.isEmpty()) {
        return;
    }

    menu.exec(QCursor::pos());
}

void DetailTable::setContextMenuEnabled(bool enabled)
{
    if (enabled) {
        tableView_->setContextMenuPolicy(Qt::CustomContextMenu);
    } else {
        tableView_->setContextMenuPolicy(Qt::NoContextMenu);
    }
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
    return updateTable(JHandlePtrCast<Icd::Table>(object_));
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

bool DetailTable::updateTable(const Icd::TablePtr &table)
{
    if (!table) {
        return false;
    }

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

    const Icd::ItemPtrArray &items = table->allItem();

    tableView_->setRowCount(static_cast<int>(items.size()));
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        setRowData(row, items.at(size_t(row)), table->bufferOffset());
    }

    setTip(tr("Size: %1").arg(table->bufferSize()));

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

    int rowIndex = tableView_->rowCount() - 1;

    // count
    tableView_->insertRow(++rowIndex);
    tableView_->setItemData(rowIndex, 0, tr("Count of array"));
    tableView_->setItemData(rowIndex, 1, array->count());

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

    return updateTable(complex->table());
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

void DetailTable::moveBegin()
{
    moving_ = true;
}

void DetailTable::moveEnd()
{
    moving_ = false;
}

void DetailTable::moveRow(int sourceRow, int targetRow)
{
    if (sourceRow < 0 || targetRow < 0) {
        return;
    }

    moveBegin();

    const QList<QStandardItem*> items = tableView_->takeRow(sourceRow);
    tableView_->insertRow(targetRow, items);
    tableView_->selectRow(targetRow);

    bool restore = false;

    if (originalRow_ == targetRow) {  // back to original status
        originalRow_ = -1;
        restore = true;
    } else if (originalRow_ == -1) {  // first moving
        originalRow_ = sourceRow;
    }

    moveEnd();

    emit rowMoved(sourceRow, targetRow, restore);
}

void DetailTable::updateOffsetAndSize()
{
    if (!object_) {
        return;
    }

    switch (object_->rtti()) {
    case Icd::ObjectTable:
    {
        updateOffsetAndSize(JHandlePtrCast<Icd::Table>(object_));
        break;
    }
    case Icd::ObjectComplex:
    {
        const Icd::ComplexItemPtr complex = JHandlePtrCast<Icd::ComplexItem>(object_);
        if (!complex) {
            return;
        }
        updateOffsetAndSize(complex->table());
        break;
    }
    default:
        break;
    }
}

void DetailTable::updateOffsetAndSize(const Icd::TablePtr &table)
{
    if (!table) {
        return;
    }

    const Icd::ItemPtrArray &items = table->allItem();
    const int rowCount = tableView_->rowCount();
    for (int row = 0; row < rowCount && row < int(items.size()); ++row) {
        const Icd::ItemPtr item = items[size_t(row)];
        const double bufferOffset = item->bufferOffset() - table->bufferOffset();
        // offset and size
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
    }

    setTip(tr("Size: %1").arg(table->bufferSize()));
}

void DetailTable::setRowData(int row, const Icd::VehiclePtr &vehicle)
{
    if (!vehicle) {
        return;
    }

    tableView_->setItemData(row, 0, QString::fromStdString(vehicle->id()), Qt::UserRole);
    tableView_->setItemData(row, 0, QString::fromStdString(vehicle->name()));
    tableView_->setItemData(row, 1, QString::fromStdString(vehicle->mark()));
    tableView_->setItemData(row, 2, QString::fromStdString(vehicle->desc()));
}

void DetailTable::setRowData(int row, const Icd::SystemPtr &system)
{
    if (!system) {
        return;
    }

    tableView_->setItemData(row, 0, QString::fromStdString(system->id()), Qt::UserRole);
    tableView_->setItemData(row, 0, QString::fromStdString(system->name()));
    tableView_->setItemData(row, 1, QString::fromStdString(system->mark()));
    tableView_->setItemData(row, 2, QString::fromStdString(system->desc()));
}

void DetailTable::setRowData(int row, const Icd::TablePtr &table)
{
    if (!table) {
        return;
    }

    tableView_->setItemData(row, 0, QString::fromStdString(table->id()), Qt::UserRole);
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

    tableView_->setItemData(row, 0, QString::fromStdString(item->id()), Qt::UserRole);
    tableView_->setItemData(row, 0, QString::fromStdString(item->name()));
    tableView_->setItemData(row, 1, QString::fromStdString(item->mark()));
    // offset and size
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

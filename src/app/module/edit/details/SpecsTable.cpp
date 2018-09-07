#include "precomp.h"
#include "SpecsTable.h"

namespace Edit {

SpecsTable::SpecsTable(const Icd::ItemPtr &item, QWidget *parent)
    : QWidget(parent)
    , item_(item)
    , blocking_(false)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBoxSpecs = new QGroupBox(tr("Specs"), this);
    layoutMain->addWidget(groupBoxSpecs);

    QVBoxLayout *layoutSpecs = new QVBoxLayout(groupBoxSpecs);
    layoutSpecs->setContentsMargins(2, 0, 2, 2);

    tableView_ = new JTableView(this);
    tableView_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tableView_->setEditTriggers(QAbstractItemView::CurrentChanged
                                | QAbstractItemView::DoubleClicked
                                | QAbstractItemView::SelectedClicked);
    tableView_->setSelectionMode(QAbstractItemView::NoSelection);
    tableView_->verticalHeader()->hide();
    layoutSpecs->addWidget(tableView_);

    connect(tableView_, &JTableView::itemChanged, this, [=](){
        if (blocking()) {
            return;
        }
        emit contentChanged();
    });
    connect(tableView_, &JTableView::itemPressed, this, [=](QStandardItem *item){
        if (!item) {
            return;
        }
        if (QApplication::mouseButtons() == Qt::RightButton) {
            QMenu menu(this);
            // add
            if (item_->type() != Icd::ItemBitMap) {
                menu.addAction(QIcon(":/datastudio/image/global/add.png"),
                               tr("Add"), this, &SpecsTable::onAdd);
            }
            // remove
            menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                           tr("Remove"), this, &SpecsTable::onRemove);
            if (item_->type() != Icd::ItemBitMap && tableView_->rowCount() > 0) {
                // clean
                menu.addAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                               tr("Clean"), this, &SpecsTable::onClean);
            }
            menu.exec(QCursor::pos());
        }
    });
}

Icd::ItemPtr SpecsTable::item() const
{
    return item_;
}

bool SpecsTable::init()
{
    lock();

    tableView_->clear();

    if (!item_) {
        unlock();
        return false;
    }

    switch (item_->type()) {
    case Icd::ItemBitMap:
    {
        const Icd::BitItemPtr bit = JHandlePtrCast<Icd::BitItem>(item_);
        if (!bit) {
            break;
        }
        // header
        QStringList labels;
        labels << tr("Bit") << tr("Describe") << tr("0") << tr("1");
        tableView_->setHorizontalHeaderLabels(labels);
        tableView_->setColumnWidth(0, 40);
        tableView_->setColumnWidth(1, 150);
        tableView_->setColumnWidth(2, 100);
        tableView_->setColumnWidth(3, 100);
        // delegate
        tableView_->setItemDelegateForColumn(0, new JReadOnlyItemDelegate(item_));
        // actions
        // - clean
        QAction *actionClean = new QAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                           tr("Clean"), tableView_);
        connect(actionClean, &QAction::triggered, this,  &SpecsTable::onClean);
        tableView_->addAction(actionClean);
        break;
    }
    case Icd::ItemNumeric:
    case Icd::ItemBitValue:
    {
        // header
        QStringList labels;
        labels << tr("Value") << tr("Describe");
        tableView_->setHorizontalHeaderLabels(labels);
        tableView_->setColumnWidth(0, 100);
        tableView_->setColumnWidth(1, 100);
        // delegate
        if (item_->type() == Icd::ItemNumeric) {
            tableView_->setItemDelegateForColumn(0, new JDoubleItemDelegate(item_));
        } else {
            tableView_->setItemDelegateForColumn(0, new JULongLongItemDelegate(item_));
        }
        // actions
        // - add
        QAction *actionAdd = new QAction(QIcon(":/datastudio/image/global/add.png"),
                                         tr("Add"), tableView_);
        connect(actionAdd, &QAction::triggered, this, &SpecsTable::onAdd);
        tableView_->addAction(actionAdd);
        // - clean
        QAction *actionClean = new QAction(QIcon(":/datastudio/image/toolbar/clean.png"),
                                           tr("Clean"), tableView_);
        connect(actionClean, &QAction::triggered, this, &SpecsTable::onClean);
        tableView_->addAction(actionClean);
        break;
    }
    default:
        break;
    }

    unlock();

    restoreContent();

    return true;
}

void SpecsTable::lock()
{
    blocking_ = true;
}

void SpecsTable::unlock()
{
    blocking_ = false;
}

bool SpecsTable::blocking() const
{
    return blocking_;
}

bool SpecsTable::validate()
{
    return true;
}

void SpecsTable::restoreContent()
{
    lock();

    tableView_->clearContents();

    if (item_) {
        switch (item_->type()) {
        case Icd::ItemNumeric:
        {
            const Icd::NumericItemPtr numeric = JHandlePtrCast<Icd::NumericItem>(item_);
            if (!numeric) {
                break;
            }
            // specs
            const auto &specs = numeric->specs();
            tableView_->setRowCount(specs.size());
            int rowIndex = 0;
            for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer, ++rowIndex) {
                tableView_->setItemData(rowIndex, 0, IcdWidget::prettyValue(citer->first));
                tableView_->setItemData(rowIndex, 1, QString::fromStdString(citer->second));
            }
            break;
        }
        case Icd::ItemBitMap:
        {
            const Icd::BitItemPtr bit = JHandlePtrCast<Icd::BitItem>(item_);
            if (!bit) {
                break;
            }
            // rows
            tableView_->setRowCount(bit->bitCount());
            for (int i = 0; i < bit->bitCount(); ++i) {
                tableView_->setItemData(i, 0, bit->bitStart() + i);
            }
            // specs
            const auto &specs = bit->specs();
            for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer) {
                const quint32 index = quint32(citer->first);
                if (index >= bit->bitCount()) {
                    continue;
                }
                const QString spec = QString::fromStdString(citer->second).trimmed();
                const QString desc = spec.section(':', 1).trimmed();
                // name
                tableView_->setItemData(int(index), 1, spec.section(':', 0, 0).trimmed());
                // desc - 0
                tableView_->setItemData(int(index), 2, desc.section(';', 0, 0).trimmed());
                // desc - 1
                tableView_->setItemData(int(index), 3, desc.section(';', 1).trimmed());
            }
            break;
        }
        case Icd::ItemBitValue:
        {
            const Icd::BitItemPtr bit = JHandlePtrCast<Icd::BitItem>(item_);
            if (!bit) {
                break;
            }
            // specs
            const auto &specs = bit->specs();
            tableView_->setRowCount(specs.size());
            int rowIndex = 0;
            for (auto citer = specs.cbegin(); citer != specs.cend(); ++citer, ++rowIndex) {
                tableView_->setItemData(rowIndex, 0, QString::number(citer->first));
                tableView_->setItemData(rowIndex, 1, QString::fromStdString(citer->second));
            }
            break;
        }
        default:
            break;
        }
    }

    unlock();
}

void SpecsTable::saveContent()
{
    if (!item_) {
        return;
    }

    switch (item_->type()) {
    case Icd::ItemNumeric:
    {
        const Icd::NumericItemPtr numeric = JHandlePtrCast<Icd::NumericItem>(item_);
        if (!numeric) {
            break;
        }
        numeric->clearSpec();
        const int rowCount = tableView_->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            const QVariant varKey = tableView_->itemData(i, 0);
            if (!varKey.isValid()) {
                continue;
            }
            const QString spec = tableView_->itemData(i, 1).toString().trimmed();
            if (spec.isEmpty()) {
                continue;
            }
            numeric->addSpec(varKey.toDouble(), spec.toStdString());
        }
        break;
    }
    case Icd::ItemBitMap:
    {
        const Icd::BitItemPtr bit = JHandlePtrCast<Icd::BitItem>(item_);
        if (!bit) {
            break;
        }
        bit->clearSpec();
        const int rowCount = tableView_->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            const QVariant varKey = tableView_->itemData(i, 0);
            if (!varKey.isValid()) {
                continue;
            }
            const QString name = tableView_->itemData(i, 1).toString().trimmed();
            if (name.isEmpty()) {
                continue;
            }
            const QString desc0 = tableView_->itemData(i, 2).toString().trimmed();
            const QString desc1 = tableView_->itemData(i, 3).toString().trimmed();
            if (desc0.isEmpty() && desc1.isEmpty()) {
                bit->addSpec(varKey.toDouble(), name.toStdString());
            } else {
                const QString spec = QString("%1:%2;%3").arg(desc0).arg(desc1);
                bit->addSpec(varKey.toDouble(), spec.toStdString());
            }
        }
        break;
    }
    case Icd::ItemBitValue:
    {
        const Icd::BitItemPtr bit = JHandlePtrCast<Icd::BitItem>(item_);
        if (!bit) {
            break;
        }
        bit->clearSpec();
        const int rowCount = tableView_->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            const QVariant varKey = tableView_->itemData(i, 0);
            if (!varKey.isValid()) {
                continue;
            }
            const QString spec = tableView_->itemData(i, 1).toString().trimmed();
            if (spec.isEmpty()) {
                continue;
            }
            bit->addSpec(varKey.toULongLong(), spec.toStdString());
        }
        break;
    }
    default:
        break;
    }
}

void SpecsTable::onAdd()
{
    if (item_->type() == Icd::ItemBitMap) {
        return;
    }

    lock();
    const int rowCount = tableView_->rowCount();
    tableView_->insertRow(rowCount);
    if (rowCount > 0) {
        tableView_->setItemData(rowCount, 0, tableView_->itemData(rowCount - 1, 0).toDouble() + 1);
    } else {
        tableView_->setItemData(rowCount, 0, 0);
    }
    tableView_->clearSelection();
    tableView_->setCurrentCell(rowCount, 1);
    unlock();

    emit contentChanged();
}

void SpecsTable::onRemove()
{
    lock();
    const int currentRow = tableView_->currentRow();
    if (item_->type() == Icd::ItemBitMap) {
        tableView_->setItemData(currentRow, 1, QString());
        tableView_->setItemData(currentRow, 2, QString());
        tableView_->setItemData(currentRow, 3, QString());
    } else {
        tableView_->removeRow(currentRow);
    }
    unlock();
    emit contentChanged();
}

void SpecsTable::onClean()
{
    lock();
    if (item_->type() == Icd::ItemBitMap) {
        const int rowCount = tableView_->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            tableView_->setItemData(i, 1, QString());
            tableView_->setItemData(i, 2, QString());
            tableView_->setItemData(i, 3, QString());
        }
    } else {
        tableView_->clearContents();
    }
    unlock();
    emit contentChanged();
}

// class JReadOnlyItemDelegate

JReadOnlyItemDelegate::JReadOnlyItemDelegate(const Icd::ItemPtr &item, QObject *parent)
    : QStyledItemDelegate(parent)
    , item_(item)
{

}

QWidget *JReadOnlyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    Q_UNUSED(parent);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return nullptr;
}

// class JULongLongItemDelegate

JULongLongItemDelegate::JULongLongItemDelegate(const Icd::ItemPtr &item, QObject *parent)
    : QStyledItemDelegate(parent)
    , item_(item)
{

}

QWidget *JULongLongItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    switch (item_->type()) {
    case Icd::ItemBitValue:
    {
        JLargeSpinBox *spin = new JLargeSpinBox(parent);
        const Icd::BitItemPtr bit = JHandlePtrCast<Icd::BitItem>(item_);
        if (!bit) {
            break;
        }
        const auto valueRange = bit->valueRange();
        spin->setRange(valueRange.first, valueRange.second);
        return spin;
    }
    default:
        Q_ASSERT(false);
        break;
    }
    return nullptr;
}

// class JDoubleItemDelegate

JDoubleItemDelegate::JDoubleItemDelegate(const Icd::ItemPtr &item, QObject *parent)
    : QStyledItemDelegate(parent)
    , item_(item)
{

}

QWidget *JDoubleItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    switch (item_->type()) {
    case Icd::ItemNumeric:
    {
        JDoubleSpinBox *spin = new JDoubleSpinBox(parent);
        const Icd::NumericItemPtr numeric = JHandlePtrCast<Icd::NumericItem>(item_);
        if (!numeric) {
            break;
        }
        const auto valueRange = numeric->valueRange();
        spin->setRange(valueRange.first, valueRange.second);
        return spin;
    }
    default:
        Q_ASSERT(false);
        break;
    }
    return nullptr;
}

void JDoubleItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                       const QModelIndex &index) const
{
    model->setData(index, IcdWidget::prettyValue(editor->property("value").toDouble()));
}

}

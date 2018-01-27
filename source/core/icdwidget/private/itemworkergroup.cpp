#include "precomp.h"
#include "itemworkergroup.h"
#include "icdworker/icdworker_pool.h"
#include "icdcore/icdcore_inc.h"
#include "coretree_widget_p.h"

namespace Icd {

ItemWorkerGroup::ItemWorkerGroup(QStandardItem *itemTable, const WorkerPtr &worker,
                                 CoreTreeWidget::ShowAttributes attrs,
                                 QObject *parent)
    : QObject(parent)
    , d_itemTable(itemTable)
    , d_worker(worker)
    , d_showAttris(attrs)
    , d_bindTableTypes(CoreTreeWidget::BindOnlyRecv)
    , d_timerId(0)
    , d_timerInterval(100)  // 100 ms
    , d_dataFormat(16)  // default: hex format
{
    //
    d_treeView = dynamic_cast<JTreeViewItem *>(itemTable)->view();

    //
    updateTimer();
}

ItemWorkerGroup::~ItemWorkerGroup()
{
    //
    setDirty();
}

void ItemWorkerGroup::updateTimer()
{
    if (!d_worker || !d_worker->isOpen()) {
        stop();
        return;
    }

    if (!(d_showAttris & (CoreTreeWidget::ShowData | CoreTreeWidget::ShowValue))) {
        stop();
        return;
    }

    if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
        if (d_worker->isOpen()) {
            start();
        } else {
            stop();
        }
    } else if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
        if (d_worker->workerRecv()->isRunning()) {
            start();
        } else {
            stop();
        }
    }
}

void ItemWorkerGroup::updateItemData()
{
    if (d_worker) {
        if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
            updateItemData(d_worker->isOpen());
        } else if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
            updateItemData(d_worker->workerRecv()->isRunning());
        }
    }
}

void ItemWorkerGroup::updateItemData(bool showValue)
{
    if (!d_itemTable) {
        return;
    }

    //
    Icd::TablePtr table = Icd::TablePtr(0);
    switch (d_bindTableTypes) {
    case CoreTreeWidget::BindOnlySend:
        table = d_worker->workerSend()->table();
        break;
    case CoreTreeWidget::BindOnlyRecv:
        table = d_worker->workerRecv()->table();
        break;
    default:
        return;
    }

    //
    if (!table) {
        return;
    }

    //
    //this->setUpdatesEnabled(false);

    //
    updateFrameTable(d_itemTable, table, showValue);

    //
    int childCount = d_itemTable->rowCount();
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (int i = 0; citer != items.cend() && i < childCount; ++citer, ++i) {
        updateItemData(d_itemTable->child(i), *citer, showValue);
    }

    //this->setUpdatesEnabled(true);
}

void ItemWorkerGroup::updateItemData(CoreTreeWidget::ShowAttributes attrs,
                                     int dataFormat)
{
    //
    d_showAttris = attrs;
    d_dataFormat = dataFormat;

    //
    updateTimer();

    //
    updateItemData();
}

void ItemWorkerGroup::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d_timerId) {
        updateItemData();
    } else {
        QObject::timerEvent(event);
    }
}

void ItemWorkerGroup::updateItemData(QStandardItem *item, const Icd::FrameItemPtr &frame,
                                     bool showValue)
{
    if (!item || !frame) {
        return;
    }

    //
    int childCount = item->rowCount();
    const Icd::TablePtrMap &tables = frame->allTable();
    Icd::TablePtrMap::const_iterator citer = tables.cbegin();
    for (int i = 0; citer != tables.cend() && i < childCount; ++citer, ++i) {
        QStandardItem *frameItem = item->child(i);
        const Icd::TablePtr &table = citer->second;
        if (!frameItem || !table) {
            continue;
        }
        //
        updateFrameTable(frameItem, table, showValue);
        //
        updateItemData(frameItem, table, showValue);
    }
}

void ItemWorkerGroup::updateItemData(QStandardItem *item, const TablePtr &table,
                                     bool showValue)
{
    if (!item || !table) {
        return;
    }

    //
    updateFrameTable(item, table, showValue);

    //
    int childCount = item->rowCount();
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (int i = 0; citer != items.cend() && i < childCount; ++citer, ++i) {
        updateItemData(item->child(i), *citer, showValue);
    }
}

void ItemWorkerGroup::start()
{
    if (d_timerId == 0) {
        d_timerId = startTimer(d_timerInterval, Qt::PreciseTimer);
    }
}

void ItemWorkerGroup::start(int interval)
{
    if (d_timerId > 0) {
        killTimer(d_timerId);
        d_timerId = 0;
    }

    //
    d_timerId = startTimer(interval);

    //
    d_timerInterval = interval;
}

void ItemWorkerGroup::stop()
{
    if (d_timerId > 0) {
        killTimer(d_timerId);
        d_timerId = 0;
        QApplication::removePostedEvents(this, QEvent::Timer);
    }
}

bool ItemWorkerGroup::isRunning() const
{
    return d_timerId > 0;
}

void ItemWorkerGroup::setRunning(bool running)
{
    if (d_worker) {
        if (running) {
            start();
        } else {
            stop();
        }
    }
}

WorkerPtr ItemWorkerGroup::worker() const
{
    return d_worker;
}

QStandardItem *ItemWorkerGroup::itemTable() const
{
    return d_itemTable;
}

void ItemWorkerGroup::setWorker(const WorkerPtr &worker)
{
    if (d_worker) {
        d_worker->disconnect(this);
        if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
            d_worker->workerSend()->disconnect(this);
        }
        if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
            d_worker->workerRecv()->disconnect(this);
        }
    }

    d_worker = worker;

    setBindTableType(d_bindTableTypes);

    //
    updateTimer();
}

void ItemWorkerGroup::setBindTableType(CoreTreeWidget::BindTableTypes type)
{
    if (d_worker) {
        //
        d_worker->disconnect(this);
        if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
            d_worker->workerSend()->disconnect(this);
        }
        if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
            d_worker->workerRecv()->disconnect(this);
        }

        //
        d_bindTableTypes = type;

        //
        connect(d_worker.get(), &Icd::Worker::opened,
                this, &Icd::ItemWorkerGroup::onWorkerOpened);
        connect(d_worker.get(), &Icd::Worker::closed,
                this, &Icd::ItemWorkerGroup::onWorkerClosed);
        if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
            connect(d_worker->workerSend().get(), &Icd::WorkerSend::started,
                    this, &Icd::ItemWorkerGroup::onWorkerStarted);
            connect(d_worker->workerSend().get(), &Icd::WorkerSend::stopped,
                    this, &Icd::ItemWorkerGroup::onWorkerStopped);
        }
        if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
            connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::started,
                    this, &Icd::ItemWorkerGroup::onWorkerStarted);
            connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::stopped,
                    this, &Icd::ItemWorkerGroup::onWorkerStopped);
        }
    }
}

void ItemWorkerGroup::setDirty()
{
    //
    stop();

    if (d_worker) {
        d_worker->disconnect(this);
        if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
            d_worker->workerSend()->disconnect(this);
            d_worker->workerSend()->setTable(Icd::TablePtr(0));
        }
        if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
            d_worker->workerRecv()->disconnect(this);
            d_worker->workerRecv()->setTable(Icd::TablePtr(0));
        }
    }
}

int ItemWorkerGroup::asciiCountOfSize(int format, int size)
{
    switch (format) {
    case 2: return size * 8;
    case 8:
        switch (size) {
        case 1: return 3;
        case 2: return 6;
        case 4: return 11;
        case 8: return 22;
            break;
        }
    case 16: return size * 2;
    default:
        break;
    }

    return 0;
}

QString ItemWorkerGroup::generateItemOffset(const Icd::ObjectPtr &object)
{
    if (!object) {
        Q_ASSERT(false);
        return QString();
    }

    QString text;

    switch (object->objectType()) {
    case Icd::ObjectTable:
    {
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(object);
        if (!table) {
            break;
        }

        qreal itemOffset = -1;
        //
        if (table->parent() && table->parent()->objectType() == Icd::ObjectItem) {
            const Icd::Item *item = dynamic_cast<Icd::Item *>(table->parent());
            if (item && item->parent()) {
                itemOffset = item->bufferOffset();
            }
        }
        //
        if (itemOffset >= 0) {
            text.append(QString("<font color=green size=2>[%1:%2:%3]</font> ")
                        .arg(table->itemOffset(), 4, 10, QChar('0'))
                        .arg((int)table->bufferOffset(), 4, 10, QChar('0'))
                        .arg((int)(table->bufferOffset() - itemOffset), 4, 10, QChar('0')));
        } else {
            text.append(QString("<font color=green size=2>[%1:%2]</font> ")
                        .arg(table->itemOffset(), 4, 10, QChar('0'))
                        .arg((int)table->bufferOffset(), 4, 10, QChar('0')));
        }

        break;
    }
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item>(object);
        if (!item) {
            break;
        }

        qreal tableOffset = -1;
        //
        if (item->parent() && item->parent()->objectType() == Icd::ObjectTable) {
            const Icd::Table *table = dynamic_cast<Icd::Table *>(item->parent());
            if (table && table->parent()) {
                tableOffset = table->bufferOffset();
            }
        }
        //
        if (tableOffset >= 0) {
            switch (item->type()) {
            case Icd::ItemBitMap:
            case Icd::ItemBitValue:
            {
#if 0
                const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(, item);
                if (!itemBit) {
                    break;
                }
                text.append(QString("<font color=green size=2>[%1:%2:%3.%4]</font> ")
                            .arg(item->itemOffset(), 4, 10, QChar('0'))
                            .arg((int)item->bufferOffset(), 4, 10, QChar('0'))
                            .arg((int)(item->bufferOffset() - tableOffset), 4, 10, QChar('0'))
                            .arg(itemBit->bitStart(), 2, 10, QChar('0')));
                break;
#endif
            }
            default:
                text.append(QString("<font color=green size=2>[%1:%2:%3]</font> ")
                            .arg(item->itemOffset(), 4, 10, QChar('0'))
                            .arg((int)item->bufferOffset(), 4, 10, QChar('0'))
                            .arg((int)(item->bufferOffset() - tableOffset), 4, 10, QChar('0')));
                break;
            }
        } else {
            text.append(QString("<font color=green size=2>[%1:%2]</font> ")
                        .arg(item->itemOffset(), 4, 10, QChar('0'))
                        .arg((int)item->bufferOffset(), 4, 10, QChar('0')));
        }
        break;
    }
    default:
        break;
    }

    return text;
}

void ItemWorkerGroup::onWorkerOpened()
{
    if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
        updateTimer();
    } else if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
        //
    }
}

void ItemWorkerGroup::onWorkerClosed()
{
    stop();
}

void ItemWorkerGroup::onWorkerStarted()
{
    start(d_timerInterval);
}

void ItemWorkerGroup::onWorkerStopped()
{
    stop();
}

void ItemWorkerGroup::updateItemData(QStandardItem *item, const ItemPtr &dataItem,
                                     bool showValue)
{
    if (!item || !dataItem) {
        return;
    }

    //
    QString info;
    int asciiCount = asciiCountOfSize(d_dataFormat, dataItem->bufferSize());
    const bool _showValue = [=]() -> bool {
        //
        if (!showValue) {
            return false;
        }
        //
        if (d_showAttris & (CoreTreeWidget::ShowData | CoreTreeWidget::ShowValue)) {
            return true;
        }
        //
        switch (item->type()) {
        case Icd::ItemBitMap:
        case Icd::ItemBitValue:
            if (d_showAttris & CoreTreeWidget::ShowSpec) {
                return true;
            }
            break;
        default:
            break;
        }
        return false;
    }();

    //
    if (_showValue) {
        switch (dataItem->type()) {
        case Icd::ItemHead:
        {
            QStringList values;
            // data
            if (d_showAttris & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, d_dataFormat, QChar('0'))
                              .toUpper());
            }
            // value
            if (d_showAttris & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(values.join(", "));
            break;
        }
        case Icd::ItemCounter:
        {
            QStringList values;
            // data
            if (d_showAttris & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, d_dataFormat, QChar('0'))
                              .toUpper());
            }
            // value
            if (d_showAttris & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(values.join(", "));
            break;
        }
        case Icd::ItemCheck:
        {
            QStringList values;
            // data
            if (d_showAttris & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, d_dataFormat, QChar('0'))
                              .toUpper());
            }
            // value
            if (d_showAttris & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(values.join(", "));
            break;
        }
        case Icd::ItemFrameCode:
        {
            QStringList values;
            // data
            if (d_showAttris & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, d_dataFormat, QChar('0'))
                              .toUpper());
            }
            // value
            if (d_showAttris & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(values.join(", "));
            break;
        }
        case Icd::ItemNumeric:
        {
            QStringList values;
            //
            const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem, Icd::Item>(dataItem);
            if (!itemNumeric) {
                break;
            }
            // data
            if (d_showAttris & CoreTreeWidget::ShowData) {
                qulonglong data = (qulonglong)(itemNumeric->orignalData());
                data &= (1ui64 << ((int)itemNumeric->bufferSize() << 3)) - 1;
                values.append(QString("%1")
                              .arg(data, asciiCount, d_dataFormat, QChar('0'))
                              .toUpper());
            }
            // value
            if (d_showAttris & CoreTreeWidget::ShowValue) {
                QString value;
                value.append(QString("%1").arg(itemNumeric->data(), 0, 'g', 20));
                // unit
                const std::string unit = itemNumeric->unit();
                if (!unit.empty()) {
                    value.append(' ').append(QString::fromStdString(unit));
                }
                values.append(value);
            }
            if (!values.isEmpty()) {
                const QString sValues = values.join(", ");
                if (itemNumeric->outOfLimit()) {
                    info.append("<font color=red><b>" + sValues + "</b></font>");
                } else {
                    info.append(sValues);
                }
            }
            break;
        }
        case Icd::ItemBitMap:
        {
            QStringList values;
            // data
            if (d_showAttris & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, d_dataFormat, QChar('0')).toUpper()
                              .toUpper());
            }
            // value
            if (d_showAttris & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((uint)dataItem->data(),
                                                int(dataItem->bufferSize() * 8), 2, QChar('0')));
            }
            info.append(values.join(", "));
            break;
        }
        case Icd::ItemBitValue:
        {
            QStringList values;
            // data
            if (d_showAttris & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, d_dataFormat, QChar('0')).toUpper()
                              .toUpper());
            }
            // value
            if (d_showAttris & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((uint)dataItem->data(),
                                                int(dataItem->bufferSize() * 8), 2, QChar('0')));
            }
            // spec
            if (d_showAttris & CoreTreeWidget::ShowSpec) {
                const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem);
                if (bitItem) {
                    std::string spec = bitItem->specAt((Icd::icd_int64)dataItem->data());
                    if (spec.empty()) {
                        spec = "--";
                    }
                    values.append(QString::fromStdString(spec));
                }
            }
            info.append(values.join(", "));
            break;
        }
        case Icd::ItemComplex:
        {
            //
            const Icd::ComplexItemPtr itemComplex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(dataItem);
            if (!itemComplex) {
                break;
            }

            //
            Icd::TablePtr table = itemComplex->table();
            if (table->itemCount() == 0) {
                const char *buffer = itemComplex->buffer();
                int bufferSize = (int)itemComplex->bufferSize();
                if (bufferSize <= 16) {
                    for (int i = 0; i < bufferSize; ++i) {
                        info.append(QString("%1").arg((ushort)(*(buffer + i)),
                                                      2, 16, QChar('0')).toUpper());
                    }
                }
            }
            break;
        }
        default:
            break;   // not supported data type
        }
    }

    //
    QString text;
    // offset
    if (d_showAttris & CoreTreeWidget::ShowOffset) {
        text.append(ItemWorkerGroup::generateItemOffset(dataItem));
    }
    // name
    text.append(QString::fromStdString(dataItem->name().empty()
                                       ? "<?>" : dataItem->name()));
    // type, value
    if (d_showAttris & CoreTreeWidget::ShowType) {
        text.append(" <font color=green size=2>[")
                .append(QString::fromStdString(dataItem->typeString()).toUpper());
        if (info.isEmpty()) {
            text.append("]</font>");
        } else {
            text.append(": </font><font color=blue>")
                    .append(info).append("</font><font color=green>]</font>");
        }
    } else {
        if (!info.isEmpty()) {
            text.append(" <font color=green size=2>[")
                    .append(info).append("</font><font color=green>]</font>");
        }
    }
    //
    item->setText(text);

    //
    switch (dataItem->type()) {
    case Icd::ItemBitMap:
    {
        const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem);
        if (!bitItem) {
            break;
        }
        updateItemBitMap(item, bitItem, showValue);
        break;
    }
    case Icd::ItemComplex:
    {
        //
        const Icd::ComplexItemPtr itemComplex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(dataItem);
        if (!itemComplex) {
            break;
        }

        //
        Icd::TablePtr table = itemComplex->table();
        if (table->itemCount() > 0) {
            updateItemData(item, table, showValue);
        }
        break;
    }
    case Icd::ItemFrame:
    {
        //
        const Icd::FrameItemPtr &itemFrame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(dataItem);
        if (!itemFrame) {
            break;
        }

        //
        updateItemData(item, itemFrame, showValue);

        break;
    }
    default:
        break;
    }
}

void ItemWorkerGroup::updateItemBitMap(QStandardItem *item, const BitItemPtr &bitItem,
                                       bool showValue)
{
    if (!item || !bitItem) {
        return;
    }

    //
    if (bitItem->type() != Icd::ItemBitMap) {
        return;
    }

    //
    int childCount = item->rowCount();
    if (childCount == 0) {
        return;
    }

    const bool _showValue = [=]() -> bool {
        //
        if (!showValue) {
            return false;
        }
        //
        if (d_showAttris & (CoreTreeWidget::ShowData
                            | CoreTreeWidget::ShowValue
                            | CoreTreeWidget::ShowSpec)) {
            return true;
        }
        return false;
    }();

    //
    for (int i = 0; i < childCount; ++i) {
        QStandardItem *childItem = item->child(i);
        if (!childItem) {
            continue;
        }
        //
        const int bitOffset = childItem->data(Icd::TreeBitOffsetRole).toInt();
        // name
        std::string name = bitItem->nameAt(bitOffset);
        if (name.empty()) {
            continue;
        }
        //
        QString text;
        // offset
        if (d_showAttris & CoreTreeWidget::ShowOffset) {
            text.append(QString("<font color=green size=2>[%1]</font> ")
                        .arg(bitOffset, 2, 10, QChar('0')));
        }

        // name
        text.append(QString::fromStdString(name));

        // value
        if (_showValue) {
            text.append(" <font color=blue>[");
            QStringList values;
            if (d_showAttris & (CoreTreeWidget::ShowData | CoreTreeWidget::ShowValue)) {
                values.append(QString("%1").arg(bitItem->testBit(bitOffset) ? 1 : 0,
                                                1, 10, QChar('0')));
            }
            // spec
            if (d_showAttris & CoreTreeWidget::ShowSpec) {
                std::string desc = bitItem->descAt(bitOffset);
                if (desc.empty()) {
                    desc = "--";
                }
                values.append(QString::fromStdString(desc));
            }
            text.append(values.join(", "));
            text.append("</font><font color=green>]</font>");
        }
        //
        childItem->setText(text);
    }
}

void ItemWorkerGroup::updateFrameTable(QStandardItem *item, const TablePtr &table, bool show)
{
    if (!item || !table) {
        return;
    }

    //
    if (table->isFrameTable()) {
        QString text;
        // name
        const std::string name = table->name();
        text.append(QString::fromStdString(name.empty() ? "<?>" : name));
        // type
        if (d_showAttris & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>[TABLE]</font>");
        }
        // channel
        if (!table->parent() && item->data(Icd::TreeChannelIdRole).isValid()) {
            text.append(QStringLiteral(" [CH: <i><font color=blue size=3>")
                        + QString::fromStdString(d_worker->channel()->name()) + "</font></i> ]");
        }
        //
        if (show) {
            // period
            text.append("[<font color=blue>")
                    .append(QStringLiteral("ÖÜÆÚ£º%1 ms").arg(table->period()))
                    .append("</font>]");
        }
        //
        TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>
                (d_treeView->itemWidget(item));
        if (itemWidget) {
            itemWidget->setText(text);
        } else {
            item->setText(text);
        }
    }
}

} // end of namespace Icd

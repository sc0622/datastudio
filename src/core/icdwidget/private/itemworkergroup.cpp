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
    , itemTable_(itemTable)
    , worker_(worker)
    , showAttris_(attrs)
    , bindTableTypes_(CoreTreeWidget::BindOnlyRecv)
    , timerId_(0)
    , timerInterval_(100)  // 100 ms
    , dataFormat_(16)  // default: hex format
{
    //
    treeView_ = qobject_cast<CoreTreeWidgetPrivate*>(
                dynamic_cast<JTreeViewItem*>(itemTable)->view());
    //
    connect(treeView_, &CoreTreeWidgetPrivate::valueColorChanged, this, [=](){
        updateItemData();
    });
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
    if (!worker_ || !worker_->isOpen()) {
        stop();
        return;
    }

    if (!(showAttris_ & (CoreTreeWidget::ShowData | CoreTreeWidget::ShowValue))) {
        stop();
        return;
    }

    if (bindTableTypes_ & CoreTreeWidget::BindOnlySend) {
        if (worker_->isOpen()) {
            start();
        } else {
            stop();
        }
    } else if (bindTableTypes_ & CoreTreeWidget::BindOnlyRecv) {
        if (worker_->workerRecv()->isRunning()) {
            start();
        } else {
            stop();
        }
    }
}

void ItemWorkerGroup::updateItemData()
{
    if (worker_) {
        if (bindTableTypes_ & CoreTreeWidget::BindOnlySend) {
            updateItemData(worker_->isOpen());
        } else if (bindTableTypes_ & CoreTreeWidget::BindOnlyRecv) {
            updateItemData(worker_->workerRecv()->isRunning());
        }
    }
}

void ItemWorkerGroup::updateItemData(bool showValue)
{
    if (!itemTable_) {
        return;
    }

    Icd::TablePtr table = Icd::TablePtr();
    switch (bindTableTypes_) {
    case CoreTreeWidget::BindOnlySend:
        table = worker_->workerSend()->table();
        break;
    case CoreTreeWidget::BindOnlyRecv:
        table = worker_->workerRecv()->table();
        break;
    default:
        return;
    }

    if (!table) {
        return;
    }

    updateFrameTable(itemTable_, table, showValue);

    int childCount = itemTable_->rowCount();
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (int i = 0; citer != items.cend() && i < childCount; ++citer, ++i) {
        updateItemData(itemTable_->child(i), *citer, showValue);
    }
}

void ItemWorkerGroup::updateItemData(CoreTreeWidget::ShowAttributes attrs,
                                     int dataFormat)
{
    showAttris_ = attrs;
    dataFormat_ = dataFormat;
    updateTimer();
    updateItemData();
}

int ItemWorkerGroup::timerInterval() const
{
    return timerInterval_;
}

void ItemWorkerGroup::setTimerInterval(int interval)
{
    if (interval != timerInterval_) {
        timerInterval_ = interval;
        if (timerId_ > 0) {
            QObject::killTimer(timerId_);
            timerId_ = 0;
            timerId_ = QObject::startTimer(interval);
        }
    }
}

void ItemWorkerGroup::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId_) {
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

    int childCount = item->rowCount();
    const Icd::TablePtrMap &tables = frame->allTable();
    Icd::TablePtrMap::const_iterator citer = tables.cbegin();
    for (int i = 0; citer != tables.cend() && i < childCount; ++citer, ++i) {
        QStandardItem *frameItem = item->child(i);
        const Icd::TablePtr &table = citer->second;
        if (!frameItem || !table) {
            continue;
        }
        updateFrameTable(frameItem, table, showValue);
        updateItemData(frameItem, table, showValue);
    }
}

void ItemWorkerGroup::updateItemData(QStandardItem *item, const TablePtr &table,
                                     bool showValue)
{
    if (!item || !table) {
        return;
    }

    if (!table->parent() || table->parent()->objectType() != Icd::ObjectItem) {
        updateFrameTable(item, table, showValue);
    }

    int childCount = item->rowCount();
    const Icd::ItemPtrArray &items = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = items.cbegin();
    for (int i = 0; citer != items.cend() && i < childCount; ++citer, ++i) {
        updateItemData(item->child(i), *citer, showValue);
    }
}

void ItemWorkerGroup::start()
{
    if (timerId_ == 0) {
        timerId_ = QObject::startTimer(timerInterval_, Qt::PreciseTimer);
    }
}

void ItemWorkerGroup::start(int interval)
{
    if (timerId_ > 0) {
        QObject::killTimer(timerId_);
        timerId_ = 0;
    }

    timerId_ = QObject::startTimer(interval);
    timerInterval_ = interval;
}

void ItemWorkerGroup::stop()
{
    if (timerId_ > 0) {
        QObject::killTimer(timerId_);
        timerId_ = 0;
        QApplication::removePostedEvents(this, QEvent::Timer);
    }
}

bool ItemWorkerGroup::isRunning() const
{
    return timerId_ > 0;
}

void ItemWorkerGroup::setRunning(bool running)
{
    if (worker_) {
        if (running) {
            start();
        } else {
            stop();
        }
    }
}

WorkerPtr ItemWorkerGroup::worker() const
{
    return worker_;
}

QStandardItem *ItemWorkerGroup::itemTable() const
{
    return itemTable_;
}

void ItemWorkerGroup::setWorker(const WorkerPtr &worker)
{
    if (worker_) {
        worker_->disconnect(this);
        if (bindTableTypes_ & CoreTreeWidget::BindOnlySend) {
            worker_->workerSend()->disconnect(this);
        }
        if (bindTableTypes_ & CoreTreeWidget::BindOnlyRecv) {
            worker_->workerRecv()->disconnect(this);
        }
    }

    worker_ = worker;

    setBindTableType(bindTableTypes_);

    //
    updateTimer();
}

void ItemWorkerGroup::setBindTableType(CoreTreeWidget::BindTableTypes type)
{
    if (worker_) {
        //
        worker_->disconnect(this);
        if (bindTableTypes_ & CoreTreeWidget::BindOnlySend) {
            worker_->workerSend()->disconnect(this);
        }
        if (bindTableTypes_ & CoreTreeWidget::BindOnlyRecv) {
            worker_->workerRecv()->disconnect(this);
        }

        //
        bindTableTypes_ = type;

        //
        connect(worker_.get(), &Icd::Worker::opened,
                this, &Icd::ItemWorkerGroup::onWorkerOpened);
        connect(worker_.get(), &Icd::Worker::closed,
                this, &Icd::ItemWorkerGroup::onWorkerClosed);
        if (bindTableTypes_ & CoreTreeWidget::BindOnlySend) {
            connect(worker_->workerSend().get(), &Icd::WorkerSend::started,
                    this, &Icd::ItemWorkerGroup::onWorkerStarted);
            connect(worker_->workerSend().get(), &Icd::WorkerSend::stopped,
                    this, &Icd::ItemWorkerGroup::onWorkerStopped);
        }
        if (bindTableTypes_ & CoreTreeWidget::BindOnlyRecv) {
            connect(worker_->workerRecv().get(), &Icd::WorkerRecv::started,
                    this, &Icd::ItemWorkerGroup::onWorkerStarted);
            connect(worker_->workerRecv().get(), &Icd::WorkerRecv::stopped,
                    this, &Icd::ItemWorkerGroup::onWorkerStopped);
        }
    }
}

void ItemWorkerGroup::setDirty()
{
    //
    stop();

    if (worker_) {
        worker_->disconnect(this);
        if (bindTableTypes_ & CoreTreeWidget::BindOnlySend) {
            worker_->workerSend()->disconnect(this);
            worker_->workerSend()->setTable(Icd::TablePtr(0));
        }
        if (bindTableTypes_ & CoreTreeWidget::BindOnlyRecv) {
            worker_->workerRecv()->disconnect(this);
            worker_->workerRecv()->setTable(Icd::TablePtr(0));
        }
    }
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
        if (table->parent() && table->parent()->objectType() == Icd::ObjectItem) {
            const Icd::Item *item = dynamic_cast<Icd::Item *>(table->parent());
            if (item && item->parent() && item->type() != Icd::ItemFrame) {
                itemOffset = item->bufferOffset();
            }
        }
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
        if (item->parent() && item->parent()->objectType() == Icd::ObjectTable) {
            const Icd::Table *table = dynamic_cast<Icd::Table *>(item->parent());
            if (table && table->parent()) {
                tableOffset = table->bufferOffset();
            }
        }
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
    if (bindTableTypes_ & CoreTreeWidget::BindOnlySend) {
        updateTimer();
    } else if (bindTableTypes_ & CoreTreeWidget::BindOnlyRecv) {
        //
    }
}

void ItemWorkerGroup::onWorkerClosed()
{
    stop();
}

void ItemWorkerGroup::onWorkerStarted()
{
    start(timerInterval_);
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

    const QString valueColorName = treeView_->valueColor().name();
    const QString dataPrefix("<font face=Consolas size=4 color=" + valueColorName + ">");
    const QString unitPrefix("<font size=3 color=" + valueColorName + ">");
    const QString suffix("</font>");

    QString info;
    int asciiCount = Icd::asciiCountOfSize(dataFormat_, int(dataItem->bufferSize()));
    const bool _showValue = [=]() -> bool {
        //
        if (!showValue) {
            return false;
        }
        //
        if (showAttris_ & (CoreTreeWidget::ShowData | CoreTreeWidget::ShowValue)) {
            return true;
        }
        //
        switch (item->type()) {
        case Icd::ItemBitMap:
        case Icd::ItemBitValue:
            if (showAttris_ & CoreTreeWidget::ShowSpec) {
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
            if (showAttris_ & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemCounter:
        {
            QStringList values;
            // data
            if (showAttris_ & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemCheck:
        {
            QStringList values;
            // data
            if (showAttris_ & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemFrameCode:
        {
            QStringList values;
            // data
            if (showAttris_ & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((qulonglong)dataItem->data(), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
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
            if (showAttris_ & CoreTreeWidget::ShowData) {
                double originalData = itemNumeric->originalData();
                switch (itemNumeric->numericType()) {
                case Icd::NumericF32:
                {
                    float _originalData = float(originalData);
                    qint32 data = *(qint32*)&_originalData;
                    values.append(QString("%1").arg(data, asciiCount, dataFormat_, QChar('0')).toUpper());
                    break;
                }
                case Icd::NumericF64:
                {
                    qint64 data = *(qint64*)&originalData;
                    values.append(QString("%1").arg(data, asciiCount, dataFormat_, QChar('0')).toUpper());
                    break;
                }
                default:
                {
                    qulonglong data = (qulonglong)(itemNumeric->originalData());
                    data &= (1ull << (int(itemNumeric->bufferSize()) << 3)) - 1;
                    values.append(QString("%1").arg(data, asciiCount, dataFormat_, QChar('0')).toUpper());
                    break;
                }}
            }
            // value
            if (showAttris_ & CoreTreeWidget::ShowValue) {
                QString value;
                switch (itemNumeric->numericType()) {
                case Icd::NumericF32:
                    value.append(QString("%1").arg(itemNumeric->data(), 0, 'g', FLT_DIG));
                    break;
                default:
                    value.append(QString("%1").arg(itemNumeric->data(), 0, 'g', DBL_DIG));
                    break;
                }
                std::string text = value.toStdString();
                std::string::size_type pos = text.find_last_not_of("0");
                if (std::string::npos != pos) {
                    text = text.substr(0, ('.' == text[pos]) ? pos : pos + 1);
                }
                values.append(QString::fromStdString(text));
            }
            // join
            if (itemNumeric->outOfLimit()) {
                info.append("<font face=Consolas size=4 color=red><b>").append(values.join(", "))
                        .append("</b></font>");
            } else {
                info.append(dataPrefix).append(values.join(", ")).append(suffix);
            }
            // unit
            const std::string unit = itemNumeric->unit();
            if (showAttris_ & CoreTreeWidget::ShowValue && !unit.empty()) {
                info.append(unitPrefix).append(' ').append(QString::fromStdString(unit)).append(suffix);
            }
            break;
        }
        case Icd::ItemBitMap:
        {
            QStringList values;
            // data
            if (showAttris_ & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, dataFormat_, QChar('0')).toUpper()
                              .toUpper());
            }
            // value
            if (showAttris_ & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((uint)dataItem->data(),
                                                int(dataItem->bufferSize() * 8), 2, QChar('0')));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemBitValue:
        {
            QStringList values;
            // data
            if (showAttris_ & CoreTreeWidget::ShowData) {
                values.append(QString("%1")
                              .arg((uint)dataItem->data(), asciiCount, dataFormat_, QChar('0')).toUpper()
                              .toUpper());
            }
            // value
            if (showAttris_ & CoreTreeWidget::ShowValue) {
                values.append(QString("%1").arg((uint)dataItem->data(),
                                                int(dataItem->bufferSize() * 8), 2, QChar('0')));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            // spec
            if (showAttris_ & CoreTreeWidget::ShowSpec) {
                const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem);
                if (bitItem) {
                    std::string spec = bitItem->specAt((Icd::icd_int64)dataItem->data());
                    if (spec.empty()) {
                        spec = "--";
                    }
                    info.append(unitPrefix);
                    if (!values.isEmpty()) {
                        info.append(", ");
                    }
                    info.append(QString::fromStdString(spec)).append(suffix);
                }
            }
            break;
        }
        case Icd::ItemComplex:
        {
            const Icd::ComplexItemPtr itemComplex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(dataItem);
            if (!itemComplex) {
                break;
            }

            Icd::TablePtr table = itemComplex->table();
            if (table->itemCount() == 0) {
                const char *buffer = itemComplex->buffer();
                int bufferSize = (int)itemComplex->bufferSize();
                if (bufferSize <= 16) {
                    info.append(dataPrefix);
                    for (int i = 0; i < bufferSize; ++i) {
                        info.append(QString("%1").arg((ushort)(*(buffer + i)),
                                                      2, 16, QChar('0')).toUpper());
                    }
                    info.append(suffix);
                }
            }
            break;
        }
        default:
            break;   // not supported data type
        }
    }

    QString text;
    // offset
    if (showAttris_ & CoreTreeWidget::ShowOffset) {
        text.append(ItemWorkerGroup::generateItemOffset(dataItem));
    }
    // name
    text.append(QString::fromStdString(dataItem->name().empty() ? "<?>" : dataItem->name()));
    // type, value
    if (showAttris_ & CoreTreeWidget::ShowType) {
        text.append(" <font color=green size=2>[")
                .append(QString::fromStdString(dataItem->typeString()).toUpper());
        if (info.isEmpty()) {
            text.append("]</font>");
        } else {
            text.append(": ").append(info).append("<font color=green size=2>]</font>");
        }
    } else {
        if (!info.isEmpty()) {
            text.append(" <font color=green size=2>[</font>").append(info)
                    .append("<font color=green size=2>]</font>");
        }
    }

    item->setText(text);

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
        const Icd::ComplexItemPtr itemComplex = JHandlePtrCast<Icd::ComplexItem, Icd::Item>(dataItem);
        if (!itemComplex) {
            break;
        }
        //
        QString text;
        // offset
        if (showAttris_ & CoreTreeWidget::ShowOffset) {
            text.append(ItemWorkerGroup::generateItemOffset(itemComplex));
        }
        // name
        const std::string name = itemComplex->name();
        text.append(QString::fromStdString(name.empty() ? "<?>" : name));
        // type
        if (showAttris_ & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>[COMPLEX]</font>");
        }
        item->setText(text);
        // sub-table
        Icd::TablePtr table = itemComplex->table();
        if (table->itemCount() > 0) {
            updateItemData(item, table, showValue);
        }
        break;
    }
    case Icd::ItemFrame:
    {
        const Icd::FrameItemPtr &itemFrame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(dataItem);
        if (!itemFrame) {
            break;
        }

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
        if (showAttris_ & (CoreTreeWidget::ShowData
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
        if (showAttris_ & CoreTreeWidget::ShowOffset) {
            text.append(QString("<font color=green size=2>[%1]</font> ")
                        .arg(bitOffset, 2, 10, QChar('0')));
        }

        // name
        text.append(QString::fromStdString(name));
        // value
        if (_showValue) {
            text.append(" <font color=#1296c3>[");
            QStringList values;
            if (showAttris_ & (CoreTreeWidget::ShowData | CoreTreeWidget::ShowValue)) {
                values.append(QString("%1").arg(bitItem->testBit(bitOffset) ? 1 : 0,
                                                1, 10, QChar('0')));
            }
            // spec
            if (showAttris_ & CoreTreeWidget::ShowSpec) {
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

    if (table->isFrameTable()) {
        QString text;
        // name
        const std::string name = table->name();
        text.append(QString::fromStdString(name.empty() ? "<?>" : name));
        // type
        if (showAttris_ & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>[TABLE]</font>");
        }
        // channel
        if (!table->parent() && item->data(Icd::TreeChannelIdRole).isValid()) {
            text.append(QStringLiteral(" <font color=#1296c3 size=3>[")
                        + QString::fromStdString(worker_->channel()->name()) + "]</font>");
        }
        //
        if (show) {
            // period
            text.append("<font color=#1296c3> (").append(QString::number(table->period()))
                    .append("ms)</font>");
        }

        item->setText(text);
    } else {
        QString text;
        // offset
        if (showAttris_ & CoreTreeWidget::ShowOffset) {
            text.append(ItemWorkerGroup::generateItemOffset(table));
        }
        // name
        const std::string name = table->name();
        text.append(QString::fromStdString(name.empty() ? "<?>" : name));
        // type
        if (showAttris_ & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>[TABLE]</font>");
        }

        item->setText(text);
    }
}

} // end of namespace Icd

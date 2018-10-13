#include "precomp.h"
#include "JWorkerGroup.h"
#include "icdworker/icdworker_pool.h"
#include "icdcore/icdcore_inc.h"
#include "JProtoTreeView_p.h"

namespace Icd {

JWorkerGroup::JWorkerGroup(QStandardItem *itemTable, const WorkerPtr &worker,
                           JProtoTreeView::ShowAttributes attrs,
                           QObject *parent)
    : QObject(parent)
    , itemTable_(itemTable)
    , worker_(worker)
    , showAttris_(attrs)
    , bindTableTypes_(JProtoTreeView::BindOnlyRecv)
    , timerId_(0)
    , timerInterval_(100)  // 100 ms
    , dataFormat_(16)  // default: hex format
{
    //
    treeView_ = qobject_cast<JProtoTreeViewPrivate*>(
                dynamic_cast<JTreeViewItem*>(itemTable)->view());
    //
    connect(treeView_, &JProtoTreeViewPrivate::valueColorChanged, this, [=](){
        updateItemData();
    });
    //
    updateTimer();
}

JWorkerGroup::~JWorkerGroup()
{
    //
    setDirty();
}

void JWorkerGroup::updateTimer()
{
    if (!worker_ || !worker_->isOpen()) {
        stop();
        return;
    }

    if (!(showAttris_ & (JProtoTreeView::ShowData | JProtoTreeView::ShowValue))) {
        stop();
        return;
    }

    if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
        if (worker_->isOpen()) {
            start();
        } else {
            stop();
        }
    } else if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
        if (worker_->workerRecv()->isRunning()) {
            start();
        } else {
            stop();
        }
    }
}

void JWorkerGroup::updateItemData()
{
    if (worker_) {
        if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
            updateItemData(worker_->isOpen());
        } else if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
            updateItemData(worker_->workerRecv()->isRunning());
        }
    }
}

void JWorkerGroup::updateItemData(bool showValue)
{
    if (!itemTable_) {
        return;
    }

    Icd::TablePtr table = Icd::TablePtr();
    switch (bindTableTypes_) {
    case JProtoTreeView::BindOnlySend:
        table = worker_->workerSend()->table();
        break;
    case JProtoTreeView::BindOnlyRecv:
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

void JWorkerGroup::updateItemData(JProtoTreeView::ShowAttributes attrs,
                                  int dataFormat)
{
    showAttris_ = attrs;
    dataFormat_ = dataFormat;
    updateTimer();
    updateItemData();
}

int JWorkerGroup::timerInterval() const
{
    return timerInterval_;
}

void JWorkerGroup::setTimerInterval(int interval)
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

void JWorkerGroup::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId_) {
        updateItemData();
    } else {
        QObject::timerEvent(event);
    }
}

void JWorkerGroup::updateItemData(QStandardItem *item, const Icd::FrameItemPtr &frame,
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
        updateFrameTable(frameItem, table, showValue, i);
        updateItemData(frameItem, table, showValue);
    }
}

void JWorkerGroup::updateItemData(QStandardItem *item, const TablePtr &table,
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

void JWorkerGroup::start()
{
    if (timerId_ == 0) {
        timerId_ = QObject::startTimer(timerInterval_, Qt::PreciseTimer);
    }
}

void JWorkerGroup::start(int interval)
{
    if (timerId_ > 0) {
        QObject::killTimer(timerId_);
        timerId_ = 0;
    }

    timerId_ = QObject::startTimer(interval);
    timerInterval_ = interval;
}

void JWorkerGroup::stop()
{
    if (timerId_ > 0) {
        QObject::killTimer(timerId_);
        timerId_ = 0;
        QApplication::removePostedEvents(this, QEvent::Timer);
    }
}

bool JWorkerGroup::isRunning() const
{
    return timerId_ > 0;
}

void JWorkerGroup::setRunning(bool running)
{
    if (worker_) {
        if (running) {
            start();
        } else {
            stop();
        }
    }
}

WorkerPtr JWorkerGroup::worker() const
{
    return worker_;
}

QStandardItem *JWorkerGroup::itemTable() const
{
    return itemTable_;
}

void JWorkerGroup::setWorker(const WorkerPtr &worker)
{
    if (worker_) {
        worker_->disconnect(this);
        if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
            worker_->workerSend()->disconnect(this);
        }
        if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
            worker_->workerRecv()->disconnect(this);
        }
    }

    worker_ = worker;

    setBindTableType(bindTableTypes_);

    //
    updateTimer();
}

void JWorkerGroup::setBindTableType(JProtoTreeView::BindTableTypes type)
{
    if (worker_) {
        //
        worker_->disconnect(this);
        if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
            worker_->workerSend()->disconnect(this);
        }
        if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
            worker_->workerRecv()->disconnect(this);
        }

        //
        bindTableTypes_ = type;

        //
        connect(worker_.get(), &Icd::Worker::opened,
                this, &Icd::JWorkerGroup::onWorkerOpened);
        connect(worker_.get(), &Icd::Worker::closed,
                this, &Icd::JWorkerGroup::onWorkerClosed);
        if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
            connect(worker_->workerSend().get(), &Icd::WorkerSend::started,
                    this, &Icd::JWorkerGroup::onWorkerStarted);
            connect(worker_->workerSend().get(), &Icd::WorkerSend::stopped,
                    this, &Icd::JWorkerGroup::onWorkerStopped);
        }
        if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
            connect(worker_->workerRecv().get(), &Icd::WorkerRecv::started,
                    this, &Icd::JWorkerGroup::onWorkerStarted);
            connect(worker_->workerRecv().get(), &Icd::WorkerRecv::stopped,
                    this, &Icd::JWorkerGroup::onWorkerStopped);
        }
    }
}

void JWorkerGroup::setDirty()
{
    stop();

    if (worker_) {
        worker_->disconnect(this);
        if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
            worker_->workerSend()->disconnect(this);
            worker_->workerSend()->setTable(Icd::TablePtr());
        }
        if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
            worker_->workerRecv()->disconnect(this);
            worker_->workerRecv()->setTable(Icd::TablePtr());
        }
    }
}

QString JWorkerGroup::generateItemOffset(Icd::Object *object, int offset)
{
    if (!object) {
        Q_ASSERT(false);
        return QString();
    }

    QString text;

    if (object->rtti() == Icd::ObjectComplex) {
        int i = 0;
        ++i;
    }

    switch (object->objectType()) {
    case Icd::ObjectTable:
    {
        Icd::Table *table = dynamic_cast<Icd::Table*>(object);
        if (!table) {
            break;
        }

        qreal itemOffset = -1;
        Icd::Object *parentObject = table->parent();
        if (parentObject && parentObject->objectType() == Icd::ObjectItem
                && parentObject->rtti() != Icd::ObjectFrame) {
            Icd::Item *item = dynamic_cast<Icd::Item*>(parentObject);
            if (item && item->parent()) {
                itemOffset = item->bufferOffset();
            }
        }
        if (itemOffset >= 0) {
            text.append(QString("<font color=green size=2>[%1:%2:%3]</font> ")
                        .arg(table->itemOffset(), 4, 10, QChar('0'))
                        .arg(int(table->bufferOffset()), 4, 10, QChar('0'))
                        .arg(int(table->bufferOffset() - itemOffset), 4, 10, QChar('0')));
        } else {
            text.append(QString("<font color=green size=2>[%1:%2]</font> ")
                        .arg(table->itemOffset() + offset, 4, 10, QChar('0'))
                        .arg(int(table->bufferOffset()), 4, 10, QChar('0')));
        }

        break;
    }
    case Icd::ObjectItem:
    {
        Icd::Item *item = dynamic_cast<Icd::Item*>(object);
        if (!item) {
            break;
        }

        qreal tableOffset = -1;
        Icd::Object *parentObject = item->parent();
        if (parentObject && parentObject->objectType() == Icd::ObjectTable) {
            Icd::Table *table = dynamic_cast<Icd::Table *>(parentObject);
            if (table && table->parent() && table->parent()->objectType() != Icd::ObjectSystem) {
                tableOffset = table->bufferOffset();
            }
        }
        if (tableOffset >= 0) {
            switch (item->type()) {
            case Icd::ItemBitMap:
            case Icd::ItemBitValue:
            {
                Icd::BitItem *itemBit = dynamic_cast<Icd::BitItem*>(item);
                if (!itemBit) {
                    break;
                }
                text.append(QString("<font color=green size=2>[%1:%2:%3.%4]</font> ")
                            .arg(item->itemOffset(), 4, 10, QChar('0'))
                            .arg(int(item->bufferOffset()), 4, 10, QChar('0'))
                            .arg(int(item->bufferOffset() - tableOffset), 4, 10, QChar('0'))
                            .arg(itemBit->bitStart(), 2, 10, QChar('0')));
                break;
            }
            default:
                text.append(QString("<font color=green size=2>[%1:%2:%3]</font> ")
                            .arg(item->itemOffset(), 4, 10, QChar('0'))
                            .arg(int(item->bufferOffset()), 4, 10, QChar('0'))
                            .arg(int(item->bufferOffset() - tableOffset), 4, 10, QChar('0')));
                break;
            }
        } else {
            switch (item->type()) {
            case Icd::ItemBitMap:
            case Icd::ItemBitValue:
            {
                Icd::BitItem *itemBit = dynamic_cast<Icd::BitItem*>(item);
                if (!itemBit) {
                    break;
                }
                text.append(QString("<font color=green size=2>[%1:%2.%3]</font> ")
                            .arg(item->itemOffset(), 4, 10, QChar('0'))
                            .arg(int(item->bufferOffset()), 4, 10, QChar('0'))
                            .arg(itemBit->bitStart(), 2, 10, QChar('0')));
                break;
            }
            default:
                text.append(QString("<font color=green size=2>[%1:%2]</font> ")
                            .arg(item->itemOffset(), 4, 10, QChar('0'))
                            .arg(int(item->bufferOffset()), 4, 10, QChar('0')));
                break;
            }
        }
        break;
    }
    default:
        break;
    }

    return text;
}

void JWorkerGroup::onWorkerOpened()
{
    if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
        updateTimer();
    } else if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
        //
    }
}

void JWorkerGroup::onWorkerClosed()
{
    stop();
}

void JWorkerGroup::onWorkerStarted()
{
    start(timerInterval_);
}

void JWorkerGroup::onWorkerStopped()
{
    stop();
}

void JWorkerGroup::updateItemData(QStandardItem *item, const ItemPtr &dataItem,
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
        if (showAttris_ & (JProtoTreeView::ShowData | JProtoTreeView::ShowValue)) {
            return true;
        }
        //
        switch (item->type()) {
        case Icd::ItemBitMap:
        case Icd::ItemBitValue:
            if (showAttris_ & JProtoTreeView::ShowSpec) {
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
        case Icd::ItemHeader:
        {
            QStringList values;
            // data
            if (showAttris_ & JProtoTreeView::ShowData) {
                values.append(QString("%1")
                              .arg(uint(dataItem->data()), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & JProtoTreeView::ShowValue) {
                values.append(QString("%1").arg(qulonglong(dataItem->data()), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemCounter:
        {
            QStringList values;
            // data
            if (showAttris_ & JProtoTreeView::ShowData) {
                values.append(QString("%1")
                              .arg(uint(dataItem->data()), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & JProtoTreeView::ShowValue) {
                values.append(QString("%1").arg(qulonglong(dataItem->data()), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemCheck:
        {
            QStringList values;
            // data
            if (showAttris_ & JProtoTreeView::ShowData) {
                values.append(QString("%1")
                              .arg(uint(dataItem->data()), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & JProtoTreeView::ShowValue) {
                values.append(QString("%1").arg(qulonglong(dataItem->data()), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemFrameCode:
        {
            QStringList values;
            // data
            if (showAttris_ & JProtoTreeView::ShowData) {
                values.append(QString("%1")
                              .arg(uint(dataItem->data()), asciiCount, dataFormat_, QChar('0'))
                              .toUpper());
            }
            // value
            if (showAttris_ & JProtoTreeView::ShowValue) {
                values.append(QString("%1").arg(qulonglong(dataItem->data()), 0, 10));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemNumeric:
        {
            QStringList values;
            //
            const Icd::NumericItemPtr itemNumeric = JHandlePtrCast<Icd::NumericItem>(dataItem);
            if (!itemNumeric) {
                break;
            }
            // data
            if (showAttris_ & JProtoTreeView::ShowData) {
                double originalData = itemNumeric->originalData();
                switch (itemNumeric->numericType()) {
                case Icd::NumericF32:
                {
                    float fOriginalData = float(originalData);
                    qint32 iOriginalData;
                    memcpy(&iOriginalData, &fOriginalData, 4);
                    values.append(QString("%1").arg(iOriginalData, asciiCount, dataFormat_, QChar('0')).toUpper());
                    break;
                }
                case Icd::NumericF64:
                {
                    qint64 iOriginalData;
                    memcpy(&iOriginalData, &originalData, 8);
                    values.append(QString("%1").arg(iOriginalData, asciiCount, dataFormat_, QChar('0')).toUpper());
                    break;
                }
                default:
                {
                    qulonglong data = qulonglong(itemNumeric->originalData());
                    data &= (1ull << (int(itemNumeric->bufferSize()) << 3)) - 1;
                    values.append(QString("%1").arg(data, asciiCount, dataFormat_, QChar('0')).toUpper());
                    break;
                }}
            }
            // value
            if (showAttris_ & JProtoTreeView::ShowValue) {
                values.append(QString::fromStdString(itemNumeric->prettyValue()));
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
            if (showAttris_ & JProtoTreeView::ShowValue && !unit.empty()) {
                info.append(unitPrefix).append(' ').append(QString::fromStdString(unit)).append(suffix);
            }
            break;
        }
        case Icd::ItemBitMap:
        {
            QStringList values;
            // data
            if (showAttris_ & JProtoTreeView::ShowData) {
                values.append(QString("%1")
                              .arg(uint(dataItem->data()), asciiCount, dataFormat_, QChar('0')).toUpper()
                              .toUpper());
            }
            // value
            if (showAttris_ & JProtoTreeView::ShowValue) {
                values.append(QString("%1").arg(uint(dataItem->data()),
                                                int(dataItem->bufferSize() * 8), 2, QChar('0')));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            break;
        }
        case Icd::ItemBitValue:
        {
            QStringList values;
            // data
            if (showAttris_ & JProtoTreeView::ShowData) {
                values.append(QString("%1")
                              .arg(uint(dataItem->data()), asciiCount, dataFormat_, QChar('0')).toUpper()
                              .toUpper());
            }
            // value
            if (showAttris_ & JProtoTreeView::ShowValue) {
                values.append(QString("%1").arg(uint(dataItem->data()),
                                                int(dataItem->bufferSize() * 8), 2, QChar('0')));
            }
            info.append(dataPrefix).append(values.join(", ")).append(suffix);
            // spec
            if (showAttris_ & JProtoTreeView::ShowSpec) {
                const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem>(dataItem);
                if (bitItem) {
                    std::string spec = bitItem->specAt(Icd::icd_uint64(dataItem->data()));
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
            const Icd::ComplexItemPtr itemComplex = JHandlePtrCast<Icd::ComplexItem>(dataItem);
            if (!itemComplex) {
                break;
            }

            Icd::TablePtr table = itemComplex->table();
            if (table->itemCount() == 0) {
                const char *buffer = itemComplex->buffer();
                int bufferSize = int(itemComplex->bufferSize());
                if (bufferSize <= 16) {
                    info.append(dataPrefix);
                    for (int i = 0; i < bufferSize; ++i) {
                        info.append(QString("%1").arg(ushort(*(buffer + i)),
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
    if (showAttris_ & JProtoTreeView::ShowOffset) {
        text.append(JWorkerGroup::generateItemOffset(dataItem.get()));
    }
    // name
    text.append(QString::fromStdString(dataItem->name().empty() ? "?" : dataItem->name()));
    // type, value
    if (showAttris_ & JProtoTreeView::ShowType) {
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
        const Icd::BitItemPtr bitItem = JHandlePtrCast<Icd::BitItem>(dataItem);
        if (!bitItem) {
            break;
        }
        updateItemBitMap(item, bitItem, showValue);
        break;
    }
    case Icd::ItemComplex:
    {
        const Icd::ComplexItemPtr itemComplex = JHandlePtrCast<Icd::ComplexItem>(dataItem);
        if (!itemComplex) {
            break;
        }
        //
        QString text;
        // offset
        if (showAttris_ & JProtoTreeView::ShowOffset) {
            text.append(JWorkerGroup::generateItemOffset(itemComplex.get()));
        }
        // name
        const std::string name = itemComplex->name();
        text.append(QString::fromStdString(name.empty() ? "?" : name));
        // type
        if (showAttris_ & JProtoTreeView::ShowType) {
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
        const Icd::FrameItemPtr &itemFrame = JHandlePtrCast<Icd::FrameItem>(dataItem);
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

void JWorkerGroup::updateItemBitMap(QStandardItem *item, const BitItemPtr &bitItem,
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
        if (showAttris_ & (JProtoTreeView::ShowData
                           | JProtoTreeView::ShowValue
                           | JProtoTreeView::ShowSpec)) {
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
        if (showAttris_ & JProtoTreeView::ShowOffset) {
            text.append(QString("<font color=green size=2>[%1]</font> ")
                        .arg(bitOffset, 2, 10, QChar('0')));
        }

        // name
        text.append(QString::fromStdString(name));
        // value
        if (_showValue) {
            text.append(" <font color=#1296c3>[");
            QStringList values;
            if (showAttris_ & (JProtoTreeView::ShowData | JProtoTreeView::ShowValue)) {
                values.append(QString("%1").arg(bitItem->testBit(bitOffset) ? 1 : 0,
                                                1, 10, QChar('0')));
            }
            // spec
            if (showAttris_ & JProtoTreeView::ShowSpec) {
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

void JWorkerGroup::updateFrameTable(QStandardItem *item, const TablePtr &table, bool show, int index)
{
    if (!item || !table) {
        return;
    }

    if (table->isFrameTable()) {
        QString text;
        // name
        const std::string name = table->name();
        text.append(QString::fromStdString(name.empty() ? "?" : name));
        // type
        if (showAttris_ & JProtoTreeView::ShowType) {
            text.append(" <font color=green size=2>[TABLE]</font>");
        }
        // channel
        if (!table->parent() && item->data(Icd::TreeChannelIdRole).isValid()) {
            text.append(QString(" <font color=#1296c3 size=3>[")
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
        if (showAttris_ & JProtoTreeView::ShowOffset) {
            text.append(JWorkerGroup::generateItemOffset(table.get(), index));
        }
        // name
        const std::string name = table->name();
        text.append(QString::fromStdString(name.empty() ? "?" : name));
        // type
        if (showAttris_ & JProtoTreeView::ShowType) {
            text.append(" <font color=green size=2>[TABLE]</font>");
        }

        item->setText(text);
    }
}

} // end of namespace Icd

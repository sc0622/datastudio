#include "precomp.h"
#include "IcdListWidget.h"
#include "icdcore/icd_table.h"
#include "icdcore/icd_item_bit.h"
#include "icdcore/icd_item_complex.h"
#include "icdcore/icd_item_frame.h"
#include "icdwidget/icdwidget_global.h"

IcdListWidget::IcdListWidget(const Icd::TablePtr &table, const Icd::ObjectPtr &first,
                             const QString &filePath, int bitOffset, QWidget *parent)
    : QWidget(parent)
    , d_table(table)
    , d_filePath(filePath)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_listWidget = new QListWidget(this);
    d_listWidget->setStyle(QStyleFactory::create("windowsxp"));
    d_listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    d_listWidget->setAlternatingRowColors(true);
    d_listWidget->setContextMenuPolicy(Qt::NoContextMenu);
    d_listWidget->setDropIndicatorShown(true);
    d_listWidget->setAcceptDrops(true);
    d_listWidget->installEventFilter(this);
    vertLayoutMain->addWidget(d_listWidget);

    QAction *actionRemove = new QAction(QStringLiteral("删除选中项"), this);
    d_listWidget->addAction(actionRemove);

    //
    connect(d_listWidget, &QListWidget::itemSelectionChanged, this, [=](){
        QList<QListWidgetItem*> selectedItems = d_listWidget->selectedItems();
        if (selectedItems.isEmpty()) {
            d_listWidget->setContextMenuPolicy(Qt::NoContextMenu);
        } else {
            d_listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
        }
    });
    connect(actionRemove, &QAction::triggered, this, [=](){
        removeSelectedItems();
    });

    //
    addItem(first, bitOffset);
}

QList<ItemInfo> IcdListWidget::items()
{
    return d_items;
}

const QList<ItemInfo> &IcdListWidget::items() const
{
    return d_items;
}

QString IcdListWidget::generateItemNamePath(const Icd::ItemPtr &item, int bitOffset)
{
    int _bitOffset = bitOffset;
    QString title;
    Icd::Object *tempObject = item.get();

    while (tempObject && tempObject->parent()) {
        if (tempObject->objectType() == Icd::ObjectItem) {
            Icd::Item *tempItem = dynamic_cast<Icd::Item *>(tempObject);
            if (!tempItem) {
                break;
            }
            if (tempItem->type() == Icd::ItemComplex) {
                tempObject = tempObject->parent();
                continue;
            }
        }
        if (_bitOffset >= 0) {
            Icd::BitItem *bitItem = dynamic_cast<Icd::BitItem *>(tempObject);
            if (!bitItem) {
                Q_ASSERT(false);
                return QStringLiteral("<?>");
            }
            title = (QString("%1*[#%2]")
                     .arg(QString::fromStdString(bitItem->specAt(_bitOffset)))
                     .arg(_bitOffset));
            _bitOffset = -1;
        }
        if (!title.isEmpty()) {
            title.prepend('/');
        }
        title.prepend(QString::fromStdString(tempObject->name()));
        tempObject = tempObject->parent();
    }

    if (title.isEmpty()) {
        return QStringLiteral("<?>");
    }

    title.replace(' ', '*');

    return title;
}

bool IcdListWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_listWidget) {
        switch (event->type()) {
        case QEvent::DragEnter:
        {
            QDragEnterEvent *deEvent = reinterpret_cast<QDragEnterEvent *>(event);
            if (!deEvent) {
                Q_ASSERT(false);
                break;
            }
            onDragEnterEvent(deEvent);
            return true;
        }
        case QEvent::Drop:
        {
            QDropEvent *dEvent = reinterpret_cast<QDropEvent *>(event);
            if (!dEvent) {
                Q_ASSERT(false);
                break;
            }
            onDropEvent(dEvent);
            return true;
        }
        case QEvent::KeyPress:
        {
            QKeyEvent *keyEvent = reinterpret_cast<QKeyEvent*>(event);
            if (!keyEvent) {
                Q_ASSERT(false);
                break;
            }
            if (keyEvent->key() == Qt::Key_Delete) {
                removeSelectedItems();
                break;
            }
            break;
        }
        case QEvent::Paint:
        {
            QPainter painter(this);
            painter.save();

            QFont font("microsoft yahei", 24);
            painter.setFont(font);
            painter.setPen(QPen(QColor(64, 64, 64, 80)));
            QTextOption textOption(Qt::AlignCenter);
            painter.drawText(rect(), QStringLiteral("<拖动左边树节点到此视图>"), textOption);

            painter.restore();

            break;
        }
        default:
            break;
        }
    }

    return false;
}

void IcdListWidget::onDragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        event->accept();
    } else {
        event->ignore();
    }
}

void IcdListWidget::onDropEvent(QDropEvent *event)
{
    //
    if (!event->mimeData()->hasFormat("icd/table-drag/analyse")) {
        return;
    }

    //
    QStandardItem *itemTable = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("itemTable").value<void *>());
    if (!itemTable) {
        return;
    }

    if (!d_table) {
        qWarning(QStringLiteral("工作组没有绑定接收端协议表！").toLocal8Bit());
        return;
    }

    //
    const QString filePath = itemTable->data(Icd::TreeFilePathRole).toString();
    if (filePath.isEmpty()) {
        return;
    }

    //
    if (filePath != d_filePath) {
        return;
    }

    //
    QStandardItem *item = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("item").value<void *>());
    if (!item) {
        return;
    }

    //
    const QString itemDomain = event->mimeData()->property("domain").toString();

    //
    Icd::ObjectPtr dataObject = Icd::ObjectPtr(0);
    switch (item->type()) {
    case Icd::TreeItemTypeTable:
    {
        dataObject = d_table;
        break;
    }
    case Icd::TreeItemTypeItemTable:
    {
        // get table object
        Icd::ObjectPtr itemTable = d_table->tableByDomain(itemDomain.section('/', 3).toStdString());
        if (!itemTable) {
            break;
        }
        //
        dataObject = itemTable;
        break;
    }
    case Icd::TreeItemTypeDataItem:
    {
        // get dataItem object
        Icd::ObjectPtr dataItem = d_table->itemByDomain(itemDomain.section('/', 3).toStdString());
        if (!dataItem) {
            break;
        }
        dataObject = dataItem;
        break;
    }
    case Icd::TreeItemTypeItemBitMap:
    {
        // get dataItem object
        Icd::ObjectPtr dataItem = d_table->itemByDomain(itemDomain.section('/', 3).toStdString());
        if (!dataItem) {
            break;
        }

        //
        QApplication::processEvents(QEventLoop::EventLoopExec);

        //
        const int bitOffset = item->data(Icd::TreeBitOffsetRole).toInt();
        if (bitOffset == -1) {
            Q_ASSERT(false);
            return;
        }

        //
        addItem(JHandlePtrCast<Icd::Item, Icd::Object>(dataItem), bitOffset);

        return;
    }
    default:
        break;
    }

    //
    if (!dataObject) {
        return;
    }

    //
    QApplication::processEvents(QEventLoop::EventLoopExec);

    //
    if (!addItem(dataObject)) {
        return;
    }
}

bool IcdListWidget::addItem(const Icd::ItemPtr &item, int bitOffset)
{
    if (!item) {
        Q_ASSERT(false);
        return false;
    }

    switch (item->type()) {
    case Icd::ItemComplex:
    {
        const Icd::ComplexItemPtr complexItem =
                JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item);
        if (!complexItem) {
            return false;
        }
        return addItem(complexItem->table());
    }
    case Icd::ItemFrame:
    {
        const Icd::FrameItemPtr frameItem =
                JHandlePtrCast<Icd::FrameItem, Icd::Item>(item);
        if (!frameItem) {
            return false;
        }
        return addItem(frameItem);
    }
    default:
        break;
    }

    const QString namePath = generateItemNamePath(item, bitOffset);
    if (namePath.isEmpty()) {
        Q_ASSERT(false);
        return false;
    }

    //
    QList<QListWidgetItem*> items = d_listWidget->findItems(namePath, Qt::MatchFixedString);
    if (!items.isEmpty()) {
        return true;
    }

    QListWidgetItem *listItem = new QListWidgetItem(namePath);
    d_listWidget->addItem(listItem);
    d_items.append(ItemInfo(item, bitOffset));

    return true;
}

bool IcdListWidget::addItem(const Icd::TablePtr &table, int bitOffset)
{
    if (!table) {
        return false;
    }

    const Icd::ItemPtrArray &dataItems = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend(); ++citer) {
        const Icd::ItemPtr &dataItem = *citer;
        if (!addItem(dataItem, bitOffset)) {
            continue;
        }
    }

    return true;
}

bool IcdListWidget::addItem(const Icd::FrameItemPtr &frame, int bitOffset)
{
    if (!frame) {
        return false;
    }

    const Icd::TablePtrMap &tables = frame->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        if (!addItem(table, bitOffset)) {
            continue;
        }
    }

    return true;
}

bool IcdListWidget::addItem(const Icd::ObjectPtr &object, int bitOffset)
{
    if (!object) {
        return false;
    }

    switch (object->objectType()) {
    case Icd::ObjectTable:
    {
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table, Icd::Object>(object);
        if (!table) {
            return false;
        }

        if (!addItem(table, bitOffset)) {
            return false;
        }

        break;
    }
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr dataItem = JHandlePtrCast<Icd::Item, Icd::Object>(object);
        if (!dataItem) {
            return false;
        }

        if (!addItem(dataItem, bitOffset)) {
            return false;
        }

        break;
    }
    default:
        break;
    }

    return true;
}

void IcdListWidget::removeSelectedItems()
{
    QList<QListWidgetItem*> selectedItems = d_listWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    QListIterator<QListWidgetItem*> citer(selectedItems);
    citer.toBack();
    while (citer.hasPrevious()) {
        QListWidgetItem *item = citer.previous();
        delete d_listWidget->takeItem(d_listWidget->row(item));
    }
}

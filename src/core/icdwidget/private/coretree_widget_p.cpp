#include "precomp.h"
#include "coretree_widget_p.h"
#include "icdworker/icdworker.h"
#include "icdworker/icdworker_pool.h"
#include "../bindchannel_widget.h"
#include "private/itemworkergroup.h"
#include "../icd_searchedit.h"
#include "../progressdialog.h"
#include <QFutureWatcher>
#include <QDomDocument>
#include <QtConcurrent>

namespace Icd {

// class TableItemWidget

TableItemWidget::TableItemWidget(const QString &text,
                                 CoreTreeWidget::BindTableTypes bindingTypes,
                                 QWidget *parent)
    : QWidget(parent)
    , d_worker(Q_NULLPTR)
    , d_bindTableTypes(bindingTypes)
{
    setObjectName("TableItemWidget");
    setFixedHeight(22);
    setStyleSheet("QWidget#TableItemWidget{background:transparent;}");

    QHBoxLayout *layoutMain = new QHBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);
    layoutMain->setSpacing(3);

    d_labelTitle = new QLabel(text, this);
    d_labelTitle->installEventFilter(this);
    layoutMain->addWidget(d_labelTitle);

    d_buttonRun = new QPushButton(this);
    d_buttonRun->setObjectName("buttonRun");
    d_buttonRun->setFixedSize(40, 20);
    d_buttonRun->setCheckable(true);
    layoutMain->addWidget(d_buttonRun);

    layoutMain->addSpacing(10);

    layoutMain->addStretch();

    connect(d_buttonRun, &QPushButton::toggled, this, [=](bool checked){
        toggle(checked);
    });

    //
    d_buttonRun->setChecked(false);
    d_buttonRun->hide();
    updateButtonIcon(d_buttonRun->isChecked());
}

TableItemWidget::~TableItemWidget()
{

}

QString TableItemWidget::text() const
{
    return d_labelTitle->text();
}

void TableItemWidget::setText(const QString &text)
{
    d_labelTitle->setText(text);
}

WorkerPtr TableItemWidget::worker() const
{
    return d_worker;
}

void TableItemWidget::setWorker(const WorkerPtr &worker)
{
    if (d_worker) {
        if (d_worker != worker) {
            stop();
        }
        d_worker->disconnect(this);
        d_worker->workerSend()->disconnect(this);
        d_worker->workerRecv()->disconnect(this);
    }

    d_worker = worker;

    if (d_worker) {
        d_worker->disconnect(this);
        d_buttonRun->show();
        if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
            d_buttonRun->setChecked(d_worker->workerSend()->isRunning());
            connect(d_worker->workerSend().get(), &Icd::WorkerSend::toggled,
                    this, [=](bool checked){
                d_buttonRun->setChecked(checked);
            });
        } else  if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
            d_buttonRun->setChecked(d_worker->workerRecv()->isRunning());
            connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::toggled,
                    this, [=](bool checked){
                d_buttonRun->setChecked(checked);
            });
        }
    } else {
        d_buttonRun->hide();
        d_buttonRun->setChecked(false);
    }
}

void TableItemWidget::start()
{
    toggle(true);
}

void TableItemWidget::stop()
{
    toggle(false);
}

void TableItemWidget::toggle(bool checked)
{
    if (d_worker) {
        Icd::WorkerTransPtr workerTrans = Q_NULLPTR;
        if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
            workerTrans = d_worker->workerSend();
        } else if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
            workerTrans = d_worker->workerRecv();
        } else if (d_bindTableTypes == CoreTreeWidget::BindAllTable) {
            Q_ASSERT(false);
        }
        if (workerTrans) {
            if (checked) {
                if (!workerTrans->isRunning()) {
                    if (!d_worker->isOpen()) {
                        if (!d_worker->open()) {
                            return;
                        }
                    }
                    workerTrans->start();
                }
            } else {
                if (workerTrans->isRunning()) {
                    workerTrans->stop();
                    d_worker->close();
                }
            }
        }
    }

    updateButtonIcon(checked);
}

bool TableItemWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_labelTitle) {
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = reinterpret_cast<QMouseEvent *>(event);
            if (mouseEvent) {
                emit clicked();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void TableItemWidget::updateButtonIcon(bool checked)
{
    if (checked) {
        d_buttonRun->setIcon(QIcon(":/icdwidget/image/tree/stop.png"));
    } else {
        d_buttonRun->setIcon(QIcon(":/icdwidget/image/tree/run.png"));
    }
}

// class CoreTreeWidgetPrivate

CoreTreeWidgetPrivate::CoreTreeWidgetPrivate(CoreTreeWidget *q)
    : JTreeView(q)
    , d_searchEdit(0)
    , q_ptr(q)
    , d_parser(0)
    , d_loadingDeep(Icd::ObjectTable)
    , d_intervalUpdate(200)     // 200ms
    , d_bindTableTypes(CoreTreeWidget::BindAllTable)
    , d_showAttris(CoreTreeWidget::ShowType | CoreTreeWidget::ShowValue)
    , d_dataFormat(16)
    , d_treeModes(0)
{

}

CoreTreeWidgetPrivate::~CoreTreeWidgetPrivate()
{
    if (d_watcher.isRunning()) {
        d_watcher.cancel();
        d_watcher.waitForFinished();
    }

    //
    Icd::WorkerPool::getInstance()->disconnect(this);

    //
    unbindChannel(invisibleRootItem());
}

void CoreTreeWidgetPrivate::init()
{
    Q_Q(CoreTreeWidget);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(1);

    d_searchEdit = new SearchEdit(q);
    vertLayoutMain->addWidget(d_searchEdit);

    vertLayoutMain->addWidget(this);

    //
    setItemDelegateForColumn(0, new JTreeItemDelegate(this));

    //
    JTreeSortFilterModel *filterModel = new JTreeSortFilterModel(this);
    setFilterModel(filterModel);

    setAnimated(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    header()->setDefaultAlignment(Qt::AlignCenter);
    header()->hide();
    setHeaderLabel(QStringLiteral("数据协议信息"));

    //
    connect(d_searchEdit, &SearchEdit::textChanged, this, [=](const QString &text){
        filterModel->setFilterPattern(text);
    });
    connect(this, SIGNAL(itemPressed(QStandardItem*)),
            this, SLOT(onTreeItemPressed(QStandardItem*)));
    connect(this, SIGNAL(itemPressed(QStandardItem*)),
            q, SIGNAL(itemPressed(QStandardItem*)));
    connect(this, SIGNAL(itemClicked(QStandardItem*)),
            q, SIGNAL(itemClicked(QStandardItem*)));
    connect(this, SIGNAL(itemDoubleClicked(QStandardItem*)),
            q, SIGNAL(itemDoubleClicked(QStandardItem*)));
    connect(this, SIGNAL(currentItemChanged(QStandardItem*,QStandardItem*)),
            q, SIGNAL(currentItemChanged(QStandardItem*,QStandardItem*)));
    connect(this, SIGNAL(itemUnloaded(QStandardItem*,QStandardItem*)),
            q, SIGNAL(itemUnloaded(QStandardItem*,QStandardItem*)));
    connect(this, SIGNAL(channelBound(QStandardItem*,QString)),
            q, SIGNAL(channelBound(QStandardItem*,QString)));
    connect(this, SIGNAL(channelUnbound(QStandardItem*,QString)),
            q, SIGNAL(channelUnbound(QStandardItem*,QString)));
    connect(this, SIGNAL(channelChanged(QStandardItem*,QString)),
            q, SIGNAL(channelChanged(QStandardItem*,QString)));
    connect(this, SIGNAL(unbindItem(QStandardItem*,QStandardItem*)),
            q, SIGNAL(unbindItem(QStandardItem*,QStandardItem*)));
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerRemoved,
            this, &Icd::CoreTreeWidgetPrivate::onWorkerRemoved);
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerCleared,
            this, &Icd::CoreTreeWidgetPrivate::onWorkerCleared);
}

void CoreTreeWidgetPrivate::setParser(const Icd::ParserPtr &parser)
{
    d_parser = parser;
}

int CoreTreeWidgetPrivate::loadingDeep() const
{
    return d_loadingDeep;
}

void CoreTreeWidgetPrivate::setLoadingDeep(int deep)
{
    d_loadingDeep = deep;
}

int CoreTreeWidgetPrivate::intervalUpdate() const
{
    return d_intervalUpdate;
}

void CoreTreeWidgetPrivate::setIntervalUpdate(int interval)
{
    if (interval != d_intervalUpdate) {
        d_intervalUpdate = interval;
        //
        QHashIterator<QStandardItem*, ItemWorkerGroup*> citer(d_workerGroups);
        while (citer.hasNext()) {
            citer.next();
            citer.value()->start(interval);
        }
    }
}

bool CoreTreeWidgetPrivate::loadData()
{
    bool result = true;
    //
    clearData();
    //
#if 1
    JTreeViewItem *itemRoot = new JTreeViewItem(QStringLiteral("协议"),
                                                Icd::TreeItemTypeRoot);
    invisibleRootItem()->appendRow(itemRoot);
#else
    JTreeViewItem *itemRoot = invisibleRootItem();
#endif
    //
    result = result && loadVehicle(itemRoot, d_loadingDeep);

    //
    if (result) {
        if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
            expandItem(itemRoot, true, 3);
        } else if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
            expandItem(itemRoot, true, 3);
        } else if (d_bindTableTypes == CoreTreeWidget::BindAllTable) {
            expandItem(itemRoot, true, 3);
        }
    }

    return result;
}

bool CoreTreeWidgetPrivate::loadData(const Icd::TablePtr &table, const QString &domain)
{
    if (!table) {
        return false;
    }

    bool result = true;

    clearData();

    invisibleRootItem()->setData(domain.section('/', 0, 1), Icd::TreeItemDomainRole);

    result = result && loadTable(invisibleRootItem(), table);

    if (result && invisibleRootItem()->rowCount() > 0) {
        //
        expandItem(invisibleRootItem()->child(0), true, 1);
    }

    return result;
}

bool CoreTreeWidgetPrivate::loadData(const TablePtr &table, const QString &filePath,
                                     bool hasTimeFormat, int headerSize, const QString &domain)
{
    if (!table || domain.isEmpty()) {
        return false;
    }

    if (!QFile::exists(filePath)) {
        return false;
    }

    bool result = true;

    QStandardItem *invisibleRootItem = this->invisibleRootItem();
    QStandardItem *itemTable = findItemTable(invisibleRootItem, filePath);
    if (itemTable) {
        removeTableItem(itemTable);
    }

    if (result) {
        //
        invisibleRootItem->setData(domain.section('/', 0, 1), Icd::TreeItemDomainRole);
        invisibleRootItem->setData(QFileInfo(filePath).fileName(), Icd::TreeItemPathRole);
        //
        QStandardItem *itemTable = loadTable(invisibleRootItem, table);
        if (itemTable) {
            itemTable->setData(filePath, TreeFilePathRole);
            itemTable->setData(hasTimeFormat, TreeHasTimeFormatRole);
            itemTable->setData(headerSize, TreeHeaderSizeRole);
            itemTable->setText(itemTable->text()
                               .append(QString(" [%1]").arg(QFileInfo(filePath).fileName())));
        } else {
            result = false;
        }
    }

    int rowCount = invisibleRootItem->rowCount();
    if (result && rowCount > 0) {
        for (int i = 0; i < rowCount; ++i) {
            QStandardItem *childItem = invisibleRootItem->child(i);
            expandItem(childItem, (i == rowCount - 1), 1);
        }
        setCurrentItem(invisibleRootItem->child(rowCount - 1));
    }

    return result;
}

void CoreTreeWidgetPrivate::clearData()
{
    clearWorkerGroup();
    unbindChannelItem(invisibleRootItem());
    Q_Q(CoreTreeWidget);
    emit q->unbindItem(invisibleRootItem(), nullptr);
    clearContents();
}

CoreTreeWidget::ShowAttributes CoreTreeWidgetPrivate::showAttributes() const
{
    return d_showAttris;
}

void CoreTreeWidgetPrivate::setShowAttributes(CoreTreeWidget::ShowAttributes attrs)
{
    if (attrs != d_showAttris) {
        d_showAttris = attrs;
        updateItemData(invisibleRootItem()->child(0));
    }
}

void CoreTreeWidgetPrivate::setShowAttribute(CoreTreeWidget::ShowAttribute attr, bool on)
{
    bool needUpdate = false;
    if (on) {
        if (!(d_showAttris & attr)) {
            d_showAttris |= attr;
            needUpdate = true;
        }
    } else {
        if (d_showAttris & attr) {
            d_showAttris &= ~attr;
            needUpdate = true;
        }
    }

    if (needUpdate) {
        switch (attr) {
        case CoreTreeWidget::ShowOffset:
        case CoreTreeWidget::ShowType:
            updateItemData(invisibleRootItem()->child(0));
            break;
        case CoreTreeWidget::ShowData:
        case CoreTreeWidget::ShowValue:
        case CoreTreeWidget::ShowSpec:
        {
            QHashIterator<QStandardItem *, ItemWorkerGroup*> citer(d_workerGroups);
            while (citer.hasNext()) {
                citer.next();
                citer.value()->updateItemData(d_showAttris, d_dataFormat);
            }
        }
        default:
            break;
        }
    }
}

bool CoreTreeWidgetPrivate::testShowAttribute(CoreTreeWidget::ShowAttribute attr) const
{
    return d_showAttris & attr;
}

int CoreTreeWidgetPrivate::dataFormat() const
{
    return d_dataFormat;
}

void CoreTreeWidgetPrivate::setDataFormat(int format)
{
    if (format != d_dataFormat) {
        d_dataFormat = format;
        updateItemData(invisibleRootItem()->child(0));
    }
}

CoreTreeWidget::TreeModes CoreTreeWidgetPrivate::treeModes() const
{
    return d_treeModes;
}

void CoreTreeWidgetPrivate::setTreeMode(CoreTreeWidget::TreeModes modes)
{
    d_treeModes = modes;
}

void CoreTreeWidgetPrivate::setTreeMode(CoreTreeWidget::TreeMode mode, bool on)
{
    if (on) {
        if (!(d_treeModes & mode)) {
            d_treeModes |= mode;
        }
    } else {
        if (d_treeModes & mode) {
            d_treeModes &= ~mode;
        }
    }
}

bool CoreTreeWidgetPrivate::testTreeMode(CoreTreeWidget::TreeMode mode) const
{
    return d_treeModes & mode;
}

bool CoreTreeWidgetPrivate::isRunning() const
{
    for (auto &workerGroup : d_workerGroups) {
        if (workerGroup->isRunning()) {
            return true;
        }
    }

    return false;
}

void CoreTreeWidgetPrivate::setRunning(bool value)
{
    for (auto &workerGroup : d_workerGroups) {
        workerGroup->setRunning(value);
    }
}

void CoreTreeWidgetPrivate::bindingChannels(const QString &filePath)
{
    //
    unbindingChannels();
    //
    auto enabledFunc = [=](bool enabled, bool success){
        Q_UNUSED(success);
        if (enabled) {
            setEnabled(true);
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, QStringLiteral("绑定结果"),
                                     QStringLiteral("绑定完成"));
        } else {
            setEnabled(false);
            QApplication::setOverrideCursor(Qt::BusyCursor);
        }
    };
    //
    enabledFunc(false, false);
    //
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        enabledFunc(true, false);
        return;
    }

    QDomDocument document;
    if (!document.setContent(&file)) {
        enabledFunc(true, false);
        return;
    }

    const QDomElement emRoot = document.documentElement();
    if (emRoot.isNull()) {
        enabledFunc(true, false);
        return;
    }
    //
    QList<BindingData> bindings;
    for (QDomElement emItem = emRoot.firstChildElement("item");
         !emItem.isNull(); emItem = emItem.nextSiblingElement("item")) {
        QString tableDomain, channelId;
        // tableDomain
        if (emItem.hasAttribute("tableDomain")) {
            tableDomain = emItem.attribute("tableDomain");
        } else {
            continue;
        }
        // channelId
        if (emItem.hasAttribute("channelId")) {
            channelId = emItem.attribute("channelId");
        } else {
            continue;
        }
        //
        bindings.append({channelId, tableDomain, this});
    }
    if (bindings.isEmpty()) {
        enabledFunc(true, false);
        return;
    }
    //
    if (d_watcher.isRunning()) {
        d_watcher.cancel();
        d_watcher.waitForFinished();
    }
    d_watcher.disconnect(this);
    //
    QFuture<BindingData> future = QtConcurrent::mapped(bindings, bindingMapTask);
    connect(&d_watcher, &QFutureWatcher<BindingData>::resultReadyAt,
            this, [=](int resultIndex){
        const BindingData data = d_watcher.resultAt(resultIndex);
        if (!data.item || !data.worker) {
            return;
        }
        //
        if (!data.item->hasChildren()) {
            return;
        }
        //
        if (!bindChannel(data.item, data.worker, data.table)) {
            //
        }
    });
    connect(&d_watcher, &QFutureWatcher<BindingData>::finished, this, [=](){
        enabledFunc(true, true);
    });
    connect(&d_watcher, &QFutureWatcher<BindingData>::canceled, this, [=](){
        enabledFunc(true, false);
    });
    d_watcher.setFuture(future);
}

void CoreTreeWidgetPrivate::unbindingChannels()
{
    //
    stopAllChannels();
    //
    unbindChannelItem(invisibleRootItem());
}

void CoreTreeWidgetPrivate::exportBindingStatus(const QString &filePath)
{
    //
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }
    file.resize(0);

    QDomDocument document("binding");
    //
    QDomElement emRoot = document.createElement("binding");
    document.appendChild(emRoot);
    //
    procAllTableItem(invisibleRootItem(),  true,
                     [=,&emRoot](QStandardItem *item, const QString &channelId) -> bool {
        if (channelId.isEmpty()) {
            return true;
        }
        const QString &domain = item->data(TreeItemDomainRole).toString();
        if (domain.isEmpty()) {
            return true;
        }
        QDomElement emItem = emRoot.ownerDocument().createElement("item");
        emItem.setAttribute("tableDomain", domain);
        emItem.setAttribute("channelId", channelId);
        emRoot.appendChild(emItem);
        return true;
    });

    QTextStream ts(&file);
    document.save(ts, QDomNode::EncodingFromTextStream);

    //
    QMessageBox::information(this, QStringLiteral("导出完成"),
                             QStringLiteral("导出成功！"));
}

void CoreTreeWidgetPrivate::runAllChannels()
{
    procAllTableItem(invisibleRootItem(), true,
                     [=](QStandardItem *item, const QString &channelId) -> bool {
        Q_UNUSED(item);
        if (channelId.isEmpty()) {
            return true;
        }
        //
        const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(channelId.toStdString());
        if (!worker) {
            return true;
        }
        Icd::WorkerTransPtr workerTrans = Q_NULLPTR;
        if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
            workerTrans = worker->workerSend();
        } else if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
            workerTrans = worker->workerRecv();
        } else if (d_bindTableTypes == CoreTreeWidget::BindAllTable) {
            return true;
        }
        if (workerTrans) {
            if (!workerTrans->isRunning()) {
                if (!worker->isOpen()) {
                    if (!worker->open()) {
                        return true;
                    }
                }
                workerTrans->start();
            }
        }

        return true;
    });
}

void CoreTreeWidgetPrivate::stopAllChannels()
{
    procAllTableItem(invisibleRootItem(), true,
                     [=](QStandardItem *item, const QString &channelId) -> bool {
        Q_UNUSED(item);
        if (channelId.isEmpty()) {
            return true;
        }
        //
        const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(channelId.toStdString());
        if (!worker) {
            return true;
        }
        Icd::WorkerTransPtr workerTrans = Q_NULLPTR;
        if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
            workerTrans = worker->workerSend();
        } else if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
            workerTrans = worker->workerRecv();
        } else if (d_bindTableTypes == CoreTreeWidget::BindAllTable) {
            return true;
        }
        if (workerTrans) {
            if (workerTrans->isRunning()) {
                workerTrans->stop();
                worker->close();
            }
        }
        return true;
    });
}

void CoreTreeWidgetPrivate::onTreeItemPressed(QStandardItem *item)
{
    if (!item) {
        return;
    }

    // get type of treeWidgetItem
    const Icd::TreeItemType itemType = (Icd::TreeItemType)item->type();

    //
    Qt::MouseButtons mouseButtons = QApplication::mouseButtons();
    switch (mouseButtons) {
    case Qt::LeftButton:
    {
        if (itemType != Icd::TreeItemTypeTable) {
            break;
        }
        //
        break;
    }
    case Qt::RightButton:
    {
        // get deep status

        // dispatch
        switch (itemType) {
        case Icd::TreeItemTypeRoot:
            itemRootRightClicked(item, Icd::ObjectTable);
            break;
        case Icd::TreeItemTypeVehicle:
            itemVehicleRightClicked(item, Icd::ObjectItem);
            break;
        case Icd::TreeItemTypeSystem:
            itemSystemRightClicked(item, Icd::ObjectItem);
            break;
        case Icd::TreeItemTypeTable:
            itemTableRightClicked(item, Icd::ObjectItem);
            break;
        case Icd::TreeItemTypeDataItem:
            itemDataItemRightClicked(item, Icd::ObjectItem);
            break;
        case Icd::TreeItemTypeItemTable:
            itemItemTableRightClicked(item, Icd::ObjectItem);
            break;
        case Icd::TreeItemTypeItemBitMap:
            itemItemBitMapRightClicked(item, Icd::ObjectItem);
            break;
        default:
            break;
        }

        break;
    }
    default:
        break;
    }
}

void CoreTreeWidgetPrivate::onWorkerRemoved(const WorkerPtr &worker)
{
    removeWorkerGroup(worker);

    procAllTableItem(invisibleRootItem(), true,
                     [=](QStandardItem *item, const QString &channelId) -> bool {
        Q_UNUSED(item);
        if (channelId.isEmpty()) {
            return true;
        }
        //
        const Icd::WorkerPtr _worker =  Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(channelId.toStdString());
        if (!_worker) {
            return true;
        }
        Icd::WorkerTransPtr workerTrans = Q_NULLPTR;
        if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
            workerTrans = _worker->workerSend();
        } else if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
            workerTrans = _worker->workerRecv();
        } else if (d_bindTableTypes == CoreTreeWidget::BindAllTable) {
            return true;
        }
        if (workerTrans) {
            if (workerTrans->isRunning()) {
                workerTrans->stop();
                _worker->close();
            }
        }
        TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>(this->itemWidget(item));
        if (itemWidget) {
            itemWidget->setWorker(Icd::WorkerPtr());
            return false;
        }
        return true;
    });
}

void CoreTreeWidgetPrivate::onWorkerCleared()
{
    clearWorkerGroup();

    //
    unbindingChannels();
}

QStringList CoreTreeWidgetPrivate::mimeTypes() const
{
    QStringList types;
    if (d_treeModes & CoreTreeWidget::TreeModeMonitor) {
        types << "icd/table-drag/monitor";
    }
    if (d_treeModes & CoreTreeWidget::TreeModeSimulator) {
        types << "icd/table-drag/simulator";
    }
    if (d_treeModes & CoreTreeWidget::TreeModeAnalyse) {
        types << "icd/table-drag/analyse";
    }
    if (d_treeModes & CoreTreeWidget::TreeModeTableSel) {
        types << "icd/table-drag/tablesel";
    }
    return types;
}

QMimeData *CoreTreeWidgetPrivate::mimeData(const QList<QStandardItem *> &items) const
{
    //
    if (items.isEmpty()) {
        return 0;
    }

    QStandardItem *item = items.first();
    if (!item) {
        return 0;
    }

    QMimeData *mData = new QMimeData();
    if (d_treeModes & CoreTreeWidget::TreeModeMonitor) {
        mData->setData("icd/table-drag/monitor", QByteArray("1"));
    }
    if (d_treeModes & CoreTreeWidget::TreeModeSimulator) {
        mData->setData("icd/table-drag/simulator", QByteArray("1"));
    }
    if (d_treeModes & CoreTreeWidget::TreeModeAnalyse) {
        mData->setData("icd/table-drag/analyse", QByteArray("1"));
    }
    if (d_treeModes & CoreTreeWidget::TreeModeTableSel) {
        if (item->type() != Icd::TreeItemTypeTable) {
            return 0;
        }
        mData->setData("icd/table-drag/tablesel", QByteArray("1"));
    }
    mData->setProperty("domain", item->data(Icd::TreeItemDomainRole));
    mData->setProperty("item", QVariant::fromValue((void *)item));
    //
    switch (item->type()) {
    case Icd::TreeItemTypeTable:
    {
        mData->setProperty("itemTable", QVariant::fromValue((void *)item));
        break;
    }
    case Icd::TreeItemTypeItemTable:
    case Icd::TreeItemTypeDataItem:
    case Icd::TreeItemTypeItemBitMap:
    {
        QStandardItem *itemTable = findItemTable(item);
        if (itemTable == 0) {
            delete mData;
            return 0;
        }

        mData->setProperty("itemTable", QVariant::fromValue((void *)itemTable));
        break;
    }
    default:
        delete mData;
        return 0;
    }

    return mData;
}

void CoreTreeWidgetPrivate::itemRootRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    //
    QMenu menu(this);
    switch (d_treeModes) {
    case CoreTreeWidget::TreeModeTableSel:
    {
        if (isItemSelected(item)) {
            menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                           QStringLiteral("全部收起"), &menu, [=](){
                expandItem(item, false, -1);
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        }
        break;
    }
    default:
    {
        if (item->hasChildren()) {
            menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                           QStringLiteral("更新分机项"), &menu, [=](){
                //
                unbindChannelItem(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, Q_NULLPTR);
                //
                if (this->loadVehicle(item, deep)) {
                    expandItem(item, true, 2);
                }
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                           QStringLiteral("卸载分机项"), &menu, [=](){
                //
                unbindChannelItem(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, Q_NULLPTR);
            });
            if (isItemSelected(item)) {
                menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                               QStringLiteral("全部收起"), &menu, [=](){
                    expandItem(item, false, -1);
                });
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            } else {
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            }
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                           QStringLiteral("加载分机项"), &menu, [=](){
                //
                if (this->loadVehicle(item, deep)) {
                    expandItem(item, true, 2);
                }
            });
        }

        //
        menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                       QStringLiteral("导出全部数据"), &menu, [=](){
            exportData(item, true);
        });
        menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                       QStringLiteral("导出已加载数据"), &menu, [=](){
            exportData(item, false);
        });
        break;
    }
    }


    //
    menu.exec(QCursor::pos());
}

void CoreTreeWidgetPrivate::itemVehicleRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    //
    QMenu menu(this);
    switch (d_treeModes) {
    case CoreTreeWidget::TreeModeTableSel:
    {
        if (isItemExpanded(item)) {
            menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                           QStringLiteral("全部收起"), &menu, [=](){
                expandItem(item, false, -1);
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        }
        break;
    }
    default:
    {
        if (item->hasChildren()) {
            menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                           QStringLiteral("更新系统项"), &menu, [=](){
                //
                unbindChannelItem(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, Q_NULLPTR);
                //
                if (this->loadSystem(item, deep)) {
                    expandItem(item, true, 2);
                }
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                           QStringLiteral("卸载系统项"), &menu, [=](){
                //
                unbindChannelItem(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, Q_NULLPTR);
            });
            if (isItemExpanded(item)) {
                menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                               QStringLiteral("全部收起"), &menu, [=](){
                    expandItem(item, false, -1);
                });
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            } else {
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            }
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                           QStringLiteral("加载系统项"), &menu, [=](){
                //
                if (this->loadSystem(item, deep)) {
                    //
                    expandItem(item, true, 2);
                }
            });
        }

        //
        menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                       QStringLiteral("导出全部数据"), &menu, [=](){
            exportData(item, true);
        });
        menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                       QStringLiteral("导出已加载数据"), &menu, [=](){
            exportData(item, false);
        });
        break;
    }
    }

    //
    menu.exec(QCursor::pos());
}

void CoreTreeWidgetPrivate::itemSystemRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    //
    QMenu menu(this);
    switch (d_treeModes) {
    case CoreTreeWidget::TreeModeTableSel:
    {
        if (isItemExpanded(item)) {
            menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                           QStringLiteral("全部收起"), &menu, [=](){
                expandItem(item, false, -1);
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        }
        break;
    }
    default:
    {
        if (item->hasChildren()) {
            menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                           QStringLiteral("更新表项"), &menu, [=](){
                //
                unbindChannelItem(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, Q_NULLPTR);
                //
                if (this->loadTable(item, deep)) {
                    expandItem(item, true, 2);
                }
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                           QStringLiteral("卸载表项"), &menu, [=](){
                //
                unbindChannelItem(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, Q_NULLPTR);
            });
            if (isItemExpanded(item)) {
                menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                               QStringLiteral("全部收起"), &menu, [=](){
                    expandItem(item, false, -1);
                });
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            } else {
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            }
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                           QStringLiteral("加载表项"), &menu, [=](){
                //
                if (this->loadTable(item, deep)) {
                    expandItem(item, true, 2);
                }
            });
        }

        //
        menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                       QStringLiteral("导出全部数据"), &menu, [=](){
            exportData(item, true);
        });
        menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                       QStringLiteral("导出已加载数据"), &menu, [=](){
            exportData(item, false);
        });
        break;
    }
    }

    //
    menu.exec(QCursor::pos());
}

void CoreTreeWidgetPrivate::itemTableRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    //
    QMenu menu(this);

    switch (d_treeModes) {
    case CoreTreeWidget::TreeModeAnalyse:
    {
        if (item->hasChildren()) {
            menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                           QStringLiteral("删除数据项"), &menu, [=](){
                //
                removeTableItem(item);
            });
            //
            const bool bound = hasItemBound(item);
            if (bound) {
                menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                               QStringLiteral("删除所有视图"), &menu, [=](){
                    //
                    clearItemBoundRole(item, true);
                });
            }
            //
            if (isItemExpanded(item)) {
                menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                               QStringLiteral("全部收起"), &menu, [=](){
                    expandItem(item, false, -1);
                });
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            } else {
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            }
        }
        break;
    }
    case CoreTreeWidget::TreeModeTableSel:
    {
        break;
    }
    default:
    {
        if (item->hasChildren()) {
            menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                           QStringLiteral("更新数据项"), &menu, [=](){
                //
                unbindChannel(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, item);
                //
                if (this->loadItem(item, deep)) {
                    expandItem(item, true, 1);
                }
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                           QStringLiteral("卸载数据项"), &menu, [=](){
                //
                unbindChannel(item);
                //
                clearChildren(item);
                //
                emit itemUnloaded(item, item);
            });
            //
            const QVariant channelId = item->data(Icd::TreeChannelIdRole);
            if (channelId.isValid()) {
                menu.addAction(QIcon(":/icdwidget/image/tree/channel.png"),
                               QStringLiteral("更改通道绑定"), &menu, [=](){
                    //
                    changeChannel(item);
                });
                menu.addAction(QIcon(":/icdwidget/image/tree/disconnect.png"),
                               QStringLiteral("解除通道绑定"), &menu, [=](){
                    //
                    unbindChannel(item);
                });
                if (d_treeModes & CoreTreeWidget::TreeModeMonitor) {
                    // get oldWorker
                    const Icd::WorkerPtr worker = queryWorker(item);
                    if (worker) {
                        if (worker->workerRecv()->isRecording()) {
                            menu.addAction(QIcon(":/icdwidget/image/tree/record_stop.png"),
                                           QStringLiteral("停止数据记录"), &menu, [=](){
                                // get oldWorker
                                const Icd::WorkerPtr worker = queryWorker(item);
                                if (worker == 0) {
                                    return;
                                }
                                worker->workerRecv()->stopRecord();
                            });
                        } else {
                            menu.addAction(QIcon(":/icdwidget/image/tree/record_start.png"),
                                           QStringLiteral("开始记录数据"), &menu, [=](){
                                worker->workerRecv()->startRecord();
                            });
                        }
                    }
                }
            } else {
                menu.addAction(QIcon(":/icdwidget/image/tree/connect.png"),
                               QStringLiteral("绑定数据通道"), &menu, [=](){
                    //
                    bindChannel(item);
                });
            }

            //
            const bool bound = hasItemBound(item);
            if (bound) {
                menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                               QStringLiteral("删除所有视图"), &menu, [=](){
                    //
                    clearItemBoundRole(item, true);
                });
            }
            //
            if (isItemExpanded(item)) {
                menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                               QStringLiteral("全部收起"), &menu, [=](){
                    expandItem(item, false, -1);
                });
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            } else {
                menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                               QStringLiteral("全部展开"), &menu, [=](){
                    expandItem(item, true, -1);
                });
            }
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                           QStringLiteral("加载数据项"), &menu, [=](){
                //
                if (this->loadItem(item, deep)) {
                    expandItem(item, true, 1);
                }
            });
        }

        //
        menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                       QStringLiteral("导出全部数据"), &menu, [=](){
            exportData(item, true);
        });
        menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                       QStringLiteral("导出已加载数据"), &menu, [=](){
            exportData(item, false);
        });

        break;
    }}
    //
    menu.exec(QCursor::pos());
}

void CoreTreeWidgetPrivate::itemDataItemRightClicked(QStandardItem *item, int deep)
{
    Q_UNUSED(deep);
    if (!item) {
        return;
    }

    //
    QMenu menu(this);

    // 获取数据项类型
    Icd::ItemType dataType = Icd::ItemInvalid;
    const QVariant dataTypeVar = item->data(Icd::TreeDataTypeRole);
    if (dataTypeVar.isValid()) {
        dataType = (Icd::ItemType)dataTypeVar.toUInt();
    }

    // bound
    bool bound = false;
    const QVariant varBound = item->data(Icd::TreeBoundRole);
    if (varBound.isValid()) {
        bound = varBound.toBool();
    }

    Q_Q(CoreTreeWidget);

    // 根据数据项类型进行相应的处理
    switch (dataType) {
    case Icd::ItemHead:
    case Icd::ItemCounter:
    case Icd::ItemCheck:
    case Icd::ItemFrameCode:
    case Icd::ItemNumeric:
    case Icd::ItemBitMap:
    case Icd::ItemBitValue:
    case Icd::ItemComplex:
    case Icd::ItemFrame:
    {
        switch (d_treeModes) {
        case CoreTreeWidget::TreeModeAnalyse:
        default:
            if (bound) {
                QAction *action = menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                                                 QStringLiteral("删除视图"), [=](){
                    QStandardItem *itemTable = findItemTable(item);
                    emit this->unbindItem(item, itemTable);
                });
                action->setToolTip(QStringLiteral("从视图窗口中删除对应数据项视图"));
            }
            if (d_treeModes == CoreTreeWidget::TreeModeAnalyse) {
                menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                               QStringLiteral("导出数据"), &menu, [=](){
                    QStandardItem *itemTable = findItemTable(item);
                    if (!itemTable) {
                        return;
                    }
                    const QString filePath = itemTable->data(TreeFilePathRole).toString();
                    const bool hasTimeFormat = itemTable->data(TreeHasTimeFormatRole).toBool();
                    const int headerSize = itemTable->data(TreeHeaderSizeRole).toInt();
                    emit q->exportAnalyseData(item, filePath, hasTimeFormat, headerSize);
                });
            }
            break;
        }
        break;
    }
    default:
        break;
    }

    //
    if (item->hasChildren()) {
        if (isItemExpanded(item)) {
            menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                           QStringLiteral("全部收起"), &menu, [=](){
                expandItem(item, false, -1);
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        }

        switch (dataType) {
        case Icd::ItemComplex:
        case Icd::ItemFrame:
            break;
        default:
            break;
        }
    }

    menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                   QStringLiteral("导出全部数据"), &menu, [=](){
        exportData(item, true);
    });
    menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                   QStringLiteral("导出已加载数据"), &menu, [=](){
        exportData(item, false);
    });

    //
    menu.exec(QCursor::pos());
}

void CoreTreeWidgetPrivate::itemItemTableRightClicked(QStandardItem *item, int deep)
{
    Q_UNUSED(deep);
    if (!item) {
        return;
    }

    // bound
    bool bound = false;
    const QVariant varBound = item->data(Icd::TreeBoundRole);
    if (varBound.isValid()) {
        bound = varBound.toBool();
    }

    Q_Q(CoreTreeWidget);

    //
    QMenu menu(this);

    //
    switch (d_treeModes) {
    case CoreTreeWidget::TreeModeAnalyse:
    default:
        if (bound || item->hasChildren()) {
            QAction *action = menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                                             QStringLiteral("删除视图"), [=](){
                QStandardItem *itemTable = findItemTable(item);
                emit this->unbindItem(item, itemTable);
            });
            action->setToolTip(QStringLiteral("从视图窗口中删除对应数据项视图"));
        }
        if (d_treeModes == CoreTreeWidget::TreeModeAnalyse) {
            menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                           QStringLiteral("导出数据"), &menu, [=](){
                QStandardItem *itemTable = findItemTable(item);
                if (!itemTable) {
                    return;
                }
                const QString filePath = itemTable->data(TreeItemPathRole).toString();
                const bool hasTimeFormat = itemTable->data(TreeHasTimeFormatRole).toBool();
                const int headerSize = itemTable->data(TreeHeaderSizeRole).toInt();
                emit q->exportAnalyseData(item, filePath, hasTimeFormat, headerSize);
            });
        }
        break;
    }

    //
    if (item->hasChildren()) {
        if (isItemExpanded(item)) {
            menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                           QStringLiteral("全部收起"), &menu, [=](){
                expandItem(item, false, -1);
            });
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        } else {
            menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                           QStringLiteral("全部展开"), &menu, [=](){
                expandItem(item, true, -1);
            });
        }
    }

    //
    menu.exec(QCursor::pos());
}

void CoreTreeWidgetPrivate::itemItemBitMapRightClicked(QStandardItem *item, int deep)
{
    Q_UNUSED(deep);
    if (!item) {
        return;
    }

    // bound
    bool bound = false;
    const QVariant varBound = item->data(Icd::TreeBoundRole);
    if (varBound.isValid()) {
        bound = varBound.toBool();
    }

    Q_Q(CoreTreeWidget);

    //
    QMenu menu(this);

    //
    switch (d_treeModes) {
    case CoreTreeWidget::TreeModeAnalyse:
    default:
        if (d_treeModes == CoreTreeWidget::TreeModeAnalyse) {
            menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                           QStringLiteral("导出数据"), &menu, [=](){
                QStandardItem *itemTable = findItemTable(item);
                if (!itemTable) {
                    return;
                }
                const QString filePath = itemTable->data(TreeFilePathRole).toString();
                const bool hasTimeFormat = itemTable->data(TreeHasTimeFormatRole).toBool();
                const int headerSize = itemTable->data(TreeHeaderSizeRole).toInt();
                emit q->exportAnalyseData(item, filePath, hasTimeFormat, headerSize);
            });
        }
        break;
    }

    //
    menu.exec(QCursor::pos());
}

bool CoreTreeWidgetPrivate::changeChannel(QStandardItem *itemTable)
{
    if (!itemTable) {
        return false;   // invalid parameter
    }

    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }

    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>
            (this->itemWidget(itemTable));
    // get oldWorker
    const Icd::WorkerPtr oldWorker = queryWorker(itemTable);
    if (oldWorker == 0) {
        return false; // invlaid worker
    }

    //
    Q_Q(CoreTreeWidget);
    BindChannelWidget *bindChannelWidget = new BindChannelWidget(q);
    if (bindChannelWidget->exec() != QDialog::Accepted) {
        return false; // cancel
    }

    //
    bindChannelWidget->deleteLater();
    const Icd::WorkerPtr selectedWorker = bindChannelWidget->selectedWorker();
    if (selectedWorker == 0) {
        return false; // invlaid worker
    }
    bindChannelWidget = 0;

    // 先恢复状态
    itemTable->setData(QVariant::Invalid, Icd::TreeChannelIdRole);
    if (itemWidget) {
        itemWidget->setText(itemWidget->text()
                            .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
        itemWidget->setWorker(Q_NULLPTR);
    } else {
        itemTable->setText(itemTable->text()
                           .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
    }

    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QStringList sections = domain.split('/');
    if (sections.count() != 3) {
        return false;   // invalid domain
    }

    // find table object
    Icd::TablePtr table = Icd::TablePtr(0);
    auto funcParseTable = [=,&table](){
        if (!d_parser->parseTable(sections.at(0).toStdString(), sections.at(1).toStdString(),
                                  sections.at(2).toStdString(), table, Icd::ObjectItem)) {
            return false; // parse failure
        }
        return true;
    };

    // 设置表项
    if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
        table = oldWorker->workerSend()->table();
        if (!table) {
            if (!funcParseTable()) {
                return false;
            }
        }
        oldWorker->workerSend()->setTable(Icd::TablePtr(0));
        selectedWorker->workerSend()->setTable(table);
    } else if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
        table = oldWorker->workerRecv()->table();
        if (!table) {
            if (!funcParseTable()) {
                return false;
            }
        }
        oldWorker->workerRecv()->setTable(Icd::TablePtr(0));
        selectedWorker->workerRecv()->setTable(table);
    } else if (d_bindTableTypes == CoreTreeWidget::BindAllTable) {
        table = oldWorker->workerSend()->table();
        if (!table) {
            table = oldWorker->workerRecv()->table();
            if (!table) {
                if (!funcParseTable()) {
                    return false;
                }
            }
        }
        oldWorker->workerSend()->setTable(Icd::TablePtr(0));
        oldWorker->workerRecv()->setTable(Icd::TablePtr(0));
        selectedWorker->workerSend()->setTable(table);
        selectedWorker->workerRecv()->setTable(Icd::TablePtr(reinterpret_cast<Table *>(table->clone())));
    }

    //
    const QString channelId = QString::fromStdString(selectedWorker->channel()->identity());

    //
    itemTable->setData(channelId, Icd::TreeChannelIdRole);
    if (itemWidget) {
        itemWidget->setText(itemWidget->text()
                            + QStringLiteral(" [CH: <i><font color=blue size=3>")
                            + QString::fromStdString(selectedWorker->channel()->name()) + "</font></i> ]");
        itemWidget->setWorker(selectedWorker);
    } else {
        itemTable->setText(itemTable->text()
                           + QStringLiteral(" [CH: <i><font color=blue size=3>")
                           + QString::fromStdString(selectedWorker->channel()->name()) + "</font></i> ]");
    }

    //
    ItemWorkerGroup *workerGroup = findWorkerGroup(itemTable);
    if (workerGroup) {
        workerGroup->setWorker(selectedWorker);
    }

    emit channelChanged(itemTable, channelId);

    return true;
}

bool CoreTreeWidgetPrivate::bindChannel(QStandardItem *itemTable)
{
    if (!itemTable) {
        return false;   // invalid parameter
    }
    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }
    //
    BindChannelWidget *bindChannelWidget = new BindChannelWidget(this);
    if (bindChannelWidget->exec() != QDialog::Accepted) {
        return false; // cancel
    }
    //
    Icd::WorkerPtr selectedWorker = bindChannelWidget->selectedWorker();
    if (!selectedWorker) {
        return false; // invlaid worker
    }

    bindChannelWidget->deleteLater();
    bindChannelWidget = 0;

    return bindChannel(itemTable, selectedWorker);
}

bool CoreTreeWidgetPrivate::bindChannel(QStandardItem *itemTable, const WorkerPtr &worker)
{
    if (!itemTable || !worker) {
        return false;   // invalid parameter
    }
    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }
    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QStringList sections = domain.split('/');
    if (sections.count() != 3) {
        return bindChannel(itemTable, worker, Q_NULLPTR);
    }
    // find table object
    Icd::TablePtr table;
    if (!d_parser->parseTable(sections.at(0).toStdString(), sections.at(1).toStdString(),
                              sections.at(2).toStdString(), table, Icd::ObjectItem)) {
        return bindChannel(itemTable, worker, Q_NULLPTR);
    }

    return bindChannel(itemTable, worker, table);
}

bool CoreTreeWidgetPrivate::bindChannel(QStandardItem *itemTable, const WorkerPtr &worker,
                                        const TablePtr &table)
{
    if (!itemTable || !worker) {
        return false;   // invalid parameter
    }
    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }
    //
    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>
            (this->itemWidget(itemTable));
    // 先恢复状态
    itemTable->setData(QVariant::Invalid, Icd::TreeChannelIdRole);
    if (itemWidget) {
        itemWidget->setText(itemWidget->text()
                            .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
        itemWidget->setWorker(Q_NULLPTR);
    } else {
        itemTable->setText(itemTable->text()
                           .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
    }
    // find table object
    if (!table) {
        return false;
    }
    // 设置表项
    if (d_bindTableTypes == CoreTreeWidget::BindOnlySend) {
        worker->workerSend()->setTable(table);
        if (d_treeModes & CoreTreeWidget::TreeModeSimulator) {
            worker->workerSend()->setFrameLoop(true);
        }
    } else if (d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
        worker->workerRecv()->setTable(table);
    } else if (d_bindTableTypes == CoreTreeWidget::BindAllTable) {
        worker->workerSend()->setTable(table);
        worker->workerRecv()->setTable(Icd::TablePtr(reinterpret_cast<Table *>(table->clone())));
        if (d_treeModes & CoreTreeWidget::TreeModeSimulator) {
            worker->workerSend()->setFrameLoop(true);
        }
    }

    //
    const QString channelId = QString::fromStdString(worker->channel()->identity());
    //
    itemTable->setData(channelId, Icd::TreeChannelIdRole);
    if (itemWidget) {
        itemWidget->setText(itemWidget->text()
                            + QStringLiteral(" [CH: <i><font color=blue size=3>")
                            + QString::fromStdString(worker->channel()->name()) + "</font></i> ]");
        itemWidget->setWorker(worker);
    } else {
        itemTable->setText(itemTable->text()
                           + QStringLiteral(" [CH: <i><font color=blue size=3>")
                           + QString::fromStdString(worker->channel()->name()) + "</font></i> ]");
    }
    //
    ItemWorkerGroup *workerGroup = new ItemWorkerGroup(
                itemTable, worker, d_showAttris, this);
    workerGroup->setBindTableType(d_bindTableTypes);
    d_workerGroups.insert(itemTable, workerGroup);
    //
    if (d_bindTableTypes & CoreTreeWidget::BindOnlySend) {
        if (worker->workerSend()->isRunning()) {
            workerGroup->start(d_intervalUpdate);
        }
    }
    if (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv) {
        if (d_showAttris & (CoreTreeWidget::ShowData | CoreTreeWidget::ShowValue)) {
            workerGroup->start(d_intervalUpdate);
        }
    }

    workerGroup->updateItemData();
    //
    emit channelBound(itemTable, channelId);

    return true;
}

bool CoreTreeWidgetPrivate::unbindChannel(QStandardItem *itemTable)
{
    if (!itemTable) {
        return false; // invalid parameter
    }

    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>
            (this->itemWidget(itemTable));
    //
    const QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        return true; // invalid channel identity
    }
    //
    const QString channelId = varChannelId.toString();
    //
    const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return false; // invlaid worker
    }
    //
    itemTable->setData(QVariant::Invalid, Icd::TreeChannelIdRole);
    //
    if (itemWidget) {
        itemWidget->setText(itemWidget->text()
                            .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
        itemWidget->setWorker(Q_NULLPTR);
    } else {
        itemTable->setText(itemTable->text()
                           .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
    }
    //
    worker->workerRecv()->stopRecord();
    //
    updateItemData(itemTable, false);
    //
    removeWorkerGroup(itemTable);
    //
    clearItemBoundRole(itemTable, false);
    //
    emit channelUnbound(itemTable, channelId);

    return true;
}

bool CoreTreeWidgetPrivate::unbindChannelItem(QStandardItem *item)
{
    if (!item) {
        return false;
    }

    //
    const int itemType = item->type();
    if (itemType > TreeItemTypeTable) {
        return false;
    }

    //
    if (itemType == TreeItemTypeTable) {
        return unbindChannel(item);
    }

    //
    int count = item->rowCount();
    for (int i = 0; i < count; ++i) {
        if (!unbindChannelItem(item->child(i))) {
            continue;
        }
    }

    return true;
}

bool CoreTreeWidgetPrivate::unbindChannel(QStandardItem *itemTable, const Icd::WorkerPtr &worker)
{
    if (!itemTable || !worker) {
        return false; // invalid parameter
    }

    //
    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>(this->itemWidget(itemTable));
    if (itemWidget) {
        itemWidget->setWorker(Icd::WorkerPtr());
    }

    //
    itemTable->setText(itemTable->text()
                       .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
    itemTable->setData(QVariant::Invalid, Icd::TreeChannelIdRole);

    //
    updateItemData(itemTable, false);

    //
    clearItemBoundRole(itemTable, false);

    //
    emit channelUnbound(itemTable, QString::fromStdString(worker->channel()->identity()));

    return true;
}

bool CoreTreeWidgetPrivate::isBoundChannel(QStandardItem *itemTable) const
{
    if (!itemTable) {
        return false;
    }

    const QVariant varBound = itemTable->data(Icd::TreeBoundRole);
    if (varBound.isValid()) {
        return varBound.toBool();
    } else {
        return false;
    }
}

bool CoreTreeWidgetPrivate::loadVehicle(QStandardItem *itemRoot, int deep)
{
    if (!itemRoot) {
        return false; // invalid parameter
    }

    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }

    JAutoCursor cursor(Qt::BusyCursor);

    // get vehicles informations
    Icd::VehiclePtrArray vehicles;
    if (!d_parser || !d_parser->parseVehicle(vehicles, deep)) {
        //Q_ASSERT(false);
        return false;
    }

    //
    for (Icd::VehiclePtrArray::const_iterator citer = vehicles.cbegin();
         citer != vehicles.cend(); ++citer) {
        const Icd::VehiclePtr &vehicle = *citer;
        // name
        const QString name = (QString::fromStdString(vehicle->name().empty() ? "<?>"
                                                                             : vehicle->name()));
        //
        QString text = name;
        // type
        if (d_showAttris & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>" + QString("[VEHICLE]") + "</font>");
        }
        // create item
        JTreeViewItem *itemVehicle = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-vehicle.png"),
                                                       text, Icd::TreeItemTypeVehicle);
        //Json::Value json;
        //json.toStyledString();
        // add item
        itemRoot->appendRow(itemVehicle);
        // drag disabled
        itemVehicle->setFlags(itemVehicle->flags() & (~Qt::ItemIsDragEnabled));
        // set tooltip
        itemVehicle->setToolTip(QString::fromStdString(vehicle->desc()));
        // bind id property
        itemVehicle->setData(QString::fromStdString(vehicle->id()), Icd::TreeItemDomainRole);
        // bind path property
        itemVehicle->setData(name, Icd::TreeItemPathRole);
        // load systems
        if (deep >= Icd::ObjectSystem && !loadSystem(itemVehicle, vehicle->allSystem(), deep)) {
            continue;   // load failure
        }
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadSystem(QStandardItem *itemVehicle, int deep)
{
    if (!itemVehicle) {
        return false;
    }

    //
    clearChildren(itemVehicle);

    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }

    JAutoCursor cursor(Qt::BusyCursor);

    //
    const QString domain = itemVehicle->data(Icd::TreeItemDomainRole).toString();

    // get systems informations
    Icd::SystemPtrArray systems;
    if (!d_parser->parseSystem(domain.toStdString(), systems, deep)) {
        Q_ASSERT(false);
        return false;
    }

    //
    if (!loadSystem(itemVehicle, systems, deep)) {
        return false;
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadSystem(QStandardItem *itemVehicle,
                                       const SystemPtrArray &systems, int deep)
{
    if (!itemVehicle || systems.empty()) {
        return false;
    }

    //
    const QString domain = itemVehicle->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemVehicle->data(Icd::TreeItemPathRole).toString();

    //
    for (Icd::SystemPtrArray::const_iterator citer = systems.cbegin();
         citer != systems.cend(); ++citer) {
        const Icd::SystemPtr &system = *citer;
        // name
        const QString name = QString::fromStdString(system->name().empty() ? "<?>"
                                                                           : system->name());
        //
        QString text = name;
        // type
        if (d_showAttris & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>" + QString("[SYSTEM]") + "</font>");
        }
        // create item
        JTreeViewItem *itemSystem = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-system.png"),
                                                      text, Icd::TreeItemTypeSystem);
        // add item
        itemVehicle->appendRow(itemSystem);
        // drag disabled
        itemSystem->setFlags(itemSystem->flags() & (~Qt::ItemIsDragEnabled));
        // set tooltip
        itemSystem->setToolTip(QString::fromStdString(system->desc()));
        // bind id property
        itemSystem->setData(domain + "/" + QString::fromStdString(system->id()),
                            Icd::TreeItemDomainRole);
        // bind path property
        itemSystem->setData(name + "@" + path, Icd::TreeItemPathRole);
        // load systems
        if (deep >= Icd::ObjectTable && !loadTable(itemSystem, system->allTable(), deep)) {
            continue;   // load failure
        }
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadTable(QStandardItem *itemSystem, int deep)
{
    //
    if (!itemSystem) {
        return false;
    }

    //
    clearChildren(itemSystem);

    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }

    JAutoCursor cursor(Qt::BusyCursor);

    //
    const QString domain = itemSystem->data(Icd::TreeItemDomainRole).toString();
    const QString vehicleId = domain.section('/', 0, 0);
    const QString systemId = domain.section('/', 1);

    // get systems informations
    Icd::TablePtrArray tables;
    if (!d_parser->parseTable(vehicleId.toStdString(),
                              systemId.toStdString(), tables, deep)) {
        Q_ASSERT(false);
        return false;
    }

    //
    if (!loadTable(itemSystem, tables, deep)) {
        return false;
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadTable(QStandardItem *itemSystem,
                                      const TablePtrArray &tables, int deep)
{
    if (!itemSystem || tables.empty()) {
        return false;
    }

    //
    const QString domain = itemSystem->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemSystem->data(Icd::TreeItemPathRole).toString();

    //
    for (Icd::TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        const Icd::TablePtr &table = *citer;
        // name
        const QString name = QString::fromStdString(table->name().empty() ? "<?>"
                                                                          : table->name());
        //
        QString text = name;
        // type
        if (d_showAttris & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>" + QString("[TABLE]") + "</font>");
        }
        // create item
        JTreeViewItem *itemTable = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-table.png"),
                                                     QString(), Icd::TreeItemTypeTable);
        // add item
        itemSystem->appendRow(itemTable);
        //
        if (d_bindTableTypes == CoreTreeWidget::BindOnlySend
                || d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
            // itemWidget
            TableItemWidget *itemWidget = new TableItemWidget(text, d_bindTableTypes, this);
            setItemWidget(itemTable, itemWidget);
            connect(itemWidget, &TableItemWidget::clicked, this, [=](){
                emit itemClicked(itemTable);
            });
        } else {
            // text
            itemTable->setText(text);
        }
        // set tooltip
        itemTable->setToolTip(QString::fromStdString(table->desc()));
        // bind id property
        itemTable->setData(domain + '/' + QString::fromStdString(table->id()),
                           Icd::TreeItemDomainRole);
        // bind path property
        itemTable->setData(name + '@' + path, Icd::TreeItemPathRole);
        // load table
        if (deep >= Icd::ObjectItem && !loadItem(itemTable, table->allItem(), deep)) {
            continue;   // load failure
        }
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadItem(QStandardItem *itemTable, int deep)
{
    //
    if (!itemTable) {
        return false;
    }

    //
    clearChildren(itemTable);

    //
    if (!d_parser) {
        Q_ASSERT(false);
        return false;   //
    }

    JAutoCursor cursor(Qt::BusyCursor);

    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QString vehicleId = domain.section('/', 0, 0);
    const QString systemId = domain.section('/', 1, 1);
    const QString tableId = domain.section('/', 2);

    // get systems informations
    Icd::ItemPtrArray dataItems;
    if (!d_parser->parseItem(vehicleId.toStdString(), systemId.toStdString(),
                             tableId.toStdString(), dataItems, Icd::ObjectItem)) {
        Q_ASSERT(false);
        return false;
    }

    //
    if (!loadItem(itemTable, dataItems, deep)) {
        return false;
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadItem(QStandardItem *itemTable,
                                     const Icd::ItemPtrArray &dataItems, int deep)
{
    Q_UNUSED(deep);
    if (!itemTable) {
        return false;
    }

    if (dataItems.empty()) {
        return true;
    }

    //
    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend(); ++citer) {
        const Icd::ItemPtr &dataItem = *citer;
        //
        if (!loadItem(itemTable, dataItem)) {
            continue;
        }
    }

    return true;
}

QStandardItem *CoreTreeWidgetPrivate::loadTable(QStandardItem *itemDataItem,
                                                const Icd::TablePtr &table)
{
    //
    if (!itemDataItem || !table) {
        return Q_NULLPTR;
    }

    //
    const QString domain = itemDataItem->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemDataItem->data(Icd::TreeItemPathRole).toString();

    //
    QString text;
    // offset
    if (d_showAttris & CoreTreeWidget::ShowOffset) {
        text.append(ItemWorkerGroup::generateItemOffset(table));
    }
    // name
    const QString name = QString::fromStdString(table->name().empty() ? "<?>" : table->name());
    text.append(name);
    // type
    if (d_showAttris & CoreTreeWidget::ShowType) {
        text.append(" <font color=green size=2>" + QString("[TABLE]") + "</font>");
    }
    int itemType = 0;
    if (itemDataItem == invisibleRootItem()) {
        itemType = Icd::TreeItemTypeTable;
    } else {
        itemType = Icd::TreeItemTypeItemTable;
    }
    // create item
    JTreeViewItem *itemTable = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-table.png"),
                                                 QString(), itemType);
    // add item
    itemDataItem->appendRow(itemTable);
    //
    if (d_bindTableTypes == CoreTreeWidget::BindOnlySend
            || d_bindTableTypes == CoreTreeWidget::BindOnlyRecv) {
        // itemWidget
        TableItemWidget *itemWidget = new TableItemWidget(text, d_bindTableTypes, this);
        setItemWidget(itemTable, itemWidget);
        connect(itemWidget, &TableItemWidget::clicked, this, [=](){
            emit itemClicked(itemTable);
        });
    } else {
        // text
        itemTable->setText(text);
    }
    // set tooltip
    itemTable->setToolTip(QString::fromStdString(table->desc()));
    // bind id property
    itemTable->setData(domain + '/' + QString::fromStdString(table->id()),
                       Icd::TreeItemDomainRole);
    // bind path property
    itemTable->setData(name + '@' + path, Icd::TreeItemPathRole);
    // load items
    const Icd::ItemPtrArray &dataItems = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend(); ++citer) {
        const Icd::ItemPtr &dataItem = *citer;
        //
        if (!loadItem(itemTable, dataItem)) {
            continue;
        }
    }

    return itemTable;
}

bool CoreTreeWidgetPrivate::loadFrameCodeItem(QStandardItem *itemTable,
                                              const Icd::FrameCodeItemPtr &frameCodeItem)
{
    //
    if (!itemTable || !frameCodeItem) {
        return false;
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadComplexItem(QStandardItem *itemDataItem,
                                            const Icd::ComplexItemPtr &complexItem)
{
    if (!itemDataItem || !complexItem || !complexItem->table()) {
        return false;
    }

    //
    if (!loadItem(itemDataItem, complexItem->table()->allItem(), Icd::ObjectItem)) {
        return false;   // load failure
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadFrameItem(QStandardItem *itemDataItem,
                                          const Icd::FrameItemPtr &fameItem)
{
    if (!itemDataItem || !fameItem) {
        return false;
    }

    //
    const Icd::TablePtrMap &allTable = fameItem->allTable();
    for (Icd::TablePtrMap::const_iterator citer = allTable.cbegin();
         citer != allTable.cend(); ++citer) {
        const Icd::TablePtr &table = citer->second;
        //
        if (!loadTable(itemDataItem, table)) {
            continue;
        }
        //
    }

    return true;
}

bool CoreTreeWidgetPrivate::loadBitItem(QStandardItem *itemDataItem, const Icd::BitItemPtr &bitItem)
{
    if (!itemDataItem || !bitItem) {
        return false;
    }

    //
    const QString domain = itemDataItem->data(Icd::TreeItemDomainRole).toString();

    int bitEnd = bitItem->bitStart() + bitItem->bitCount();
    for (int i = bitItem->bitStart(); i < bitEnd; ++i) {
        // name
        std::string name = bitItem->nameAt(i);
        if (name.empty()) {
            continue;
        }
        //
        QString text;
        // offset
        if (d_showAttris & CoreTreeWidget::ShowOffset) {
            text.append(QString("<font color=green size=2>[%1]</font> ")
                        .arg(i, 2, 10, QChar('0')));
        }
        // name
        text.append(QString::fromStdString(name));
        // create item
        JTreeViewItem *itemBitItem = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-bit.png"),
                                                       text, Icd::TreeItemTypeItemBitMap);
        // add item
        itemDataItem->appendRow(itemBitItem);
        // domain
        itemBitItem->setData(domain, Icd::TreeItemDomainRole);
        // bitOffset
        itemBitItem->setData(i, Icd::TreeBitOffsetRole);
        // icon
        //itemBitItem->setIcon(QIcon(":/icdwidget/image/tree/icon-item.png"));
        // set tooltip
        itemBitItem->setToolTip(QString::fromStdString(name));
    }

    return true;
}

void CoreTreeWidgetPrivate::removeTableItem(QStandardItem *item)
{
    if (!item || item->type() != TreeItemTypeTable) {
        return;
    }

    switch (d_treeModes) {
    case CoreTreeWidget::TreeModeAnalyse:
        //
        emit unbindItem(item, item);
        //
        emit itemUnloaded(item, item);
        //
        invisibleRootItem()->removeRow(item->row());
        //
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

bool CoreTreeWidgetPrivate::loadItem(QStandardItem *itemTable,
                                     const ItemPtr &item)
{
    if (!itemTable || !item) {
        return false;
    }

    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemTable->data(Icd::TreeItemPathRole).toString();

    // name
    const QString name = [=](){
        const std::string name = item->name();
        if (name.empty()) {
            return QString::fromStdString("--");
        } else {
            return QString::fromStdString(name);
        }
    }();

    QString text = name;
    // offset
    if (d_showAttris & CoreTreeWidget::ShowOffset) {
        text.prepend(ItemWorkerGroup::generateItemOffset(item));
    }
    // type
    if (d_showAttris & CoreTreeWidget::ShowType) {
        text.append(" <font color=green size=2>[" + QString::fromStdString(
                        item->typeString()).toUpper() + "]</font>");
    }
    // create item
    JTreeViewItem *itemDataItem = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-item.png"),
                                                    text, Icd::TreeItemTypeDataItem);
    // add item
    itemTable->appendRow(itemDataItem);
    // set tooltip
    itemDataItem->setToolTip(QString::fromStdString(item->desc()));
    // bind id property
    itemDataItem->setData(domain + '/' + QString::fromStdString(item->id()),
                          Icd::TreeItemDomainRole);
    //
    itemDataItem->setData(uint(item->type()), Icd::TreeDataTypeRole);
    //
    // bind path property
    itemDataItem->setData(name + '@' + path, Icd::TreeItemPathRole);
    // load ???
    switch (item->type()) {
    case Icd::ItemBitMap:
        if (!loadBitItem(itemDataItem, JHandlePtrCast<Icd::BitItem, Icd::Item>(item))) {
            // load failure
        }
        break;
    case Icd::ItemFrameCode:
        if (!loadFrameCodeItem(itemTable, JHandlePtrCast<Icd::FrameCodeItem, Icd::Item>(item))) {
            // load failure
        }
        break;
    case Icd::ItemComplex:
        // icon
        itemDataItem->setIcon(QIcon(":/icdwidget/image/tree/icon-complex.png"));
        if (!loadComplexItem(itemDataItem, JHandlePtrCast<Icd::ComplexItem, Icd::Item>(item))) {
            // load failure
        }
        break;
    case Icd::ItemFrame:
        if (!loadFrameItem(itemDataItem, JHandlePtrCast<Icd::FrameItem, Icd::Item>(item))) {
            // load failure
        }
        break;
    default:
        break;
    }

    return true;
}

void CoreTreeWidgetPrivate::clearChildren(QStandardItem *item)
{
    if (!item) {
        return;
    }

    while (item->hasChildren()) {
        item->removeRow(0);
    }
}

void CoreTreeWidgetPrivate::expandItem(QStandardItem *item, bool expanded, int deep)
{
    if (!item) {
        return;
    }

    setItemExpanded(item, expanded);

    if (deep > 0) {
        if (--deep <= 0) {
            return;
        }
    }

    // children
    int childCount = item->rowCount();
    for (int i = 0; i < childCount; ++i) {
        expandItem(item->child(i, 0), expanded, deep);
    }
}

Icd::WorkerPtr CoreTreeWidgetPrivate::queryWorker(const QStandardItem *itemTable) const
{
    if (!itemTable) {
        return Icd::WorkerPtr(0);
    }

    //
    if (itemTable->type() != Icd::TreeItemTypeTable) {
        return Icd::WorkerPtr(0);
    }

    //
    const QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        return Icd::WorkerPtr(0);
    }

    //
    const QString channelId = varChannelId.toString();

    //
    const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return Icd::WorkerPtr(0);
    }

    return worker;
}

void CoreTreeWidgetPrivate::updateItemData(QStandardItem *item)
{
    if (!item) {
        return;
    }

    //
    const int itemType = item->type();
    switch (itemType) {
    case Icd::TreeItemTypeVehicle:
    {
        QString text = item->text().remove(QRegExp("<font[^>]*>[\\s\\S]*<\\/font>")).trimmed();
        if (d_showAttris & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>" + QString("[VEHICLE]") + "</font>");
        }
        item->setText(text);
        break;
    }
    case Icd::TreeItemTypeSystem:
    {
        QString text = item->text().remove(QRegExp("<font[^>]*>[\\s\\S]*<\\/font>")).trimmed();
        if (d_showAttris & CoreTreeWidget::ShowType) {
            text.append(" <font color=green size=2>" + QString("[SYSTEM]") + "</font>");
        }
        item->setText(text);
        break;
    }
    case Icd::TreeItemTypeTable:
    {
        ItemWorkerGroup *workerGroup = findWorkerGroup(item);
        if (workerGroup) {
            workerGroup->updateItemData(d_showAttris, d_dataFormat);
        } else {
            if (!item->hasChildren()) {
                if (!isBoundChannel(item)) {
                    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>
                            (this->itemWidget(item));
                    QString text = itemWidget->text().remove(QRegExp("<font[^>]*>[\\s\\S]*<\\/font>")).trimmed();
                    if (d_showAttris & CoreTreeWidget::ShowType) {
                        text.append(" <font color=green size=2>" + QString("[TABLE]") + "</font>");
                    }
                    itemWidget->setText(text);
                }
                return;
            }
            //
            const QString domain = item->data(Icd::TreeItemDomainRole).toString();
            const QStringList sections = domain.split('/');
            if (sections.count() != 3) {
                return;
            }
            // find table object
            Icd::TablePtr table;
            if (!d_parser->parseTable(sections.at(0).toStdString(), sections.at(1).toStdString(),
                                      sections.at(2).toStdString(), table, Icd::ObjectItem)) {
                return;
            }
            //
            updateItemData(item, table);
        }
        break;
    }
    default:
        break;
    }

    //
    int childCount = item->rowCount();
    for (int i = 0; i < childCount; ++i) {
        updateItemData(item->child(i, 0));
    }
}

void CoreTreeWidgetPrivate::updateBitItemData(QStandardItem *itemDataItem,
                                              const BitItemPtr &bitItem)
{
    if (!itemDataItem || !bitItem) {
        return;
    }

    int childCount = itemDataItem->rowCount();
    for (int i = 0; i < childCount; ++i) {
        QStandardItem *childItem = itemDataItem->child(i);
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
        //
        childItem->setText(text);
    }
}

void CoreTreeWidgetPrivate::updateItemData(QStandardItem *itemDataItem, const Icd::ItemPtr &dataItem)
{
    if (!itemDataItem || !dataItem) {
        return;
    }

    //
    QString text;
    // offset
    if (d_showAttris & CoreTreeWidget::ShowOffset) {
        text.append(ItemWorkerGroup::generateItemOffset(dataItem));
    }
    // name
    text.append(QString::fromStdString(dataItem->name().empty() ? "<?>" : dataItem->name()));
    // type
    if (d_showAttris & CoreTreeWidget::ShowType) {
        text.append(" <font color=green size=2>[" + QString::fromStdString(
                        dataItem->typeString()).toUpper() + "]</font>");
    }

    itemDataItem->setText(text);

    //
    switch (dataItem->type()) {
    case Icd::ItemBitMap:
    {
        //
        const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem, Icd::Item>(dataItem);
        if (!itemBit) {
            break;
        }

        //
        updateBitItemData(itemDataItem, itemBit);

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
        updateItemData(itemDataItem, itemComplex);

        break;
    }
    case Icd::ItemFrame:
    {
        //
        const Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem, Icd::Item>(dataItem);
        if (!frame) {
            break;
        }

        //
        updateItemData(itemDataItem, frame);

        break;
    }
    default:
        break;
    }
}

void CoreTreeWidgetPrivate::updateItemData(QStandardItem *itemTable, const Icd::ItemPtrArray &dataItems)
{
    if (!itemTable || dataItems.empty()) {
        return;
    }

    int i = 0;
    int childCount = itemTable->rowCount();
    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend() && i < childCount; ++citer, ++i) {
        updateItemData(itemTable->child(i), *citer);
    }
}

void CoreTreeWidgetPrivate::updateItemData(QStandardItem *itemDataItem, const ComplexItemPtr &complex)
{
    if (!itemDataItem || !complex) {
        return;
    }
    //
    QString text;
    // offset
    if (d_showAttris & CoreTreeWidget::ShowOffset) {
        text.append(ItemWorkerGroup::generateItemOffset(complex));
    }
    // name
    text.append(QString::fromStdString(complex->name().empty() ? "<?>" : complex->name()));
    // type
    if (d_showAttris & CoreTreeWidget::ShowType) {
        text.append(" <font color=green size=2>" + QString("[COMPLEX]") + "</font>");
    }

    //
    itemDataItem->setText(text);

    //
    updateItemData(itemDataItem, complex->table()->allItem());
}

void CoreTreeWidgetPrivate::updateItemData(QStandardItem *itemDataItem, const Icd::FrameItemPtr &frame)
{
    if (!itemDataItem || !frame) {
        return;
    }

    //
    if (!itemDataItem->hasChildren()) {
        return;
    }

    //
    int i = 0;
    int childCount = itemDataItem->rowCount();
    const Icd::TablePtrMap &tables = frame->allTable();
    for (Icd::TablePtrMap::const_iterator citer = tables.cbegin();
         citer != tables.cend() && i < childCount; ++citer, ++i) {
        updateItemData(itemDataItem->child(i), citer->second);
    }
}

void CoreTreeWidgetPrivate::updateItemData(QStandardItem *itemTable, const TablePtr &table)
{
    if (!itemTable || !table) {
        return;
    }
    //
    QString text;
    // offset
    if (d_showAttris & CoreTreeWidget::ShowOffset) {
        text.append(ItemWorkerGroup::generateItemOffset(table));
    }
    // name
    text.append(QString::fromStdString(table->name().empty() ? "<?>" : table->name()));
    // type
    if (d_showAttris & CoreTreeWidget::ShowType) {
        text.append(" <font color=green size=2>" + QString("[TABLE]") + "</font>");
    }
    // fileName
    const QString filePath = itemTable->data(TreeFilePathRole).toString();
    if (!filePath.isEmpty()) {
        text.append(" [").append(QFileInfo(filePath).fileName()).append(']');
    }
    //
    TableItemWidget *itemWidget =
            qobject_cast<TableItemWidget *>(this->itemWidget(itemTable));
    if (itemWidget) {
        itemWidget->setText(text);
    } else {
        itemTable->setText(text);
    }
    //
    updateItemData(itemTable, table->allItem());
}

void CoreTreeWidgetPrivate::updateItemData(QStandardItem *itemTable, bool showValue)
{
    QHash<QStandardItem*, ItemWorkerGroup*>::ConstIterator citer =
            d_workerGroups.find(itemTable);
    if (citer != d_workerGroups.cend()) {
        citer.value()->updateItemData(showValue);
    }
}

void CoreTreeWidgetPrivate::removeWorkerGroup(QStandardItem *itemTable)
{
    QHash<QStandardItem *, ItemWorkerGroup*>::Iterator iter =
            d_workerGroups.find(itemTable);
    if (iter != d_workerGroups.end()) {
        ItemWorkerGroup *workerGroup = iter.value();
        workerGroup->setDirty();
        workerGroup->deleteLater();
        d_workerGroups.erase(iter);
    }
}

void CoreTreeWidgetPrivate::removeWorkerGroup(const WorkerPtr &worker)
{
    QHash<QStandardItem *, ItemWorkerGroup*>::Iterator iter =
            d_workerGroups.begin();
    for (; iter != d_workerGroups.end(); ++iter) {
        ItemWorkerGroup *workerGroup = iter.value();
        if (workerGroup->worker() == worker) {
            unbindChannel(iter.key(), worker);
            workerGroup->setDirty();
            workerGroup->deleteLater();
            d_workerGroups.erase(iter);
            break;
        }
    }
}

void CoreTreeWidgetPrivate::clearWorkerGroup()
{
    QHashIterator<QStandardItem *, ItemWorkerGroup*> citer(d_workerGroups);
    while (citer.hasNext()) {
        citer.next();
        ItemWorkerGroup *workerGroup = citer.value();
        workerGroup->setDirty();
        workerGroup->deleteLater();
        unbindChannel(citer.key(), workerGroup->worker());
    }
    d_workerGroups.clear();
}

ItemWorkerGroup *CoreTreeWidgetPrivate::findWorkerGroup(QStandardItem *itemTable) const
{
    QHash<QStandardItem *, ItemWorkerGroup*>::ConstIterator citer =
            d_workerGroups.find(itemTable);
    if (citer != d_workerGroups.cend()) {
        return citer.value();
    }

    return 0;
}

bool CoreTreeWidgetPrivate::hasItemBound(QStandardItem *item)
{
    if (!item) {
        return false;
    }

    //
    const int itemType = item->type();
    if (itemType == Icd::TreeItemTypeTable) {
        int count = item->rowCount();
        for (int i = 0; i < count; ++i) {
            if (hasItemBound(item->child(i))) {
                return true;
            }
        }
    } else if (itemType == Icd::TreeItemTypeDataItem) {
        return item->data(Icd::TreeBoundRole).toBool();
    }

    return false;
}

void CoreTreeWidgetPrivate::clearItemBoundRole(QStandardItem *item, bool bEmit)
{
    if (!item) {
        return;
    }

    //
    const int itemType = item->type();
    if (itemType == Icd::TreeItemTypeTable) {
        int count = item->rowCount();
        for (int i = 0; i < count; ++i) {
            clearItemBoundRole(item->child(i), bEmit);
        }
    } else if (itemType == Icd::TreeItemTypeDataItem) {
        if (bEmit) {
            QStandardItem *itemTable = findItemTable(item);
            emit this->unbindItem(item, itemTable);
        } else {
            item->setData(QVariant::Invalid, Icd::TreeBoundRole);
        }
    }
}

QStandardItem *CoreTreeWidgetPrivate::findItemTable(QStandardItem *item,
                                                    const QString &filePath) const
{
    if (!item) {
        return Q_NULLPTR;
    }

    const int itemType = item->type();
    if (itemType == TreeItemTypeTable) {
        const QString _filePath = item->data(TreeItemPathRole).toString();
        if (_filePath == filePath) {
            return item;
        }
    } else if (itemType > TreeItemTypeTable) {
        return Q_NULLPTR;
    }

    int rowCount = item->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem *childItem = item->child(i);
        childItem = findItemTable(childItem, filePath);
        if (childItem) {
            return childItem;
        }
    }

    return Q_NULLPTR;
}

QStandardItem *CoreTreeWidgetPrivate::findItemTable(QStandardItem *item) const
{
    if (!item) {
        return 0;
    }

    const int itemType = item->type();
    if (itemType == Icd::TreeItemTypeTable) {
        return item;
    } else if (itemType > Icd::TreeItemTypeTable) {
        return findItemTable(item->parent());
    }

    return Q_NULLPTR;
}

bool CoreTreeWidgetPrivate::exportData(const QStandardItem *item, bool exportAll)
{
    if (!item) {
        return false;
    }

    if (!d_parser) {
        return false;
    }

    // 获取数据项类型
    QStringList filters;

    // common
    //filters.append("Html File (*.htm *.html)");
    //filters.append("Excel File (*.xls *.xlsx)");
    filters.append("Word File (*.doc *.docx)");
    filters.append("PDF File (*.pdf)");
    filters.append("Xml File (*.xml)");
    filters.append("JSON File (*.json)");

    const int itemType = item->type();
    //
    switch (itemType) {
    case TreeItemTypeRoot:
        break;
    case TreeItemTypeVehicle:
        break;
    case TreeItemTypeSystem:
        break;
    case TreeItemTypeTable:
    {
        filters.append("C/C++ File (*.h *.hpp *.hxx)");
        //filters.append("C# File (*.cs)");
        filters.append("Matlab File (*.m)");
        break;
    }
    case TreeItemTypeDataItem:
        break;
    default:
        return false;
    }

    if (filters.isEmpty()) {
        return false;
    }

    Q_Q(CoreTreeWidget);
    const QString filePath = QFileDialog::getSaveFileName(
                q, QStringLiteral("导出文件路径选择"), QApplication::applicationDirPath(), filters.join(";;"));
    if (filePath.isEmpty()) {
        return false;
    }

    //
    Icd::ProgressDialog *progressDialog = new Icd::ProgressDialog(this);
    progressDialog->setWindowTitle(QStringLiteral("导出数据结构文档"));
    progressDialog->setProgressValue(100);
    //progressDialog->show();

    //
    QTimer *timerExport = new QTimer(this);

    //
    QFuture<bool> future = QtConcurrent::run([=](){
        return d_parser->saveAs(item, exportAll,
                                (d_bindTableTypes & CoreTreeWidget::BindOnlyRecv),
                                filePath.toStdString());
    });

    //
    auto deleteRes = [=](){
        progressDialog->hide();
        progressDialog->disconnect(this);
        timerExport->disconnect(this);
        progressDialog->deleteLater();
        timerExport->deleteLater();
        ///this->setEnabled(true);
        //d_searchEdit->setEnabled(true);
    };

    connect(progressDialog, &ProgressDialog::accepted, this, [=](){
        //
        QDesktopServices::openUrl(QUrl(filePath));
        //
        deleteRes();
    });
    connect(progressDialog, &ProgressDialog::rejected, this, [=](){
        d_parser->cancelSaveAs(true);
        progressDialog->waitForFinished();
        timerExport->stop();
        QApplication::removePostedEvents(this, QEvent::Timer);
        deleteRes();
    });
    connect(progressDialog, &ProgressDialog::finished, this, [=](){
        timerExport->stop();
        QApplication::removePostedEvents(this, QEvent::Timer);
        if (progressDialog->futureResult()) {
            progressDialog->setProgressVisible(false);
            progressDialog->setMessage(QStringLiteral("文档导出成功，是否现在打开文档？\n%1").arg(filePath));
            progressDialog->setAcceptText(QStringLiteral("是"));
            progressDialog->setCancelText(QStringLiteral("否"));
            progressDialog->setAcceptVisible(true);
        } else {
            progressDialog->hide();
            QMessageBox::information(q, QStringLiteral("执行结果"), QStringLiteral("导出失败！"));
            deleteRes();
        }
    });
    connect(timerExport, &QTimer::timeout, this, [=](){
        progressDialog->setMessage(QString::fromStdString(d_parser->message()));
        //progressDialog->setProgressValue(d_parser->progressValue());
    });

    progressDialog->setFuture(future);
    timerExport->start(300);

    //this->setEnabled(false);
    //d_searchEdit->setEnabled(false);

    progressDialog->exec();

    return true;
}

QStandardItem *CoreTreeWidgetPrivate::findTableItem(QStandardItem *itemParent,
                                                    const QString &domain)
{
    if (!itemParent) {
        return Q_NULLPTR;
    }

    int rowCount = itemParent->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem *childItem = itemParent->child(i);
        if (!childItem) {
            continue;
        }
        int itemType = childItem->type();
        if (itemType < TreeItemTypeTable) {
            QStandardItem *item = findTableItem(childItem, domain);
            if (item) {
                return item;
            }
        } else if (itemType == TreeItemTypeTable) {
            const QVariant varDomain = childItem->data(Icd::TreeItemDomainRole);
            if (varDomain.isValid()) {
                if (varDomain.toString() == domain) {
                    return childItem;
                }
            }
        } else {
            break;
        }
    }

    return Q_NULLPTR;
}

void CoreTreeWidgetPrivate::findAllTableItem(QList<QStandardItem *> &items,
                                             QStandardItem *itemParent, bool binding)
{
    if (!itemParent) {
        return;
    }

    int rowCount = itemParent->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem *childItem = itemParent->child(i);
        if (!childItem) {
            continue;
        }
        int itemType = childItem->type();
        if (itemType < TreeItemTypeTable) {
            findAllTableItem(items, childItem, binding);
        } else if (itemType == TreeItemTypeTable) {
            const QVariant varChannelId = childItem->data(Icd::TreeChannelIdRole);
            if (binding) {
                if (!varChannelId.isNull()) {
                    items.append(childItem);
                }
            } else {
                if (varChannelId.isNull()) {
                    items.append(childItem);
                }
            }
        } else {
            break;
        }
    }
}

void CoreTreeWidgetPrivate::procAllTableItem(QStandardItem *itemParent, bool binding,
                                             std::function<bool(QStandardItem *,const QString&)> callback)
{
    if (!itemParent) {
        return;
    }

    int rowCount = itemParent->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem *childItem = itemParent->child(i);
        if (!childItem) {
            continue;
        }
        int itemType = childItem->type();
        if (itemType < TreeItemTypeTable) {
            procAllTableItem(childItem, binding, callback);
        } else if (itemType == TreeItemTypeTable) {
            const QVariant varChannelId = childItem->data(Icd::TreeChannelIdRole);
            if (binding) {
                if (!varChannelId.isNull()) {
                    if (!childItem->hasChildren()) {
                        continue;
                    }
                    callback(childItem, varChannelId.toString());
                }
            } else {
                if (varChannelId.isNull()) {
                    if (!callback(childItem, QString())) {
                        break;
                    }
                }
            }
        } else {
            break;
        }
    }
}

BindingData CoreTreeWidgetPrivate::bindingMapTask(BindingData data)
{
    // find item
    QStandardItem *item = data.d->findTableItem(
                data.d->invisibleRootItem(), data.tableDomain);
    if (!item) {
        return data;
    }
    data.item = item;
    // worker
    Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(data.channelId.toStdString());
    if (!worker) {
        return data;
    }
    data.worker = worker;
    //
    const QStringList sections = data.tableDomain.split('/');
    if (sections.count() != 3) {
        return data;
    }
    // find table object
    if (!data.d->d_parser->parseTable(sections.at(0).toStdString(),
                                      sections.at(1).toStdString(),
                                      sections.at(2).toStdString(),
                                      data.table, Icd::ObjectItem)) {
        return data;
    }

    return data;
}

void CoreTreeWidgetPrivate::bindingMapReduce(int &count, const BindingData &data)
{
    Q_UNUSED(data);
    ++count;
}

} // end of namespace Icd

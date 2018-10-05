#include "precomp.h"
#include "JProtoTreeView_p.h"
#include "icdworker/icdworker.h"
#include "icdworker/icdworker_pool.h"
#include "../JGroupChannelPane.h"
#include "JWorkerGroup.h"
#include "../JSearchEdit.h"
#include "../JProgressDialog.h"
#include <QDomDocument>
#include <QtConcurrent>
#include <QTextDocument>
#include <QHBoxLayout>

namespace Icd {

// class TreeItemDelegateData

class TreeItemDelegateData
{
public:
    TreeItemDelegateData()
        : treeView(nullptr)
        , buttonRun(new QPushButton("Run"))
    {
        buttonRun->setObjectName("buttonRun");
        buttonRun->setCheckable(true);
        if (qApp) {
            buttonRun->setStyleSheet(qApp->styleSheet());
        }
        QObject::connect(buttonRun.data(), &QPushButton::clicked, [=](bool checked){
            qDebug() << "clicked:" << checked;
        });
    }

    JTreeView *treeView;
    QScopedPointer<QPushButton> buttonRun;
};

// class TreeItemDelegate

TreeItemDelegate::TreeItemDelegate(JTreeView *treeView, QObject *parent)
    : JTreeItemDelegate(parent)
    , d(new TreeItemDelegateData())
{
    d->treeView = treeView;
}

TreeItemDelegate::~TreeItemDelegate()
{
    delete d;
}

QSize TreeItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid()) {
        return qvariant_cast<QSize>(value);
    }

    QStyleOptionViewItem viewOption = option;
    initStyleOption(&viewOption, index);

    const QString simplifiedText = simplified(viewOption.text.trimmed());
    viewOption.text = simplifiedText.section(" [", 0, -2, QString::SectionSkipEmpty).trimmed();
    if (viewOption.text.isEmpty()) {
        viewOption.text = simplifiedText;
    }
    const QWidget *widget = viewOption.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    QSize size = style->sizeFromContents(QStyle::CT_ItemViewItem, &viewOption, QSize(), widget);
    size.setWidth(viewOption.fontMetrics.width(simplifiedText));
    return size;
}

void TreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    JTreeItemDelegate::paint(painter, option, index);

    QStandardItem *tableItem = this->tableItem(index);
    if (!tableItem) {
        return;
    }

    if (!tableItem->data(Icd::TreeChannelIdRole).isValid()) {
        return;
    }

    if (!d->treeView) {
        return;
    }

    QStyleOptionViewItem viewOption = option;
    initStyleOption(&viewOption, index);

    const QSize size = sizeHint(viewOption, index);

    TableItemWidget *itemWidget = qobject_cast<TableItemWidget*>(d->treeView->itemWidget(tableItem));
    if (!itemWidget) {
        return;
    }

    itemWidget->setSpacing(size.width() + 5);
}

QStandardItem *TreeItemDelegate::tableItem(const QModelIndex &index)
{
    QStandardItemModel *standardItemModel =
            sourceModel(const_cast<QAbstractItemModel*>(index.model()));
    QModelIndex _index = mapToSource(index);
    QStandardItem *item = standardItemModel->itemFromIndex(_index);
    if (!item) {
        return nullptr;
    }

    if (item->type() != Icd::TreeItemTypeTable) {
        return nullptr;
    }

    return item;
}

// class TableItemWidget

TableItemWidget::TableItemWidget(JProtoTreeView::BindTableTypes bindingTypes,
                                 QWidget *parent)
    : QWidget(parent)
    , worker_(nullptr)
    , bindTableTypes_(bindingTypes)
{
    setObjectName("TableItemWidget");
    setFixedHeight(22);
    setStyleSheet("QWidget#TableItemWidget{background:transparent;}");

    QHBoxLayout *layoutMain = new QHBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    spacer_ = new QSpacerItem(10, sizeHint().height());
    layoutMain->addSpacerItem(spacer_);

    buttonRun_ = new QPushButton(this);
    buttonRun_->setObjectName("buttonRun");
    buttonRun_->setFixedSize(40, 20);
    buttonRun_->setCheckable(true);
    layoutMain->addWidget(buttonRun_);

    layoutMain->addStretch();

    connect(buttonRun_, &QPushButton::toggled, this, [=](bool checked){
        if (toggle(checked)) {
            emit this->toggled(checked);
        }
    });

    //
    buttonRun_->setChecked(false);
    buttonRun_->hide();
    updateButtonIcon(buttonRun_->isChecked());
}

TableItemWidget::~TableItemWidget()
{

}

WorkerPtr TableItemWidget::worker() const
{
    return worker_;
}

void TableItemWidget::setWorker(const WorkerPtr &worker)
{
    if (worker_) {
        if (worker_ != worker) {
            stop();
        }
        worker_->disconnect(this);
        worker_->workerSend()->disconnect(this);
        worker_->workerRecv()->disconnect(this);
    }

    worker_ = worker;

    if (worker_) {
        worker_->disconnect(this);
        buttonRun_->show();
        if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
            buttonRun_->setChecked(worker_->workerSend()->isRunning());
            connect(worker_->workerSend().get(), &Icd::WorkerSend::toggled,
                    this, [=](bool checked){
                buttonRun_->setChecked(checked);
            });
        } else  if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
            buttonRun_->setChecked(worker_->workerRecv()->isRunning());
            connect(worker_->workerRecv().get(), &Icd::WorkerRecv::toggled,
                    this, [=](bool checked){
                buttonRun_->setChecked(checked);
            });
        }
    } else {
        buttonRun_->hide();
        buttonRun_->setChecked(false);
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

bool TableItemWidget::toggle(bool checked)
{
    if (worker_) {
        Icd::WorkerTransPtr workerTrans = nullptr;
        if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
            workerTrans = worker_->workerSend();
        } else if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
            workerTrans = worker_->workerRecv();
        } else if (bindTableTypes_ == JProtoTreeView::BindAllTable) {
            Q_ASSERT(false);
            return false;
        }
        if (workerTrans) {
            if (checked) {
                if (!workerTrans->isRunning()) {
                    if (!worker_->isOpen()) {
                        if (!worker_->open()) {
                            return false;
                        }
                    }
                    if (!workerTrans->start()) {
                        return false;
                    }
                }
            } else {
                if (workerTrans->isRunning()) {
                    workerTrans->stop();
                    worker_->close();
                }
            }
        }
    }

    updateButtonIcon(checked);

    return true;
}

bool TableItemWidget::isRunning() const
{
    return buttonRun_->isChecked();
}

void TableItemWidget::setSpacing(int spacing)
{
    spacer_->changeSize(spacing, height());
    layout()->invalidate();
}

void TableItemWidget::updateButtonIcon(bool checked)
{
    if (checked) {
        buttonRun_->setIcon(QIcon(":/icdwidget/image/tree/stop.png"));
    } else {
        buttonRun_->setIcon(QIcon(":/icdwidget/image/tree/run.png"));
    }
}

// class JProtoTreeViewPrivate

JProtoTreeViewPrivate::JProtoTreeViewPrivate(JProtoTreeView *q)
    : JTreeView(q)
    , J_QPTR(q)
    , searchEdit_(nullptr)
    , parser_(nullptr)
    , loadingDeep_(Icd::ObjectTable)
    , intervalUpdate_(200)     // 200ms
    , bindTableTypes_(JProtoTreeView::BindAllTable)
    , showAttris_(JProtoTreeView::ShowType | JProtoTreeView::ShowValue)
    , dataFormat_(16)
    , treeModes_(nullptr)
    , newItem_(nullptr)
{

}

JProtoTreeViewPrivate::~JProtoTreeViewPrivate()
{
    if (watcher_.isRunning()) {
        watcher_.cancel();
        watcher_.waitForFinished();
    }

    //
    Icd::WorkerPool::getInstance()->disconnect(this);

    //
    unbindChannel(invisibleRootItem());
}

QColor JProtoTreeViewPrivate::valueColor() const
{
    return valueColor_;
}

void JProtoTreeViewPrivate::init()
{
    Q_Q(JProtoTreeView);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    searchEdit_ = new JSearchEdit(q);
    vertLayoutMain->addWidget(searchEdit_);

    vertLayoutMain->addWidget(this);
    //
    setItemDelegateForColumn(0, new TreeItemDelegate(this, this));
    //
    JTreeSortFilterModel *filterModel = new JTreeSortFilterModel(this);
    setFilterModel(filterModel);

    setAnimated(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    header()->setDefaultAlignment(Qt::AlignCenter);
    header()->hide();
    setHeaderLabel(tr("Protocol information"));

    //
    connect(searchEdit_, &JSearchEdit::textChanged, this, [=](const QString &text){
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
            this, SLOT(onCurrentItemChanged(QStandardItem*,QStandardItem*)));
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
            this, &JProtoTreeViewPrivate::onWorkerRemoved);
    connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerCleared,
            this, &JProtoTreeViewPrivate::onWorkerCleared);
}

void JProtoTreeViewPrivate::setParser(const Icd::ParserPtr &parser)
{
    parser_ = parser;
}

int JProtoTreeViewPrivate::loadingDeep() const
{
    return loadingDeep_;
}

void JProtoTreeViewPrivate::setLoadingDeep(int deep)
{
    loadingDeep_ = deep;
}

int JProtoTreeViewPrivate::intervalUpdate() const
{
    return intervalUpdate_;
}

void JProtoTreeViewPrivate::setIntervalUpdate(int interval)
{
    if (interval != intervalUpdate_) {
        intervalUpdate_ = interval;
        QHashIterator<QStandardItem*, JWorkerGroup*> citer(workerGroups_);
        while (citer.hasNext()) {
            citer.next();
            citer.value()->setTimerInterval(interval);
        }
    }
}

bool JProtoTreeViewPrivate::loadData()
{
    bool result = true;

    clearData();

    JTreeViewItem *itemRoot = new JTreeViewItem(tr("Protocol"), Icd::TreeItemTypeRoot);
    invisibleRootItem()->appendRow(itemRoot);

    result = result && loadRoot(itemRoot, loadingDeep_);
    if (result) {
        expandItem(itemRoot, true, 3);
    }

    return result;
}

bool JProtoTreeViewPrivate::loadData(const Icd::TablePtr &table, const QString &domain)
{
    if (!table) {
        return false;
    }

    bool result = true;

    clearData();

    invisibleRootItem()->setData(domain.section('/', 0, 1), Icd::TreeItemDomainRole);

    result = result && loadSystem(this, invisibleRootItem(), table);
    if (result && invisibleRootItem()->rowCount() > 0) {
        expandItem(invisibleRootItem()->child(0), true, 1);
    }

    return result;
}

bool JProtoTreeViewPrivate::loadData(const TablePtr &table, const QString &filePath,
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
        QStandardItem *itemTable = loadSystem(this, invisibleRootItem, table);
        if (itemTable) {
            itemTable->setData(filePath, TreeFilePathRole);
            itemTable->setData(QString::fromStdString(table->mark()), Icd::TreeItemMarkRole);
            itemTable->setData(hasTimeFormat, TreeHasTimeFormatRole);
            itemTable->setData(headerSize, TreeHeaderSizeRole);
            itemTable->setText(itemTable->text().append(QString(" [%1]").arg(QFileInfo(filePath)
                                                                             .fileName())));
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

void JProtoTreeViewPrivate::clearData()
{
    clearWorkerGroup();
    unbindChannelItem(invisibleRootItem());
    Q_Q(JProtoTreeView);
    emit q->unbindItem(invisibleRootItem(), nullptr);
    clearContents();
    protoRoot_.reset();
}

JProtoTreeView::ShowAttributes JProtoTreeViewPrivate::showAttributes() const
{
    return showAttris_;
}

void JProtoTreeViewPrivate::setShowAttributes(JProtoTreeView::ShowAttributes attrs)
{
    if (attrs != showAttris_) {
        showAttris_ = attrs;
        updateItemData(invisibleRootItem()->child(0));
    }
}

void JProtoTreeViewPrivate::setShowAttribute(JProtoTreeView::ShowAttribute attr, bool on)
{
    bool needUpdate = false;
    if (on) {
        if (!(showAttris_ & attr)) {
            showAttris_ |= attr;
            needUpdate = true;
        }
    } else {
        if (showAttris_ & attr) {
            showAttris_ &= ~attr;
            needUpdate = true;
        }
    }

    if (needUpdate) {
        switch (attr) {
        case JProtoTreeView::ShowOffset:
        case JProtoTreeView::ShowType:
            updateItemData(invisibleRootItem()->child(0));
            break;
        case JProtoTreeView::ShowData:
        case JProtoTreeView::ShowValue:
        case JProtoTreeView::ShowSpec:
        {
            QHashIterator<QStandardItem *, JWorkerGroup*> citer(workerGroups_);
            while (citer.hasNext()) {
                citer.next();
                citer.value()->updateItemData(showAttris_, dataFormat_);
            }
            break;
        }}
    }
}

bool JProtoTreeViewPrivate::testShowAttribute(JProtoTreeView::ShowAttribute attr) const
{
    return showAttris_ & attr;
}

int JProtoTreeViewPrivate::dataFormat() const
{
    return dataFormat_;
}

void JProtoTreeViewPrivate::setDataFormat(int format)
{
    if (format != dataFormat_) {
        dataFormat_ = format;
        updateItemData(invisibleRootItem()->child(0));
    }
}

quint32 JProtoTreeViewPrivate::treeModes() const
{
    return quint32(treeModes_);
}

void JProtoTreeViewPrivate::setTreeMode(JProtoTreeView::TreeModes modes)
{
    treeModes_ = modes;
}

void JProtoTreeViewPrivate::setTreeMode(JProtoTreeView::TreeMode mode, bool on)
{
    if (on) {
        if (!(treeModes_ & mode)) {
            treeModes_ |= mode;
        }
    } else {
        if (treeModes_ & mode) {
            treeModes_ &= ~mode;
        }
    }
}

bool JProtoTreeViewPrivate::testTreeMode(JProtoTreeView::TreeMode mode) const
{
    return treeModes_ & mode;
}

bool JProtoTreeViewPrivate::isRunning() const
{
    for (QHash<QStandardItem *, JWorkerGroup*>::const_iterator
         citer = workerGroups_.cbegin(); citer != workerGroups_.cend(); ++citer) {
        JWorkerGroup *workerGroup = citer.value();
        if (workerGroup && workerGroup->isRunning()) {
            return true;
        }
    }

    return false;
}

void JProtoTreeViewPrivate::setRunning(bool value)
{
    for (QHash<QStandardItem *, JWorkerGroup*>::const_iterator
         citer = workerGroups_.cbegin(); citer != workerGroups_.cend(); ++citer) {
        JWorkerGroup *workerGroup = citer.value();
        if (workerGroup) {
            workerGroup->setRunning(value);
        }
    }
}

void JProtoTreeViewPrivate::bindingChannels(const QString &filePath)
{
    //
    unbindingChannels();
    //
    auto enabledFunc = [=](bool enabled, bool success){
        Q_UNUSED(success);
        if (enabled) {
            setEnabled(true);
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, tr("Binding result"), tr("Binding success"));
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
        bindings.append(BindingData(channelId, tableDomain, this));
    }
    if (bindings.isEmpty()) {
        enabledFunc(true, false);
        return;
    }
    //
    if (watcher_.isRunning()) {
        watcher_.cancel();
        watcher_.waitForFinished();
    }
    watcher_.disconnect(this);
    //
    QFuture<BindingData> future = QtConcurrent::mapped(bindings, bindingMapTask);
    connect(&watcher_, &QFutureWatcher<BindingData>::resultReadyAt,
            this, [=](int resultIndex){
        const BindingData data = watcher_.resultAt(resultIndex);
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
    connect(&watcher_, &QFutureWatcher<BindingData>::finished, this, [=](){
        enabledFunc(true, true);
    });
    connect(&watcher_, &QFutureWatcher<BindingData>::canceled, this, [=](){
        enabledFunc(true, false);
    });
    watcher_.setFuture(future);
}

void JProtoTreeViewPrivate::unbindingChannels()
{
    //
    stopAllChannels();
    //
    unbindChannelItem(invisibleRootItem());
}

void JProtoTreeViewPrivate::exportBindingStatus(const QString &filePath)
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
    QMessageBox::information(this, tr("Export finish"), tr("Export success!"));
}

void JProtoTreeViewPrivate::exportTableData(QStandardItem *item)
{
    if (!item) {
        return;
    }

    QStandardItem *itemTable = findItemTable(item);
    if (!itemTable) {
        return;
    }

    const QString filePath = itemTable->data(TreeFilePathRole).toString();
    const bool hasTimeFormat = itemTable->data(TreeHasTimeFormatRole).toBool();
    const int headerSize = itemTable->data(TreeHeaderSizeRole).toInt();

    Q_Q(JProtoTreeView);
    emit q->exportAnalyseData(item, filePath, hasTimeFormat, headerSize);
}

void JProtoTreeViewPrivate::runAllChannels()
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
        Icd::WorkerTransPtr workerTrans = nullptr;
        if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
            workerTrans = worker->workerSend();
        } else if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
            workerTrans = worker->workerRecv();
        } else if (bindTableTypes_ == JProtoTreeView::BindAllTable) {
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

void JProtoTreeViewPrivate::stopAllChannels()
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
        Icd::WorkerTransPtr workerTrans = nullptr;
        if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
            workerTrans = worker->workerSend();
        } else if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
            workerTrans = worker->workerRecv();
        } else if (bindTableTypes_ == JProtoTreeView::BindAllTable) {
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

bool JProtoTreeViewPrivate::isEditMode() const
{
    return (treeModes_ == JProtoTreeView::TreeModeEdit);
}

bool JProtoTreeViewPrivate::hasUnloadedItem(QStandardItem *item) const
{
    return (unloadedItem(item) != nullptr);
}

QStandardItem *JProtoTreeViewPrivate::unloadedItem(QStandardItem *item) const
{
    if (!item) {
        item = invisibleRootItem();
    } else if (item->type() > Icd::TreeItemTypeTable) {
        return nullptr;
    }

    if (!isEditMode()) {
        return nullptr;
    }

    Q_Q(const JProtoTreeView);

    const int count = item->rowCount();
    for (int i = 0; i < count; ++i) {
        QStandardItem *childItem = item->child(i);
        if (childItem->type() > Icd::TreeItemTypeTable) {
            continue;
        }
        if (!q->isItemLoaded(childItem)) {
            return childItem;
        }
        QStandardItem *ret = unloadedItem(childItem);
        if (ret) {
            return ret;
        }
    }

    return nullptr;
}

bool JProtoTreeViewPrivate::isItemLoaded(QStandardItem *item)
{
    if (!item) {
        return false;
    }

    const QVariant varLoad = item->data(Icd::TreeLoadStatusRole);
    if (varLoad.isValid()) {
        return varLoad.toBool();
    } else {
        return false;
    }
}

ObjectPtr JProtoTreeViewPrivate::findObject(QStandardItem *item) const
{
    if (!item) {
        return Icd::ObjectPtr();
    }

    if (item == newItem_) {
        return newObject_;
    }

    const QString domain = item->data(Icd::TreeItemDomainRole).toString();
    if (domain.isEmpty()) {
        return protoRoot_;
    }

    if (!protoRoot_) {
        return Icd::ObjectPtr();
    }

    return protoRoot_->findByDomain(domain.toStdString());
}

void JProtoTreeViewPrivate::onTreeItemPressed(QStandardItem *item)
{
    if (!item) {
        return;
    }

    Qt::MouseButtons mouseButtons = QApplication::mouseButtons();
    switch (mouseButtons) {
    case Qt::LeftButton:
    {
        if (item->hasChildren()) {
            break;
        }

        switch (treeModes_) {
        case JProtoTreeView::TreeModeTableSel:
            break;
        default:
        {
            switch (item->type()) {
            case Icd::TreeItemTypeRoot:
                if (loadRoot(item, loadingDeep_)) {
                    expandItem(item, true, 3);
                }
                break;
            case Icd::TreeItemTypeVehicle:
                if (loadVehicle(item, loadingDeep_)) {
                    expandItem(item, true, 2);
                }
                break;
            case Icd::TreeItemTypeSystem:
                if (loadSystem(item, loadingDeep_)) {
                    expandItem(item, true, 2);
                }
                break;
            case Icd::TreeItemTypeTable:
                if (loadTable(item, loadingDeep_)) {
                    expandItem(item, true, 1);
                }
                break;
            default:
                break;
            }
        }}

        break;
    }
    case Qt::RightButton:
    {
        // get deep status

        // dispatch
        switch (item->type()) {
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

void JProtoTreeViewPrivate::onCurrentItemChanged(QStandardItem *current, QStandardItem *previous)
{
    if (treeModes_ != JProtoTreeView::TreeModeEdit || !parser_) {
        return;
    }

    Q_UNUSED(previous);
    if (!current || current->hasChildren()) {
        return;
    }

    if (isEditMode() && isItemLoaded(current)) {
        return;
    }

    const QVariant varDomain = current->data(Icd::TreeItemDomainRole);

    switch (current->type()) {
    case Icd::TreeItemTypeRoot:
    {
        Icd::RootPtr root = findByDomain<Icd::Root>(QString());
        if (!root || !root->isEmpty()) {
            break;
        }

        if (!parser_->parse(root, Icd::ObjectSystem)) {
            break;
        }

        break;
    }
    case Icd::TreeItemTypeVehicle:
    {
        if (!varDomain.isValid()) {
            return;
        }

        const QString domain = varDomain.toString();
        Icd::VehiclePtr vehicle = findByDomain<Icd::Vehicle>(varDomain.toString());
        if (!vehicle || !vehicle->isEmpty()) {
            break;
        }

        if (!parser_->parse(domain.toStdString(), vehicle, Icd::ObjectSystem)) {
            break;
        }
        break;
    }
    case Icd::TreeItemTypeSystem:
    {
        if (!varDomain.isValid()) {
            return;
        }

        const QString domain = varDomain.toString();
        Icd::SystemPtr system = findByDomain<Icd::System>(varDomain.toString());
        if (!system || !system->isEmpty()) {
            break;
        }

        if (!parser_->parse(domain.section('/', 0, 0).toStdString(),
                            domain.section('/', 1).toStdString(),
                            system, Icd::ObjectTable)) {
            break;
        }
        break;
    }
    case Icd::TreeItemTypeTable:
    {
        if (!varDomain.isValid()) {
            return;
        }

        const QString domain = varDomain.toString();
        Icd::TablePtr table = findByDomain<Icd::Table>(varDomain.toString());
        if (!table || !table->isEmpty()) {
            break;
        }

        if (!parser_->parse(domain.section('/', 0, 0).toStdString(),
                            domain.section('/', 1, 1).toStdString(),
                            domain.section('/', 2).toStdString(),
                            table, Icd::ObjectItem)) {
            break;
        }
        break;
    }
    case Icd::TreeItemTypeDataItem:
    case Icd::TreeItemTypeItemTable:
    case Icd::TreeItemTypeItemBitMap:
    default:
        break;
    }
}

void JProtoTreeViewPrivate::onWorkerRemoved(const WorkerPtr &worker)
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
        Icd::WorkerTransPtr workerTrans = nullptr;
        if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
            workerTrans = _worker->workerSend();
        } else if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
            workerTrans = _worker->workerRecv();
        } else if (bindTableTypes_ == JProtoTreeView::BindAllTable) {
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

void JProtoTreeViewPrivate::onWorkerCleared()
{
    clearWorkerGroup();
    unbindingChannels();
}

QStringList JProtoTreeViewPrivate::mimeTypes() const
{
    QStringList types;
    if (treeModes_ & JProtoTreeView::TreeModeMonitor) {
        types << "icd/table-drag/monitor";
    }
    if (treeModes_ & JProtoTreeView::TreeModeSimulator) {
        types << "icd/table-drag/simulator";
    }
    if (treeModes_ & JProtoTreeView::TreeModeAnalyse) {
        types << "icd/table-drag/analyse";
    }
    if (treeModes_ & JProtoTreeView::TreeModeTableSel) {
        types << "icd/table-drag/tablesel";
    }
    return types;
}

QMimeData *JProtoTreeViewPrivate::mimeData(const QList<QStandardItem *> &items) const
{
    if (items.isEmpty()) {
        return nullptr;
    }

    QStandardItem *item = items.first();
    if (!item) {
        return nullptr;
    }

    QMimeData *mData = new QMimeData();
    if (treeModes_ & JProtoTreeView::TreeModeMonitor) {
        mData->setData("icd/table-drag/monitor", QByteArray("1"));
    }
    if (treeModes_ & JProtoTreeView::TreeModeSimulator) {
        mData->setData("icd/table-drag/simulator", QByteArray("1"));
    }
    if (treeModes_ & JProtoTreeView::TreeModeAnalyse) {
        mData->setData("icd/table-drag/analyse", QByteArray("1"));
    }
    if (treeModes_ & JProtoTreeView::TreeModeTableSel) {
        if (item->type() != Icd::TreeItemTypeTable) {
            return nullptr;
        }
        mData->setData("icd/table-drag/tablesel", QByteArray("1"));
    }
    mData->setProperty("domain", item->data(Icd::TreeItemDomainRole));
    mData->setProperty("item", QVariant::fromValue(static_cast<void*>(item)));
    //
    switch (item->type()) {
    case Icd::TreeItemTypeTable:
    {
        mData->setProperty("itemTable", QVariant::fromValue(static_cast<void*>(item)));
        break;
    }
    case Icd::TreeItemTypeItemTable:
    case Icd::TreeItemTypeDataItem:
    case Icd::TreeItemTypeItemBitMap:
    {
        QStandardItem *itemTable = findItemTable(item);
        if (!itemTable) {
            delete mData;
            return nullptr;
        }

        mData->setProperty("itemTable", QVariant::fromValue(static_cast<void*>(itemTable)));
        break;
    }
    default:
        delete mData;
        return nullptr;
    }

    return mData;
}

void JProtoTreeViewPrivate::itemRootRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    Q_Q(JProtoTreeView);

    //
    if (isEditMode()) {
        if (!item->hasChildren() && !isItemLoaded(item)) {
            if (loadRoot(item, deep, true)) {
                expandItem(item, true, 3);
            }
        }
    }

    QMenu menu(this);
    switch (treeModes_) {
    case JProtoTreeView::TreeModeTableSel:
    {
        if (isItemSelected(item)) {
            // collapse all
            QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                     tr("Collapse all"));
            connect(actionCollapse, &QAction::triggered, this, [=](){
                expandItem(item, false, -1);
            });
        }
        // expand all
        QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                  tr("Expand all"));
        connect(actionExpandAll, &QAction::triggered, this, [=](){
            expandItem(item, true, -1);
        });
        break;
    }
    default:
    {
        auto loadVehicleFunc = [=](){
            if (loadRoot(item, deep, true)) {
                expandItem(item, true, 2);
            }
        };
        auto unloadVehicleFunc = [=](){
            unbindChannelItem(item);
            emit itemUnloaded(item, nullptr);
            clearItemChildren(item);
            //
            if (isEditMode()) {
                setItemLoadStatus(item, false);
                emit q->itemUpdated(item, true, false);
            }
        };
        //
        if (isEditMode()) {
            // add vehicle
            QAction *actionAddVehicle = menu.addAction(QIcon(":/icdwidget/image/tree/add.png"),
                                                       tr("Add vehicle"));
            connect(actionAddVehicle, &QAction::triggered, this, [=](){
                emit q->requestAdd(item);
            });
            // clear vehicle
            if (item->hasChildren()) {
                QAction *actionClearVehicle = menu.addAction(QIcon(":/icdwidget/image/tree/clean.png"),
                                                             tr("Clear vehicle"));
                connect(actionClearVehicle, &QAction::triggered, this, [=](){
                    cleanItem(item);
                });
            }
        }
        //
        if (item->hasChildren()) {
            // update vehicle
            QAction *actionUpdateVehicle = menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                                                          tr("Update vehicle"));
            connect(actionUpdateVehicle, &QAction::triggered, this, [=](){
                unloadVehicleFunc();
                loadVehicleFunc();
            });
            // unload vehicle
            QAction *actionUnloadVehicle = menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                                                          tr("Unload vehicle"));
            connect(actionUnloadVehicle, &QAction::triggered, this, [=](){
                unloadVehicleFunc();
            });
            if (isItemSelected(item)) {
                // collapse all
                QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                         tr("Collapse all"));
                connect(actionCollapse, &QAction::triggered, this, [=](){
                    expandItem(item, false, -1);
                });
            }
            // expand all
            QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                      tr("Expand all"));
            connect(actionExpandAll, &QAction::triggered, this, [=](){
                expandItem(item, true, -1);
            });
        } else {
            // load vehicle
            QAction *actionUpdateVehicle = menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                                                          tr("Load vehicle"));
            connect(actionUpdateVehicle, &QAction::triggered, this, [=](){
                loadVehicleFunc();
            });
        }
        // exprot all
        QAction *actionExportAll = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                                  tr("Export all protocol"));
        connect(actionExportAll, &QAction::triggered, this, [=](){
            exportData(item, true);
        });
        // export exists
        QAction *actionExportExists = menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                                                     tr("Export loaded protocol"));
        connect(actionExportExists, &QAction::triggered, this, [=](){
            exportData(item, false);
        });
        break;
    }}

    menu.exec(QCursor::pos());
}

void JProtoTreeViewPrivate::itemVehicleRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    Q_Q(JProtoTreeView);

    //
    if (isEditMode()) {
        if (!item->hasChildren() && !isItemLoaded(item)) {
            if (loadVehicle(item, deep, true)) {
                expandItem(item, true, 2);
            }
        }
    }

    QMenu menu(this);
    switch (treeModes_) {
    case JProtoTreeView::TreeModeTableSel:
    {
        if (isItemSelected(item)) {
            // collapse all
            QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                     tr("Collapse all"));
            connect(actionCollapse, &QAction::triggered, this, [=](){
                expandItem(item, false, -1);
            });
        }
        // expand all
        QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                  tr("Expand all"));
        connect(actionExpandAll, &QAction::triggered, this, [=](){
            expandItem(item, true, -1);
        });
        break;
    }
    default:
    {
        auto loadSystemFunc = [=](){
            if (loadVehicle(item, deep, true)) {
                expandItem(item, true, 2);
            }
        };
        auto unloadSystemFunc = [=](){
            unbindChannelItem(item);
            emit itemUnloaded(item, nullptr);
            clearItemChildren(item);
            //
            if (isEditMode()) {
                setItemLoadStatus(item, false);
                emit q->itemUpdated(item, true, false);
            }
        };
        //
        if (isEditMode()) {
            // add system
            QAction *actionAddSystem = menu.addAction(QIcon(":/icdwidget/image/tree/add.png"),
                                                      tr("Add system"));
            connect(actionAddSystem, &QAction::triggered, this, [=](){
                emit q->requestAdd(item);
            });
            // clean system
            if (item->hasChildren()) {
                QAction *actionClearSystem = menu.addAction(QIcon(":/icdwidget/image/tree/clean.png"),
                                                            tr("Clear system"));
                connect(actionClearSystem, &QAction::triggered, this, [=](){
                    cleanItem(item);
                });
            }
            // remove vehicle
            QAction *actionRemoveVehicle = menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                                                          tr("Remove vehicle"));
            connect(actionRemoveVehicle, &QAction::triggered, this, [=](){
                removeRow(item);
            });
            // insert vehicle [before]
            QAction *actionInsertAbove = menu.addAction(QIcon(":/icdwidget/image/tree/insert-above.png"),
                                                        tr("Insert vehicle above"));
            connect(actionInsertAbove, &QAction::triggered, this, [=](){
                insertNewRow(item, InsertAboveAction);
            });
            // insert vehicle [after]
            QAction *actionInserBelow = menu.addAction(QIcon(":/icdwidget/image/tree/insert-below.png"),
                                                       tr("Insert vehicle below"));
            connect(actionInserBelow, &QAction::triggered, this, [=](){
                insertNewRow(item, InsertBelowAction);
            });
        }
        //
        if (item->hasChildren()) {
            // upload system
            QAction *actionUpdateSystem = menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                                                         tr("Update system"));
            connect(actionUpdateSystem, &QAction::triggered, this, [=](){
                unloadSystemFunc();
                loadSystemFunc();
            });
            // unload system
            QAction *actionUnloadSystem = menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                                                         tr("Unload system"));
            connect(actionUnloadSystem, &QAction::triggered, this, [=](){
                unloadSystemFunc();
            });
            if (isItemSelected(item)) {
                // collapse all
                QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                         tr("Collapse all"));
                connect(actionCollapse, &QAction::triggered, this, [=](){
                    expandItem(item, false, -1);
                });
            }
            // expand all
            QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                      tr("Expand all"));
            connect(actionExpandAll, &QAction::triggered, this, [=](){
                expandItem(item, true, -1);
            });
        } else {
            // load system
            QAction *actionLoadSystem = menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                                                       tr("Load system"));
            connect(actionLoadSystem, &QAction::triggered, this, [=](){
                loadSystemFunc();
            });
        }
        // export all
        QAction *exportAll = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                            tr("Export all protocol"));
        connect(exportAll, &QAction::triggered, this, [=](){
            exportData(item, true);
        });
        // export exists
        QAction *actionExportExists = menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                                                     tr("Export loaded protocol"));
        connect(actionExportExists, &QAction::triggered, this, [=](){
            exportData(item, false);
        });
        break;
    }}

    menu.exec(QCursor::pos());
}

void JProtoTreeViewPrivate::itemSystemRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    Q_Q(JProtoTreeView);

    //
    if (isEditMode()) {
        if (!item->hasChildren() && !isItemLoaded(item)) {
            if (loadSystem(item, deep, true)) {
                expandItem(item, true, 2);
            }
        }
    }

    QMenu menu(this);
    switch (treeModes_) {
    case JProtoTreeView::TreeModeTableSel:
    {
        if (isItemSelected(item)) {
            // collapse all
            QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                     tr("Collapse all"));
            connect(actionCollapse, &QAction::triggered, this, [=](){
                expandItem(item, false, -1);
            });
        }
        // expand all
        QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                  tr("Expand all"));
        connect(actionExpandAll, &QAction::triggered, this, [=](){
            expandItem(item, true, -1);
        });
        break;
    }
    default:
    {
        auto loadTableFunc = [=](){
            if (loadSystem(item, deep, true)) {
                expandItem(item, true, 2);
            }
        };
        auto unloadTableFunc = [=](){
            unbindChannelItem(item);
            emit itemUnloaded(item, nullptr);
            clearChildren(item);
            //
            if (isEditMode()) {
                setItemLoadStatus(item, false);
                emit q->itemUpdated(item, true, false);
            }
        };
        //
        if (isEditMode()) {
            // add table
            QAction *actionAddTable = menu.addAction(QIcon(":/icdwidget/image/tree/add.png"),
                                                     tr("Add table"));
            connect(actionAddTable, &QAction::triggered, this, [=](){
                emit q->requestAdd(item);
            });
            // clean table
            if (item->hasChildren()) {
                QAction *actionClearTable = menu.addAction(QIcon(":/icdwidget/image/tree/clean.png"),
                                                           tr("Clear table"));
                connect(actionClearTable, &QAction::triggered, this, [=](){
                    cleanItem(item);
                });
            }
            // remove system
            QAction *actionRemoveSystem = menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                                                         tr("Remove system"));
            connect(actionRemoveSystem, &QAction::triggered, this, [=](){
                removeRow(item);
            });
            // insert system [before]
            QAction *actionInsertAbove = menu.addAction(QIcon(":/icdwidget/image/tree/insert-above.png"),
                                                        tr("Insert system above"));
            connect(actionInsertAbove, &QAction::triggered, this, [=](){
                insertNewRow(item, InsertAboveAction);
            });
            // insert system [after]
            QAction *actionInserBelow = menu.addAction(QIcon(":/icdwidget/image/tree/insert-below.png"),
                                                       tr("Insert system below"));
            connect(actionInserBelow, &QAction::triggered, this, [=](){
                insertNewRow(item, InsertBelowAction);
            });
        }
        //
        if (item->hasChildren()) {
            // update table
            QAction *actionUpdateTable = menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                                                        tr("Update table"));
            connect(actionUpdateTable, &QAction::triggered, this, [=](){
                unloadTableFunc();
                loadTableFunc();
            });
            // unload table
            QAction *actionUnloadTable = menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                                                        tr("Unload table"));
            connect(actionUnloadTable, &QAction::triggered, this, [=](){
                unloadTableFunc();
            });
            if (isItemSelected(item)) {
                // collapse all
                QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                         tr("Collapse all"));
                connect(actionCollapse, &QAction::triggered, this, [=](){
                    expandItem(item, false, -1);
                });
            }
            // expand all
            QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                      tr("Expand all"));
            connect(actionExpandAll, &QAction::triggered, this, [=](){
                expandItem(item, true, -1);
            });
        } else {
            QAction *actionLoadTable = menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                                                      tr("Load table"));
            connect(actionLoadTable, &QAction::triggered, this, [=](){
                loadTableFunc();
            });
        }
        // export all
        QAction *actionExportAll = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                                  tr("Export all protocol"));
        connect(actionExportAll, &QAction::triggered, this, [=](){
            exportData(item, true);
        });
        // export exists
        QAction *actionExportExists = menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                                                     tr("Export loaded protocol"));
        connect(actionExportExists, &QAction::triggered, this, [=](){
            exportData(item, false);
        });
        break;
    }}

    menu.exec(QCursor::pos());
}

void JProtoTreeViewPrivate::itemTableRightClicked(QStandardItem *item, int deep)
{
    if (!item) {
        return;
    }

    Q_Q(JProtoTreeView);

    //
    if (isEditMode()) {
        if (!item->hasChildren() && !isItemLoaded(item)) {
            if (loadTable(item, deep, true)) {
                expandItem(item, true, 1);
            }
        }
    }

    QMenu menu(this);
    switch (treeModes_) {
    case JProtoTreeView::TreeModeAnalyse:
    {
        if (item->hasChildren()) {
            // delete item
            QAction *actionDeleteItem = menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                                                       tr("Remove item"));
            connect(actionDeleteItem, &QAction::triggered, this, [=](){
                removeTableItem(item);
            });
            // delete all view
            if (hasItemBound(item)) {
                QAction *actionDeleteAllView = menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                                                              tr("Remove all view"));
                connect(actionDeleteAllView, &QAction::triggered, this, [=](){
                    clearItemBoundRole(item, true);
                });
            }
            if (isItemSelected(item)) {
                // collapse all
                QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                         tr("Collapse all"));
                connect(actionCollapse, &QAction::triggered, this, [=](){
                    expandItem(item, false, -1);
                });
            }
            // expand all
            QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                      tr("Expand all"));
            connect(actionExpandAll, &QAction::triggered, this, [=](){
                expandItem(item, true, -1);
            });
        }
        break;
    }
    case JProtoTreeView::TreeModeTableSel:
    {
        break;
    }
    default:
    {
        auto loadItemFunc = [=](){
            if (loadTable(item, deep, true)) {
                expandItem(item, true, 1);
            }
        };
        auto unloadItemFunc = [=](){
            unbindChannel(item);
            emit itemUnloaded(item, item);
            clearChildren(item);
            //
            if (isEditMode()) {
                setItemLoadStatus(item, false);
                emit q->itemUpdated(item, true, false);
            }
        };
        //
        if (isEditMode()) {
            // add item
            QAction *actionAddItem = menu.addAction(QIcon(":/icdwidget/image/tree/add.png"),
                                                    tr("Add item"));
            createAddItemMenu(item, actionAddItem, AddAction);
            // clean item
            if (item->hasChildren()) {
                QAction *actionClearItem = menu.addAction(QIcon(":/icdwidget/image/tree/clean.png"),
                                                          tr("Clear item"));
                connect(actionClearItem, &QAction::triggered, this, [=](){
                    cleanItem(item);
                });
            }
            // remove table
            QAction *actionRemoveTable = menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                                                        tr("Remove table"));
            connect(actionRemoveTable, &QAction::triggered, this, [=](){
                removeRow(item);
            });
            // insert table [before]
            QAction *actionInsertAbove = menu.addAction(QIcon(":/icdwidget/image/tree/insert-above.png"),
                                                        tr("Insert table above"));
            connect(actionInsertAbove, &QAction::triggered, this, [=](){
                insertNewRow(item, InsertAboveAction);
            });
            // insert table [after]
            QAction *actionInserBelow = menu.addAction(QIcon(":/icdwidget/image/tree/insert-below.png"),
                                                       tr("Insert table below"));
            connect(actionInserBelow, &QAction::triggered, this, [=](){
                insertNewRow(item, InsertBelowAction);
            });
        }
        //
        if (item->hasChildren()) {
            // update item
            QAction *actionUpdateItem = menu.addAction(QIcon(":/icdwidget/image/tree/update.png"),
                                                       tr("Update item"));
            connect(actionUpdateItem, &QAction::triggered, this, [=](){
                unloadItemFunc();
                loadItemFunc();
            });
            // unload item
            QAction *actionUnloadItem = menu.addAction(QIcon(":/icdwidget/image/tree/unload.png"),
                                                       tr("Unload item"));
            connect(actionUnloadItem, &QAction::triggered, this, [=](){
                unloadItemFunc();
            });
            //
            if (item->data(Icd::TreeChannelIdRole).isValid()) {
                // change channel binding
                QAction *actionChangeBinding = menu.addAction(QIcon(":/icdwidget/image/tree/channel.png"),
                                                              tr("Change channel binding"));
                connect(actionChangeBinding, &QAction::triggered, this, [=](){
                    changeChannel(item);
                });
                // unbinding channel
                QAction *actionUnbinding = menu.addAction(QIcon(":/icdwidget/image/tree/disconnect.png"),
                                                          tr("Unbinding channel"));
                connect(actionUnbinding, &QAction::triggered, this, [=](){
                    unbindChannel(item);
                });
                if (treeModes_ & JProtoTreeView::TreeModeMonitor) {
                    // get oldWorker
                    const Icd::WorkerPtr worker = queryWorker(item);
                    if (worker) {
                        if (worker->workerRecv()->isRecording()) {
                            // stop record
                            QAction *actionStopRecord = menu.addAction(QIcon(":/icdwidget/image/tree/record_stop.png"),
                                                                       tr("Stop recording"));
                            connect(actionStopRecord, &QAction::triggered, this, [=](){
                                const Icd::WorkerPtr worker = queryWorker(item);
                                if (worker) {
                                    worker->workerRecv()->stopRecord();
                                }
                            });
                        } else {
                            // start record
                            QAction *actionStartRecord = menu.addAction(QIcon(":/icdwidget/image/tree/record_start.png"),
                                                                        tr("Start recording"));
                            connect(actionStartRecord, &QAction::triggered, this, [=](){
                                const Icd::WorkerPtr worker = queryWorker(item);
                                if (worker) {
                                    worker->workerRecv()->startRecord();
                                }
                            });
                        }
                    }
                }
            } else {
                // binding channel
                QAction *actionBinding = menu.addAction(QIcon(":/icdwidget/image/tree/connect.png"),
                                                        tr("Binding channel"));
                connect(actionBinding, &QAction::triggered, this, [=](){
                    bindChannel(item);
                });
            }
            // delete all view
            if (hasItemBound(item)) {
                QAction *actionDeleteAllView = menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                                                              tr("Remove all view"));
                connect(actionDeleteAllView, &QAction::triggered, this, [=](){
                    clearItemBoundRole(item, true);
                });
            }
            if (isItemSelected(item)) {
                // collapse all
                QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                         tr("Collapse all"));
                connect(actionCollapse, &QAction::triggered, this, [=](){
                    expandItem(item, false, -1);
                });
            }
            // expand all
            QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                      tr("Expand all"));
            connect(actionExpandAll, &QAction::triggered, this, [=](){
                expandItem(item, true, -1);
            });
        } else {
            // load item
            if (!isEditMode() || !isItemLoaded(item)) {
                QAction *actionLoadItem = menu.addAction(QIcon(":/icdwidget/image/tree/load.png"),
                                                         tr("Load item"));
                connect(actionLoadItem, &QAction::triggered, this, [=](){
                    loadItemFunc();
                });
            }
        }
        // export all
        QAction *actionExportAll = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                                  tr("Export all protocol"));
        connect(actionExportAll, &QAction::triggered, this, [=](){
            exportData(item, true);
        });
        // export exists
        QAction *actionExportExists = menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                                                     tr("Export loaded protocol"));
        connect(actionExportExists, &QAction::triggered, this, [=](){
            exportData(item, false);
        });
        break;
    }}

    menu.exec(QCursor::pos());
}

void JProtoTreeViewPrivate::itemDataItemRightClicked(QStandardItem *item, int deep)
{
    Q_UNUSED(deep);
    if (!item) {
        return;
    }

    Q_Q(JProtoTreeView);

    QMenu menu(this);

    // get item type
    int dataType = Icd::ItemInvalid;
    const QVariant dataTypeVar = item->data(Icd::TreeDataTypeRole);
    if (dataTypeVar.isValid()) {
        dataType = dataTypeVar.toInt();
    }

    //
    if (isEditMode()) {
        switch (dataType) {
        case Icd::ItemFrame:
        {
            // add table
            QAction *actionAdd = menu.addAction(QIcon(":/icdwidget/image/tree/add.png"),
                                                tr("Add table"));
            connect(actionAdd, &QAction::triggered, this, [=](){
                emit q->requestAdd(item);
            });
            break;
        }
        case Icd::ItemComplex:
        {
            // add item
            QAction *actionAdd = menu.addAction(QIcon(":/icdwidget/image/tree/add.png"),
                                                tr("Add item"));
            connect(actionAdd, &QAction::triggered, this, [=](){
                emit q->requestAdd(item);
            });
            createAddItemMenu(item, actionAdd, AddAction);
            break;
        }
        default:
            break;
        }
        // remove item
        QAction *actionRemoveItem = menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                                                   tr("Remove item"));
        connect(actionRemoveItem, &QAction::triggered, this, [=](){
            removeRow(item);
        });
        // clean table/item
        if (item->hasChildren() && dataType != Icd::ItemBitMap && dataType != Icd::ItemBitValue) {
            QString targetName = dataType == Icd::ItemFrame ? tr("table") : tr("item");
            QAction *actionClear = menu.addAction(QIcon(":/icdwidget/image/tree/clean.png"),
                                                  tr("Clear %1").arg(targetName));
            connect(actionClear, &QAction::triggered, this, [=](){
                cleanItem(item);
            });
        }
        // insert item [before]
        QAction *actionInsertAbove = menu.addAction(QIcon(":/icdwidget/image/tree/insert-above.png"),
                                                    tr("Insert item above"));
        createAddItemMenu(item, actionInsertAbove, InsertAboveAction);
        // insert item [after]
        QAction *actionInserBelow = menu.addAction(QIcon(":/icdwidget/image/tree/insert-below.png"),
                                                   tr("Insert item below"));
        createAddItemMenu(item, actionInserBelow, InsertBelowAction);
    }
    // bound
    const QVariant varBound = item->data(Icd::TreeBoundRole);
    if (varBound.isValid() && varBound.toBool()) {
        // delete view
        QAction *actionDeleteView = menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                                                   tr("Remove view"));
        actionDeleteView->setToolTip(tr("Remove chart from view with identity"));
        connect(actionDeleteView, &QAction::triggered, this, [=](){
            QStandardItem *itemTable = findItemTable(item);
            emit unbindItem(item, itemTable);
        });
    }
    //
    if (treeModes_ == JProtoTreeView::TreeModeAnalyse) {
        // export
        QAction *actionExport = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                               tr("Export data"));
        connect(actionExport, &QAction::triggered, this, [=](){
            exportTableData(item);
        });
    }
    //
    if (item->hasChildren()) {
        if (isItemSelected(item)) {
            // collapse all
            QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                     tr("Collapse all"));
            connect(actionCollapse, &QAction::triggered, this, [=](){
                expandItem(item, false, -1);
            });
        }
        // expand all
        QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                  tr("Expand all"));
        connect(actionExpandAll, &QAction::triggered, this, [=](){
            expandItem(item, true, -1);
        });

        switch (dataType) {
        case Icd::ItemComplex:
        case Icd::ItemFrame:
            break;
        default:
            break;
        }
    }
    // export all
    QAction *actionExportAll = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                              tr("Export all protocol"));
    connect(actionExportAll, &QAction::triggered, this, [=](){
        exportData(item, true);
    });
    // export exists
    QAction *actionExportExists = menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                                                 tr("Export loaded protocol"));
    connect(actionExportExists, &QAction::triggered, this, [=](){
        exportData(item, false);
    });

    menu.exec(QCursor::pos());
}

void JProtoTreeViewPrivate::itemItemTableRightClicked(QStandardItem *item, int deep)
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

    QMenu menu(this);

    switch (treeModes_) {
    case JProtoTreeView::TreeModeAnalyse:
    default:
        if (treeModes_ != JProtoTreeView::TreeModeEdit) {
            if (bound || item->hasChildren()) {
                // delete view
                QAction *actionDeleteView = menu.addAction(QIcon(":/icdwidget/image/tree/view_off.png"),
                                                           tr("Remove view"));
                actionDeleteView->setToolTip(tr("Remove chart from view with identity"));
                connect(actionDeleteView, &QAction::triggered, this, [=](){
                    QStandardItem *itemTable = findItemTable(item);
                    emit unbindItem(item, itemTable);
                });
            }
        }
        if (treeModes_ == JProtoTreeView::TreeModeAnalyse) {
            QAction *actionExport = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                                   tr("Export data"));
            connect(actionExport, &QAction::triggered, this, [=](){
                exportTableData(item);
            });
        }
        break;
    }
    //
    if (isEditMode()) {
        // add item
        QAction *actionAddItem = menu.addAction(QIcon(":/icdwidget/image/tree/add.png"),
                                                tr("Add item"));
        createAddItemMenu(item, actionAddItem, AddAction);
        // clean item
        if (item->hasChildren()) {
            QAction *actionClearItem = menu.addAction(QIcon(":/icdwidget/image/tree/clean.png"),
                                                      tr("Clear item"));
            connect(actionClearItem, &QAction::triggered, this, [=](){
                cleanItem(item);
            });
        }
        // remove table
        QAction *actionRemoveTable = menu.addAction(QIcon(":/icdwidget/image/tree/remove.png"),
                                                    tr("Remove table"));
        connect(actionRemoveTable, &QAction::triggered, this, [=](){
            removeRow(item);
        });
        // insert table [before]
        QAction *actionInsertAbove = menu.addAction(QIcon(":/icdwidget/image/tree/insert-above.png"),
                                                    tr("Insert table above"));
        connect(actionInsertAbove, &QAction::triggered, this, [=](){
            insertNewRow(item, InsertAboveAction);
        });
        // insert table [after]
        QAction *actionInserBelow = menu.addAction(QIcon(":/icdwidget/image/tree/insert-below.png"),
                                                   tr("Insert table below"));
        connect(actionInserBelow, &QAction::triggered, this, [=](){
            insertNewRow(item, InsertBelowAction);
        });
    }
    //
    if (item->hasChildren()) {
        if (isItemSelected(item)) {
            // collapse all
            QAction *actionCollapse = menu.addAction(QIcon(":/icdwidget/image/tree/collapse.png"),
                                                     tr("Collapse all"));
            connect(actionCollapse, &QAction::triggered, this, [=](){
                expandItem(item, false, -1);
            });
        }
        // expand all
        QAction *actionExpandAll = menu.addAction(QIcon(":/icdwidget/image/tree/expand.png"),
                                                  tr("Expand all"));
        connect(actionExpandAll, &QAction::triggered, this, [=](){
            expandItem(item, true, -1);
        });
    }
    // export all
    QAction *actionExportAll = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                              tr("Export all protocol"));
    connect(actionExportAll, &QAction::triggered, this, [=](){
        exportData(item, true);
    });
    // export exists
    QAction *actionExportExists = menu.addAction(QIcon(":/icdwidget/image/tree/export_exists.png"),
                                                 tr("Export loaded protocol"));
    connect(actionExportExists, &QAction::triggered, this, [=](){
        exportData(item, false);
    });

    //
    menu.exec(QCursor::pos());
}

void JProtoTreeViewPrivate::itemItemBitMapRightClicked(QStandardItem *item, int deep)
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

    //
    QMenu menu(this);
    menu.setProperty("item", qVariantFromValue(static_cast<void*>(item)));
    menu.setProperty("deep", deep);

    //
    switch (treeModes_) {
    case JProtoTreeView::TreeModeAnalyse:
    default:
        if (treeModes_ == JProtoTreeView::TreeModeAnalyse) {
            QAction *actionExport = menu.addAction(QIcon(":/icdwidget/image/tree/export_all.png"),
                                                   tr("Export data"), this, SLOT(onActionExportTableData()));
            connect(actionExport, &QAction::triggered, this, [=](){
                exportTableData(item);
            });
        }
        break;
    }

    //
    menu.exec(QCursor::pos());
}

bool JProtoTreeViewPrivate::changeChannel(QStandardItem *itemTable)
{
    if (!itemTable) {
        return false;   // invalid parameter
    }

    //
    if (!parser_) {
        Q_ASSERT(false);
        return false;   //
    }

    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>(this->itemWidget(itemTable));
    // get oldWorker
    const Icd::WorkerPtr oldWorker = queryWorker(itemTable);
    if (oldWorker == nullptr) {
        return false; // invlaid worker
    }

    //
    Q_Q(JProtoTreeView);
    Icd::JGroupChannelDlg *groupChannelDlg = new Icd::JGroupChannelDlg(q);
    if (groupChannelDlg->exec() != QDialog::Accepted) {
        return false; // cancel
    }

    //
    groupChannelDlg->deleteLater();
    const Icd::WorkerPtr selectedWorker = groupChannelDlg->selectedWorker();
    if (selectedWorker == nullptr) {
        return false; // invlaid worker
    }
    groupChannelDlg = nullptr;
    //
    Icd::TablePtr oldTable;
    if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
        oldTable = oldWorker->workerSend()->table();
    } else if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
        oldTable = oldWorker->workerRecv()->table();
    }
    // restore status
    if (oldTable) {
        restoreChannelItem(itemTable, oldTable);
    }
    //
    if (itemWidget) {
        itemWidget->setWorker(Icd::WorkerPtr());
    }

    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QStringList sections = domain.split('/');
    if (sections.count() != 3) {
        return false;   // invalid domain
    }

    // find table object
    Icd::TablePtr table = Icd::TablePtr();
    auto funcParseTable = [=,&table]() -> bool {
        if (!parser_->parse(sections.at(0).toStdString(), sections.at(1).toStdString(),
                            sections.at(2).toStdString(), table, Icd::ObjectItem)) {
            return false; // parse failure
        }
        return true;
    };

    // set table
    if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
        table = oldWorker->workerSend()->table();
        if (!table) {
            if (!funcParseTable()) {
                return false;
            }
        }
        oldWorker->workerSend()->setTable(Icd::TablePtr());
        selectedWorker->workerSend()->setTable(table);
    } else if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
        table = oldWorker->workerRecv()->table();
        if (!table) {
            if (!funcParseTable()) {
                return false;
            }
        }
        oldWorker->workerRecv()->setTable(Icd::TablePtr());
        selectedWorker->workerRecv()->setTable(table);
    } else if (bindTableTypes_ == JProtoTreeView::BindAllTable) {
        table = oldWorker->workerSend()->table();
        if (!table) {
            table = oldWorker->workerRecv()->table();
            if (!table) {
                if (!funcParseTable()) {
                    return false;
                }
            }
        }
        oldWorker->workerSend()->setTable(Icd::TablePtr());
        oldWorker->workerRecv()->setTable(Icd::TablePtr());
        selectedWorker->workerSend()->setTable(table);
        selectedWorker->workerRecv()->setTable(JHandlePtrCast<Icd::Table>(table->clone()));
    }

    //
    const QString channelId = QString::fromStdString(selectedWorker->channel()->identity());

    //
    itemTable->setData(channelId, Icd::TreeChannelIdRole);
    itemTable->setText(itemTable->text()
                       + QString(" <font color=steelblue size=3>[")
                       + QString::fromStdString(selectedWorker->channel()->name()) + "]</font>");
    if (itemWidget) {
        itemWidget->setWorker(selectedWorker);
    }

    //
    JWorkerGroup *workerGroup = findWorkerGroup(itemTable);
    if (workerGroup) {
        workerGroup->setWorker(selectedWorker);
    }

    emit channelChanged(itemTable, channelId);

    return true;
}

bool JProtoTreeViewPrivate::bindChannel(QStandardItem *itemTable)
{
    if (!itemTable) {
        return false;   // invalid parameter
    }
    //
    if (!parser_) {
        Q_ASSERT(false);
        return false;   //
    }
    //
    Icd::JGroupChannelDlg *groupChannelDlg = new Icd::JGroupChannelDlg(this);
    if (groupChannelDlg->exec() != QDialog::Accepted) {
        return false; // cancel
    }
    //
    Icd::WorkerPtr selectedWorker = groupChannelDlg->selectedWorker();
    if (!selectedWorker) {
        return false; // invlaid worker
    }

    groupChannelDlg->deleteLater();
    groupChannelDlg = nullptr;

    return bindChannel(itemTable, selectedWorker);
}

bool JProtoTreeViewPrivate::bindChannel(QStandardItem *itemTable, const WorkerPtr &worker)
{
    if (!itemTable || !worker) {
        return false;   // invalid parameter
    }
    //
    if (!parser_) {
        Q_ASSERT(false);
        return false;   //
    }
    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QStringList sections = domain.split('/');
    if (sections.count() != 3) {
        return bindChannel(itemTable, worker, nullptr);
    }
    // find table object
    Icd::TablePtr table;
    if (!parser_->parse(sections.at(0).toStdString(), sections.at(1).toStdString(),
                        sections.at(2).toStdString(), table, Icd::ObjectItem)) {
        return bindChannel(itemTable, worker, nullptr);
    }

    return bindChannel(itemTable, worker, table);
}

bool JProtoTreeViewPrivate::bindChannel(QStandardItem *itemTable, const WorkerPtr &worker,
                                        const TablePtr &table)
{
    if (!itemTable || !worker) {
        return false;   // invalid parameter
    }
    //
    if (!parser_) {
        Q_ASSERT(false);
        return false;   //
    }
    //
    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>(this->itemWidget(itemTable));
    // restore status
    if (table) {
        restoreChannelItem(itemTable, table);
    }
    //
    if (itemWidget) {
        itemWidget->setWorker(Icd::WorkerPtr());
    }
    // find table object
    if (!table) {
        return false;
    }
    //
    if (bindTableTypes_ == JProtoTreeView::BindOnlySend) {
        worker->workerSend()->setTable(table);
        if (treeModes_ & JProtoTreeView::TreeModeSimulator) {
            worker->workerSend()->setFrameLoop(true);
        }
    } else if (bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
        worker->workerRecv()->setTable(table);
    } else if (bindTableTypes_ == JProtoTreeView::BindAllTable) {
        worker->workerSend()->setTable(table);
        worker->workerRecv()->setTable(JHandlePtrCast<Icd::Table>(table->clone()));
        if (treeModes_ & JProtoTreeView::TreeModeSimulator) {
            worker->workerSend()->setFrameLoop(true);
        }
    }
    //
    const QString channelId = QString::fromStdString(worker->channel()->identity());
    //
    itemTable->setData(channelId, Icd::TreeChannelIdRole);
    itemTable->setText(itemTable->text()
                       + QString(" <font color=steelblue size=3>[")
                       + QString::fromStdString(worker->channel()->name()) + "]</font>");
    if (itemWidget) {
        itemWidget->setWorker(worker);
    }
    //
    JWorkerGroup *workerGroup = new JWorkerGroup(itemTable, worker, showAttris_, this);
    workerGroup->setBindTableType(bindTableTypes_);
    workerGroups_.insert(itemTable, workerGroup);
    //
    if (bindTableTypes_ & JProtoTreeView::BindOnlySend) {
        if (worker->workerSend()->isRunning()) {
            workerGroup->setTimerInterval(intervalUpdate_);
            workerGroup->updateTimer();
        }
    }
    if (bindTableTypes_ & JProtoTreeView::BindOnlyRecv) {
        if (showAttris_ & (JProtoTreeView::ShowData | JProtoTreeView::ShowValue)) {
            workerGroup->setTimerInterval(intervalUpdate_);
            workerGroup->updateTimer();
        }
    }

    workerGroup->updateItemData();
    //
    emit channelBound(itemTable, channelId);

    return true;
}

bool JProtoTreeViewPrivate::unbindChannel(QStandardItem *itemTable)
{
    if (!itemTable) {
        return false; // invalid parameter
    }

    TableItemWidget *itemWidget = qobject_cast<TableItemWidget *>(this->itemWidget(itemTable));
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
    itemTable->setText(itemTable->text().section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
    if (itemWidget) {
        itemWidget->setWorker(Icd::WorkerPtr());
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

bool JProtoTreeViewPrivate::unbindChannelItem(QStandardItem *item)
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

bool JProtoTreeViewPrivate::unbindChannel(QStandardItem *itemTable, const Icd::WorkerPtr &worker)
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
    itemTable->setText(itemTable->text().section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
    itemTable->setData(QVariant::Invalid, Icd::TreeChannelIdRole);

    //
    updateItemData(itemTable, false);

    //
    clearItemBoundRole(itemTable, false);

    //
    emit channelUnbound(itemTable, QString::fromStdString(worker->channel()->identity()));

    return true;
}

bool JProtoTreeViewPrivate::isBoundChannel(QStandardItem *itemTable) const
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

void JProtoTreeViewPrivate::setValueColor(const QColor &color)
{
    if (color != valueColor_) {
        valueColor_ = color;
        emit valueColorChanged(color);
    }
}

bool JProtoTreeViewPrivate::loadRoot(QStandardItem *itemRoot, int deep, bool force)
{
    if (!parser_ || !itemRoot) {
        return false;
    }

    if (isEditMode()) {
        if (!force && isItemLoaded(itemRoot)) {
            return true;
        }

        setItemLoadStatus(itemRoot, true);
    }

    JAutoCursor cursor(Qt::BusyCursor);

    // get vehicles
    Icd::RootPtr root = findByDomain<Icd::Root>(QString());
    if (!parser_->parse(root, deep)) {
        return false;
    }

    const Icd::VehiclePtrArray &vehicles = root->allVehicle();

    for (Icd::VehiclePtrArray::const_iterator citer = vehicles.cbegin();
         citer != vehicles.cend(); ++citer) {
        insertVehicle(itemRoot->rowCount(), itemRoot, *citer, deep);
    }

    if (isEditMode()) {
        protoRoot_ = root;  // save to local
        //
        Q_Q(JProtoTreeView);
        emit q->itemUpdated(itemRoot, false, false);
    }

    return true;
}

bool JProtoTreeViewPrivate::loadVehicle(QStandardItem *itemVehicle, int deep, bool force)
{
    if (!parser_ || !itemVehicle) {
        return false;
    }

    if (isEditMode()) {
        if (!force && isItemLoaded(itemVehicle)) {
            return true;
        }

        setItemLoadStatus(itemVehicle, true);
    }

    JAutoCursor cursor(Qt::BusyCursor);

    clearItemChildren(itemVehicle);

    // get systems
    const QString domain = itemVehicle->data(Icd::TreeItemDomainRole).toString();
    Icd::VehiclePtr vehicle = findByDomain<Icd::Vehicle>(domain);
    if (!parser_->parse(domain.toStdString(), vehicle, deep)) {
        return false;
    }

    const Icd::SystemPtrArray &systems = vehicle->allSystem();
    if (!loadVehicle(itemVehicle, systems, deep)) {
        return false;
    }

    if (isEditMode()) {
        Q_Q(JProtoTreeView);
        emit q->itemUpdated(itemVehicle, false, false);
    }

    return true;
}

bool JProtoTreeViewPrivate::loadVehicle(QStandardItem *itemVehicle, const SystemPtrArray &systems, int deep)
{
    if (!itemVehicle) {
        return false;
    }

    if (systems.empty()) {
        return true;
    }

    const QString domain = itemVehicle->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemVehicle->data(Icd::TreeItemPathRole).toString();

    for (Icd::SystemPtrArray::const_iterator citer = systems.cbegin();
         citer != systems.cend(); ++citer) {
        insertSystem(itemVehicle->rowCount(), itemVehicle, *citer, domain, path, deep);
    }

    return true;
}

bool JProtoTreeViewPrivate::loadSystem(QStandardItem *itemSystem, int deep, bool force)
{
    if (!parser_ || !itemSystem) {
        return false;
    }

    if (isEditMode()) {
        if (!force && isItemLoaded(itemSystem)) {
            return true;
        }

        setItemLoadStatus(itemSystem, true);
    }

    JAutoCursor cursor(Qt::BusyCursor);

    clearItemChildren(itemSystem);

    const QString domain = itemSystem->data(Icd::TreeItemDomainRole).toString();
    const QString vehicleId = domain.section('/', 0, 0);
    const QString systemId = domain.section('/', 1);

    // get tables
    Icd::SystemPtr system = findByDomain<Icd::System>(domain);
    if (!parser_->parse(vehicleId.toStdString(), systemId.toStdString(), system, deep + 1)) {
        return false;
    }

    const Icd::TablePtrArray &tables = system->allTable();
    if (!loadSystem(itemSystem, tables, deep)) {
        return false;
    }

    if (isEditMode()) {
        Q_Q(JProtoTreeView);
        emit q->itemUpdated(itemSystem, false, false);
    }

    return true;
}

bool JProtoTreeViewPrivate::loadSystem(QStandardItem *itemSystem, const TablePtrArray &tables, int deep)
{
    if (!itemSystem) {
        return false;
    }

    if (tables.empty()) {
        return true;
    }

    const QString domain = itemSystem->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemSystem->data(Icd::TreeItemPathRole).toString();

    for (Icd::TablePtrArray::const_iterator citer = tables.cbegin();
         citer != tables.cend(); ++citer) {
        insertTable(itemSystem->rowCount(), itemSystem, *citer, domain, path, deep);
    }

    return true;
}

bool JProtoTreeViewPrivate::loadTable(QStandardItem *itemTable, int deep, bool force)
{
    if (!parser_ || !itemTable) {
        return false;
    }

    if (isEditMode()) {
        if (!force && isItemLoaded(itemTable)) {
            return true;
        }

        setItemLoadStatus(itemTable, true);
    }

    JAutoCursor cursor(Qt::BusyCursor);

    clearItemChildren(itemTable);

    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QString vehicleId = domain.section('/', 0, 0);
    const QString systemId = domain.section('/', 1, 1);
    const QString tableId = domain.section('/', 2);

    // get items
    Icd::TablePtr table = findByDomain<Icd::Table>(domain);
    if (!parser_->parse(vehicleId.toStdString(), systemId.toStdString(),
                        tableId.toStdString(), table, Icd::ObjectItem)) {
        return false;
    }

    const Icd::ItemPtrArray &dataItems = table->allItem();
    if (!loadTable(this, itemTable, dataItems, deep)) {
        return false;
    }

    if (isEditMode()) {
        Q_Q(JProtoTreeView);
        emit q->itemUpdated(itemTable, false, false);
    }

    return true;
}

QStandardItem *JProtoTreeViewPrivate::insertVehicle(int row, QStandardItem *itemRoot,
                                                    const VehiclePtr &vehicle, int deep)
{
    if (!itemRoot || !vehicle) {
        return nullptr;
    }

    // create item
    JTreeViewItem *itemVehicle = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-vehicle.png"),
                                                   QString(), Icd::TreeItemTypeVehicle);
    // add item
    itemRoot->insertRow(row, itemVehicle);
    // base properties
    updateVehicleData(itemVehicle, vehicle);
    // drag disabled
    itemVehicle->setFlags(itemVehicle->flags() & (~Qt::ItemIsDragEnabled));
    // bind id property
    itemVehicle->setData(QString::fromStdString(vehicle->id()), Icd::TreeItemIdRole);
    // bind domain property
    itemVehicle->setData(QString::fromStdString(vehicle->id()), Icd::TreeItemDomainRole);
    // load systems
    if (deep >= Icd::ObjectSystem) {
        //
        if (isEditMode()) {
            setItemLoadStatus(itemVehicle, true);
        }
        //
        if (!loadVehicle(itemVehicle, vehicle->allSystem(), deep)) {
            return nullptr;   // load failure
        }
    }

    return itemVehicle;
}

QStandardItem *JProtoTreeViewPrivate::insertSystem(int row, QStandardItem *itemVehicle, const SystemPtr &system,
                                                   const QString &domain, const QString &path, int deep)
{
    if (!itemVehicle || !system) {
        return nullptr;
    }

    // create item
    JTreeViewItem *itemSystem = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-system.png"),
                                                  QString(), Icd::TreeItemTypeSystem);
    // add item
    itemVehicle->insertRow(row, itemSystem);
    // base properties
    updateSystemData(itemSystem, system, path);
    // drag disabled
    itemSystem->setFlags(itemSystem->flags() & (~Qt::ItemIsDragEnabled));
    // bind id property
    itemSystem->setData(QString::fromStdString(system->id()), Icd::TreeItemIdRole);
    // bind domain property
    itemSystem->setData(domain + "/" + QString::fromStdString(system->id()),
                        Icd::TreeItemDomainRole);
    // load systems
    if (deep >= Icd::ObjectTable) {
        //
        if (isEditMode()) {
            setItemLoadStatus(itemSystem, true);
        }
        //
        if (!loadSystem(itemSystem, system->allTable(), deep)) {
            return nullptr;   // load failure
        }
    }

    return itemSystem;
}

QStandardItem *JProtoTreeViewPrivate::insertSystem(int row, QStandardItem *itemVehicle,
                                                   const SystemPtr &system, int deep)
{
    if (!itemVehicle) {
        return nullptr;
    }

    const QString domain = itemVehicle->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemVehicle->data(Icd::TreeItemPathRole).toString();

    return insertSystem(row, itemVehicle, system, domain, path, deep);
}

QStandardItem *JProtoTreeViewPrivate::insertTable(int row, QStandardItem *itemSystem, const TablePtr &table,
                                                  const QString &domain, const QString &path, int deep)
{
    if (!itemSystem || !table) {
        return nullptr;
    }

    // create item
    JTreeViewItem *itemTable = new JTreeViewItem(QIcon(QString(":/icdwidget/image/tree/%1.png")
                                                       .arg(isEditMode() ? "circle-gray" : "icon-table")),
                                                 QString(), Icd::TreeItemTypeTable);
    // add item
    itemSystem->insertRow(row, itemTable);
    // base properties
    updateTableData(itemTable, table, path);
    //
    if (bindTableTypes_ == JProtoTreeView::BindOnlySend
            || bindTableTypes_ == JProtoTreeView::BindOnlyRecv) {
        // itemWidget
        TableItemWidget *itemWidget = new TableItemWidget(bindTableTypes_, this);
        setItemWidget(itemTable, itemWidget);
        connect(itemWidget, &TableItemWidget::clicked, this, [=](){
            emit itemClicked(itemTable);
        });
    }
    // bind id property
    itemTable->setData(QString::fromStdString(table->id()), Icd::TreeItemIdRole);
    // bind domain property
    itemTable->setData(domain + '/' + QString::fromStdString(table->id()),
                       Icd::TreeItemDomainRole);
    // load table
    if (deep >= Icd::ObjectItem) {
        //
        if (isEditMode()) {
            setItemLoadStatus(itemTable, true);
        }
        //
        if (!loadTable(this, itemTable, table->allItem(), deep)) {
            return nullptr;   // load failure
        }
    }

    return itemTable;
}

QStandardItem *JProtoTreeViewPrivate::insertTable(int row, QStandardItem *itemSystem,
                                                  const TablePtr &table, int deep)
{
    if (!itemSystem) {
        return nullptr;
    }

    const QString domain = itemSystem->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemSystem->data(Icd::TreeItemPathRole).toString();

    return insertTable(row, itemSystem, table, domain, path, deep);
}

bool JProtoTreeViewPrivate::loadTable(QObject *target, QStandardItem *itemTable,
                                      const Icd::ItemPtrArray &dataItems, int deep)
{
    Q_UNUSED(deep);
    if (!target || !itemTable) {
        return false;
    }

    if (dataItems.empty()) {
        return true;
    }

    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend(); ++citer) {
        const Icd::ItemPtr &dataItem = *citer;
        //
        if (!insertItem(target, itemTable->rowCount(), itemTable, dataItem)) {
            continue;
        }
    }

    return true;
}

QStandardItem *JProtoTreeViewPrivate::loadSystem(QObject *target, QStandardItem *itemDataItem,
                                                 const Icd::TablePtr &table, int index)
{
    if (!target || !itemDataItem || !table) {
        return nullptr;
    }

    JTreeView *treeView = qobject_cast<JTreeView*>(target);
    if (!treeView) {
        return nullptr;
    }

    const QVariant varBindTableTypes = target->property("bindTableTypes");
    if (varBindTableTypes.isValid()) {

    } else {
        //
    }

    //
    const QString domain = itemDataItem->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemDataItem->data(Icd::TreeItemPathRole).toString();
    const quint32 showAttris = target->property("showAttris").toUInt();

    //
    QString text;
    // offset
    if (showAttris & JProtoTreeView::ShowOffset) {
        text.append(JWorkerGroup::generateItemOffset(table, index));
    }
    // name
    const QString name = QString::fromStdString(table->name().empty() ? "?" : table->name());
    text.append(name);
    // type
    if (showAttris & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[TABLE]</font>");
    }
    int itemType = 0;
    if (itemDataItem == treeView->invisibleRootItem()) {
        itemType = Icd::TreeItemTypeTable;
    } else {
        itemType = Icd::TreeItemTypeItemTable;
    }
    // create item
    JTreeViewItem *itemTable = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-table.png"),
                                                 text, itemType);
    // add item
    itemDataItem->appendRow(itemTable);
    //
    if (varBindTableTypes.isValid()) {
        const JProtoTreeView::BindTableTypes bindTableTypes =
                JProtoTreeView::BindTableTypes(varBindTableTypes.toInt());
        if (bindTableTypes == JProtoTreeView::BindOnlySend
                || bindTableTypes == JProtoTreeView::BindOnlyRecv) {
            JProtoTreeViewPrivate *privateWidget = qobject_cast<JProtoTreeViewPrivate*>(treeView);
            if (!treeView) {
                return nullptr;
            }
            // itemWidget
            TableItemWidget *itemWidget = new TableItemWidget(bindTableTypes, privateWidget);
            privateWidget->setItemWidget(itemTable, itemWidget);
            QObject::connect(itemWidget, &TableItemWidget::clicked, target, [=](){
                emit privateWidget->itemClicked(itemTable);
            });
        }
    }
    // set tooltip
    itemTable->setToolTip(QString::fromStdString(table->desc()));
    // bind id property
    itemTable->setData(domain + '/' + QString::fromStdString(table->id()),
                       Icd::TreeItemDomainRole);
    // bind path property
    itemTable->setData(name + '@' + path, Icd::TreeItemPathRole);
    // bind mark property
    itemTable->setData(QString::fromStdString(table->mark()), Icd::TreeItemMarkRole);
    // load items
    const Icd::ItemPtrArray &dataItems = table->allItem();
    for (Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
         citer != dataItems.cend(); ++citer) {
        const Icd::ItemPtr &dataItem = *citer;
        //
        if (!insertItem(target, itemTable->rowCount(), itemTable, dataItem)) {
            continue;
        }
    }

    return itemTable;
}

QStandardItem *JProtoTreeViewPrivate::insertItem(QObject *target, int row, QStandardItem *itemTable,
                                                 const ItemPtr &item)
{
    if (!target || !itemTable || !item) {
        return nullptr;
    }

    //
    const QString domain = itemTable->data(Icd::TreeItemDomainRole).toString();
    const QString path = itemTable->data(Icd::TreeItemPathRole).toString();
    const quint32 treeModes = target->property("treeModes").toUInt();
    const quint32 showAttris = target->property("showAttris").toUInt();

    // name
    const QString name = item->name().empty() ? "?" : QString::fromStdString(item->name());
    QString text = name;
    // offset
    if (showAttris & JProtoTreeView::ShowOffset) {
        text.prepend(JWorkerGroup::generateItemOffset(item));
    }
    // type
    if (showAttris & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[" + QString::fromStdString(
                        item->typeString()).toUpper() + "]</font>");
    }
    // create item
    JTreeViewItem *itemDataItem = new JTreeViewItem(QIcon(":/icdwidget/image/tree/icon-item.png"),
                                                    text, Icd::TreeItemTypeDataItem);
    // add item
    itemTable->insertRow(row, itemDataItem);
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
    // bind mark property
    itemDataItem->setData(QString::fromStdString(item->mark()), Icd::TreeItemMarkRole);
    // load ???
    switch (item->type()) {
    case Icd::ItemBitMap:
        if (treeModes == JProtoTreeView::TreeModeEdit) {
            break;
        }
        if (!loadBitItem(target, itemDataItem, JHandlePtrCast<Icd::BitItem>(item))) {
            // load failure
        }
        break;
    case Icd::ItemFrameCode:
        if (!loadFrameCodeItem(target, itemTable, JHandlePtrCast<Icd::FrameCodeItem>(item))) {
            // load failure
        }
        break;
    case Icd::ItemComplex:
        // icon
        itemDataItem->setIcon(QIcon(":/icdwidget/image/tree/icon-complex.png"));
        if (!loadComplexItem(target, itemDataItem, JHandlePtrCast<Icd::ComplexItem>(item))) {
            // load failure
        }
        break;
    case Icd::ItemFrame:
        if (!loadFrameItem(target, itemDataItem, JHandlePtrCast<Icd::FrameItem>(item))) {
            // load failure
        }
        break;
    default:
        break;
    }

    return itemDataItem;
}

bool JProtoTreeViewPrivate::loadFrameCodeItem(QObject *target, QStandardItem *itemTable,
                                              const Icd::FrameCodeItemPtr &frameCodeItem)
{
    //
    if (!target || !itemTable || !frameCodeItem) {
        return false;
    }

    return true;
}

bool JProtoTreeViewPrivate::loadComplexItem(QObject *target, QStandardItem *itemDataItem,
                                            const Icd::ComplexItemPtr &complexItem)
{
    if (!target || !itemDataItem || !complexItem || !complexItem->table()) {
        return false;
    }

    //
    if (!loadTable(target, itemDataItem, complexItem->table()->allItem(), Icd::ObjectItem)) {
        return false;   // load failure
    }

    return true;
}

bool JProtoTreeViewPrivate::loadFrameItem(QObject *target, QStandardItem *itemDataItem,
                                          const Icd::FrameItemPtr &fameItem)
{
    if (!target || !itemDataItem || !fameItem) {
        return false;
    }

    //
    int index = 0;
    const Icd::TablePtrMap &allTable = fameItem->allTable();
    for (Icd::TablePtrMap::const_iterator citer = allTable.cbegin();
         citer != allTable.cend(); ++citer,++index) {
        const Icd::TablePtr &table = citer->second;
        //
        if (!loadSystem(target, itemDataItem, table, index)) {
            continue;
        }
        //
    }

    return true;
}

bool JProtoTreeViewPrivate::loadBitItem(QObject *target, QStandardItem *itemDataItem, const Icd::BitItemPtr &bitItem)
{
    if (!target || !itemDataItem || !bitItem) {
        return false;
    }

    //
    const QString domain = itemDataItem->data(Icd::TreeItemDomainRole).toString();
    const quint32 showAttris = target->property("showAttris").toUInt();

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
        if (showAttris & JProtoTreeView::ShowOffset) {
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

void JProtoTreeViewPrivate::removeTableItem(QStandardItem *item)
{
    if (!item || item->type() != TreeItemTypeTable) {
        return;
    }

    switch (treeModes_) {
    case JProtoTreeView::TreeModeAnalyse:
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

Icd::WorkerPtr JProtoTreeViewPrivate::queryWorker(const QStandardItem *itemTable) const
{
    if (!itemTable) {
        return Icd::WorkerPtr();
    }

    //
    if (itemTable->type() != Icd::TreeItemTypeTable) {
        return Icd::WorkerPtr();
    }

    //
    const QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        return Icd::WorkerPtr();
    }

    //
    const QString channelId = varChannelId.toString();

    //
    const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return Icd::WorkerPtr();
    }

    return worker;
}

void JProtoTreeViewPrivate::updateVehicleData(QStandardItem *item, const VehiclePtr &vehicle)
{
    if (!item || !vehicle) {
        return;
    }

    // name
    const QString name = (QString::fromStdString(vehicle->name().empty()
                                                 ? "?" : vehicle->name()));
    QString text = name;
    if (showAttris_ & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[VEHICLE]</font>");
    }
    item->setText(text);
    // set tooltip
    item->setToolTip(QString::fromStdString(vehicle->desc()));
    // bind path property
    item->setData(name, Icd::TreeItemPathRole);
    // bind mark property
    item->setData(QString::fromStdString(vehicle->mark()), Icd::TreeItemMarkRole);
}

void JProtoTreeViewPrivate::updateSystemData(QStandardItem *item, const SystemPtr &system,
                                             const QString &path)
{
    if (!item || !system) {
        return;
    }

    // name
    const QString name = QString::fromStdString(system->name().empty() ? "?" : system->name());
    QString text = name;
    if (showAttris_ & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[SYSTEM]</font>");
    }
    item->setText(text);
    // set tooltip
    item->setToolTip(QString::fromStdString(system->desc()));
    // bind path property
    item->setData(name + "@" + path, Icd::TreeItemPathRole);
    // bind mark property
    item->setData(QString::fromStdString(system->mark()), Icd::TreeItemMarkRole);
}

void JProtoTreeViewPrivate::updateSystemData(QStandardItem *item, const SystemPtr &system)
{
    if (!item) {
        return;
    }

    const QString path = item->data(Icd::TreeItemPathRole).toString();

    updateSystemData(item, system, path);
}

void JProtoTreeViewPrivate::updateTableData(QStandardItem *item, const TablePtr &table,
                                            const QString &path)
{
    if (!item || !table) {
        return;
    }

    // name
    const QString name = QString::fromStdString(table->name().empty() ? "?" : table->name());
    //
    QString text = name;
    // type
    if (showAttris_ & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[TABLE]</font>");
    }
    item->setText(text);
    // set tooltip
    item->setToolTip(QString::fromStdString(table->desc()));
    // bind path property
    item->setData(name + '@' + path, Icd::TreeItemPathRole);
    // bind mark property
    item->setData(QString::fromStdString(table->mark()), Icd::TreeItemMarkRole);
}

void JProtoTreeViewPrivate::updateTableData(QStandardItem *item, const TablePtr &table)
{
    if (!item) {
        return;
    }

    const QString path = item->data(Icd::TreeItemPathRole).toString();

    updateTableData(item, table, path);
}

void JProtoTreeViewPrivate::updateItemData(QStandardItem *item)
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
        if (showAttris_ & JProtoTreeView::ShowType) {
            text.append(" <font color=green size=2>[VEHICLE]</font>");
        }
        item->setText(text);
        break;
    }
    case Icd::TreeItemTypeSystem:
    {
        QString text = item->text().remove(QRegExp("<font[^>]*>[\\s\\S]*<\\/font>")).trimmed();
        if (showAttris_ & JProtoTreeView::ShowType) {
            text.append(" <font color=green size=2>[SYSTEM]</font>");
        }
        item->setText(text);
        break;
    }
    case Icd::TreeItemTypeTable:
    {
        JWorkerGroup *workerGroup = findWorkerGroup(item);
        if (workerGroup) {
            workerGroup->updateItemData(showAttris_, dataFormat_);
        } else {
            if (!item->hasChildren()) {
                if (!isBoundChannel(item)) {
                    QString text = item->text().remove(QRegExp("<font[^>]*>[\\s\\S]*<\\/font>")).trimmed();
                    if (showAttris_ & JProtoTreeView::ShowType) {
                        text.append(" <font color=green size=2>[TABLE]</font>");
                    }
                    item->setText(text);
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
            if (!parser_->parse(sections.at(0).toStdString(), sections.at(1).toStdString(),
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

void JProtoTreeViewPrivate::updateBitItemData(QStandardItem *itemDataItem,
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
        if (showAttris_ & JProtoTreeView::ShowOffset) {
            text.append(QString("<font color=green size=2>[%1]</font> ")
                        .arg(bitOffset, 2, 10, QChar('0')));
        }
        // name
        text.append(QString::fromStdString(name));
        //
        childItem->setText(text);
    }
}

void JProtoTreeViewPrivate::updateItemData(QStandardItem *itemDataItem, const Icd::ItemPtr &dataItem)
{
    if (!itemDataItem || !dataItem) {
        return;
    }

    //
    QString text;
    // offset
    if (showAttris_ & JProtoTreeView::ShowOffset) {
        text.append(JWorkerGroup::generateItemOffset(dataItem));
    }
    // name
    text.append(QString::fromStdString(dataItem->name().empty() ? "?" : dataItem->name()));
    // type
    if (showAttris_ & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[" + QString::fromStdString(
                        dataItem->typeString()).toUpper() + "]</font>");
    }

    itemDataItem->setText(text);

    //
    switch (dataItem->type()) {
    case Icd::ItemBitMap:
    {
        //
        const Icd::BitItemPtr itemBit = JHandlePtrCast<Icd::BitItem>(dataItem);
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
        const Icd::ComplexItemPtr itemComplex = JHandlePtrCast<Icd::ComplexItem>(dataItem);
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
        const Icd::FrameItemPtr frame = JHandlePtrCast<Icd::FrameItem>(dataItem);
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

void JProtoTreeViewPrivate::updateItemData(QStandardItem *itemTable, const Icd::ItemPtrArray &dataItems)
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

void JProtoTreeViewPrivate::updateItemData(QStandardItem *itemDataItem, const ComplexItemPtr &complex)
{
    if (!itemDataItem || !complex) {
        return;
    }
    //
    QString text;
    // offset
    if (showAttris_ & JProtoTreeView::ShowOffset) {
        text.append(JWorkerGroup::generateItemOffset(complex));
    }
    // name
    text.append(QString::fromStdString(complex->name().empty() ? "?" : complex->name()));
    // type
    if (showAttris_ & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[COMPLEX]</font>");
    }

    //
    itemDataItem->setText(text);

    //
    updateItemData(itemDataItem, complex->table()->allItem());
}

void JProtoTreeViewPrivate::updateItemData(QStandardItem *itemDataItem, const Icd::FrameItemPtr &frame)
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

void JProtoTreeViewPrivate::updateItemData(QStandardItem *itemTable, const TablePtr &table)
{
    if (!itemTable || !table) {
        return;
    }
    //
    QString text;
    // offset
    if (showAttris_ & JProtoTreeView::ShowOffset) {
        text.append(JWorkerGroup::generateItemOffset(table));
    }
    // name
    text.append(QString::fromStdString(table->name().empty() ? "?" : table->name()));
    // type
    if (showAttris_ & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[TABLE]</font>");
    }
    // fileName
    const QString filePath = itemTable->data(TreeFilePathRole).toString();
    if (!filePath.isEmpty()) {
        text.append(" [").append(QFileInfo(filePath).fileName()).append(']');
    }
    //
    itemTable->setText(text);
    //
    updateItemData(itemTable, table->allItem());
}

void JProtoTreeViewPrivate::updateItemData(QStandardItem *itemTable, bool showValue)
{
    QHash<QStandardItem*, JWorkerGroup*>::ConstIterator citer =
            workerGroups_.find(itemTable);
    if (citer != workerGroups_.cend()) {
        citer.value()->updateItemData(showValue);
    }
}

void JProtoTreeViewPrivate::removeWorkerGroup(QStandardItem *itemTable)
{
    QHash<QStandardItem *, JWorkerGroup*>::Iterator iter =
            workerGroups_.find(itemTable);
    if (iter != workerGroups_.end()) {
        JWorkerGroup *workerGroup = iter.value();
        workerGroup->setDirty();
        workerGroup->deleteLater();
        workerGroups_.erase(iter);
    }
}

void JProtoTreeViewPrivate::removeWorkerGroup(const WorkerPtr &worker)
{
    QHash<QStandardItem *, JWorkerGroup*>::Iterator iter =
            workerGroups_.begin();
    for (; iter != workerGroups_.end(); ++iter) {
        JWorkerGroup *workerGroup = iter.value();
        if (workerGroup->worker() == worker) {
            unbindChannel(iter.key(), worker);
            workerGroup->setDirty();
            workerGroup->deleteLater();
            workerGroups_.erase(iter);
            break;
        }
    }
}

void JProtoTreeViewPrivate::clearWorkerGroup()
{
    QHashIterator<QStandardItem *, JWorkerGroup*> citer(workerGroups_);
    while (citer.hasNext()) {
        citer.next();
        JWorkerGroup *workerGroup = citer.value();
        workerGroup->setDirty();
        workerGroup->deleteLater();
        unbindChannel(citer.key(), workerGroup->worker());
    }
    workerGroups_.clear();
}

JWorkerGroup *JProtoTreeViewPrivate::findWorkerGroup(QStandardItem *itemTable) const
{
    QHash<QStandardItem *, JWorkerGroup*>::ConstIterator citer =
            workerGroups_.find(itemTable);
    if (citer != workerGroups_.cend()) {
        return citer.value();
    }

    return nullptr;
}

bool JProtoTreeViewPrivate::hasItemBound(QStandardItem *item)
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

void JProtoTreeViewPrivate::clearItemBoundRole(QStandardItem *item, bool bEmit)
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

void JProtoTreeViewPrivate::clearItemChildren(QStandardItem *item)
{
    if (!item) {
        return;
    }

    clearChildren(item);

    const QVariant varDomain = item->data(Icd::TreeItemDomainRole);
    if (varDomain.isValid()) {
        Icd::ObjectPtr object = findByDomain(varDomain.toString());
        if (object) {
            object->clearChildren();
        }
    }
}

void JProtoTreeViewPrivate::setItemLoadStatus(QStandardItem *item, bool loaded)
{
    if (!item) {
        return;
    }

    item->setData(loaded, Icd::TreeLoadStatusRole);

    if (item->type() == Icd::TreeItemTypeTable) {
        item->setIcon(QIcon(QString(":icdwidget/image/tree/circle-%1.png")
                            .arg(loaded ? "green" : "gray")));
    }
}

QStandardItem *JProtoTreeViewPrivate::findItemTable(QStandardItem *item, const QString &filePath) const
{
    if (!item) {
        return nullptr;
    }

    const int itemType = item->type();
    if (itemType == TreeItemTypeTable) {
        const QString _filePath = item->data(TreeItemPathRole).toString();
        if (_filePath == filePath) {
            return item;
        }
    } else if (itemType > TreeItemTypeTable) {
        return nullptr;
    }

    int rowCount = item->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem *childItem = item->child(i);
        childItem = findItemTable(childItem, filePath);
        if (childItem) {
            return childItem;
        }
    }

    return nullptr;
}

QStandardItem *JProtoTreeViewPrivate::findItemTable(QStandardItem *item) const
{
    if (!item) {
        return nullptr;
    }

    const int itemType = item->type();
    if (itemType == Icd::TreeItemTypeTable) {
        return item;
    } else if (itemType > Icd::TreeItemTypeTable) {
        return findItemTable(item->parent());
    }

    return nullptr;
}

bool JProtoTreeViewPrivate::exportData(const QStandardItem *item, bool exportAll)
{
    if (!item || !parser_) {
        return false;
    }

    //
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
    case TreeItemTypeItemTable:
    case TreeItemTypeDataItem:
        break;
    default:
        return false;
    }

    if (filters.isEmpty()) {
        return false;
    }

    Q_Q(JProtoTreeView);
    const QString filePath = QFileDialog::getSaveFileName(
                q, tr("Select export file path"), QApplication::applicationDirPath(), filters.join(";;"));
    if (filePath.isEmpty()) {
        return false;
    }

    //
    Icd::JProgressDialog *progressDialog = new Icd::JProgressDialog(this);
    progressDialog->setWindowTitle(tr("Export protocol as document"));
    progressDialog->setProgressValue(100);
    //progressDialog->show();

    //
    QTimer *timerExport = new QTimer(this);

    //
    QFuture<bool> future = QtConcurrent::run([=](){
        return parser_->saveAs(item, exportAll,
                               (bindTableTypes_ & JProtoTreeView::BindOnlyRecv),
                               filePath.toStdString());
    });

    //
    auto deleteRes = [=](){
        progressDialog->hide();
        progressDialog->disconnect(this);
        timerExport->disconnect(this);
        progressDialog->deleteLater();
        timerExport->deleteLater();
        //this->setEnabled(true);
        //d_searchEdit->setEnabled(true);
    };

    connect(progressDialog, &JProgressDialog::accepted, this, [=](){
        //
        QDesktopServices::openUrl(QUrl(filePath));
        //
        deleteRes();
    });
    connect(progressDialog, &JProgressDialog::rejected, this, [=](){
        parser_->cancelSaveAs(true);
        progressDialog->waitForFinished();
        timerExport->stop();
        QApplication::removePostedEvents(this, QEvent::Timer);
        deleteRes();
    });
    connect(progressDialog, &JProgressDialog::finished, this, [=](){
        timerExport->stop();
        QApplication::removePostedEvents(this, QEvent::Timer);
        if (progressDialog->futureResult()) {
            progressDialog->setProgressVisible(false);
            const QString message = tr("Export success, open it now?\n%1");
            progressDialog->setMessage(message.arg(filePath));
            progressDialog->setAcceptText(tr("Yes"));
            progressDialog->setCancelText(tr("No"));
            progressDialog->setAcceptVisible(true);
        } else {
            progressDialog->hide();
            QMessageBox::information(q, tr("Process result"), tr("Export failure!"));
            deleteRes();
        }
    });
    connect(timerExport, &QTimer::timeout, this, [=](){
        progressDialog->setMessage(QString::fromStdString(parser_->message()));
        //progressDialog->setProgressValue(d_parser->progressValue());
    });

    progressDialog->setFuture(future);
    timerExport->start(300);

    //this->setEnabled(false);
    //d_searchEdit->setEnabled(false);

    progressDialog->exec();

    return true;
}

QStandardItem *JProtoTreeViewPrivate::findTableItem(QStandardItem *itemParent,
                                                    const QString &domain)
{
    if (!itemParent) {
        return nullptr;
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

    return nullptr;
}

void JProtoTreeViewPrivate::findAllTableItem(QList<QStandardItem *> &items,
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

void JProtoTreeViewPrivate::procAllTableItem(QStandardItem *itemParent, bool binding,
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

BindingData JProtoTreeViewPrivate::bindingMapTask(BindingData data)
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
    if (!data.d->parser_->parse(sections.at(0).toStdString(),
                                sections.at(1).toStdString(),
                                sections.at(2).toStdString(),
                                data.table, Icd::ObjectItem)) {
        return data;
    }

    return data;
}

void JProtoTreeViewPrivate::bindingMapReduce(int &count, const BindingData &data)
{
    Q_UNUSED(data);
    ++count;
}

void JProtoTreeViewPrivate::restoreChannelItem(QStandardItem *itemTable, const Icd::TablePtr &table)
{
    if (!itemTable || !table) {
        return;
    }
    //
    itemTable->setData(QVariant::Invalid, Icd::TreeChannelIdRole);
    //
    QString text;
    // name
    const QString name = QString::fromStdString(table->name().empty() ? "?" : table->name());
    text.append(name);
    // type
    if (showAttris_ & JProtoTreeView::ShowType) {
        text.append(" <font color=green size=2>[TABLE]</font>");
    }
    itemTable->setText(text);
}

QStandardItem *JProtoTreeViewPrivate::itemFromAction(QObject *action) const
{
    if (!action) {
        return nullptr;
    }

    QAction *_action = qobject_cast<QAction*>(action);
    if (!_action) {
        return nullptr;
    }

    QMenu *menu = qobject_cast<QMenu *>(_action->parentWidget());
    if (!menu) {
        return nullptr;
    }

    const QVariant varItem = menu->property("item");
    if (!varItem.isValid()) {
        return nullptr;
    }

    return jVariantFromVoid<QStandardItem>(varItem);
}

QVariant JProtoTreeViewPrivate::varFromAction(QObject *action, const char *name) const
{
    if (!action) {
        return QVariant::Invalid;
    }

    QAction *_action = qobject_cast<QAction*>(action);
    if (!_action) {
        return QVariant::Invalid;
    }

    QMenu *menu = qobject_cast<QMenu *>(_action->parentWidget());
    if (!menu) {
        return QVariant::Invalid;
    }

    return menu->property(name);
}

QStandardItem *JProtoTreeViewPrivate::findItemByDomain(QStandardItem *parentItem,
                                                       const QString &domain, int domainType)
{
    if (domain.isEmpty()) {
        return nullptr;
    }

    switch (domainType) {
    case Icd::DomainId:
    case Icd::DomainName:
        break;
    case Icd::DomainMark:
    default: return nullptr;
    }

    if (!parentItem) {
        parentItem = invisibleRootItem();
    }

    while (parentItem && parentItem->type() <= Icd::TreeItemTypeRoot) {
        if (parentItem->rowCount() <= 0) {
            return nullptr;
        }
        parentItem = parentItem->child(0);
    }

    if (!parentItem) {
        return nullptr;
    }

    if (!parentItem || parentItem == invisibleRootItem()) {
        parentItem = invisibleRootItem();
        const int childCount = parentItem->rowCount();
        for (int i = 0; i < childCount; ++i) {
            auto childItem = findItemByDomain(parentItem->child(i), domain, domainType);
            if (childItem) {
                return childItem;
            }
        }
    } else {
        const QString currentSection = domain.section('/', 0, 0, QString::SectionSkipEmpty).trimmed();
        if (currentSection.isEmpty()) {
            return nullptr;
        }

        QString itemSection;
        switch (domainType) {
        case Icd::DomainId:
            itemSection = parentItem->data(Icd::TreeItemIdRole).toString();
            break;
        case Icd::DomainName:
            itemSection = JTreeItemDelegate::simplified(parentItem->text())
                    .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed();
            break;
        case Icd::DomainMark:
        default: return nullptr;
        }

        if (itemSection != currentSection) {
            return nullptr;
        }

        const QString nextSection = domain.section('/', 1, -1, QString::SectionSkipEmpty).trimmed();
        if (nextSection.isEmpty()) {
            return parentItem;
        }

        const int childCount = parentItem->rowCount();
        for (int i = 0; i < childCount; ++i) {
            auto childItem = findItemByDomain(parentItem->child(i), nextSection, domainType);
            if (childItem) {
                return childItem;
            }
        }
    }

    return nullptr;
}

void JProtoTreeViewPrivate::selectItem(const QString &domain, int domainType)
{
    clearSelection();

    auto item = findItemByDomain(nullptr, domain, domainType);
    if (!item) {
        return;
    }

    setCurrentItem(item);
}

QString JProtoTreeViewPrivate::itemDomain(QStandardItem *item, int domainType) const
{
    if (!item || item->type() == Icd::TreeItemTypeRoot) {
        return QString();
    }

    switch (domainType) {
    case Icd::DomainId:
    case Icd::DomainName:
        break;
    case Icd::DomainMark:
    default: return QString();
    }

    QString domain;

    while (item && item->type() > Icd::TreeItemTypeRoot) {
        if (!domain.isEmpty()) {
            domain.prepend('/');
        }
        switch (domainType) {
        case Icd::DomainId:
            domain.prepend(item->data(Icd::TreeItemIdRole).toString());
            break;
        case Icd::DomainName:
            domain.prepend(JTreeItemDelegate::simplified(item->text())
                           .section(" [", 0, 0, QString::SectionSkipEmpty).trimmed());
            break;
        default:
            return QString();
        }
        item = item->parent();
    }

    return domain;
}

Icd::ObjectPtr JProtoTreeViewPrivate::findByDomain(const QString &domain) const
{
    if (domain.isEmpty()) {
        return protoRoot_;
    }

    if (protoRoot_) {
        return protoRoot_->findByDomain(domain.toStdString());
    }

    return Icd::ObjectPtr();
}

QString JProtoTreeViewPrivate::idDomain(QStandardItem *item)
{
    if (!item) {
        return QString();
    }

    QStringList ids;
    while (item) {
        const QVariant varId = item->data(Icd::TreeItemIdRole);
        if (!varId.isValid()) {
            break;
        }
        ids.prepend(varId.toString());
        item = item->parent();
    }

    return ids.join('/');
}

QString JProtoTreeViewPrivate::markDomain(QStandardItem *item)
{
    if (!item) {
        return QString();
    }

    QStringList marks;
    while (item) {
        const QVariant varMark = item->data(Icd::TreeItemMarkRole);
        if (!varMark.isValid()) {
            break;
        }
        marks.prepend(varMark.toString());
        item = item->parent();
    }

    return marks.join('/');
}

bool JProtoTreeViewPrivate::loadSystem(JTreeView *treeView, QStandardItem *itemParent,
                                       const Icd::TablePtr &table, int deep)
{
    if (!treeView || !itemParent || !table) {
        return false;
    }

    const Icd::ItemPtrArray &items = table->allItem();
    if (!loadTable(treeView, itemParent, items, deep)) {
        return false;
    }

    return true;
}

QMenu *JProtoTreeViewPrivate::createAddItemMenu(QStandardItem *item, QAction *action,
                                                EditAction editAction)
{
    if (!item) {
        return nullptr;
    }

    Q_Q(JProtoTreeView);

    // create menu
    QMenu *menu = new QMenu(this);
    const auto items = IcdWidget::protoItemMapTypes();
    for (auto citer = items.constBegin(); citer != items.constEnd(); ++citer) {
        const QString icon = ":/icdwidget/image/tree/item-"
                + QString::fromStdString(Icd::Item::typeString(Icd::ItemType(citer.key())))
                + ".png";
        QAction *newAction = menu->addAction(QIcon(icon), citer.value());
        newAction->setData(citer.key());
    }

    // event
    action->setMenu(menu);
    connect(menu, &QMenu::triggered, this, [=](QAction *action){
        switch (editAction) {
        case InsertAboveAction:
        case InsertBelowAction:
            insertNewRow(item, editAction, action->data());
            break;
        default:
            emit q->requestAdd(item, action->data());
            break;
        }
    });

    return menu;
}

void JProtoTreeViewPrivate::insertRow(int row, const ObjectPtr &target, const QVariant &data)
{
    Q_UNUSED(data);

    if (!target) {
        return;
    }

    QStandardItem *parentItem = this->currentItem();
    if (!parentItem) {
        return;
    }

    switch (target->objectType()) {
    case Icd::ObjectVehicle:
    {
        auto vehicle = JHandlePtrCast<Icd::Vehicle>(target);
        if (!vehicle) {
            break;
        }
        insertVehicle(row, parentItem, vehicle, Icd::ObjectSystem);
        break;
    }
    case Icd::ObjectSystem:
    {
        auto system = JHandlePtrCast<Icd::System>(target);
        if (!system) {
            break;
        }
        insertSystem(row, parentItem, system, Icd::ObjectTable);
        break;
    }
    case Icd::ObjectTable:
    {
        auto table = JHandlePtrCast<Icd::Table>(target);
        if (!table) {
            break;
        }
        insertTable(row, parentItem, table, Icd::ObjectItem);
        break;
    }
    case Icd::ObjectItem:
    {
        auto item = JHandlePtrCast<Icd::Item>(target);
        if (!item) {
            break;
        }
        insertItem(this, row, parentItem, item);
        break;
    }
    default:
        break;
    }
}

void JProtoTreeViewPrivate::updateRow(int row, const ObjectPtr &target, const QVariant &data)
{
    Q_UNUSED(data);

    if (!target) {
        return;
    }

    QStandardItem *targetItem = this->currentItem();
    if (row >= 0) {
        targetItem = this->currentItem();
        if (targetItem) {
            targetItem = targetItem->child(row);
        }
    }

    if (!targetItem) {
        return;
    }

    switch (target->objectType()) {
    case Icd::ObjectVehicle:
    {
        auto vehicle = JHandlePtrCast<Icd::Vehicle>(target);
        if (!vehicle) {
            break;
        }
        updateVehicleData(targetItem, vehicle);
        break;
    }
    case Icd::ObjectSystem:
    {
        auto system = JHandlePtrCast<Icd::System>(target);
        if (!system) {
            break;
        }
        updateSystemData(targetItem, system);
        break;
    }
    case Icd::ObjectTable:
    {
        auto table = JHandlePtrCast<Icd::Table>(target);
        if (!table) {
            break;
        }
        updateTableData(targetItem, table);
        break;
    }
    case Icd::ObjectItem:
    {
        auto item = JHandlePtrCast<Icd::Item>(target);
        if (!item) {
            break;
        }
        updateItemData(targetItem, item);
        break;
    }
    default:
        break;
    }
}

void JProtoTreeViewPrivate::removeRow(int row, const ObjectPtr &target, const QVariant &data)
{
    Q_UNUSED(row);
    Q_UNUSED(target);
    Q_UNUSED(data);
}

void JProtoTreeViewPrivate::insertNewRow(QStandardItem *item, int editAction, const QVariant &data)
{
    Q_UNUSED(data);

    newItem_ = nullptr;
    newObject_.reset();

    if (!item) {
        return;
    }

    const Icd::ObjectPtr object = findObject(item);
    if (!object) {
        return;
    }

    QStandardItem *parentItem = item->parent();
    if (!parentItem) {
        return;
    }

    int row = -1;

    switch (editAction) {
    case InsertAboveAction: row = item->row(); break;
    case InsertBelowAction: row = item->row() + 1; break;
    default: return;
    }

    switch (item->type()) {
    case Icd::TreeItemTypeVehicle:
    {
        auto newVehicle = std::make_shared<Icd::Vehicle>(object->parent());
        newObject_ = newVehicle;
        newItem_ = insertVehicle(row, parentItem, newVehicle, Icd::ObjectSystem);
        break;
    }
    case Icd::TreeItemTypeSystem:
    {
        auto newSystem = std::make_shared<Icd::System>(object->parent());
        newObject_ = newSystem;
        newItem_ = insertSystem(row, parentItem, newSystem, Icd::ObjectTable);
        break;
    }
    case Icd::TreeItemTypeTable:
    {
        auto newTable = std::make_shared<Icd::Table>(object->parent());
        newObject_ = newTable;
        newItem_ = insertTable(row, parentItem, newTable, Icd::ObjectItem);
        break;
    }
    case Icd::TreeItemTypeDataItem:
    {
        auto newItem = Icd::Item::create(Icd::ItemType(data.toInt()));
        newObject_ = newItem;
        newItem_ = insertItem(this, row, parentItem, newItem);
        break;
    }
    default:
        break;
    }

    if (newItem_) {
        newItem_->setData(true, Icd::TreeItemNewRole);
        setCurrentItem(newItem_);
    }
}

void JProtoTreeViewPrivate::applyInsert(const Icd::ObjectPtr &target)
{
    if (!newItem_ || !newObject_ || !target) {
        return;
    }

    updateRow(-1, target);

    newItem_->setData(QVariant::Invalid, Icd::TreeItemNewRole);
    newItem_ = nullptr;
    newObject_.reset();
}

void JProtoTreeViewPrivate::cancelInsert()
{
    if (!newItem_ || !newObject_) {
        return;
    }

    newItem_->setData(QVariant::Invalid, Icd::TreeItemNewRole);

    if (newItem_->parent()) {
        newItem_->parent()->removeRow(newItem_->row());
    }

    newItem_ = nullptr;
    newObject_.reset();
}

void JProtoTreeViewPrivate::removeRow(QStandardItem *item)
{
    if (!item || item == newItem_) {
        return;
    }

    setCurrentItem(nullptr);

    auto object = findByDomain(item->data(Icd::TreeItemDomainRole).toString());
    if (object && object->parent()) {
        auto parent = object->parent();
        if (parent->rtti() == Icd::ObjectFrame) {
            if (object->rtti() != Icd::ObjectTable) {
                return;
            }
            auto table = dynamic_cast<Icd::Table*>(parent);
            if (!table) {
                return;
            }
            parent->removeChild(table->frameCode());
        } else {
            parent->removeChild(Icd::icd_uint64(item->row()));
        }
    }

    Q_Q(JProtoTreeView);
    emit q->itemUpdated(item, false, true);

    QStandardItem *parent = item->parent();
    if (parent) {
        parent->removeRow(item->row());
    }
}

void JProtoTreeViewPrivate::cleanItem(QStandardItem *item)
{
    if (!item || !item->hasChildren() || item == newItem_) {
        return;
    }

    clearChildren(item);

    auto object = findByDomain(item->data(Icd::TreeItemDomainRole).toString());
    if (object) {
        object->clearChildren();
    }

    Q_Q(JProtoTreeView);
    emit q->itemUpdated(item, true, false);
}

} // end of namespace Icd

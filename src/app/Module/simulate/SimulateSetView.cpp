#include "precomp.h"
#include "SimulateSetView.h"
#include "module/icdtab_widget.h"
#include "module/datatable_widget.h"

namespace Simulate {

SetView::SetView(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);
    vertLayoutMain->setSpacing(0);

    d_tabVehicles = new IcdTabWidget(this);
    d_tabVehicles->setTabPosition(QTabWidget::North);
    d_tabVehicles->setDocumentMode(true);
    d_tabVehicles->setDocumentMode(true);
    d_tabVehicles->setTabsClosable(true);
    vertLayoutMain->addWidget(d_tabVehicles);

    connect(this, SIGNAL(dropTriggled(QStandardItem*,QStandardItem*,QString)),
            this, SLOT(onDropTriggled(QStandardItem*,QStandardItem*,QString)),
            Qt::QueuedConnection);

    jnotify->on("simulate.tree.item.clicked", this, [=](JNEvent &event){
        QStandardItem *item = jVariantFromVoid<QStandardItem>(event.argument());
        if (!item) {
            return;
        }
        const int itemType = item->type();
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        switch (itemType) {
        case Icd::TreeItemTypeRoot:
            break;
        case Icd::TreeItemTypeVehicle:
        case Icd::TreeItemTypeSystem:
        case Icd::TreeItemTypeTable:
        case Icd::TreeItemTypeDataItem:
        case Icd::TreeItemTypeItemTable:
            focusItem(domain);
            break;
        default:
            break;
        }
    });
    jnotify->on("simulate.tree.item.unloaded", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const int itemType = item->type();
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        switch (itemType) {
        case Icd::TreeItemTypeRoot:
            break;
        case Icd::TreeItemTypeVehicle:
        case Icd::TreeItemTypeSystem:
        case Icd::TreeItemTypeTable:
        case Icd::TreeItemTypeDataItem:
        case Icd::TreeItemTypeItemTable:
            removeItem(domain);
            break;
        default:
            break;
        }
    });
    jnotify->on("simulate.tree.channel.unbound", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        removeItem(domain);
    });
    jnotify->on("simulate.tree.channel.changed", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const QString channelId = args.at(1).toString();
        const Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                ->workerByChannelIdentity(channelId.toStdString());
        if (!worker) {
            return;
        }
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        changeWorker(domain.section('/', 0, 0).trimmed(),
                     domain.section('/', 1, 1).trimmed(),
                     domain.section('/', 2, 2).trimmed(),
                     worker);
    });
    jnotify->on("simulate.tree.item.unbind", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }
        const QString domain = item->data(Icd::TreeItemDomainRole).toString();
        removeItem(domain);
        item->setData(QVariant(), Icd::TreeBoundRole);
    });
    jnotify->on("simulate.toolbar.set.clean", this, [=](JNEvent &){
        d_tabVehicles->clear();
    });
}

SetView::~SetView()
{

}

bool SetView::init()
{
    bool result = true;

    return result;
}

void SetView::onDropTriggled(QStandardItem *itemTable, QStandardItem *item,
                             const QString &domain)
{
    if (!itemTable || !item) {
        return;
    }

    //
    QVariant varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        //
        bool result = false;
        if (!QMetaObject::invokeMethod(
                    itemTable->model()->parent(), "bindChannel", Qt::DirectConnection,
                    Q_RETURN_ARG(bool, result),
                    Q_ARG(QStandardItem *, itemTable))) {
            return;
        }
        if (!result) {
            return;
        }
    }

    //
    varChannelId = itemTable->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        return;     //
    }

    //
    const QString channelId = varChannelId.toString();

    // get worker object
    Icd::WorkerPtr worker =
            Icd::WorkerPool::getInstance()->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return;
    }

    //
    const Icd::TablePtr &tableSend = worker->workerSend()->table();
    if (!tableSend) {
        qWarning(QStringLiteral("工作组没有绑定发送端协议表 [通道：%1]！")
                 .arg(channelId).toLocal8Bit());
        return;
    }

    //
    switch (item->type()) {
    case Icd::TreeItemTypeTable:
        break;
    case Icd::TreeItemTypeDataItem:
        break;
    case Icd::TreeItemTypeItemTable:
        break;
    default:
        return; // cannot drop
    }

    //
    QApplication::processEvents(QEventLoop::EventLoopExec);

    //
    if (!addDataItem(domain, worker, tableSend, itemTable)) {
        return;
    }
}

void SetView::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.save();
    QFont font = this->font();
    font.setPointSize(24);
    painter.setFont(font);
    painter.setPen(QColor(128, 128, 80, 150));
    QTextOption textOption(Qt::AlignCenter);
    painter.drawText(rect(), tr("Drag and drog left tree node to here"), textOption);
    painter.restore();
}

void SetView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/simulator")) {
        event->accept();
    } else {
        event->ignore();
    }
}

void SetView::dropEvent(QDropEvent *event)
{
    //
    if (!event->mimeData()->hasFormat("icd/table-drag/simulator")) {
        return;
    }

    //
    QStandardItem *itemTable = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("itemTable").value<void *>());
    if (!itemTable) {
        return;
    }

    //
    QStandardItem *item = reinterpret_cast<QStandardItem *>
            (event->mimeData()->property("item").value<void *>());
    if (!item) {
        return;
    }

    //
    const QString domain = event->mimeData()->property("domain").toString();

    //
    emit dropTriggled(itemTable, item, domain);
}

void SetView::changeWorker(const QString &vehicleId, const QString &systemId,
                           const QString &tableId, const Icd::WorkerPtr &worker)
{
    DataTableWidget *widgetTable = findTabTable(vehicleId, systemId, tableId);
    if (!widgetTable) {
        return;
    }

    //
    widgetTable->setWorker(worker);
}

IcdTabWidget *SetView::findTabVehicle(const QString &vehicleId)
{
    return qobject_cast<IcdTabWidget *>(d_tabVehicles->findTab(vehicleId));
}

IcdTabWidget *SetView::findTabSystem(const QString &vehicleId, const QString &systemId)
{
    //
    IcdTabWidget *tabVehicle = findTabVehicle(vehicleId);
    if (!tabVehicle) {
        return 0;
    }

    //
    return qobject_cast<IcdTabWidget *>(tabVehicle->findTab(systemId));
}

DataTableWidget *SetView::findTabTable(const QString &vehicleId, const QString &systemId,
                                       const QString &tableId)
{
    //
    IcdTabWidget *tabSystem = findTabSystem(vehicleId, systemId);
    if (!tabSystem) {
        return 0;
    }

    //
    return qobject_cast<DataTableWidget *>(tabSystem->findTab(tableId));
}

IcdTabWidget *SetView::addTabVehicle(const QString &domain)
{
    // find
    IcdTabWidget *tabVehicle = findTabVehicle(domain);

    const Icd::ParserPtr parser = JMain::instance()->parser("simulate");
    if (!parser) {
        d_tabVehicles->removeTabById(domain);
        delete tabVehicle;
        Q_ASSERT(false);
        return nullptr;
    }

    //
    Icd::VehiclePtr vehicle;
    if (!parser->parse(domain.toStdString(), vehicle, Icd::ObjectVehicle)) {
        return nullptr;
    }

    //
    if (tabVehicle) {
        d_tabVehicles->setTabText(d_tabVehicles->indexOf(tabVehicle), QStringLiteral("分机：")
                                  + QString::fromStdString(vehicle->name()));
    } else {
        // create
        tabVehicle = new IcdTabWidget(d_tabVehicles);
        tabVehicle->setTabPosition(QTabWidget::South);
        tabVehicle->setTabsClosable(true);
        // add
        d_tabVehicles->addTab(tabVehicle, QStringLiteral("分机：")
                              + QString::fromStdString(vehicle->name()));
        d_tabVehicles->tabBar()->setTabData(d_tabVehicles->count() - 1, domain);
    }

    // focus
    d_tabVehicles->setCurrentWidget(tabVehicle);

    return tabVehicle;
}

IcdTabWidget *SetView::addTabSystem(const QString &domain)
{
    //
    const QString vehicleId = domain.section('/', 0, 0);

    //
    IcdTabWidget *tabVehicle = addTabVehicle(vehicleId);
    if (tabVehicle == 0) {
        return nullptr;
    }

    //
    const QString systemId = domain.section('/', 1, 1);

    //
    IcdTabWidget *tabSystem =
            qobject_cast<IcdTabWidget *>(tabVehicle->findTab(systemId));

    // parse vehicle object
    const Icd::ParserPtr parser = JMain::instance()->parser("simulate");
    if (!parser) {
        Q_ASSERT(false);
        return nullptr;
    }
    Icd::SystemPtr system;
    if (!parser->parse(vehicleId.toStdString(), systemId.toStdString(),
                       system, Icd::ObjectSystem)) {
        return nullptr;
    }

    //
    if (tabSystem) {
        tabVehicle->setTabText(tabVehicle->indexOf(tabSystem), QStringLiteral("系统：")
                               + QString::fromStdString(system->name()));
    } else {
        // create
        tabSystem = new IcdTabWidget(tabVehicle);
        // add
        tabVehicle->addTab(tabSystem, QStringLiteral("系统：")
                           + QString::fromStdString(system->name()));
        tabVehicle->tabBar()->setTabData(tabVehicle->count() - 1, systemId);
    }

    // focus
    tabVehicle->setCurrentWidget(tabSystem);

    return tabSystem;
}

bool SetView::addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                          const Icd::ObjectPtr &object, QStandardItem *itemTable)
{
    if (domain.isEmpty() || !worker || !object || !itemTable) {
        return false;
    }

    //
    IcdTabWidget *tabSystem = addTabSystem(domain.section('/', 0, 1));
    if (!tabSystem) {
        return false;
    }

    //
    const QString section = domain.section('/', 1);
    if (section.isEmpty()) {
        return true;
    }

    //
    if (!tabSystem->addDataItem(section, worker, object, itemTable)) {
        return false;
    }

    return true;
}

int SetView::focusItem(const QString &domain)
{
    return d_tabVehicles->focusItem(domain);
}

void SetView::removeItem(const QString &domain)
{
    d_tabVehicles->removeItem(domain);
}

}

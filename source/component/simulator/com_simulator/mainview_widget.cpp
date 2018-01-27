#include "precomp.h"
#include "mainview_widget.h"
#include "module/icdtab_widget.h"
#include "module/datatable_widget.h"
#include "icdcore/icdcore_inc.h"
#include "icdparser/icdparser_inc.h"
#include "icdworker/icdworker_pool.h"
#include "icdwidget/icdwidget_global.h"

MainViewWidget::MainViewWidget(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("mainView");

    // drag - drop
    setAcceptDrops(true);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_tabVehicles = new IcdTabWidget(this);
    d_tabVehicles->setTabPosition(QTabWidget::North);
    d_tabVehicles->setDocumentMode(true);
    d_tabVehicles->setDocumentMode(true);
    d_tabVehicles->setTabsClosable(true);
    vertLayoutMain->addWidget(d_tabVehicles);

    //
    connect(this, SIGNAL(dropTriggled(QStandardItem*,QStandardItem*,QString)),
            this, SLOT(onDropTriggled(QStandardItem*,QStandardItem*,QString)),
            Qt::QueuedConnection);

    //
    QFile file(":/icdsystem/com_simulator/qss/default.qss");
    if (file.open(QFile::ReadOnly)) {
        setStyleSheet(file.readAll());
    }
#if 1
    QFileSystemWatcher *fileWatcher = new QFileSystemWatcher(this);
    fileWatcher->addPath(QString::fromStdString(jframeFacade()->thisDirPath())
                         .append("/source/component/simulator/com_simulator/resource/qss/"
                                 "default.qss"));
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, [=](const QString &path){
        QFile file(path);
        if (file.open(QFile::ReadOnly)) {
            setStyleSheet(file.readAll());
        }
    });
#endif
}

void MainViewWidget::onDropTriggled(QStandardItem *itemTable, QStandardItem *item,
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

void MainViewWidget::onTreeItemClicked(QStandardItem *item)
{
    if (!item) {
        return;
    }

    //
    // get type of treeWidgetItem
    const Icd::TreeItemType itemType = (Icd::TreeItemType)item->type();
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    // dispatch
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
}

void MainViewWidget::onTreeItemUnloaded(QStandardItem *item, QStandardItem *itemTable)
{
    Q_UNUSED(itemTable);
    if (!item) {
        return;
    }

    //
    // get type of treeWidgetItem
    const Icd::TreeItemType itemType = (Icd::TreeItemType)item->type();
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    // dispatch
    switch (itemType) {
    case Icd::TreeItemTypeRoot:
        break;
    case Icd::TreeItemTypeVehicle:
    case Icd::TreeItemTypeSystem:
    case Icd::TreeItemTypeTable:
    case Icd::TreeItemTypeDataItem:
        removeItem(domain);
        break;
    default:
        break;
    }
}

void MainViewWidget::onChannelUnbound(QStandardItem *item, const QString &channelId)
{
    Q_UNUSED(channelId);
    if (!item) {
        return;
    }

    //
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    //
    removeItem(domain);
}

void MainViewWidget::onChannelChanged(QStandardItem *item, const QString &channelId)
{
    Q_UNUSED(channelId);
    if (!item) {
        return;
    }

    //
    const Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return;
    }

    //
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();

    //
    changeWorker(domain.section('/', 0, 0).trimmed(),
                 domain.section('/', 1, 1).trimmed(),
                 domain.section('/', 2, 2).trimmed(),
                 worker);
}

void MainViewWidget::onUnbindItem(QStandardItem *item)
{
    if (!item) {
        return;
    }

    //
    const QString domain = item->data(Icd::TreeItemDomainRole).toString();
    //
    removeItem(domain);
    //
    item->setData(QVariant::Invalid, Icd::TreeBoundRole);
}

void MainViewWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("icd/table-drag/simulator")) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainViewWidget::dropEvent(QDropEvent *event)
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

void MainViewWidget::paintEvent(QPaintEvent *event)
{
    //
    QPainter painter(this);
    painter.save();

    QFont font("microsoft yahei", 24);
    painter.setFont(font);
    painter.setPen(QPen(QColor(64, 64, 64, 80)));
    QTextOption textOption(Qt::AlignCenter);
    painter.drawText(rect(), QStringLiteral("<拖动[仿真系统]左边树节点到此视图>"), textOption);

    painter.restore();

    QFrame::paintEvent(event);
}

void MainViewWidget::changeWorker(const QString &vehicleId, const QString &systemId,
                                  const QString &tableId, const Icd::WorkerPtr &worker)
{
    DataTableWidget *widgetTable = findTabTable(vehicleId, systemId, tableId);
    if (!widgetTable) {
        return;
    }

    //
    widgetTable->setWorker(worker);
}

IcdTabWidget *MainViewWidget::findTabVehicle(const QString &vehicleId)
{
    return qobject_cast<IcdTabWidget *>(d_tabVehicles->findTab(vehicleId));
}

IcdTabWidget *MainViewWidget::findTabSystem(const QString &vehicleId, const QString &systemId)
{
    //
    IcdTabWidget *tabVehicle = findTabVehicle(vehicleId);
    if (!tabVehicle) {
        return 0;
    }

    //
    return qobject_cast<IcdTabWidget *>(tabVehicle->findTab(systemId));
}

DataTableWidget *MainViewWidget::findTabTable(const QString &vehicleId, const QString &systemId,
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

IcdTabWidget *MainViewWidget::addTabVehicle(const QString &domain)
{
    // find
    IcdTabWidget *tabVehicle = findTabVehicle(domain);

    // parse vehicle object
    Icd::Parser *parser = queryIcdParser();
    if (!parser) {
        d_tabVehicles->removeTabById(domain);
        delete tabVehicle;
        Q_ASSERT(false);
        return 0;
    }

    //
    Icd::VehiclePtr vehicle;
    if (!parser->parseVehicle(domain.toStdString(), vehicle, Icd::ObjectVehicle)) {
        return 0;
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

IcdTabWidget *MainViewWidget::addTabSystem(const QString &domain)
{
    //
    const QString vehicleId = domain.section('/', 0, 0);

    //
    IcdTabWidget *tabVehicle = addTabVehicle(vehicleId);
    if (tabVehicle == 0) {
        return 0;
    }

    //
    const QString systemId = domain.section('/', 1, 1);

    //
    IcdTabWidget *tabSystem =
            qobject_cast<IcdTabWidget *>(tabVehicle->findTab(systemId));

    // parse vehicle object
    Icd::Parser *parser = queryIcdParser();
    if (!parser) {
        Q_ASSERT(false);
        return 0;
    }
    Icd::SystemPtr system;
    if (!parser->parseSystem(vehicleId.toStdString(), systemId.toStdString(),
                             system, Icd::ObjectSystem)) {
        return 0;
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

bool MainViewWidget::addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
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

int MainViewWidget::focusItem(const QString &domain)
{
    return d_tabVehicles->focusItem(domain);
}

void MainViewWidget::removeItem(const QString &domain)
{
    d_tabVehicles->removeItem(domain);
}

Icd::Parser *MainViewWidget::queryIcdParser() const
{
    Icd::Parser *parser = 0;
    if (jframeNotifier().sendMessage("query_icd_parser@com_icdsystem",
                                     (JWPARAM)&parser) != JFrame::ResultSuccess) {
        return 0;
    } else {
        return parser;
    }
}

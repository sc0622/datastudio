#include "precomp.h"
#include "datatable_widget.h"
#include "dataitem_widget.h"
#include "icdcore/icd_table.h"
#include "icdworker/icdworker.h"
#include "icdwidget/icdwidget_global.h"
#include "jwt/jtableview.h"

namespace Simulate {

DataTableWidget::DataTableWidget(QWidget *parent)
    : QWidget(parent)
    , d_itemTable(0)
{
    setObjectName("tableWidget");
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_tableView = new JTableView(this);
    d_tableView->horizontalHeader()->hide();
    d_tableView->verticalHeader()->hide();
    d_tableView->setColumnCount(1);
    d_tableView->horizontalHeader()->setStretchLastSection(true);
    d_tableView->setSelectionMode(QAbstractItemView::NoSelection);
    d_tableView->setSortingEnabled(false);
    //d_tableView->verticalHeader()->setResizeContentsPrecision(0);
    d_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    d_tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    vertLayoutMain->addWidget(d_tableView);
}

DataTableWidget::~DataTableWidget()
{
    //
    if (d_itemTable) {
        d_itemTable->setData(QVariant::Invalid, Icd::TreeBoundRole);
    }
}

void DataTableWidget::setWorker(const Icd::WorkerPtr &worker)
{
    if (worker != d_worker) {
        d_worker = worker;
    }

    //
    int rowCount = d_tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        DataItemWidget *itemWidget =
                qobject_cast<DataItemWidget *>(d_tableView->cellWidget(i, 0));
        if (itemWidget) {
            itemWidget->setWorker(worker);
        }
    }
}

DataItemWidget *DataTableWidget::findDataItem(const QString &dataItemId)
{
    const std::string id = dataItemId.toStdString();
    int rowCount = d_tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        DataItemWidget *itemWidget =
                qobject_cast<DataItemWidget *>(d_tableView->cellWidget(i, 0));
        if (itemWidget && itemWidget->dataItem()->id() == id) {
            return itemWidget;
        }
    }

    return 0;
}

bool DataTableWidget::addDataItem(const QString &domain, const Icd::WorkerPtr &worker,
                                  const Icd::ObjectPtr &object)
{
    //
    d_worker = worker;

    if (!worker || !object || !d_itemTable) {
        return false;
    }
    //
    if (object->objectType() != Icd::ObjectTable) {
        Q_ASSERT(false);
        return false;
    }
    //
    const Icd::TablePtr table = JHandlePtrCast<Icd::Table>(object);
    if (!table) {
        return false;
    }
    //
    const QString section = domain.section('/', 1);
    //
    int i = 0;
    const std::string dataItemId = section.section('/', 0, 0).toStdString();
    const Icd::ItemPtrArray dataItems = table->allItem();
    Icd::ItemPtrArray::const_iterator citer = dataItems.cbegin();
    int childCount = d_itemTable->rowCount();
    if (dataItemId.empty()) {
        for (; citer != dataItems.cend() && i < childCount; ++citer, ++i) {
            const Icd::ItemPtr &dataItem = *citer;
            QStandardItem *childItem = d_itemTable->child(i);
            if (dataItem && childItem) {
                if (!addDataItem(section, dataItem, childItem)) {
                    continue;
                }
            }
        }

        //
        if (childCount > 0) {
            d_itemTable->setData(true, Icd::TreeBoundRole);
        }
    } else {
        for (; citer != dataItems.cend() && i < childCount; ++citer, ++i) {
            const Icd::ItemPtr &dataItem = *citer;
            QStandardItem *childItem = d_itemTable->child(i);
            if (dataItem && childItem && dataItem->id() == dataItemId) {
                if (!addDataItem(section, dataItem, childItem)) {
                    return false;
                }
                break;
            }
        }
    }

    return true;
}

int DataTableWidget::focusItem(const QString &domain)
{
    if (domain.isEmpty()) {
        return 0;
    }
    //
    const std::string id = domain.section('/', 0, 0).toStdString();
    if (id.empty()) {
        return 0;
    }
    //
    int pos = 0;
    DataItemWidget *itemWidget = 0;
    int rowCount = d_tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        DataItemWidget *widget =
                qobject_cast<DataItemWidget *>(d_tableView->cellWidget(i, 0));
        if (widget && widget->dataItem()->id() == id) {
            pos = d_tableView->verticalHeader()->sectionPosition(i);
            itemWidget = widget;
            break;
        }
    }
    //
    if (!itemWidget) {
        return 0;
    }

    const QString section = domain.section('/', 1);
    if (!section.isEmpty()) {
        pos += itemWidget->focusItem(domain.section('/', 1));
    }
    d_tableView->verticalScrollBar()->setValue(pos);
    return pos;
}

void DataTableWidget::removeItem(const QString &domain)
{
    if (domain.isEmpty()) {
        return;
    }
    //
    const std::string id = domain.section('/', 0, 0).toStdString();
    if (id.empty()) {
        return;
    }
    //
    const QString section = domain.section('/', 1);
    //
    DataItemWidget *widget = 0;
    int rowCount = d_tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        DataItemWidget *_widget =
                qobject_cast<DataItemWidget *>(d_tableView->cellWidget(i, 0));
        if (_widget && _widget->dataItem()->id() == id) {
            widget = _widget;
            if (section.isEmpty()) {
                _widget->item()->setData(QVariant::Invalid, Icd::TreeBoundRole);
                _widget->disconnect(this);
                d_tableView->removeRow(i);
                return;
            }
            break;
        }
    }
    //
    if (!widget) {
        return;
    }
    //
    if (!QMetaObject::invokeMethod(widget, "removeItem", Q_ARG(QString, section))) {
        return;
    }
}

int DataTableWidget::contentsHeight() const
{
    int height = 0;
    int rowCount = d_tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        height += d_tableView->verticalHeader()->sectionSize(i);
    }

    return height;
}

QStandardItem *DataTableWidget::itemTable() const
{
    return d_itemTable;
}

void DataTableWidget::setItemTable(QStandardItem *item)
{
    d_itemTable = item;
}

int DataTableWidget::rowCount() const
{
    return d_tableView->rowCount();
}

bool DataTableWidget::addDataItem(const QString &domain, const Icd::ItemPtr &dataItem,
                                  QStandardItem *item)
{
    if (!d_worker || !dataItem || !item) {
        return false;
    }

    // find and update dataitem
    int rowCount = d_tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        DataItemWidget *widget = qobject_cast<DataItemWidget *>(d_tableView->cellWidget(i, 0));
        if (widget && widget->dataItem() == dataItem) {
            widget->addDataItem(domain);
            d_tableView->resizeRowToContents(i);
            return true;
        }
    }
    //
    DataItemWidget *itemWidget = DataItemWidget::createWidget(dataItem);
    if (!itemWidget) {
        return false;
    }
    //
    itemWidget->setWorker(d_worker);
    itemWidget->setItem(item);
    itemWidget->addDataItem(domain);
    itemWidget->updateUi();
    //
    int row = d_tableView->rowCount();
    d_tableView->insertRow(row);
    d_tableView->setCellWidget(row, 0, itemWidget);
    item->setData(true, Icd::TreeBoundRole);
    //
    connect(itemWidget, &DataItemWidget::send, this, [=](){
        if (d_worker) {
            d_worker->workerSend()->sendOnce();
        }
    });
    connect(itemWidget, &DataItemWidget::remove, this, [=](){
        int rowCount = d_tableView->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            if (d_tableView->cellWidget(i, 0) == itemWidget) {
                itemWidget->disconnect(this);
                d_tableView->removeRow(i);
                emit itemRemoved(QString::fromStdString(dataItem->id()));
                break;
            }
        }
    });
    connect(itemWidget, &DataItemWidget::heightChanged, this, [=](int height){
        int rowCount = d_tableView->rowCount();
        for (int i = 0; i < rowCount; ++i) {
            if (d_tableView->cellWidget(i, 0) == itemWidget) {
                d_tableView->resizeRowToContents(i);
                emit heightChanged(height);
                break;
            }
        }
    });

    return true;
}

}

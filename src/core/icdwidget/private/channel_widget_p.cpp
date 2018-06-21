#include "precomp.h"
#include "channel_widget_p.h"
#include "icdworker/icdworker_pool.h"
#include "../worker_label.h"

namespace Icd {

ChannelWidgetPrivate::ChannelWidgetPrivate(ChannelWidget *q)
    : J_QPTR(q)
{

}

ChannelWidgetPrivate::~ChannelWidgetPrivate()
{
    Q_Q(ChannelWidget);
    Icd::WorkerPool::getInstance()->disconnect(q);
}

void ChannelWidgetPrivate::init()
{
    Q_Q(ChannelWidget);

    QHBoxLayout *horiLayoutMain = new QHBoxLayout(q);
    horiLayoutMain->setContentsMargins(0, 0, 0, 0);

    tableView = new JTableView(q);
    horiLayoutMain->addWidget(tableView);

    tableView->setFrameShape(QFrame::NoFrame);
    tableView->setAlternatingRowColors(true);

    tableView->horizontalHeader()->hide();
    tableView->setColumnCount(1);

    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSortingEnabled(false);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setFixedWidth(30);
    tableView->verticalHeader()->setSectionsMovable(true);
    tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableView->verticalHeader()->setMinimumSectionSize(60);
    tableView->setDragDropMode(QAbstractItemView::DragDrop);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QObject::connect(tableView, &JTableView::currentCellChanged, q, [=]
                     (int currentRow, int, int previousRow, int){
        if (previousRow >= 0) {
            WorkerLabel *label = qobject_cast<WorkerLabel *>(tableView->cellWidget(previousRow, 0));
            if (label) {
                label->setIndicatorVisible(false);
            }
        }
        if (currentRow >= 0) {
            WorkerLabel *label = qobject_cast<WorkerLabel *>(tableView->cellWidget(currentRow, 0));
            if (label) {
                label->setIndicatorVisible(true);
            }
        }
        emit q->currentRowChanged(currentRow, previousRow);
    });
    QObject::connect(tableView, &JTableView::cellDoubleClicked, q, [=](int row, int){
        emit q->rowDoubleClicked(row);
    });
    QObject::connect(tableView->verticalHeader(), &QHeaderView::sectionMoved, q, [=]
                     (int, int oldVisualIndex, int newVisualIndex){
        emit q->rowMoved(oldVisualIndex, newVisualIndex);
    });
    QObject::connect(Icd::WorkerPool::getInstance().get(), &Icd::WorkerPool::workerRemoved, q,
                     [=](const Icd::WorkerPtr &worker){
        int count = tableView->rowCount();
        for (int i = 0; i < count; ++i) {
            WorkerLabel *label = qobject_cast<WorkerLabel *>(tableView->cellWidget(i, 0));
            if (!label || worker != label->worker()) {
                continue;
            }
            tableView->removeRow(i);
            break;
        }
    });
    QObject::connect(Icd::WorkerPool::getInstance().get(),
                     &Icd::WorkerPool::workerCleared, q, [=](){
        q->clearWorker();
    });
}

void ChannelWidgetPrivate::moveItemToBottom(QStandardItem *item)
{
    if (!item) {
        return;
    }

    WorkerLabel *label = qobject_cast<WorkerLabel *>
            (tableView->cellWidget(item->row(), item->column()));
    if (!label) {
        return;
    }

    Q_Q(ChannelWidget);
    q->addWorker(label->worker(), label->attrs());
    Icd::WorkerPool::getInstance()->moveToBottom(label->worker());
    Icd::WorkerPool::getInstance()->saveConfig();
    tableView->removeRow(0);
}

void ChannelWidgetPrivate::swapItem(QStandardItem *item1, QStandardItem *item2)
{
    if (!item1 || !item2) {
        return;
    }

    // swap icon
    QIcon icon = item1->icon();
    item1->setIcon(item2->icon());
    item2->setIcon(icon);
    // swap text
    QString text = item1->text();
    item1->setText(item2->text());
    item2->setText(text);
    // swap cellWidget
    WorkerLabel *label1 = qobject_cast<WorkerLabel *>
            (tableView->cellWidget(item1->row(), item1->column()));
    WorkerLabel *label2 = qobject_cast<WorkerLabel *>
            (tableView->cellWidget(item2->row(), item2->column()));
    if (label1 && label2) {
        //
        Icd::WorkerPool::getInstance()->swapWorker(label1->worker(), label2->worker());
        Icd::WorkerPool::getInstance()->saveConfig();
        //
    }
}

} // end of namespace Icd

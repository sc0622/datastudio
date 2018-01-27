#include "precomp.h"
#include "channel_widget.h"
#include "private/channel_widget_p.h"
#include "icdworker/icdworker_pool.h"
#include "worker_label.h"

namespace Icd {

// class ChannelWidget

ChannelWidget::ChannelWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new ChannelWidgetPrivate(this))
{
    Q_D(ChannelWidget);
    d->init();
}

ChannelWidget::~ChannelWidget()
{
    Q_D(ChannelWidget);
    delete d;
}

Icd::WorkerPtr ChannelWidget::selectedWorker() const
{
    Q_D(const ChannelWidget);
    int row = d->tableView->currentRow();
    if (row == -1) {
        return 0;
    }

    WorkerLabel *label = qobject_cast<WorkerLabel *>(d->tableView->cellWidget(row, 0));
    if (!label) {
        return Icd::WorkerPtr(0);
    }

    return label->worker();
}

int ChannelWidget::rowCount() const
{
    Q_D(const ChannelWidget);
    return d->tableView->rowCount();
}

void ChannelWidget::setCurrentRow(int row)
{
    Q_D(ChannelWidget);
    d->tableView->setCurrentCell(row, 0);
}

void ChannelWidget::addWorker(const Icd::WorkerPtr &worker,
                              ChannelWidget::OperateAttributes attrs)
{
    Q_D(ChannelWidget);
    insertWorker(d->tableView->rowCount(), worker, attrs);
}

void ChannelWidget::insertWorker(int row, const Icd::WorkerPtr &worker,
                                      OperateAttributes attrs)
{
    Q_D(ChannelWidget);
    d->tableView->insertRow(row);
    WorkerLabel *label = new WorkerLabel(worker, attrs, this);
    if (attrs == ChannelWidget::NoOperate) {
        if (worker->channel()->channelType() == Icd::ChannelFile) {
            if (worker->workerRecv()->table() || worker->workerSend()->table()) {
                d->tableView->setRowHidden(row, true);
            }
        }
    }
    d->tableView->setCellWidget(row, 0, label);
}

void ChannelWidget::removeWorker(const Icd::WorkerPtr &worker)
{
    Q_D(ChannelWidget);
    int rowCount = d->tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        WorkerLabel *label = qobject_cast<WorkerLabel *>(d->tableView->cellWidget(i, 0));
        if (label && label->worker() == worker) {
            d->tableView->removeRow(i);
            break;
        }
    }
}

void ChannelWidget::removeWorker(const QString &channelId)
{
    Q_D(ChannelWidget);
    const std::string id = channelId.toStdString();
    int rowCount = d->tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        WorkerLabel *label = qobject_cast<WorkerLabel *>(d->tableView->cellWidget(i, 0));
        if (label && label->worker()->channel()->identity() == id) {
            d->tableView->removeRow(i);
            break;
        }
    }
}

void ChannelWidget::clearWorker()
{
    Q_D(ChannelWidget);
    d->tableView->clearContents();
}

void ChannelWidget::setTableEnabled(bool enabled)
{
    Q_D(ChannelWidget);
    d->tableView->setEnabled(enabled);
}

void ChannelWidget::setEditEnabled(bool enabled)
{
    Q_D(ChannelWidget);
    d->tableView->setDragEnabled(enabled);
    d->tableView->setAcceptDrops(enabled);
    if (enabled) {
        d->tableView->viewport()->installEventFilter(this);
    } else {
        d->tableView->viewport()->removeEventFilter(this);
    }
}

bool ChannelWidget::eventFilter(QObject *watcher, QEvent *event)
{
    Q_D(ChannelWidget);

    if (watcher == d->tableView->viewport()) {
        switch (event->type()) {
        case QEvent::Drop:
        {
            if (d->tableView->rowCount() < 2) {
                return true;
            }

            QDropEvent *dropEvent = ((QDropEvent *)event);
            const QMimeData *mime = dropEvent->mimeData();
            QByteArray encodeData = mime->data("application/x-qabstractitemmodeldatalist");
            if (encodeData.isEmpty()) {
                break;
            }

            QDataStream stream(&encodeData, QIODevice::ReadOnly);
            if (stream.atEnd()) {
                break;
            }

            int row, column;
            QMap<int, QVariant> roleDataMap;
            stream >> row >> column >> roleDataMap;

            QStandardItem *dragItem = d->tableView->item(row, column);
            if (!dragItem) {
                return true;
            }

            QStandardItem *lastItem = d->tableView->item(d->tableView->rowCount() - 1, 0);
            if (!lastItem) {
                return true;
            }

            const QRect itemRect = d->tableView->visualItemRect(lastItem);
            if (itemRect.bottom() < dropEvent->pos().y()) {
                d->moveItemToBottom(dragItem);
            } else {
                QStandardItem *dropItem = d->tableView->itemAt(dropEvent->pos());
                if (!dropItem) {
                    return true;
                }

                if (dropItem->row() == row) {
                    return true;
                }

                d->swapItem(dragItem, dropItem);
            }

            return true;
        }
        default:
            break;
        }
    }

    return false;
}

} // end of namespace Icd

#include "precomp.h"
#include "JChannelPane.h"
#include "private/JChannelPane_p.h"
#include "icdworker/icdworker_pool.h"
#include "JWorkerLabel.h"

namespace Icd {

// class JChannelPane

JChannelPane::JChannelPane(QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new JChannelPanePrivate(this))
{
    Q_D(JChannelPane);
    d->init();
}

JChannelPane::~JChannelPane()
{
    Q_D(JChannelPane);
    delete d;
}

Icd::WorkerPtr JChannelPane::selectedWorker() const
{
    Q_D(const JChannelPane);
    int row = d->tableView->currentRow();
    if (row == -1) {
        return Icd::WorkerPtr();
    }

    JWorkerLabel *label = qobject_cast<JWorkerLabel *>(d->tableView->cellWidget(row, 0));
    if (!label) {
        return Icd::WorkerPtr();
    }

    return label->worker();
}

int JChannelPane::rowCount() const
{
    Q_D(const JChannelPane);
    return d->tableView->rowCount();
}

void JChannelPane::setCurrentRow(int row)
{
    Q_D(JChannelPane);
    d->tableView->setCurrentCell(row, 0);
}

void JChannelPane::addWorker(const Icd::WorkerPtr &worker,
                             JChannelPane::OperateAttributes attrs)
{
    Q_D(JChannelPane);
    insertWorker(d->tableView->rowCount(), worker, attrs);
}

void JChannelPane::insertWorker(int row, const Icd::WorkerPtr &worker,
                                OperateAttributes attrs)
{
    Q_D(JChannelPane);
    d->tableView->insertRow(row);
    JWorkerLabel *label = new JWorkerLabel(worker, attrs, this);
    if (attrs == JChannelPane::NoOperate) {
        if (worker->channel()->channelType() == Icd::ChannelFile) {
            if (worker->workerRecv()->table() || worker->workerSend()->table()) {
                d->tableView->setRowHidden(row, true);
            }
        }
    }
    d->tableView->setCellWidget(row, 0, label);
}

void JChannelPane::removeWorker(const Icd::WorkerPtr &worker)
{
    Q_D(JChannelPane);
    int rowCount = d->tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        JWorkerLabel *label = qobject_cast<JWorkerLabel *>(d->tableView->cellWidget(i, 0));
        if (label && label->worker() == worker) {
            d->tableView->removeRow(i);
            break;
        }
    }
}

void JChannelPane::removeWorker(const QString &channelId)
{
    Q_D(JChannelPane);
    const std::string id = channelId.toStdString();
    int rowCount = d->tableView->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        JWorkerLabel *label = qobject_cast<JWorkerLabel *>(d->tableView->cellWidget(i, 0));
        if (label && label->worker()->channel()->identity() == id) {
            d->tableView->removeRow(i);
            break;
        }
    }
}

void JChannelPane::clearWorker()
{
    Q_D(JChannelPane);
    d->tableView->clearContents();
}

void JChannelPane::setTableEnabled(bool enabled)
{
    Q_D(JChannelPane);
    d->tableView->setEnabled(enabled);
}

void JChannelPane::setEditEnabled(bool enabled)
{
    Q_D(JChannelPane);
    d->tableView->setDragEnabled(enabled);
    d->tableView->setAcceptDrops(enabled);
    if (enabled) {
        d->tableView->viewport()->installEventFilter(this);
    } else {
        d->tableView->viewport()->removeEventFilter(this);
    }
}

bool JChannelPane::eventFilter(QObject *watcher, QEvent *event)
{
    Q_D(JChannelPane);

    if (watcher == d->tableView->viewport()) {
        switch (event->type()) {
        case QEvent::Drop:
        {
            if (d->tableView->rowCount() < 2) {
                return true;
            }

            QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
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

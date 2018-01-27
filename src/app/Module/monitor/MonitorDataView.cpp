#include "precomp.h"
#include "MonitorDataView.h"

namespace Monitor {

DataView::DataView(QWidget *parent)
    : QWidget(parent)
    , d_item(nullptr)
    , d_timerId(0)
{
    QVBoxLayout *vertLyoutMain = new QVBoxLayout(this);
    vertLyoutMain->setContentsMargins(0, 0, 0, 0);

    d_dataView = new JUtralEdit::JView(this);
    d_dataView->setAsciiVisible(false);
    vertLyoutMain->addWidget(d_dataView);

    jnotify->on("monitor.tree.item.clicked", this, [=](JNEvent &event){
        QStandardItem *item = jVariantFromVoid<QStandardItem>(event.argument());
        if (!item) {
            return;
        }
        if (item == d_item) {
            return;
        }

        d_item = item;

        updateData(item);
    });
    jnotify->on("monitor.tree.channel.changed", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }

        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }

        if (item != d_item) {
            return;
        }

        updateData(d_item);
    });
    jnotify->on("monitor.tree.item.unbind", this, [=](JNEvent &event){
        const QVariantList args = event.argument().toList();
        if (args.count() != 2) {
            Q_ASSERT(args.count() == 2);
            return;
        }
        QStandardItem *item = jVariantFromVoid<QStandardItem>(args.at(0));
        if (!item) {
            return;
        }

        if (item != d_item) {
            return;
        }

        reset();
    });
}

DataView::~DataView()
{

}

bool DataView::init()
{
    bool result = true;

    return result;
}

void DataView::start()
{
    if (d_timerId) {
        killTimer(d_timerId);
        d_timerId = 0;
    }

    d_timerId = startTimer(300);    // 300ms
}

void DataView::stop()
{
    if (d_timerId) {
        killTimer(d_timerId);
        d_timerId = 0;
    }
}

void DataView::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == d_timerId) {
        updateView();
    }
}

void Monitor::DataView::reset()
{
    stop();

    if (d_worker) {
        d_worker->workerRecv()->disconnect(this);
    }

    d_item = nullptr;
    d_worker = Icd::WorkerPtr();
    d_dataView->setTitle(QStringLiteral("内存数据"));

    updateView();
}

void DataView::updateData(QStandardItem *item)
{
    if (!item) {
        return;
    }

    const int itemType = item->type();
    if (itemType > Icd::TreeItemTypeTable) {
        return;
    }

    reset();

    const QVariant varChannelId = item->data(Icd::TreeChannelIdRole);
    if (varChannelId.isNull()) {
        return;
    }

    const QString channelId = varChannelId.toString();
    const Icd::WorkerPtr worker =  Icd::WorkerPool::getInstance()
            ->workerByChannelIdentity(channelId.toStdString());
    if (!worker) {
        return;
    }

    d_item = item;
    d_worker = worker;
    d_dataView->setTitle(QStringLiteral("内存数据——") + QString::fromStdString(
                             d_worker->workerRecv()->table()->name()));

    updateView();

    if (d_worker->workerRecv()->isRunning()) {
        start();
    }

    connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::started,
            this, [=](){
        start();
    });
    connect(d_worker->workerRecv().get(), &Icd::WorkerRecv::stopped,
            this, [=](){
        stop();
    });
}

void Monitor::DataView::updateView()
{
    if (isHidden()) {
        return;
    }

    if (d_worker) {
        const Icd::TablePtr table = d_worker->workerRecv()->table();
        if (table) {
            d_dataView->updateUi(table->buffer(), table->bufferSize());
        } else {
            d_dataView->updateUi(QByteArray());
        }
    } else {
        d_dataView->updateUi(QByteArray());
    }
}

}

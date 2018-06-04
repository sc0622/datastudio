#include "precomp.h"
#include "icdworker/icdworker_pool.h"
#include "worker_label.h"
#include "private/worker_label_p.h"

namespace Icd {

WorkerLabel::WorkerLabel(const Icd::WorkerPtr &worker,
                         ChannelWidget::OperateAttributes attrs,
                         QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new WorkerLabelPrivate(worker, this))
{
    Q_D(WorkerLabel);
    d->init(attrs);

    //
    updateUi();
}

WorkerLabel::~WorkerLabel()
{
    Q_D(WorkerLabel);
    delete d;
}

void WorkerLabel::updateUi()
{
    Q_D(WorkerLabel);
    d->updateUi();
}

Icd::WorkerPtr WorkerLabel::worker() const
{
    Q_D(const WorkerLabel);
    return d->worker;
}

ChannelWidget::OperateAttributes WorkerLabel::attrs() const
{
    Q_D(const WorkerLabel);
    return d->attrs();
}

QPixmap WorkerLabel::icon() const
{
    Q_D(const WorkerLabel);
    return d->labelChannelIcon->pixmap()->scaled(42, 42);
}

void WorkerLabel::setIndicatorVisible(bool visible)
{
    Q_D(WorkerLabel);
    if (visible) {
        d->labelIndicator->setPixmap(QPixmap(":/icdwidget/image/indicator.png"));
    } else {
        d->labelIndicator->setPixmap(QPixmap());
    }
}

} // end of namespace Icd

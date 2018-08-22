#include "precomp.h"
#include "JWorkerLabel.h"
#include "icdworker/icdworker_pool.h"
#include "private/JWorkerLabel_p.h"

namespace Icd {

JWorkerLabel::JWorkerLabel(const Icd::WorkerPtr &worker,
                           JChannelPane::OperateAttributes attrs,
                           QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new JWorkerLabelPrivate(worker, this))
{
    Q_D(JWorkerLabel);
    d->init(attrs);

    //
    updateUi();
}

JWorkerLabel::~JWorkerLabel()
{
    Q_D(JWorkerLabel);
    delete d;
}

void JWorkerLabel::updateUi()
{
    Q_D(JWorkerLabel);
    d->updateUi();
}

Icd::WorkerPtr JWorkerLabel::worker() const
{
    Q_D(const JWorkerLabel);
    return d->worker;
}

JChannelPane::OperateAttributes JWorkerLabel::attrs() const
{
    Q_D(const JWorkerLabel);
    return d->attrs();
}

QPixmap JWorkerLabel::icon() const
{
    Q_D(const JWorkerLabel);
    return d->labelChannelIcon->pixmap()->scaled(42, 42);
}

void JWorkerLabel::setIndicatorVisible(bool visible)
{
    Q_D(JWorkerLabel);
    if (visible) {
        d->labelIndicator->setPixmap(QPixmap(":/icdwidget/image/indicator.png"));
    } else {
        d->labelIndicator->setPixmap(QPixmap());
    }
}

} // end of namespace Icd

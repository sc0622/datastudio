#include "precomp.h"
#include "bindchannel_widget.h"
#include "private/bindchannel_widget_p.h"
#include "icdworker/icdworker_pool.h"

namespace Icd {

BindChannelWidget::BindChannelWidget(QWidget *parent)
    : QDialog(parent)
    , J_DPTR(new BindChannelWidgetPrivate(this))
{
    Q_D(BindChannelWidget);
    d->init();
}

BindChannelWidget::~BindChannelWidget()
{
    Q_D(BindChannelWidget);
    delete d;
}

Icd::WorkerPtr BindChannelWidget::selectedWorker() const
{
    Q_D(const BindChannelWidget);
    return d->selectedWorker();
}

} // end of namespace Icd

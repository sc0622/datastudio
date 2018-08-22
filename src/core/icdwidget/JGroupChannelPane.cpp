#include "precomp.h"
#include "JGroupChannelPane.h"
#include "private/JGroupChannelPane_p.h"
#include "icdworker/icdworker_pool.h"

namespace Icd {

JGroupChannelPane::JGroupChannelPane(QWidget *parent)
    : QDialog(parent)
    , J_DPTR(new JGroupChannelPanePrivate(this))
{
    Q_D(JGroupChannelPane);
    d->init();
}

JGroupChannelPane::~JGroupChannelPane()
{
    Q_D(JGroupChannelPane);
    delete d;
}

Icd::WorkerPtr JGroupChannelPane::selectedWorker() const
{
    Q_D(const JGroupChannelPane);
    return d->selectedWorker();
}

} // end of namespace Icd

#include "precomp.h"
#include "JGroupChannelPane.h"
#include "private/JGroupChannelPane_p.h"
#include "icdworker/icdworker_pool.h"

namespace Icd {

// class JGroupChannelPane

JGroupChannelPane::JGroupChannelPane(QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new JGroupChannelPanePrivate(this))
{
    Q_D(JGroupChannelPane);
    d->init();
}

JGroupChannelPane::JGroupChannelPane(JChannelPane::OperateAttribute attrs, QWidget *parent)
    : QWidget(parent)
    , J_DPTR(new JGroupChannelPanePrivate(attrs, this))
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

int JGroupChannelPane::currentTabIndex() const
{
    Q_D(const JGroupChannelPane);
    return d->currentTabIndex();
}

int JGroupChannelPane::currentRowIndex() const
{
    Q_D(const JGroupChannelPane);
    return d->currentRowIndex();
}

int JGroupChannelPane::currentRowCount() const
{
    Q_D(const JGroupChannelPane);
    return d->currentRowCount();
}

ChannelType JGroupChannelPane::currentChannelType() const
{
    Q_D(const JGroupChannelPane);
    return d->currentChannelType();
}

void JGroupChannelPane::addWorker(const WorkerPtr &worker)
{
    Q_D(JGroupChannelPane);
    return d->addWorker(worker);
}

void JGroupChannelPane::updateAllTab()
{
    Q_D(JGroupChannelPane);
    d->updateAllTab();
}

void JGroupChannelPane::updateCurrentTab()
{
    Q_D(JGroupChannelPane);
    d->updateCurrentList();
}

void JGroupChannelPane::setCurrentTabIndex(int index)
{
    Q_D(JGroupChannelPane);
    d->tabWidget->setCurrentIndex(index);
}

void JGroupChannelPane::setCurrentRowIndex(int index)
{
    Q_D(JGroupChannelPane);
    d->setCurrentRowIndex(index);
}

// class JGroupChannelDlg

JGroupChannelDlg::JGroupChannelDlg(QWidget *parent)
    : QDialog(parent)
    , pane_(new JGroupChannelPane(this))
{
    init();
}

JGroupChannelDlg::JGroupChannelDlg(JChannelPane::OperateAttribute attrs, QWidget *parent)
    : QDialog(parent)
    , pane_(new JGroupChannelPane(attrs, this))
{
    init();
}

Icd::WorkerPtr JGroupChannelDlg::selectedWorker() const
{
    return pane_->selectedWorker();
}

JGroupChannelPane *JGroupChannelDlg::pane() const
{
    return pane_;
}

void JGroupChannelDlg::init()
{
    resize(800, 600);

    QHBoxLayout *layoutMain = new QHBoxLayout(this);
    layoutMain->setContentsMargins(0, 0, 0, 0);

    layoutMain->addWidget(pane_);

    connect(pane_, &Icd::JGroupChannelPane::accepted, this, &JGroupChannelDlg::accept);
    connect(pane_, &Icd::JGroupChannelPane::rejected, this, &JGroupChannelDlg::reject);
}

} // end of namespace Icd

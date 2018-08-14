#include "precomp.h"
#include "bindchannel_widget_p.h"
#include "icdworker/icdworker_pool.h"
#include "../channel_widget.h"

namespace Icd {

BindChannelWidgetPrivate::BindChannelWidgetPrivate(BindChannelWidget *q)
    : QObject(q)
    , J_QPTR(q)
{

}

BindChannelWidgetPrivate::~BindChannelWidgetPrivate()
{

}

void BindChannelWidgetPrivate::init()
{
    Q_Q(BindChannelWidget);

    q->setWindowTitle(BindChannelWidget::tr("Binding Channel"));
    q->resize(800, 600);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);
    vertLayoutMain->setContentsMargins(0, 0, 0, 10);

    tabWidget = new QTabWidget(q);
    tabWidget->setIconSize(QSize(20, 20));
    tabWidget->setStyleSheet("QTabWidget:tab-bar{alignment:center;}");
    vertLayoutMain->addWidget(tabWidget);

    widgetFile = new ChannelWidget(tabWidget);
    widgetSerial = new ChannelWidget(tabWidget);
    widgetUdp = new ChannelWidget(tabWidget);

    tabWidget->addTab(widgetFile, QIcon(":/icdwidget/image/file.png"),
                      BindChannelWidget::tr("File Channel"));
    tabWidget->addTab(widgetSerial, QIcon(":/icdwidget/image/serial.png"),
                      BindChannelWidget::tr("Serial Channel"));
    tabWidget->addTab(widgetUdp, QIcon(":/icdwidget/image/udp.png"),
                      BindChannelWidget::tr("UDP Channel"));

    vertLayoutMain->addSpacing(10);

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    buttonOk = new QPushButton(BindChannelWidget::tr("Ok"), q);
    buttonOk->setMinimumWidth(120);
    buttonOk->setDefault(true);
    horiLayoutBottom->addWidget(buttonOk);

    buttonCancel = new QPushButton(BindChannelWidget::tr("Cancel"), q);
    buttonCancel->setMinimumWidth(120);
    horiLayoutBottom->addSpacing(10);
    horiLayoutBottom->addWidget(buttonCancel);

    horiLayoutBottom->addStretch();

    //
    connect(tabWidget, &QTabWidget::currentChanged, this, [=](int index){
        updateCurrentList(index);
    });
    connect(widgetFile, &ChannelWidget::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(widgetFile, row);
    });
    connect(widgetSerial, &ChannelWidget::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(widgetSerial, row);
    });
    connect(widgetUdp, &ChannelWidget::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(widgetUdp, row);
    });
    connect(buttonOk, SIGNAL(clicked(bool)), q, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked(bool)), q, SLOT(reject()));

    //
    updateCurrentList();
}

Icd::WorkerPtr BindChannelWidgetPrivate::selectedWorker() const
{
    ChannelWidget *widget = nullptr;
    switch (tabWidget->currentIndex()) {
    case 0: widget = widgetFile; break;
    case 1: widget = widgetSerial; break;
    case 2: widget = widgetUdp; break;
    default: return Icd::WorkerPtr();
    }

    if (!widget) {
        return Icd::WorkerPtr();
    }

    return widget->selectedWorker();
}

void BindChannelWidgetPrivate::onRowDoubleClicked(ChannelWidget *widget, int row)
{
    if (!widget || row == -1) {
        return;
    }

    Q_Q(BindChannelWidget);
    q->accept();
}

void BindChannelWidgetPrivate::updateCurrentList(int index)
{
    //
    if (index == -1) {
        index = tabWidget->currentIndex();
        if (index == -1) {
            return;
        }
    }

    //
    Icd::ChannelType channelType = Icd::ChannelInvalid;
    ChannelWidget *widget = nullptr;
    switch (index) {
    case 0:
        channelType = Icd::ChannelFile;
        widget = widgetFile;
        break;
    case 1:
        channelType = Icd::ChannelSerial;
        widget = widgetSerial;
        break;
    case 2:
        channelType = Icd::ChannelUdp;
        widget = widgetUdp;
        break;
    default:
        return; // invalid channel type
    }

    //
    if (!widget) {
        return;
    }

    //
    widget->clearWorker();

    //
    const Icd::WorkerPtrArray &workers = Icd::WorkerPool::getInstance()->allWorkers();
    Icd::WorkerPtrArray::const_iterator citerWorkers = workers.cbegin();
    for (; citerWorkers != workers.cend(); ++citerWorkers) {
        const Icd::WorkerPtr &worker = *citerWorkers;
        if (worker->channel()->channelType() != channelType) {
            continue;   //
        }
        //
        widget->addWorker(worker, ChannelWidget::NoOperate);
    }

    //
    widget->setCurrentRow(0);
}

} // end of namespace Icd

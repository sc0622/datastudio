#include "precomp.h"
#include "JGroupChannelPane_p.h"
#include "icdworker/icdworker_pool.h"
#include "../JChannelPane.h"

namespace Icd {

JGroupChannelPanePrivate::JGroupChannelPanePrivate(JGroupChannelPane *q)
    : QObject(q)
    , J_QPTR(q)
{

}

JGroupChannelPanePrivate::~JGroupChannelPanePrivate()
{

}

void JGroupChannelPanePrivate::init()
{
    Q_Q(JGroupChannelPane);

    q->setWindowTitle(JGroupChannelPane::tr("Binding Channel"));
    q->resize(800, 600);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);
    vertLayoutMain->setContentsMargins(0, 0, 0, 10);

    tabWidget = new QTabWidget(q);
    tabWidget->setIconSize(QSize(20, 20));
    tabWidget->setStyleSheet("QTabWidget:tab-bar{alignment:center;}");
    vertLayoutMain->addWidget(tabWidget);

    widgetFile = new Icd::JChannelPane(tabWidget);
    widgetSerial = new Icd::JChannelPane(tabWidget);
    widgetUdp = new Icd::JChannelPane(tabWidget);

    tabWidget->addTab(widgetFile, QIcon(":/icdwidget/image/file.png"),
                      JGroupChannelPane::tr("File Channel"));
    tabWidget->addTab(widgetSerial, QIcon(":/icdwidget/image/serial.png"),
                      JGroupChannelPane::tr("Serial Channel"));
    tabWidget->addTab(widgetUdp, QIcon(":/icdwidget/image/udp.png"),
                      JGroupChannelPane::tr("UDP Channel"));

    vertLayoutMain->addSpacing(10);

    QHBoxLayout *horiLayoutBottom = new QHBoxLayout();
    vertLayoutMain->addLayout(horiLayoutBottom);

    horiLayoutBottom->addStretch();

    buttonOk = new QPushButton(JGroupChannelPane::tr("Ok"), q);
    buttonOk->setMinimumWidth(120);
    buttonOk->setDefault(true);
    horiLayoutBottom->addWidget(buttonOk);

    buttonCancel = new QPushButton(JGroupChannelPane::tr("Cancel"), q);
    buttonCancel->setMinimumWidth(120);
    horiLayoutBottom->addSpacing(10);
    horiLayoutBottom->addWidget(buttonCancel);

    horiLayoutBottom->addStretch();

    //
    connect(tabWidget, &QTabWidget::currentChanged, this, [=](int index){
        updateCurrentList(index);
    });
    connect(widgetFile, &Icd::JChannelPane::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(widgetFile, row);
    });
    connect(widgetSerial, &Icd::JChannelPane::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(widgetSerial, row);
    });
    connect(widgetUdp, &Icd::JChannelPane::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(widgetUdp, row);
    });
    connect(buttonOk, SIGNAL(clicked(bool)), q, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked(bool)), q, SLOT(reject()));

    //
    updateCurrentList();
}

Icd::WorkerPtr JGroupChannelPanePrivate::selectedWorker() const
{
    Icd::JChannelPane *widget = nullptr;
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

void JGroupChannelPanePrivate::onRowDoubleClicked(Icd::JChannelPane *widget, int row)
{
    if (!widget || row == -1) {
        return;
    }

    Q_Q(JGroupChannelPane);
    q->accept();
}

void JGroupChannelPanePrivate::updateCurrentList(int index)
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
    Icd::JChannelPane *widget = nullptr;
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
        widget->addWorker(worker, Icd::JChannelPane::NoOperate);
    }

    //
    widget->setCurrentRow(0);
}

} // end of namespace Icd

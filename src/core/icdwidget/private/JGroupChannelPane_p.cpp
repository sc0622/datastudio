#include "precomp.h"
#include "JGroupChannelPane_p.h"
#include "icdworker/icdworker_pool.h"
#include "../JChannelPane.h"

namespace Icd {

JGroupChannelPanePrivate::JGroupChannelPanePrivate(JGroupChannelPane *q)
    : QObject(q)
    , J_QPTR(q)
    , operateAttrs(Icd::JChannelPane::NoOperate)
{

}

JGroupChannelPanePrivate::JGroupChannelPanePrivate(JChannelPane::OperateAttribute attrs,
                                                   JGroupChannelPane *q)
    : QObject(q)
    , J_QPTR(q)
    , operateAttrs(attrs)
    , buttonOk(nullptr)
    , buttonCancel(nullptr)
{

}

JGroupChannelPanePrivate::~JGroupChannelPanePrivate()
{

}

void JGroupChannelPanePrivate::init()
{
    Q_Q(JGroupChannelPane);

    q->setWindowTitle(JGroupChannelPane::tr("Binding Channel"));

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);

    tabWidget = new QTabWidget(q);
    tabWidget->setIconSize(QSize(20, 20));
    vertLayoutMain->addWidget(tabWidget);

    channelPaneFile = new Icd::JChannelPane(tabWidget);
    channelPaneSerial = new Icd::JChannelPane(tabWidget);
    channelPaneUdp = new Icd::JChannelPane(tabWidget);

    tabWidget->addTab(channelPaneFile, QIcon(":/icdwidget/image/file.png"),
                      JGroupChannelPane::tr("File Channel"));
    tabWidget->addTab(channelPaneSerial, QIcon(":/icdwidget/image/serial.png"),
                      JGroupChannelPane::tr("Serial Channel"));
    tabWidget->addTab(channelPaneUdp, QIcon(":/icdwidget/image/udp.png"),
                      JGroupChannelPane::tr("UDP Channel"));

    if (operateAttrs != Icd::JChannelPane::AllOperate) {
        vertLayoutMain->setContentsMargins(0, 0, 0, 10);
        tabWidget->setStyleSheet("QTabWidget:tab-bar{alignment:center;}");

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

        connect(buttonOk, &QPushButton::clicked, q, &Icd::JGroupChannelPane::accepted);
        connect(buttonCancel, &QPushButton::clicked, q, &Icd::JGroupChannelPane::rejected);
    } else {
        vertLayoutMain->setContentsMargins(0, 2, 0, 0);
    }

    //
    connect(tabWidget, &QTabWidget::currentChanged, this, [=](int index){
        updateCurrentList(index);
    });
    connect(channelPaneFile, &Icd::JChannelPane::currentRowChanged, this, [=](int currentRow){
        const int currentTabIndex = tabWidget->currentIndex();
        if (currentTabIndex != 0) {
            return;     // ignore
        }
        emit q->currentRowChanged(currentRow, currentTabIndex);
    });
    connect(channelPaneSerial, &Icd::JChannelPane::currentRowChanged, this, [=](int currentRow){
        const int currentTabIndex = tabWidget->currentIndex();
        if (currentTabIndex != 1) {
            return;     // ignore
        }
        emit q->currentRowChanged(currentRow, currentTabIndex);
    });
    connect(channelPaneUdp, &Icd::JChannelPane::currentRowChanged, this, [=](int currentRow){
        const int currentTabIndex = tabWidget->currentIndex();
        if (currentTabIndex != 2) {
            return;     // ignore
        }
        emit q->currentRowChanged(currentRow, currentTabIndex);
    });
    connect(channelPaneFile, &Icd::JChannelPane::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(channelPaneFile, row);
    });
    connect(channelPaneSerial, &Icd::JChannelPane::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(channelPaneSerial, row);
    });
    connect(channelPaneUdp, &Icd::JChannelPane::rowDoubleClicked, this, [=](int row){
        onRowDoubleClicked(channelPaneUdp, row);
    });

    //
    updateCurrentList(0);
}

Icd::WorkerPtr JGroupChannelPanePrivate::selectedWorker() const
{
    Icd::JChannelPane *widget = currentPane();
    if (!widget) {
        return Icd::WorkerPtr();
    }

    return widget->selectedWorker();
}

ChannelType JGroupChannelPanePrivate::currentChannelType() const
{
    switch (tabWidget->currentIndex()) {
    case 0: return Icd::ChannelFile;
    case 1: return Icd::ChannelSerial;
    case 2: return Icd::ChannelUdp;
    default: return Icd::ChannelInvalid;
    }
}

JChannelPane *JGroupChannelPanePrivate::currentPane() const
{
    switch (tabWidget->currentIndex()) {
    case 0: return channelPaneFile;
    case 1: return channelPaneSerial;
    case 2: return channelPaneUdp;
    default: return nullptr;
    }
}

int JGroupChannelPanePrivate::currentTabIndex() const
{
    return tabWidget->currentIndex();
}

int JGroupChannelPanePrivate::currentRowIndex() const
{
    Icd::JChannelPane *currentPane = this->currentPane();
    if (!currentPane) {
        return -1;
    }

    return currentPane->currentRow();
}

int JGroupChannelPanePrivate::currentRowCount() const
{
    Icd::JChannelPane *currentPane = this->currentPane();
    if (!currentPane) {
        return 0;
    }

    return currentPane->rowCount();
}

void JGroupChannelPanePrivate::addWorker(const WorkerPtr &worker)
{
    Icd::JChannelPane *currentPane = this->currentPane();
    if (!currentPane) {
        return;
    }

    //
    currentPane->addWorker(worker, operateAttrs);
}

void JGroupChannelPanePrivate::updateAllTab()
{
    const int tabCount = tabWidget->count();
    for (int i = 0; i < tabCount; ++i) {
        updateCurrentList(i);
    }
}

void JGroupChannelPanePrivate::setCurrentRowIndex(int index)
{
    Icd::JChannelPane *currentPane = this->currentPane();
    if (!currentPane) {
        return;
    }

    currentPane->setCurrentRow(index);
}

void JGroupChannelPanePrivate::onRowDoubleClicked(Icd::JChannelPane *widget, int row)
{
    if (!widget || row == -1) {
        return;
    }

    Q_Q(JGroupChannelPane);
    emit q->accepted();
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
    case 0: channelType = Icd::ChannelFile; widget = channelPaneFile; break;
    case 1: channelType = Icd::ChannelSerial; widget = channelPaneSerial; break;
    case 2: channelType = Icd::ChannelUdp; widget = channelPaneUdp; break;
    default: return; // invalid channel type
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
        widget->addWorker(worker, operateAttrs);
    }

    //
    widget->setCurrentRow(0);
}

} // end of namespace Icd

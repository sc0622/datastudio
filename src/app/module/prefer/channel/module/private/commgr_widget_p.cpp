#include "precomp.h"
#include "commgr_widget_p.h"
#include "commdetail_widget.h"
#include "icdwidget/JGroupChannelPane.h"

ComMgrWidgetPrivate::ComMgrWidgetPrivate(ComMgrWidget *q)
    : QObject(q)
    , J_QPTR(q)
{

}

ComMgrWidgetPrivate::~ComMgrWidgetPrivate()
{
    JMain::saveWidgetState(groupChannelPane->tabWidget());
}

void ComMgrWidgetPrivate::init()
{
    Q_Q(ComMgrWidget);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    splitterTop = new JSplitter(q);
    splitterTop->setHandleWidth(4);
    splitterTop->setHandleColor(QColor(0, 0, 0, 0));
    splitterTop->setScales(QList<double>() << 2.5 << 1);
    vertLayoutMain->addWidget(splitterTop);

    QWidget *widgetArea = new QWidget(q);
    splitterTop->addWidget(widgetArea);

    QVBoxLayout *vertLayoutArea = new QVBoxLayout(widgetArea);
    vertLayoutArea->setContentsMargins(0, 0, 0, 6);

    groupChannelPane = new Icd::JGroupChannelPane(Icd::JChannelPane::AllOperate, q);
    vertLayoutArea->addWidget(groupChannelPane);

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutArea->addLayout(horiLayoutButtons);

    horiLayoutButtons->addStretch();
    buttonAdd = new QPushButton(tr("Add channel"), widgetArea);
    buttonAdd->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(buttonAdd);

    horiLayoutButtons->addSpacing(10);

    buttonClear = new QPushButton(tr("Clear channel"), widgetArea);
    buttonClear->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(buttonClear);

    horiLayoutButtons->addSpacing(10);

    commDetailWidget = new CommDetailWidget(q);
    commDetailWidget->hide();
    splitterTop->addWidget(commDetailWidget);

    connect(groupChannelPane, &Icd::JGroupChannelPane::currentRowChanged,
            q, [=](int rowIndex, int tabIndex){
        if (tabIndex >= 0 && rowIndex >= 0) {
            Icd::WorkerPtr worker = groupChannelPane->selectedWorker();
            if (worker) {
                commDetailWidget->updateUi(worker);
                commDetailWidget->show();
            }
        } else {
            commDetailWidget->hide();
        }
    });
    connect(buttonAdd, &QPushButton::clicked, q, [=](){
        //
        const Icd::ChannelType channelType = groupChannelPane->currentChannelType();
        if (channelType == Icd::ChannelInvalid) {
            return;
        }
        //
        Icd::ChannelPtr channel = Icd::Channel::createInstance(channelType);
        if (!channel) {
            return;
        }
        //
        Icd::WorkerPtr worker = Icd::WorkerPtr(new Icd::Worker(channel));
        Icd::WorkerPool::getInstance()->appendWorker(worker);
        Icd::WorkerPool::getInstance()->saveConfig();
        groupChannelPane->addWorker(worker);
        groupChannelPane->setCurrentRowIndex(groupChannelPane->currentRowCount() - 1);
    });
    connect(buttonClear, &QPushButton::clicked, q, [=](){
        int result = QMessageBox::warning(q, tr("Warning"),
                                          tr("Clear channel config could not be restore, continue?"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result != QMessageBox::Yes) {
            return;
        }
        Icd::WorkerPool::getInstance()->clearWorker();
    });

    //
    JMain::restoreWidgetState(groupChannelPane->tabWidget());
}

void ComMgrWidgetPrivate::updateUi()
{
    groupChannelPane->updateAllTab();
}

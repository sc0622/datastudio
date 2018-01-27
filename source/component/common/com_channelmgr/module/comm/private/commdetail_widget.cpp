#include "precomp.h"
#include "commdetail_widget.h"
#include "file/filedetail_widget.h"
#include "serial/serialdetail_widget.h"
#include "udp/udpdetail_widget.h"

CommDetailWidget::CommDetailWidget(QWidget *parent)
    : QFrame(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 4, 4);

    d_stackedWidget = new QStackedWidget(this);
    vertLayoutMain->addWidget(d_stackedWidget);

    d_fileDetailWidget = new FileDetailWidget(this);
    d_stackedWidget->addWidget(d_fileDetailWidget);

    d_serialDetailWidget = new SerialDetailWidget(this);
    d_stackedWidget->addWidget(d_serialDetailWidget);

    d_udpDetailWidget = new UdpDetailWidget(this);
    d_stackedWidget->addWidget(d_udpDetailWidget);
}

void CommDetailWidget::updateUi(const Icd::WorkerPtr &worker)
{
    //
    if (!worker) {
        d_stackedWidget->setCurrentIndex(-1);
        d_serialDetailWidget->updateUi(Icd::WorkerPtr(0));
        d_udpDetailWidget->updateUi(Icd::WorkerPtr(0));
        d_fileDetailWidget->updateUi(Icd::WorkerPtr(0));
    } else {
        //
        switch (worker->channel()->channelType()) {
        case Icd::ChannelFile:
            d_stackedWidget->setCurrentIndex(0);
            d_fileDetailWidget->updateUi(worker);
            break;
        case Icd::ChannelSerial:
            d_stackedWidget->setCurrentIndex(1);
            d_serialDetailWidget->updateUi(worker);
            break;
        case Icd::ChannelUdp:
            d_stackedWidget->setCurrentIndex(2);
            d_udpDetailWidget->updateUi(worker);
            break;
        default:
            d_stackedWidget->setCurrentIndex(-1);
            break;
        }
    }
}

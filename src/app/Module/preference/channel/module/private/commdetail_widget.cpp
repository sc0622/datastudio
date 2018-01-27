#include "precomp.h"
#include "commdetail_widget.h"
#include "file/filedetail_widget.h"
#include "serial/serialdetail_widget.h"
#include "udp/udpdetail_widget.h"

CommDetailWidget::CommDetailWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    d_scrollArea = new QScrollArea(this);
    d_scrollArea->setWidgetResizable(true);
    d_scrollArea->setFrameShape(QFrame::NoFrame);
    vertLayoutMain->addWidget(d_scrollArea);

    d_stackedWidget = new QStackedWidget(d_scrollArea);
    d_stackedWidget->setFrameShape(QFrame::NoFrame);
    d_scrollArea->setWidget(d_stackedWidget);

    d_fileDetailWidget = new FileDetailWidget(d_scrollArea);
    d_stackedWidget->addWidget(d_fileDetailWidget);

    d_serialDetailWidget = new SerialDetailWidget(d_scrollArea);
    d_stackedWidget->addWidget(d_serialDetailWidget);

    d_udpDetailWidget = new UdpDetailWidget(d_scrollArea);
    d_stackedWidget->addWidget(d_udpDetailWidget);
}

void CommDetailWidget::updateUi(const Icd::WorkerPtr &worker)
{
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

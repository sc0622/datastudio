#include "precomp.h"
#include "worker_label_p.h"
#include "icdworker_pool.h"

namespace Icd {

WorkerLabelPrivate::WorkerLabelPrivate(const Icd::WorkerPtr &worker,
                                       WorkerLabel *q)
    : q_ptr(q)
    , worker(worker)
    , attributes(ChannelWidget::NoOperate)
    , buttonOpen(0)
    , buttonSwitchRecv(0)
    , buttonRemove(0)
    , labelIndicator(0)
{

}

WorkerLabelPrivate::~WorkerLabelPrivate()
{

}

void WorkerLabelPrivate::init(ChannelWidget::OperateAttributes attrs)
{
    Q_Q(WorkerLabel);

    attributes = attrs;

    q->setFixedHeight(60);

    QHBoxLayout *horiLayoutMain = new QHBoxLayout(q);
    horiLayoutMain->setContentsMargins(4, 0, 0, 0);
    horiLayoutMain->setSpacing(4);

    labelChannelIcon = new QLabel(q);
    labelChannelIcon->setObjectName("channelIcon");
    labelChannelIcon->setScaledContents(true);
    labelChannelIcon->setFixedSize(42, 42);
    horiLayoutMain->addWidget(labelChannelIcon);

    labelChannelType = new QLabel(q);
    labelChannelType->setObjectName("channelType");
    labelChannelType->setFixedSize(70, 60);
    labelChannelType->setMinimumWidth(70);
    horiLayoutMain->addWidget(labelChannelType);

    QVBoxLayout *vertLayoutCenter = new QVBoxLayout();
    vertLayoutCenter->setContentsMargins(0, 0, 0, 0);
    vertLayoutCenter->setSpacing(1);
    horiLayoutMain->addLayout(vertLayoutCenter);

    labeName = new QLabel(q);
    labeName->setObjectName("labelName");
    labeName->setFixedHeight(30);
    vertLayoutCenter->addWidget(labeName);

    labelDetail = new QLabel(q);
    labelDetail->setObjectName("detail");
    labelDetail->setFixedHeight(30);
    vertLayoutCenter->addWidget(labelDetail);

    if (attrs & ChannelWidget::OperateOpen) {
        buttonOpen = new QPushButton(QStringLiteral("��"), q);
        buttonOpen->setObjectName("buttonOpen");
        buttonOpen->setFixedSize(70, 35);
        buttonOpen->setMinimumWidth(70);
        horiLayoutMain->addWidget(buttonOpen);

        horiLayoutMain->addSpacing(5);

        //
        QObject::connect(buttonOpen, &QPushButton::clicked, q, [=](){
            if (worker) {
                if (worker->isOpen()) {
                    worker->close();
                } else {
                    worker->open();
                }
            }
        });
        QObject::connect(worker.get(), &Icd::Worker::opened, q, [=](){
            if (buttonOpen) {
                buttonOpen->setText(QStringLiteral("�ر�"));
            }
        });
    }

    if (attrs & ChannelWidget::OperateSwitchRecv) {
        buttonSwitchRecv = new QPushButton(QStringLiteral("��������"), q);
        buttonSwitchRecv->setObjectName("buttonSwitchRecv");
        buttonSwitchRecv->setFixedSize(70, 35);
        buttonSwitchRecv->setMinimumWidth(70);
        horiLayoutMain->addWidget(buttonSwitchRecv);

        horiLayoutMain->addSpacing(10);

        //
        QObject::connect(buttonSwitchRecv, &QPushButton::clicked, q, [=](){
            if (worker) {
                if (worker->workerRecv()->isRunning()) {
                    worker->workerRecv()->stop();
                } else {
                    if (!worker->isOpen()) {
                        if (!worker->open()) {
                            return;
                        }
                    }
                    worker->workerRecv()->start();
                }
            }
        });
        QObject::connect(worker->workerRecv().get(), &Icd::WorkerRecv::toggled, q, [=](bool checked){
            if (buttonSwitchRecv) {
                if (checked) {
                    buttonSwitchRecv->setText(QStringLiteral("ֹͣ����"));
                } else {
                    buttonSwitchRecv->setText(QStringLiteral("��������"));
                }
            }
        });
    }

    if (attrs & ChannelWidget::OperateRemove) {
        buttonRemove = new QPushButton(QStringLiteral("ɾ��"), q);
        buttonRemove->setObjectName("buttonRemove");
        buttonRemove->setFixedSize(70, 35);
        buttonRemove->setMinimumWidth(70);
        horiLayoutMain->addWidget(buttonRemove);

        horiLayoutMain->addSpacing(10);

        //
        QObject::connect(buttonRemove, &QPushButton::clicked, q, [=](){
            if (worker) {
                int result = QMessageBox::warning(q, QStringLiteral("����"),
                                                  QStringLiteral("ɾ��ͨ�����ý����ɻָ�ԭ�����ã��Ƿ������"),
                                                  QMessageBox::Yes | QMessageBox::No);
                if (result != QMessageBox::Yes) {
                    return;
                }
                //
                setDirty();
                //
                Icd::WorkerPool::getInstance()->removeWorker(worker);
                //
                Icd::WorkerPool::getInstance()->saveConfig();
            }
        });
    }

    labelIndicator = new QLabel(q);
    labelIndicator->setFixedSize(15, 30);
    horiLayoutMain->addWidget(labelIndicator);

    //
    QObject::connect(worker.get(), &Icd::Worker::channelConfigChanged, q, [=](){
        if (worker) {
            // name
            labeName->setText(QString::fromStdString(worker->channel()->name()));
            // desc
            labelDetail->setText(QString::fromStdString(worker->channel()->desc()));
        }
    });
    QObject::connect(worker.get(), &Icd::Worker::closed, q, [=](){
        if (buttonOpen) {
            buttonOpen->setText(QStringLiteral("��"));
        }
        if (buttonSwitchRecv) {
            buttonSwitchRecv->setText(QStringLiteral("��������"));
        }
    });
}

void WorkerLabelPrivate::updateUi()
{
    // channelType
    switch (worker->channel()->channelType()) {
    case Icd::ChannelSerial:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/serial.png"));
        labelChannelType->setText(QStringLiteral("����"));
        break;
    case Icd::ChannelUdp:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/udp.png"));
        labelChannelType->setText(QStringLiteral("UDP"));
        break;
    case Icd::ChannelFile:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/file.png"));
        labelChannelType->setText(QStringLiteral("�ļ�"));
        break;
    default:
        break;
    }

    // name
    labeName->setText(QString::fromStdString(worker->channel()->name()));
    // desc
    labelDetail->setText(QString::fromStdString(worker->channel()->desc()));
    // switchRecv
    if (buttonSwitchRecv) {
        buttonSwitchRecv->setText(worker->workerRecv()->isRunning()
                                  ? QStringLiteral("ֹͣ����")
                                  : QStringLiteral("��������"));
    }
}

void WorkerLabelPrivate::setDirty()
{
    Q_Q(WorkerLabel);
    //
    if (worker) {
        worker->disconnect(q);
        worker->workerRecv()->disconnect(q);
    }
}

ChannelWidget::OperateAttributes WorkerLabelPrivate::attrs() const
{
    return attributes;
}

} // end of namespace Icd

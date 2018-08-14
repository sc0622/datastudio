#include "precomp.h"
#include "worker_label_p.h"
#include "icdworker/icdworker_pool.h"

namespace Icd {

WorkerLabelPrivate::WorkerLabelPrivate(const Icd::WorkerPtr &worker, WorkerLabel *q)
    : J_QPTR(q)
    , worker(worker)
    , attributes(ChannelWidget::NoOperate)
    , buttonOpen(nullptr)
    , buttonSwitchRecv(nullptr)
    , buttonRemove(nullptr)
    , labelIndicator(nullptr)
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
    horiLayoutMain->setContentsMargins(3, 0, 0, 0);
    horiLayoutMain->setSpacing(3);

    labelChannelIcon = new QLabel(q);
    labelChannelIcon->setObjectName("channelIcon");
    labelChannelIcon->setScaledContents(true);
    labelChannelIcon->setFixedSize(42, 42);
    horiLayoutMain->addWidget(labelChannelIcon);

    labelChannelType = new QLabel(q);
    labelChannelType->setObjectName("channelType");
    labelChannelType->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    horiLayoutMain->addWidget(labelChannelType);

    horiLayoutMain->addSpacing(6);

    QVBoxLayout *vertLayoutCenter = new QVBoxLayout();
    vertLayoutCenter->setContentsMargins(0, 8, 0, 8);
    vertLayoutCenter->setSpacing(6);
    horiLayoutMain->addLayout(vertLayoutCenter);

    labelName = new QLineEdit(q);
    labelName->setObjectName("labelName");
    labelName->setDisabled(true);
    vertLayoutCenter->addWidget(labelName);

    labelDetail = new QLineEdit(q);
    labelDetail->setObjectName("labelDetail");
    labelDetail->setDisabled(true);
    vertLayoutCenter->addWidget(labelDetail);

    if (attrs & ChannelWidget::OperateOpen) {
        buttonOpen = new QPushButton(WorkerLabel::tr("Open"), q);
        buttonOpen->setProperty("_flat_", true);
        buttonOpen->setFixedSize(70, 40);
        horiLayoutMain->addWidget(buttonOpen);
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
                buttonOpen->setText(WorkerLabel::tr("Close"));
            }
        });
    }

    if (attrs & ChannelWidget::OperateSwitchRecv) {
        buttonSwitchRecv = new QPushButton(WorkerLabel::tr("Start Receiving"), q);
        buttonSwitchRecv->setProperty("_flat_", true);
        buttonSwitchRecv->setFixedSize(70, 40);
        horiLayoutMain->addWidget(buttonSwitchRecv);
        //
        QObject::connect(buttonSwitchRecv, &QPushButton::clicked, q, [=](){
            if (worker) {
                if (worker->workerRecv()->isRunning()) {
                    worker->workerRecv()->stop();
                } else {
                    if (!worker->open()) {
                        return;
                    }
                    worker->workerRecv()->start();
                }
            }
        });
        QObject::connect(worker->workerRecv().get(), &Icd::WorkerRecv::toggled, q, [=](bool checked){
            if (buttonSwitchRecv) {
                if (checked) {
                    buttonSwitchRecv->setText(WorkerLabel::tr("Stop Receiving"));
                } else {
                    buttonSwitchRecv->setText(WorkerLabel::tr("Start Receiving"));
                }
            }
        });
    }

    if (attrs & ChannelWidget::OperateRemove) {
        buttonRemove = new QPushButton(WorkerLabel::tr("Remove"), q);
        buttonRemove->setProperty("_flat_", true);
        buttonRemove->setFixedSize(70, 40);
        horiLayoutMain->addWidget(buttonRemove);
        //
        QObject::connect(buttonRemove, &QPushButton::clicked, q, [=](){
            if (worker) {
                int result = QMessageBox::warning(q, WorkerLabel::tr("Warning"),
                                                  WorkerLabel::tr("Removing channel configuration won't restore, continue?"),
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
    labelIndicator->setFixedSize(8, 15);
    horiLayoutMain->addWidget(labelIndicator);

    QObject::connect(worker.get(), &Icd::Worker::channelConfigChanged, q, [=](){
        if (worker) {
            // name
            labelName->setText(QString::fromStdString(worker->channel()->name()));
            // desc
            labelDetail->setText(QString::fromStdString(worker->channel()->desc()));
            //
            updateDetailText();
        }
    });
    QObject::connect(worker.get(), &Icd::Worker::closed, q, [=](){
        if (buttonOpen) {
            buttonOpen->setText(WorkerLabel::tr("Open"));
        }
        if (buttonSwitchRecv) {
            buttonSwitchRecv->setText(WorkerLabel::tr("Start Receiving"));
        }
    });
}

void WorkerLabelPrivate::updateUi()
{
    // channelType
    switch (worker->channel()->channelType()) {
    case Icd::ChannelSerial:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/serial.png"));
        labelChannelType->setText(WorkerLabel::tr("Serial Channel"));
        break;
    case Icd::ChannelUdp:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/udp.png"));
        labelChannelType->setText(WorkerLabel::tr("UDP Channel"));
        break;
    case Icd::ChannelFile:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/file.png"));
        labelChannelType->setText(WorkerLabel::tr("File Channel"));
        break;
    default:
        break;
    }

    // name
    labelName->setText(QString::fromStdString(worker->channel()->name()));
    // desc
    labelDetail->setText(QString::fromStdString(worker->channel()->desc()));
    // switchRecv
    if (buttonSwitchRecv) {
        buttonSwitchRecv->setText(worker->workerRecv()->isRunning()
                                  ? WorkerLabel::tr("Stop Receiving")
                                  : WorkerLabel::tr("Start Receiving"));
    }
    //
    updateDetailText();
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

void WorkerLabelPrivate::updateDetailText()
{
    const QString text = elidedText(labelDetail->font(), labelDetail->text(), 300);
    labelDetail->setText(text);
}

QString WorkerLabelPrivate::elidedText(const QFont &font, const QString &text, int width)
{
    QFontMetrics fontMetrics(font);
    const int fontWidth = fontMetrics.width(text);
    if(fontWidth >= width) {
        return fontMetrics.elidedText(text, Qt::ElideMiddle, width);
    } else {
        return text;
    }
}

} // end of namespace Icd

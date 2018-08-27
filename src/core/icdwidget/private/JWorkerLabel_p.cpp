#include "precomp.h"
#include "JWorkerLabel_p.h"
#include "icdworker/icdworker_pool.h"

namespace Icd {

JWorkerLabelPrivate::JWorkerLabelPrivate(const Icd::WorkerPtr &worker, JWorkerLabel *q)
    : J_QPTR(q)
    , worker(worker)
    , attributes(JChannelPane::NoOperate)
    , buttonOpen(nullptr)
    , buttonSwitchRecv(nullptr)
    , buttonRemove(nullptr)
    , labelIndicator(nullptr)
{

}

JWorkerLabelPrivate::~JWorkerLabelPrivate()
{

}

void JWorkerLabelPrivate::init(JChannelPane::OperateAttributes attrs)
{
    Q_Q(JWorkerLabel);

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

    if (attrs & JChannelPane::OperateOpen) {
        buttonOpen = new QPushButton(JWorkerLabel::tr("Open"), q);
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
                buttonOpen->setText(JWorkerLabel::tr("Close"));
            }
        });
    }

    if (attrs & JChannelPane::OperateSwitchRecv) {
        buttonSwitchRecv = new QPushButton(JWorkerLabel::tr("Start Receiving"), q);
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
                    buttonSwitchRecv->setText(JWorkerLabel::tr("Stop Receiving"));
                } else {
                    buttonSwitchRecv->setText(JWorkerLabel::tr("Start Receiving"));
                }
            }
        });
    }

    if (attrs & JChannelPane::OperateRemove) {
        buttonRemove = new QPushButton(JWorkerLabel::tr("Remove"), q);
        buttonRemove->setProperty("_flat_", true);
        buttonRemove->setFixedSize(70, 40);
        horiLayoutMain->addWidget(buttonRemove);
        //
        QObject::connect(buttonRemove, &QPushButton::clicked, q, [=](){
            if (worker) {
                int result = QMessageBox::warning(q, JWorkerLabel::tr("Warning"),
                                                  JWorkerLabel::tr("Removing channel configuration won't restore, continue?"),
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
            buttonOpen->setText(JWorkerLabel::tr("Open"));
        }
        if (buttonSwitchRecv) {
            buttonSwitchRecv->setText(JWorkerLabel::tr("Start Receiving"));
        }
    });
}

void JWorkerLabelPrivate::updateUi()
{
    // channelType
    switch (worker->channel()->channelType()) {
    case Icd::ChannelSerial:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/serial.png"));
        labelChannelType->setText(JWorkerLabel::tr("Serial Channel"));
        break;
    case Icd::ChannelUdp:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/udp.png"));
        labelChannelType->setText(JWorkerLabel::tr("UDP Channel"));
        break;
    case Icd::ChannelFile:
        labelChannelIcon->setPixmap(QPixmap(":/icdwidget/image/file.png"));
        labelChannelType->setText(JWorkerLabel::tr("File Channel"));
        break;
    default:
        break;
    }

    // name
    labelName->setText(QString::fromStdString(worker->channel()->name()));
    // desc
    labelDetail->setText(QString::fromStdString(worker->channel()->desc()));
    // open status
    if (buttonOpen) {
        buttonOpen->setText(worker->channel()->isOpen()
                            ? JWorkerLabel::tr("Close")
                            : JWorkerLabel::tr("Open"));
    }
    // switchRecv
    if (buttonSwitchRecv) {
        buttonSwitchRecv->setText(worker->workerRecv()->isRunning()
                                  ? JWorkerLabel::tr("Stop Receiving")
                                  : JWorkerLabel::tr("Start Receiving"));
    }
    //
    updateDetailText();
}

void JWorkerLabelPrivate::setDirty()
{
    Q_Q(JWorkerLabel);
    //
    if (worker) {
        worker->disconnect(q);
        worker->workerRecv()->disconnect(q);
    }
}

JChannelPane::OperateAttributes JWorkerLabelPrivate::attrs() const
{
    return attributes;
}

void JWorkerLabelPrivate::updateDetailText()
{
    const QString text = elidedText(labelDetail->font(), labelDetail->text(), 300);
    labelDetail->setText(text);
}

QString JWorkerLabelPrivate::elidedText(const QFont &font, const QString &text, int width)
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

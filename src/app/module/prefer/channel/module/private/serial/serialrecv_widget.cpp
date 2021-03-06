#include "precomp.h"
#include "serialrecv_widget.h"

SerialRecvWidget::SerialRecvWidget(QWidget *parent)
    : QWidget(parent)
    , d_worker(Icd::WorkerPtr())
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBoxSettings = new QGroupBox(tr("Receiving settings"), this);
    vertLayoutMain->addWidget(groupBoxSettings);

    QVBoxLayout *vertLayoutSettings = new QVBoxLayout(groupBoxSettings);

    QFormLayout *formLayoutSettings = new QFormLayout();
    formLayoutSettings->setLabelAlignment(Qt::AlignRight);
    formLayoutSettings->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    vertLayoutSettings->addLayout(formLayoutSettings);

    d_spinBoxInterval = new QSpinBox(this);
    d_spinBoxInterval->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    d_spinBoxInterval->setRange(1, INT_MAX);
    d_spinBoxInterval->setSuffix(" ms");
    formLayoutSettings->addRow(tr("Peroid of receiving:"), d_spinBoxInterval);

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutSettings->addLayout(horiLayoutButtons);

    d_buttonRestore = new QPushButton(tr("Restore"), this);
    d_buttonRestore->setFixedSize(70, 30);
    horiLayoutButtons->addWidget(d_buttonRestore);

    horiLayoutButtons->addSpacing(10);

    d_buttonApply = new QPushButton(tr("Apply"), this);
    d_buttonApply->setFixedSize(70, 30);
    horiLayoutButtons->addWidget(d_buttonApply);

    QGroupBox *groupBoxStatus = new QGroupBox(tr("Status of receiving"), this);
    vertLayoutMain->addWidget(groupBoxStatus);

    QVBoxLayout *vertLayoutStatus = new QVBoxLayout(groupBoxStatus);

    QFormLayout *formLayoutStatus = new QFormLayout();
    formLayoutStatus->setLabelAlignment(Qt::AlignRight);
    formLayoutStatus->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    vertLayoutStatus->addLayout(formLayoutStatus);

    d_editSequence = new QLineEdit(this);
    d_editSequence->setReadOnly(true);
    formLayoutStatus->addRow(tr("Sequence of reciving:"), d_editSequence);

    //
    auto enableButtons = [=](bool enabled){
        d_buttonApply->setEnabled(enabled);
        d_buttonRestore->setEnabled(enabled);
    };

    connect(d_spinBoxInterval, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](){
        enableButtons(true);
    });
    connect(d_buttonApply, &QPushButton::clicked, this, [=](){
        //
        if (!d_worker) {
            return;
        }
        //
        Icd::SerialChannelPtr channel = JHandlePtrCast<Icd::SerialChannel>(d_worker->channel());
        if (!channel) {
            return;
        }
        //
        bool isRunning = d_worker->workerRecv()->isRunning();
        d_worker->workerRecv()->stop();
        // interval
        d_worker->workerRecv()->setInterval(d_spinBoxInterval->value());
        // save configuration
        Icd::WorkerPool::getInstance()->saveConfig();
        //
        if (isRunning) {
            d_worker->workerRecv()->start();
        }
        //
        enableButtons(false);
        //
        emit d_worker->channelConfigChanged();
    });
    connect(d_buttonRestore, &QPushButton::clicked, this, [=](){
        //
        updateUi(d_worker);
        //
        enableButtons(false);
    });
}

SerialRecvWidget::~SerialRecvWidget()
{

}

void SerialRecvWidget::updateUi(const Icd::WorkerPtr &worker)
{
    if (d_worker) {
        d_worker->disconnect(this);
        d_worker->workerRecv()->disconnect(this);
    }

    // clear

    //
    d_worker = worker;
    //
    if (!d_worker) {
        return;
    }

    //
    d_spinBoxInterval->setValue(d_worker->workerRecv()->interval());

    d_buttonApply->setEnabled(false);
    d_buttonRestore->setEnabled(false);
}

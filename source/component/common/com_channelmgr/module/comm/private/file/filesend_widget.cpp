#include "precomp.h"
#include "filesend_widget.h"
#include "icdcomm/icdcomm_filechannel.h"

FileSendWidget::FileSendWidget(QWidget *parent)
    : QWidget(parent)
    , d_worker(Icd::WorkerPtr(0))
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("发送设置"), this);
    vertLayoutMain->addWidget(groupBox);

    QVBoxLayout *vertLayoutGroupBox = new QVBoxLayout(groupBox);

    d_formLayout = new QFormLayout();
    vertLayoutGroupBox->addLayout(d_formLayout);

    d_checkBoxPeriodic = new QCheckBox(QStringLiteral(""), this);
    d_formLayout->addRow(QStringLiteral("自动发送："), d_checkBoxPeriodic);

    d_spinBoxInterval = new QSpinBox(this);
    d_spinBoxInterval->setRange(1, INT_MAX);
    d_spinBoxInterval->setSuffix(" ms");
    d_formLayout->addRow(QStringLiteral("发送周期："), d_spinBoxInterval);

    vertLayoutGroupBox->addSpacing(20);

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutGroupBox->addLayout(horiLayoutButtons);

    horiLayoutButtons->addStretch();

    d_buttonSwitch = new QPushButton(QStringLiteral("启动"), this);
    d_buttonSwitch->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(d_buttonSwitch);

    horiLayoutButtons->addSpacing(10);

    d_buttonRestore = new QPushButton(QStringLiteral("恢复"), this);
    d_buttonRestore->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(d_buttonRestore);

    horiLayoutButtons->addSpacing(10);

    d_buttonApply = new QPushButton(QStringLiteral("应用"), this);
    d_buttonApply->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(d_buttonApply);

    horiLayoutButtons->addSpacing(15);

    //
    auto enableButtons = [=](bool enabled){
        d_buttonSwitch->setEnabled(!enabled);
        d_buttonApply->setEnabled(enabled);
        d_buttonRestore->setEnabled(enabled);
    };

    //
    connect(d_checkBoxPeriodic, &QCheckBox::toggled, this, [=](bool checked){
        d_spinBoxInterval->setVisible(checked);
        d_formLayout->labelForField(d_spinBoxInterval)->setVisible(checked);
        d_buttonSwitch->setVisible(checked);
        enableButtons(true);
    });
    connect(d_spinBoxInterval, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [=](){
        enableButtons(true);
    });
    connect(d_buttonSwitch, &QPushButton::clicked,
            this, &FileSendWidget::onButtonSwitchClicked);
    connect(d_buttonApply, &QPushButton::clicked, this, [=](){
        //
        if (!d_worker) {
            return;
        }
        //
        Icd::FileChannelPtr channel =
                JHandlePtrCast<Icd::FileChannel, Icd::Channel>(d_worker->channel());
        if (!channel) {
            return;
        }
        //
        bool isSendRunning = d_worker->workerSend()->isRunning();
        d_worker->workerSend()->stop();
        // timeEvent
        if (d_checkBoxPeriodic->isChecked()) {
            d_worker->workerSend()->setTimeEvent(Icd::WorkerTrans::TimePeriodic);
        } else {
            d_worker->workerSend()->setTimeEvent(Icd::WorkerTrans::TimeOneShot);
        }
        // interval
        d_worker->workerSend()->setInterval(d_spinBoxInterval->value());
        // save configuration
        Icd::WorkerPool::getInstance()->saveConfig();
        //
        if (isSendRunning) {
            d_worker->workerSend()->start();
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

FileSendWidget::~FileSendWidget()
{

}

void FileSendWidget::updateUi(const Icd::WorkerPtr &worker)
{
    if (d_worker) {
        d_worker->disconnect(this);
        d_worker->workerSend()->disconnect(this);
    }

    // clear
    d_checkBoxPeriodic->setChecked(false);
    d_buttonSwitch->hide();
    d_spinBoxInterval->hide();
    d_formLayout->labelForField(d_spinBoxInterval)->hide();
    d_buttonSwitch->hide();
    d_buttonSwitch->setText(QStringLiteral("启动"));

    //
    d_worker = worker;
    //
    if (!d_worker) {
        return;
    }

    //
    Icd::WorkerTrans::TimeEvent timeEvent = d_worker->workerSend()->timeEvent();
    d_checkBoxPeriodic->setChecked(timeEvent == Icd::WorkerTrans::TimePeriodic);
    d_spinBoxInterval->setValue((int)d_worker->workerSend()->interval());
    if (d_worker->workerSend()->isRunning()) {
        d_buttonSwitch->setText(QStringLiteral("停止"));
    } else {
        d_buttonSwitch->setText(QStringLiteral("启动"));
    }

    //
    connect(d_worker.get(), &Icd::Worker::closed,
            this, &FileSendWidget::onWorkerClosed);
    connect(d_worker->workerSend().get(), &Icd::WorkerSend::started,
            this, &FileSendWidget::onWorkerStarted);
    connect(d_worker->workerSend().get(), &Icd::WorkerSend::stopped,
            this, &FileSendWidget::onWorkerStopped);

    //
    d_buttonSwitch->setEnabled(true);
    d_buttonApply->setEnabled(false);
    d_buttonRestore->setEnabled(false);
}

void FileSendWidget::onWorkerClosed()
{
    d_buttonSwitch->setText(QStringLiteral("启动"));
}

void FileSendWidget::onWorkerStarted()
{
    d_buttonSwitch->setText(QStringLiteral("停止"));
}

void FileSendWidget::onWorkerStopped()
{
    d_buttonSwitch->setText(QStringLiteral("启动"));
}

void FileSendWidget::onButtonSwitchClicked()
{
    if (!d_worker) {
        return;
    }

    if (d_worker->workerSend()->isRunning()) {
        d_worker->workerSend()->stop();
    } else {
        if (!d_worker->isOpen()) {
            if (!d_worker->open()) {
                return;
            }
        }
        d_worker->workerSend()->start();
    }
}

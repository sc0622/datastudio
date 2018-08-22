#include "precomp.h"
#include "udpsetting_widget.h"
#include "jwt/jipaddressedit.h"
#include "icdcomm/icdcomm_udpchannel.h"
#include "icdworker/icdworker_pool.h"
#include "icdwidget/JGroupChannelPane.h"

UdpSettingWidget::UdpSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBox = new QGroupBox(tr("Network settings"), this);
    vertLayoutMain->addWidget(groupBox);

    QVBoxLayout *vertLayoutGroupBox = new QVBoxLayout(groupBox);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    vertLayoutGroupBox->addLayout(formLayout);

    d_editLocalIP = new JIPAddressEdit(this);
    formLayout->addRow(tr("Local address:"), d_editLocalIP);

    d_spinBoxLocalPort = new QSpinBox(this);
    d_spinBoxLocalPort->setRange(1, 65535);
    formLayout->addRow(tr("Local port:"), d_spinBoxLocalPort);

    d_editRemoteIP = new JIPAddressEdit(this);
    formLayout->addRow(tr("Remove address:"), d_editRemoteIP);

    d_spinBoxRemotePort = new QSpinBox(this);
    d_spinBoxRemotePort->setRange(1, 65535);
    formLayout->addRow(tr("Remote port:"), d_spinBoxRemotePort);

    d_checkBoxReadOnly = new QCheckBox(tr("Read Only"), this);
    d_checkBoxReadOnly->setChecked(true);
    formLayout->addRow(tr("Open Mode:"), d_checkBoxReadOnly);

    d_checkBoxWriteOnly = new QCheckBox(tr("Write Only"), this);
    d_checkBoxWriteOnly->setChecked(true);
    formLayout->addRow(" ", d_checkBoxWriteOnly);

    d_editName = new QLineEdit(this);
    formLayout->addRow(tr("Name of channel:"), d_editName);

    QHBoxLayout *horiLayoutRelayer = new QHBoxLayout();
    horiLayoutRelayer->setSpacing(0);
    formLayout->addRow(tr("Relayer channel:"), horiLayoutRelayer);

    d_editRelayer = new QLineEdit(this);
    horiLayoutRelayer->addWidget(d_editRelayer);

    QPushButton *buttonRelayer = new QPushButton("...", this);
    buttonRelayer->setFixedSize(d_editRelayer->sizeHint().height() + 5, d_editRelayer->sizeHint().height());
    buttonRelayer->setStyleSheet("border-radius: 1px;");
    horiLayoutRelayer->addWidget(buttonRelayer);

    vertLayoutGroupBox->addSpacing(20);

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutGroupBox->addLayout(horiLayoutButtons);

    horiLayoutButtons->addStretch();

    d_buttonRestore = new QPushButton(tr("Restore"), this);
    d_buttonRestore->setFixedSize(70, 30);
    horiLayoutButtons->addWidget(d_buttonRestore);

    horiLayoutButtons->addSpacing(10);

    d_buttonApply = new QPushButton(tr("Apply"), this);
    d_buttonApply->setFixedSize(70, 30);
    horiLayoutButtons->addWidget(d_buttonApply);

    horiLayoutButtons->addSpacing(15);

    // init

    // local - IP
    d_editLocalIP->setText("127.0.0.1");
    // local - port
    d_spinBoxLocalPort->setValue(8080);
    // remote - IP
    d_editRemoteIP->setText("127.0.0.1");
    // remote - port
    d_spinBoxRemotePort->setValue(8080);

    //
    auto enableButtons = [=](bool enabled){
        d_buttonApply->setEnabled(enabled);
        d_buttonRestore->setEnabled(enabled);
    };

    //
    connect(d_editLocalIP, &JIPAddressEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_spinBoxLocalPort, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](){
        enableButtons(true);
    });
    connect(d_editRemoteIP, &JIPAddressEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_spinBoxRemotePort, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](){
        enableButtons(true);
    });
    connect(d_checkBoxReadOnly, &QCheckBox::stateChanged, this, [=](int){
        enableButtons(true);
    });
    connect(d_checkBoxWriteOnly, &QCheckBox::stateChanged, this, [=](int){
        enableButtons(true);
    });
    connect(d_editName, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_editRelayer, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(buttonRelayer, &QPushButton::clicked, this, [=](){
        //
        Icd::JGroupChannelDlg *groupChannelDlg = new Icd::JGroupChannelDlg(this);
        if (groupChannelDlg->exec() != QDialog::Accepted) {
            return; // cancel
        }

        //
        groupChannelDlg->deleteLater();
        Icd::WorkerPtr selectedWorker = groupChannelDlg->selectedWorker();
        if (selectedWorker) {
            const Icd::ChannelPtr channel = selectedWorker->channel();
            d_editRelayer->setProperty("relayer", QString::fromStdString(channel->identity()));
            d_editRelayer->setText(QString::fromStdString(channel->name()));
        } else {
            d_editRelayer->setProperty("relayer", QVariant::Invalid);
            d_editRelayer->clear();
        }
    });
    connect(d_buttonApply, &QPushButton::clicked, this, [=](){
        //
        if (!d_worker) {
            return;
        }
        //
        Icd::UdpChannelPtr channel =
                JHandlePtrCast<Icd::UdpChannel, Icd::Channel>(d_worker->channel());
        if (!channel) {
            return;
        }
        //
        if (channel->isOpen()) {
            const QString message = tr("Channel \"%1\" has been opened and cannot modify!");
            QMessageBox::warning(this, tr("Warning"),
                                 message.arg(QString::fromStdString(channel->name())));
            return;
        }
        //
        bool isSendRunning = d_worker->workerSend()->isRunning();
        bool isRecvRunning = d_worker->workerRecv()->isRunning();
        d_worker->workerSend()->stop();
        d_worker->workerRecv()->stop();
        // local - ip
        channel->setLocalIP(d_editLocalIP->text().toStdString());
        // local - port
        channel->setLocalPort(d_spinBoxLocalPort->value());
        // remote - ip
        channel->setRemoteIP(d_editRemoteIP->text().toStdString());
        // remote -port
        channel->setRemotePort(d_spinBoxRemotePort->value());
        // OpenMode
        unsigned int openMode = Icd::UdpChannel::NotOpen;
        if (d_checkBoxReadOnly->isChecked()) {
            openMode |= Icd::UdpChannel::ReadOnly;
        }
        if (d_checkBoxWriteOnly->isChecked()) {
            openMode |= Icd::UdpChannel::WriteOnly;
        }
        channel->setOpenMode(static_cast<Icd::UdpChannel::OpenMode>(openMode));
        // name
        channel->setName(d_editName->text().toStdString());
        // relayer
        Icd::WorkerPtr relayer = d_worker->relayer();
        const QString relayerName = d_editRelayer->text();
        if (relayerName.isEmpty()) {
            relayer = Icd::WorkerPtr();
        } else {
            QString relayerIdentity = d_editRelayer->property("relayer").toString();
            if (relayerIdentity.isEmpty()) {
                Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                        ->workerByChannelName(relayerName.toStdString());
                if (worker && worker != d_worker) {
                    d_editRelayer->setProperty("relayer", QString::fromStdString(
                                                   d_worker->channel()->identity()));
                    relayer = worker;
                }
            } else {
                Icd::WorkerPtr worker = Icd::WorkerPool::getInstance()
                        ->workerByChannelIdentity(relayerIdentity.toStdString());
                if (worker && worker != d_worker) {
                    relayer = worker;
                } else {
                    relayer = Icd::WorkerPtr();
                }
            }
        }
        d_worker->setRelayer(relayer);
        // save configuration
        Icd::WorkerPool::getInstance()->saveConfig();
        //
        if (isSendRunning) {
            d_worker->workerSend()->start();
        }
        if (isRecvRunning) {
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

UdpSettingWidget::~UdpSettingWidget()
{

}

void UdpSettingWidget::updateUi(const Icd::WorkerPtr &worker)
{
    //
    d_worker = worker;

    // clear

    // local - IP
    d_editLocalIP->setText("127.0.0.1");
    // local - port
    d_spinBoxLocalPort->setValue(8080);
    // remote - IP
    d_editRemoteIP->setText("127.0.0.1");
    // remote - port
    d_spinBoxRemotePort->setValue(8080);
    // read
    d_checkBoxReadOnly->setChecked(true);
    // write
    d_checkBoxWriteOnly->setChecked(true);
    // relayer
    d_editRelayer->clear();


    //
    if (!d_worker) {
        return;
    }

    //
    Icd::UdpChannelPtr channel =
            JHandlePtrCast<Icd::UdpChannel, Icd::Channel>(d_worker->channel());
    if (!channel) {
        return;
    }

    // local - IP
    d_editLocalIP->setText(QString::fromStdString(channel->localIP()));
    // local - port
    d_spinBoxLocalPort->setValue(channel->localPort());
    // remote - IP
    d_editRemoteIP->setText(QString::fromStdString(channel->remoteIP()));
    // remote - port
    d_spinBoxRemotePort->setValue(channel->remotePort());
    // read
    d_checkBoxReadOnly->setChecked(channel->openMode() & Icd::UdpChannel::ReadOnly);
    // write
    d_checkBoxWriteOnly->setChecked(channel->openMode() & Icd::UdpChannel::WriteOnly);
    // name
    d_editName->setText(QString::fromStdString(channel->name()));
    // relayer
    if (channel->relayer()) {
        d_editRelayer->setText(QString::fromStdString(channel->relayer()->name()));
        d_editRelayer->setProperty("relayer", QString::fromStdString(channel->relayer()->identity()));
    }

    //
    d_buttonApply->setEnabled(false);
    d_buttonRestore->setEnabled(false);
}

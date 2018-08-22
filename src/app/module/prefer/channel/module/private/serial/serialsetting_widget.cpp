#include "precomp.h"
#include "serialsetting_widget.h"
#include "icdcomm/icdcomm_serialchannel.h"
#include "icdworker/icdworker_pool.h"
#include <icdwidget/JGroupChannelPane.h>
#include <QSerialPort>
#include <QSerialPortInfo>

SerialSettingWidget::SerialSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBox = new QGroupBox(tr("Serial settings"), this);
    vertLayoutMain->addWidget(groupBox);

    QVBoxLayout *vertLayoutGroupBox = new QVBoxLayout(groupBox);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    vertLayoutGroupBox->addLayout(formLayout);

    d_comboBoxPortName = new QComboBox(this);
    d_comboBoxPortName->setEditable(true);
    d_comboBoxPortName->lineEdit()->setValidator(new QRegExpValidator(QRegExp("COM[\\d]{1,3}")));
    formLayout->addRow(tr("Port name:"), d_comboBoxPortName);

    d_comboBoxBaudRate = new QComboBox(this);
    d_comboBoxBaudRate->setEditable(true);
    d_comboBoxBaudRate->setAutoCompletion(false);
    d_comboBoxBaudRate->setValidator(new QIntValidator(d_comboBoxBaudRate));
    formLayout->addRow(tr("Baud rate:"), d_comboBoxBaudRate);

    d_comboBoxDataBits = new QComboBox(this);
    formLayout->addRow(tr("Data bits:"), d_comboBoxDataBits);

    d_comboBoxStopBits = new QComboBox(this);
    formLayout->addRow(tr("Stop bits:"), d_comboBoxStopBits);

    d_comboBoxParity = new QComboBox(this);
    formLayout->addRow(tr("Parity:"), d_comboBoxParity);

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

    // PortName
    const QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    QListIterator<QSerialPortInfo> citerPorts(availablePorts);
    while (citerPorts.hasNext()) {
        const QSerialPortInfo &portInfo = citerPorts.next();
        d_comboBoxPortName->addItem(portInfo.portName());
    }
    // BaudRate
    QList<qint32> baudRates = QSerialPortInfo::standardBaudRates();
    // custom
    baudRates.append(230400);
    baudRates.append(460800);
    baudRates.append(500000);
    baudRates.append(512000);
    baudRates.append(600000);
    baudRates.append(750000);
    baudRates.append(921600);
    baudRates.append(1000000);
    baudRates.append(1500000);
    baudRates.append(2000000);
    baudRates.append(3000000);
    // sort
    qSort(baudRates);
    // add
    QListIterator<qint32> citerBaudRates(baudRates);
    while (citerBaudRates.hasNext()) {
        const qint32 baudRate = citerBaudRates.next();
        d_comboBoxBaudRate->addItem(QString::number(baudRate));
    }
    // DataBits
    for (int i = 5; i < 9; ++i) {
        d_comboBoxDataBits->addItem(QString::number(i));
    }
    // StopBits
    d_comboBoxStopBits->addItem("1");
    d_comboBoxStopBits->addItem("1.5");
    d_comboBoxStopBits->addItem("2");
    // Parity
    d_comboBoxParity->addItem(tr("None"));
    d_comboBoxParity->addItem(tr("Odd"));
    d_comboBoxParity->addItem(tr("Even"));
    d_comboBoxParity->addItem(tr("Space"));
    d_comboBoxParity->addItem(tr("Mark"));

    //
    auto enableButtons = [=](bool enabled){
        d_buttonApply->setEnabled(enabled);
        d_buttonRestore->setEnabled(enabled);
    };

    //
    connect(d_comboBoxPortName, &QComboBox::currentTextChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_comboBoxBaudRate, &QComboBox::currentTextChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_comboBoxDataBits, &QComboBox::currentTextChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_comboBoxStopBits, &QComboBox::currentTextChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_comboBoxParity, &QComboBox::currentTextChanged, this, [=](){
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
        Icd::JGroupChannelPane *groupChannelPane = new Icd::JGroupChannelPane(this);
        if (groupChannelPane->exec() != QDialog::Accepted) {
            return; // cancel
        }

        //
        groupChannelPane->deleteLater();
        Icd::WorkerPtr selectedWorker = groupChannelPane->selectedWorker();
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
        Icd::SerialChannelPtr channel =
                JHandlePtrCast<Icd::SerialChannel, Icd::Channel>(d_worker->channel());
        if (!channel) {
            return;
        }
        //
        if (channel->isOpen()) {
            const QString message = tr("Channel \"%1\" has been opened, can't modify!");
            QMessageBox::warning(this, tr("Warning"),
                                 message.arg(QString::fromStdString(channel->name())));
            return;
        }
        //
        const QString portName = d_comboBoxPortName->currentText();
        if (portName.size() < 3) {
            enableButtons(false);
            return;
        }
        // PortName
        channel->setPortName(portName.toStdString());
        // BaudRate
        channel->setBaudRate(d_comboBoxBaudRate->currentText().toInt());
        // DataBits
        switch (d_comboBoxDataBits->currentIndex()) {
        case 0:
            channel->setDataBits(Icd::SerialChannel::Data5);
            break;
        case 1:
            channel->setDataBits(Icd::SerialChannel::Data6);
            break;
        case 2:
            channel->setDataBits(Icd::SerialChannel::Data7);
            break;
        case 3:
            channel->setDataBits(Icd::SerialChannel::Data8);
            break;
        }
        // StopBits
        switch (d_comboBoxStopBits->currentIndex()) {
        case 0:
        default:
            channel->setStopBits(Icd::SerialChannel::OneStop);
            break;
        case 1:
            channel->setStopBits(Icd::SerialChannel::OneAndHalfStop);
            break;
        case 2:
            channel->setStopBits(Icd::SerialChannel::TwoStop);
            break;
        }
        // Parity
        switch (d_comboBoxParity->currentIndex()) {
        case 0:
        default:
            channel->setParity(Icd::SerialChannel::NoParity);
            break;
        case 1:
            channel->setParity(Icd::SerialChannel::EvenParity);
            break;
        case 2:
            channel->setParity(Icd::SerialChannel::OddParity);
            break;
        case 3:
            channel->setParity(Icd::SerialChannel::OddParity);
            break;
        case 4:
            channel->setParity(Icd::SerialChannel::MarkParity);
            break;
        }
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

SerialSettingWidget::~SerialSettingWidget()
{

}

void SerialSettingWidget::updateUi(const Icd::WorkerPtr &worker)
{
    if (d_worker) {
        d_worker->disconnect(this);
        //
        Icd::SerialChannelPtr channel =
                JHandlePtrCast<Icd::SerialChannel, Icd::Channel>(d_worker->channel());
    }

    //
    d_worker = worker;

    // clear
    d_comboBoxPortName->setCurrentIndex(-1);
    d_comboBoxDataBits->setCurrentIndex(-1);
    d_comboBoxStopBits->setCurrentIndex(-1);
    d_comboBoxParity->setCurrentIndex(-1);
    d_editName->clear();
    d_buttonApply->setEnabled(false);
    d_buttonRestore->setEnabled(false);
    // relayer
    d_editRelayer->clear();

    //
    if (!d_worker) {
        return;
    }

    //
    Icd::SerialChannelPtr channel =
            JHandlePtrCast<Icd::SerialChannel, Icd::Channel>(d_worker->channel());
    if (!channel) {
        return;
    }

    // PortName
    d_comboBoxPortName->setCurrentText(QString::fromStdString(channel->portName()));
    // BaudRate
    d_comboBoxBaudRate->setCurrentText(QString::number(channel->baudRate()));
    // DataBits
    switch (channel->dataBits()) {
    case Icd::SerialChannel::Data5:
        d_comboBoxDataBits->setCurrentIndex(0);
        break;
    case Icd::SerialChannel::Data6:
        d_comboBoxDataBits->setCurrentIndex(1);
        break;
    case Icd::SerialChannel::Data7:
        d_comboBoxDataBits->setCurrentIndex(2);
        break;
    case Icd::SerialChannel::Data8:
    default:
        d_comboBoxDataBits->setCurrentIndex(3);
        break;
    }
    // StopBits
    switch (channel->stopBits()) {
    case Icd::SerialChannel::OneStop:
    default:
        d_comboBoxStopBits->setCurrentIndex(0);
        break;
    case Icd::SerialChannel::OneAndHalfStop:
        d_comboBoxStopBits->setCurrentIndex(1);
        break;
    case Icd::SerialChannel::TwoStop:
        d_comboBoxStopBits->setCurrentIndex(2);
        break;
    }
    // Parity
    switch (channel->parity()) {
    case Icd::SerialChannel::NoParity:
    default:
        d_comboBoxParity->setCurrentIndex(0);
        break;
    case Icd::SerialChannel::EvenParity:
        d_comboBoxParity->setCurrentIndex(1);
        break;
    case Icd::SerialChannel::OddParity:
        d_comboBoxParity->setCurrentIndex(2);
        break;
    case Icd::SerialChannel::SpaceParity:
        d_comboBoxParity->setCurrentIndex(3);
        break;
    case Icd::SerialChannel::MarkParity:
        d_comboBoxParity->setCurrentIndex(4);
        break;
    }
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

#include "precomp.h"
#include "filesetting_widget.h"
#include "icdcomm/icdcomm_filechannel.h"
#include "icdworker/icdworker_pool.h"
#include "icdwidget/bindchannel_widget.h"

FileSettingWidget::FileSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    QGroupBox *groupBox = new QGroupBox(QStringLiteral("文件流设置"), this);
    vertLayoutMain->addWidget(groupBox);

    QVBoxLayout *vertLayoutGroupBox = new QVBoxLayout(groupBox);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    vertLayoutGroupBox->addLayout(formLayout);

    d_editFilePath = new QLineEdit(this);

    QHBoxLayout *horiLayoutFilePath = new QHBoxLayout();
    horiLayoutFilePath->setSpacing(0);
    formLayout->addRow(QStringLiteral("文件路径："), horiLayoutFilePath);

    horiLayoutFilePath->addWidget(d_editFilePath);

    QPushButton *buttonView = new QPushButton(QStringLiteral("..."), this);
    buttonView->setFixedSize(d_editFilePath->sizeHint().height() + 6, d_editFilePath->sizeHint().height());
    buttonView->setStyleSheet("border-radius: 1px;");
    horiLayoutFilePath->addWidget(buttonView);

    d_checkBoxReadOnly = new QCheckBox(QStringLiteral("只读"), this);
    formLayout->addRow(QStringLiteral("打开模式："), d_checkBoxReadOnly);

    d_checkBoxWriteOnly = new QCheckBox(QStringLiteral("只写"), this);
    formLayout->addRow(QStringLiteral(" "), d_checkBoxWriteOnly);

    d_checkBoxAppend = new QCheckBox(QStringLiteral("追加写入"), this);
    formLayout->addRow(QStringLiteral(" "), d_checkBoxAppend);

    d_checkBoxTruncate = new QCheckBox(QStringLiteral("删除已存在文件"), this);
    formLayout->addRow(QStringLiteral(" "), d_checkBoxTruncate);

    d_checkBoxText = new QCheckBox(QStringLiteral("文本存储模式"), this);
    formLayout->addRow(QStringLiteral(" "), d_checkBoxText);

    d_editName = new QLineEdit(this);
    formLayout->addRow(QStringLiteral("通道名称："), d_editName);

    d_checkBoxDomain = new QCheckBox(QStringLiteral("表域名（表协议查询ID）"), this);
    formLayout->addRow(QStringLiteral("存储格式："), d_checkBoxDomain);

    d_checkBoxTimestamp = new QCheckBox(QStringLiteral("时间戳"), this);
    formLayout->addRow(QStringLiteral(""), d_checkBoxTimestamp);

    QHBoxLayout *horiLayoutRelayer = new QHBoxLayout();
    horiLayoutRelayer->setSpacing(0);
    formLayout->addRow(QStringLiteral("转发通道："), horiLayoutRelayer);

    d_editRelayer = new QLineEdit(this);
    horiLayoutRelayer->addWidget(d_editRelayer);

    QPushButton *buttonRelayer = new QPushButton(QStringLiteral("..."), this);
    buttonRelayer->setFixedSize(d_editRelayer->sizeHint().height() + 6, d_editRelayer->sizeHint().height());
    buttonRelayer->setStyleSheet("border-radius: 1px;");
    horiLayoutRelayer->addWidget(buttonRelayer);

    vertLayoutGroupBox->addSpacing(20);

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutGroupBox->addLayout(horiLayoutButtons);

    horiLayoutButtons->addStretch();

    d_buttonRestore = new QPushButton(QStringLiteral("恢复"), this);
    d_buttonRestore->setFixedSize(70, 30);
    horiLayoutButtons->addWidget(d_buttonRestore);

    horiLayoutButtons->addSpacing(10);

    d_buttonApply = new QPushButton(QStringLiteral("应用"), this);
    d_buttonApply->setFixedSize(70, 30);
    horiLayoutButtons->addWidget(d_buttonApply);

    horiLayoutButtons->addSpacing(15);

    // init

    // FilePath
    d_editFilePath->setText("");
    // read
    d_checkBoxReadOnly->setChecked(true);
    // write
    d_checkBoxWriteOnly->setChecked(true);

    //
    auto enableButtons = [=](bool enabled){
        d_buttonApply->setEnabled(enabled);
        d_buttonRestore->setEnabled(enabled);
    };

    //
    connect(d_editFilePath, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(buttonView, &QPushButton::clicked, this, [=](){
        const QString filePath = QFileDialog::getExistingDirectory(
                    this, QStringLiteral("文件名称选择"), QApplication::applicationDirPath());
        if (filePath.isEmpty()) {
            return;
        }
        //
        d_editFilePath->setText(filePath);
    });
    connect(d_checkBoxReadOnly, &QCheckBox::stateChanged, this, [=](bool checked){
        if (checked) {
            d_checkBoxWriteOnly->setChecked(false);
        }
        enableButtons(true);
    });
    connect(d_checkBoxWriteOnly, &QCheckBox::stateChanged, this, [=](bool checked){
        if (checked) {
            d_checkBoxReadOnly->setChecked(false);
        }
        enableButtons(true);
    });
    connect(d_checkBoxAppend, &QCheckBox::stateChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_checkBoxTruncate, &QCheckBox::stateChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_checkBoxText, &QCheckBox::stateChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_editName, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_checkBoxDomain, &QCheckBox::stateChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_checkBoxTimestamp, &QCheckBox::stateChanged, this, [=](){
        enableButtons(true);
    });
    connect(d_editRelayer, &QLineEdit::textChanged, this, [=](){
        enableButtons(true);
    });
    connect(buttonRelayer, &QPushButton::clicked, this, [=](){
        //
        Icd::BindChannelWidget *bindChannelWidget = new Icd::BindChannelWidget(this);
        if (bindChannelWidget->exec() != QDialog::Accepted) {
            return; // cancel
        }

        //
        bindChannelWidget->deleteLater();
        Icd::WorkerPtr selectedWorker = bindChannelWidget->selectedWorker();
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
        Icd::FileChannelPtr channel =
                JHandlePtrCast<Icd::FileChannel, Icd::Channel>(d_worker->channel());
        if (!channel) {
            return;
        }
        //
        if (channel->isOpen()) {
            QMessageBox::warning(this, QStringLiteral("警告"),
                                 QStringLiteral("[%1]通道已打开，不能修改属性！")
                                 .arg(QString::fromStdString(channel->name())));
            return;
        }
        //
        bool isSendRunning = d_worker->workerSend()->isRunning();
        bool isRecvRunning = d_worker->workerRecv()->isRunning();
        d_worker->workerSend()->stop();
        d_worker->workerRecv()->stop();
        // FilePath
        QFileInfo fileInfo(QString::fromStdString(channel->filePath()));
        channel->setFilePath(d_editFilePath->text().trimmed().append("/")
                             .append(fileInfo.fileName()).toStdString());
        // OpenMode
        unsigned int openMode = Icd::FileChannel::NotOpen;
        if (d_checkBoxReadOnly->isChecked()) {
            openMode |= Icd::FileChannel::ReadOnly;
        }
        if (d_checkBoxWriteOnly->isChecked()) {
            openMode |= Icd::FileChannel::WriteOnly;
        }
        if (d_checkBoxAppend->isChecked()) {
            openMode |= Icd::FileChannel::Append;
        }
        if (d_checkBoxTruncate->isChecked()) {
            openMode |= Icd::FileChannel::Truncate;
        }
        if (d_checkBoxText->isChecked()) {
            openMode |= Icd::FileChannel::Text;
        }
        channel->setOpenMode((Icd::FileChannel::OpenMode)openMode);
        // name
        channel->setName(d_editName->text().toStdString());
        // SaveFormat
        unsigned int saveFormat = 0;
        if (d_checkBoxDomain->isChecked()) {
            saveFormat |= Icd::FileChannel::SaveFormatDomain;
        }
        if (d_checkBoxTimestamp->isChecked()) {
            saveFormat |= Icd::FileChannel::SaveFormatTimestamp;
        }
        channel->setSaveFormat((Icd::FileChannel::SaveFormat)saveFormat);
        // relayer
        Icd::WorkerPtr relayer = d_worker->relayer();
        const QString relayerName = d_editRelayer->text();
        if (relayerName.isEmpty()) {
            relayer = Icd::WorkerPtr(0);
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
                    relayer = Icd::WorkerPtr(0);
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

FileSettingWidget::~FileSettingWidget()
{

}

void FileSettingWidget::updateUi(const Icd::WorkerPtr &worker)
{
    //
    d_worker = worker;

    // clear

    // FilePath
    d_editFilePath->setText("");
    // read
    d_checkBoxReadOnly->setChecked(true);
    // write
    d_checkBoxWriteOnly->setChecked(true);
    // append
    d_checkBoxAppend->setChecked(false);
    // truncate
    d_checkBoxTruncate->setChecked(true);
    // text
    d_checkBoxText->setChecked(false);
    // name
    d_editName->clear();
    // domain
    d_checkBoxDomain->setChecked(true);
    // timestamp
    d_checkBoxTimestamp->setChecked(true);
    // relayer
    d_editRelayer->clear();

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

    // FilePath
    d_editFilePath->setText(QFileInfo(QString::fromStdString(channel->filePath())).path());
    // read
    d_checkBoxReadOnly->setChecked(channel->openMode() & Icd::FileChannel::ReadOnly);
    // write
    d_checkBoxWriteOnly->setChecked(channel->openMode() & Icd::FileChannel::WriteOnly);
    // append
    d_checkBoxAppend->setChecked(channel->openMode() & Icd::FileChannel::Append);
    // truncate
    d_checkBoxTruncate->setChecked(channel->openMode() & Icd::FileChannel::Truncate);
    // text
    d_checkBoxText->setChecked(channel->openMode() & Icd::FileChannel::Text);
    // name
    d_editName->setText(QString::fromStdString(channel->name()));
    // domain
    d_checkBoxDomain->setChecked(channel->saveFormat() & Icd::FileChannel::SaveFormatDomain);
    // timestamp
    d_checkBoxTimestamp->setChecked(channel->saveFormat() & Icd::FileChannel::SaveFormatTimestamp);
    // relayer
    if (channel->relayer()) {
        d_editRelayer->setText(QString::fromStdString(channel->relayer()->name()));
        d_editRelayer->setProperty("relayer", QString::fromStdString(channel->relayer()->identity()));
    }

    //
    d_buttonApply->setEnabled(false);
    d_buttonRestore->setEnabled(false);
}

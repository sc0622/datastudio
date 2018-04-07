#include "precomp.h"
#include "commgr_widget_p.h"
#include "commdetail_widget.h"
#include "icdwidget/channel_widget.h"

ComMgrWidgetPrivate::ComMgrWidgetPrivate(ComMgrWidget *q)
    : QObject(q)
    , q_ptr(q)
{

}

ComMgrWidgetPrivate::~ComMgrWidgetPrivate()
{

}

void ComMgrWidgetPrivate::init()
{
    Q_Q(ComMgrWidget);

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(q);
    vertLayoutMain->setContentsMargins(0, 0, 0, 0);

    splitterTop = new JSplitter(q);
    splitterTop->setHandleWidth(4);
    splitterTop->setHandleColor(QColor(0, 0, 0, 0));
    splitterTop->setScales(QList<double>() << 2.5 << 1);
    vertLayoutMain->addWidget(splitterTop);

    QWidget *widgetArea = new QWidget(q);
    splitterTop->addWidget(widgetArea);

    QVBoxLayout *vertLayoutArea = new QVBoxLayout(widgetArea);
    vertLayoutArea->setContentsMargins(0, 0, 0, 6);

    channelWidget = new Icd::ChannelWidget(q);
    vertLayoutArea->addWidget(channelWidget);

    QHBoxLayout *horiLayoutButtons = new QHBoxLayout();
    vertLayoutArea->addLayout(horiLayoutButtons);

    horiLayoutButtons->addStretch();
    buttonAdd = new QPushButton(QStringLiteral("增加通道"), widgetArea);
    buttonAdd->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(buttonAdd);

    horiLayoutButtons->addSpacing(10);

    buttonClear = new QPushButton(QStringLiteral("清空通道"), widgetArea);
    buttonClear->setFixedSize(120, 30);
    horiLayoutButtons->addWidget(buttonClear);

    horiLayoutButtons->addSpacing(10);

    commDetailWidget = new CommDetailWidget(q);
    commDetailWidget->hide();
    splitterTop->addWidget(commDetailWidget);

    //
    connect(channelWidget, &Icd::ChannelWidget::currentRowChanged, q, [=]
            (int currentRow, int){
        if (currentRow >= 0) {
            Icd::WorkerPtr worker = channelWidget->selectedWorker();
            if (worker) {
                commDetailWidget->updateUi(worker);
                commDetailWidget->show();
            }
        } else {
            commDetailWidget->hide();
        }
    });
    connect(channelWidget, &Icd::ChannelWidget::rowMoved, q, [=](int oldRow, int newRow){
        Q_UNUSED(oldRow);
        Q_UNUSED(newRow);
    });
    //
    connect(buttonAdd, &QPushButton::clicked, q, [=](){
        QDialog dialog(q);
        QVBoxLayout *vertLayoutMain = new QVBoxLayout(&dialog);
        QFormLayout *formLayout = new QFormLayout();
        vertLayoutMain->addLayout(formLayout);
        QComboBox *comboBox = new QComboBox(&dialog);
        formLayout->addRow(QStringLiteral("通道类型："), comboBox);
        QDialogButtonBox *buttonBox = new QDialogButtonBox(&dialog);
        buttonBox->setStyleSheet("QPushButton{min-width:80px;min-height:24px;}");
        buttonBox->addButton(QStringLiteral("确定"), QDialogButtonBox::AcceptRole);
        buttonBox->addButton(QStringLiteral("取消"), QDialogButtonBox::RejectRole);
        vertLayoutMain->addWidget(buttonBox, 0, Qt::AlignRight);
        QObject::connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
        // init
        comboBox->addItem(QStringLiteral("串口"));
        comboBox->addItem(QStringLiteral("UDP"));
        comboBox->addItem(QStringLiteral("文件"));
        // exec
        dialog.resize(350, 100);
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
        //
        Icd::ChannelType channelType = Icd::ChannelInvalid;
        switch (comboBox->currentIndex()) {
        case 0: channelType = Icd::ChannelSerial; break;
        case 1: channelType = Icd::ChannelUdp; break;
        case 2: channelType = Icd::ChannelFile; break;
        default: break;
        }
        //
        if (channelType == Icd::ChannelInvalid) {
            return;
        }
        //
        Icd::ChannelPtr channel = Icd::Channel::createInstance(channelType);
        if (!channel) {
            return;
        }
        //
        Icd::WorkerPtr worker = Icd::WorkerPtr(new Icd::Worker(channel));
        Icd::WorkerPool::getInstance()->appendWorker(worker);
        Icd::WorkerPool::getInstance()->saveConfig();
        //
        channelWidget->addWorker(worker, Icd::ChannelWidget::AllOperate);

        //
        channelWidget->setCurrentRow(channelWidget->rowCount() - 1);
    });
    connect(buttonClear, &QPushButton::clicked, q, [=](){
        int result = QMessageBox::warning(q, QStringLiteral("警告"),
                                          QStringLiteral("清空通道配置将不可恢复原有配置，是否继续？"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result != QMessageBox::Yes) {
            return;
        }
        //
        Icd::WorkerPool::getInstance()->clearWorker();
    });

    //
    QFile file(":/icdsystem/com_channelmgr/qss/default.qss");
    if (file.open(QFile::ReadOnly)) {
        q->setStyleSheet(file.readAll());
    }
}

void ComMgrWidgetPrivate::updateUi()
{
    //
    channelWidget->clearWorker();

    //
    const Icd::WorkerPtrArray workers =
            Icd::WorkerPool::getInstance()->allWorkers();
    Icd::WorkerPtrArray::const_iterator citer = workers.begin();
    for (; citer != workers.end(); ++citer) {
        channelWidget->addWorker(*citer, Icd::ChannelWidget::AllOperate);
    }

    //
    channelWidget->setCurrentRow(0);
}

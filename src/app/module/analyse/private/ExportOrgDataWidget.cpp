#include "precomp.h"
#include "ExportOrgDataWidget.h"
#include "icdcore/icd_table.h"

ExportOrgDataWidget::ExportOrgDataWidget(const QString &filePath,
                                         const Icd::TablePtr &table,
                                         const Icd::ObjectPtr &objectItem,
                                         bool hasTimeFormat, int headerSize,
                                         int bitOffset, QWidget *parent)
    : QWidget(parent)
    , d_filePath(filePath)
    , d_table(table)
    , d_objectItem(objectItem)
    , d_hasTimeFormat(hasTimeFormat)
    , d_headerSize(headerSize)
    , d_bitOffset(bitOffset)
{
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();
    layoutMain->addLayout(formLayout);
    QHBoxLayout *layoutPath = new QHBoxLayout();
    formLayout->addRow(QStringLiteral("����·����"), layoutPath);
    QLineEdit *editPath = new QLineEdit(this);
    QPushButton *buttonView = new QPushButton(QStringLiteral("��"), this);
    buttonView->setFixedWidth(60);
    layoutPath->addWidget(editPath);
    layoutPath->addWidget(buttonView);
    QCheckBox *checkBox = nullptr;
    if (hasTimeFormat) {
        checkBox = new QCheckBox(this);
        formLayout->addRow(QStringLiteral("������ʱ�䣺"), checkBox);
    }
    QPushButton *buttonOk = new QPushButton(QStringLiteral("ȷ��"), this);
    QPushButton *buttonCancel = new QPushButton(QStringLiteral("ȡ��"), this);
    buttonOk->setEnabled(false);
    connect(editPath, &QLineEdit::textChanged, [=](const QString &text){
        if (text.isEmpty()) {
            buttonOk->setEnabled(false);
        } else {
            buttonOk->setEnabled(true);
        }
    });
    connect(buttonView, &QPushButton::clicked, [=](){
        QString selectedFilter = QStringLiteral("Data file (*.dat)");
        const QString targetFilePath =
                QFileDialog::getSaveFileName(this, QStringLiteral("���Ϊ"),
                                             editPath->text(),
                                             "Any file (*.*);;"
                                             "ICD file (*.icd);;"
                                             "Data file (*.dat);;"
                                             "Text file (*.txt)", &selectedFilter);
        if (targetFilePath.isEmpty()) {
            return;
        }
        editPath->setText(targetFilePath);
        editPath->setToolTip(targetFilePath);
        //
    });
    connect(buttonOk, &QPushButton::clicked, [=](){
        const QString target = editPath->text().trimmed();
        if (target.isEmpty()) {
            editPath->selectAll();
            editPath->setFocus();
            return;
        }
        // export
        bool exportTime = false;
        if (checkBox) {
            exportTime = checkBox->isChecked();
        }
        //
        exportData(target, exportTime);
    });
    connect(buttonCancel, &QPushButton::clicked, [=](){
        emit rejected();
    });
    QHBoxLayout *layoutBottom = new QHBoxLayout();
    layoutMain->addLayout(layoutBottom);
    layoutBottom->addWidget(buttonOk);
    layoutBottom->addWidget(buttonCancel);
}

void ExportOrgDataWidget::exportData(const QString &target, bool exportTime)
{
    if (target.isEmpty()) {
        return;
    }

    //
    int bufferOffset = 0, bufferSize = 0;
    switch (d_objectItem->objectType()) {
    case Icd::ObjectTable:
    {
        const Icd::TablePtr table = JHandlePtrCast<Icd::Table, Icd::Object>(d_objectItem);
        if (!table) {
            return;
        }
        bufferOffset = (int)table->bufferOffset();
        bufferSize = qCeil(table->bufferSize());
        break;
    }
    case Icd::ObjectItem:
    {
        const Icd::ItemPtr item = JHandlePtrCast<Icd::Item, Icd::Object>(d_objectItem);
        if (!item) {
            return;
        }
        bufferOffset = (int)item->bufferOffset();
        bufferSize = qCeil(item->bufferSize());
        break;
    }
    default:
        return;
    }

    Icd::ProgressDialog *progressDialog = new Icd::ProgressDialog(this);
    progressDialog->setWindowTitle(QStringLiteral("��������"));
    progressDialog->setProgressValue(100);
    progressDialog->setCancelVisible(false);
    progressDialog->setMessage(QStringLiteral("���ڵ������ݡ���"));
    QFuture<bool> future = QtConcurrent::run([=]() -> bool {
        QFile sourceFile(d_filePath);
        QFile targetFile(target);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            return false;
        }
        if (!targetFile.open(QIODevice::WriteOnly)) {
            return false;
        }
        targetFile.resize(0);
        qint64 bufferSize = sourceFile.size() - d_headerSize;
        int tableSize = (int)d_table->bufferSize();
        if (d_hasTimeFormat) {
            tableSize += 8;
            const int count = bufferSize / tableSize;
            for (int i = 0; i < count; ++i) {
                if (exportTime) {
                    sourceFile.seek(d_headerSize + tableSize * i);
                    targetFile.write(sourceFile.read(8));
                }
                sourceFile.seek(d_headerSize + tableSize * i + 8 + bufferOffset);
                targetFile.write(sourceFile.read(bufferSize));
            }
        } else {
            const int count = bufferSize / tableSize;
            for (int i = 0; i < count; ++i) {
                sourceFile.seek(d_headerSize + tableSize * i + bufferOffset);
                targetFile.write(sourceFile.read(bufferSize));
            }
        }
        return true;
    });
    connect(progressDialog, &Icd::ProgressDialog::finished, this, [=](){
        progressDialog->hide();
        progressDialog->disconnect(this);
        const QString message = progressDialog->futureResult()
                ? QStringLiteral("������ɣ�") : QStringLiteral("����ʧ�ܣ�");
        QMessageBox::information(this, QStringLiteral("�������"), message);
        progressDialog->deleteLater();
    });
    progressDialog->setFuture(future);
    progressDialog->exec();
}
